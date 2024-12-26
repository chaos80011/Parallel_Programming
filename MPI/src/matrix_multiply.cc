#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>

// *********************************************
// ** ATTENTION: YOU CANNOT MODIFY THIS FILE. **
// *********************************************

void construct_matrices(std::ifstream &in, int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr) {
    in >> *n_ptr >> *m_ptr >> *l_ptr;

    int n = *n_ptr, m = *m_ptr, l = *l_ptr;

    *a_mat_ptr = new int[n * m];
    *b_mat_ptr = new int[m * l];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            in >> (*a_mat_ptr)[i * m + j];
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < l; j++) {
            in >> (*b_mat_ptr)[i * l + j];
        }
    }
}

void matrix_multiply(const int n, const int m, const int l,
                     const int *a_mat, const int *b_mat) {
    int size, rank;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int numworker, rows, averow, extra, offset;
    numworker = size - 1;

    if (rank == 0) {
        // Result matrix C
        std::vector<int> c(n * l);

        // Scatter the rows of matrix A to workers
        averow = n / numworker;
        extra = n % numworker;
        offset = 0;

        for (int dest = 1; dest <= numworker; dest++) {
            MPI_Send(&n, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&l, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            
            rows = (dest <= extra) ? averow + 1 : averow;
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&a_mat[offset * m], rows * m, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&b_mat[0], m * l, MPI_INT, dest, 1, MPI_COMM_WORLD);
            
            offset += rows;
        }

        // Receive results from workers
        for (int i = 1; i <= numworker; i++) {
            int source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset * l], rows * l, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
        }

        // Print the result matrix C
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < l; j++) {
                std::cout << c[i * l + j];
                if (j != l - 1) std::cout << " ";
            }
            std::cout << std::endl;
        }
    }

    if (rank > 0) {
        // Worker tasks
        int N, M, L;
        MPI_Recv(&N, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&M, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&L, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        std::vector<int> a(N * M);
        std::vector<int> b(M * L);
        std::vector<int> c(N * L, 0);

        MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&a[0], rows * M, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&b[0], M * L, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        // Perform matrix multiplication for the rows assigned to this worker
        for (int i = 0; i < rows; i++) {
            for (int k = 0; k < L; k++) {
                c[i * L + k] = 0;
                for (int j = 0; j < M; j++) {
                    c[i * L + k] += a[i * M + j] * b[j * L + k];
                }
            }
        }

        // Send the result back to master
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c[0], rows * L, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
}

void destruct_matrices(int *a_mat, int *b_mat) {
    delete[] a_mat;
    delete[] b_mat;
}
