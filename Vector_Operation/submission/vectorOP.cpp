#include "PPintrin.h"
#include <iostream>

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x, result, clamp;
  __pp_vec_int exp, count;
  __pp_mask maskAll, maskIsZero, maskIsNonZero, maskIsClamp, maskPadding;

  // Constants
  __pp_vec_float maxResult = _pp_vset_float(9.999999f);
  __pp_vec_float oneResult = _pp_vset_float(1.000000f);
  __pp_vec_float zeroResult = _pp_vset_float(0.0);
  __pp_vec_int zeroExp = _pp_vset_int(0);
  __pp_vec_int oneExp = _pp_vset_int(1);
  bool needPadding = false;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // Initialize masks
    maskAll = _pp_init_ones();
    if(i + VECTOR_WIDTH > N) {
      maskPadding = _pp_init_ones();
      maskPadding = _pp_mask_not(maskPadding);
      int start = N % VECTOR_WIDTH;
      for(int j = start; j < VECTOR_WIDTH; j++) {
        maskPadding.value[j] = true;
      }
      needPadding = true;
    }
    // for(int j = 0; j < VECTOR_WIDTH; j++) {
    //   std::cout << maskPadding.value[j] << " ";
    // }
    // std::cout << std::endl;


    // Load values and exponents
    _pp_vload_float(x, values + i, maskAll);
    _pp_vload_int(exp, exponents + i, maskAll);
    _pp_vmove_int(count, exp, maskAll); // Set initial value of result as x

    // Initialize result as 1.0
    result = _pp_vset_float(1.0f);

    // Mask for exponents[i] == 0
    _pp_veq_int(maskIsZero, exp, zeroExp, maskAll);
    _pp_vmove_float(result, oneResult, maskIsZero); // Set initial value of result as x

    // Mask for exponents[i] != 0 (inverse of maskIsZero)
    maskIsNonZero = _pp_mask_not(maskIsZero);

    // For exponents[i] != 0, compute the power
    _pp_vmove_float(result, x, maskIsNonZero); // Set initial value of result as x
    _pp_vsub_int(count, exp, oneExp, maskIsNonZero); // Decrement exponent
    
    _pp_veq_int(maskIsZero, count, zeroExp, maskAll);
    maskIsNonZero = _pp_mask_not(maskIsZero);

    // While exp[i] > 1, keep multiplying
    while (_pp_cntbits(maskIsNonZero) > 0)
    {
      _pp_vmult_float(result, result, x, maskIsNonZero); // result *= x;
      _pp_vsub_int(count, count, oneExp, maskIsNonZero); // count--;
      _pp_vgt_int(maskIsNonZero, count, zeroExp, maskAll); // Update mask for further iterations
    }

    // Clamp result to maxResult (9.999)
    _pp_vgt_float(maskIsClamp, result, maxResult, maskAll);
    _pp_vmove_float(result, maxResult, maskIsClamp);
    if(needPadding)
      _pp_vmove_float(result, zeroResult, maskPadding);

    // Write result back to output array
    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
  }

  return 0.0;
}
