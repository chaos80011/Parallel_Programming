#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>

// *********************************************
// ** ATTENTION: YOU CANNOT MODIFY THIS FILE. **
// *********************************************

void construct_matrices(std::ifstream &in, int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr) {

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) {
        in >> *n_ptr >> *m_ptr >> *l_ptr;
        
        int n = *n_ptr;
        int m = *m_ptr;
        int l = *l_ptr;

        *a_mat_ptr = new int[n * m];
        *b_mat_ptr = new int[m * l];

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                in >> (*a_mat_ptr)[i * m + j];
            }
        }

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < l; ++j) {
                in >> (*b_mat_ptr)[i * l + j];
            }
        }

    }
}

void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){

    MPI_Status status;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD ,&size);
    
    int rows, rem, mtype, offset;
    int num_workers = size - 1;
    if (rank == 0) {
        rows = n / num_workers;
        rem = n % num_workers;
        mtype = 1;
        offset = 0;
        int *c_mat = new int[n * l];

        for (int i = 1; i <= num_workers; i++)
        {
            int current_rows = rows + (rem > 0);

            MPI_Send(&n, 1, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&l, 1, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&offset, 1, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&current_rows, 1, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&a_mat[offset * m], current_rows*m, MPI_INT, i, mtype, MPI_COMM_WORLD);
            MPI_Send(&b_mat[0], m*l, MPI_INT, i, mtype, MPI_COMM_WORLD);

            offset += current_rows;
            rem--; 
        }

        mtype = 2;
        for (int i = 1; i <= num_workers; i++)
        {
            MPI_Recv(&offset, 1, MPI_INT, i, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, i, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&c_mat[offset * l], rows*l, MPI_INT, i, mtype, MPI_COMM_WORLD, &status); 
        } 

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < l; j++)
                printf("%d ", c_mat[i * l + j]); 
            printf("\n");
        } 

        delete[] c_mat;
    }

    if (rank > 0) {
        mtype = 1;

        int nCopy, mCopy, lCopy;
        MPI_Recv(&nCopy, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&mCopy, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&lCopy, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        int *a = new int[rows * mCopy];
        int *b = new int[mCopy * lCopy];
        int *c = new int[rows * lCopy];

        MPI_Recv(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&a[0], rows * mCopy, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&b[0], mCopy * lCopy, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status); 
        for (int k = 0; k < lCopy; k++){
            for (int i = 0; i < rows; i++) {
                c[i * lCopy + k] = 0;
                for (int j = 0; j < mCopy; j++)
                    c[i * lCopy + k] += a[i * mCopy + j] * b[j * lCopy + k];
            }
        }
        

        mtype = 2;
        MPI_Send(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&c[0], rows * lCopy, MPI_INT, 0, mtype, MPI_COMM_WORLD);

        delete[] a;
        delete[] b;
        delete[] c;
    } 
}

void destruct_matrices(int *a_mat, int *b_mat)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        delete[] a_mat;
        delete[] b_mat;
    }
}
