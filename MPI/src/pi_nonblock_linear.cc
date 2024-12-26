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
    long long int tosses = atoll(argv[1]); // 總點數
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // 取得當前進程編號
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // 取得進程總數
    // ---

    // 每個進程的點數計算
    long long int tosses_per_process = tosses / world_size;
    unsigned int seed = time(NULL) + world_rank; // 確保每個進程的隨機數種子不同
    long long int local_count = 0;

    // 隨機生成點並計算落入圓內的點數
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
        // TODO: MPI workers
        // 非主進程將局部結果傳遞給主進程
        MPI_Send(&local_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Allocate arrays for requests and received counts
        MPI_Request *requests = (MPI_Request *)malloc((world_size - 1) * sizeof(MPI_Request));
        long long int *received_counts = (long long int *)malloc((world_size - 1) * sizeof(long long int));

        // 發起非阻塞接收操作
        for (int i = 1; i < world_size; i++)
        {
            MPI_Irecv(&received_counts[i - 1], 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, &requests[i - 1]);
        }

        // 主進程自己的局部結果
        long long int total_count = local_count;

        // 等待所有非阻塞接收完成
        MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);

        // 累加接收到的結果
        for (int i = 0; i < world_size - 1; i++)
        {
            total_count += received_counts[i];
        }

        // 計算 π
        pi_result = 4.0 * total_count / tosses;

        // 釋放資源
        free(requests);
        free(received_counts);
    }

    if (world_rank == 0)
    {
        // TODO: PI result

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
