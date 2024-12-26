#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoll(argv[1]);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // ---

    long long int tosses_per_process = tosses / world_size;
    unsigned int seed = time(NULL) + world_rank;
    long long int local_count = 0;

    for (long long int i = 0; i < tosses_per_process; i++)
    {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0)
        {
            local_count++;
        }
    }

    if (world_rank > 0)
    {
        // TODO: handle workers
        MPI_Send(&local_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: master
        long long int total_count = local_count;

        for (int i = 1; i < world_size; i++)
        {
            long long int received_count;
            MPI_Recv(&received_count, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += received_count;
        }

        pi_result = 4.0 * total_count / tosses;
    }

    if (world_rank == 0)
    {
        // TODO: process PI result

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
