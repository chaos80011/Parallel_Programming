#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

long long compute_pi_points(long long tosses_per_process, int rank) {
    unsigned int seed = time(NULL) + rank;
    long long count = 0;

    for (long long i = 0; i < tosses_per_process; i++) {
        double x = 2.0 * ((double)rand_r(&seed) / RAND_MAX) - 1.0;
        double y = 2.0 * ((double)rand_r(&seed) / RAND_MAX) - 1.0;
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }
    return count;
}

int main(int argc, char **argv) {
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoll(argv[1]);
    int world_rank, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // ---

    long long tosses_per_process = tosses / world_size;
    long long local_count = compute_pi_points(tosses_per_process, world_rank);
    long long global_count = 0;

    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        pi_result = 4.0 * global_count / (double)tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}