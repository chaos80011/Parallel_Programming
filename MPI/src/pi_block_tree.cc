#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

// 計算每個進程內的圓內點數
long long compute_local_pi(long long int tosses_per_process, int rank) {
    unsigned int seed = time(NULL) + rank; // 隨機種子
    long long local_count = 0;

    for (long long toss = 0; toss < tosses_per_process; toss++) {
        double x = 2.0 * (double)rand_r(&seed) / RAND_MAX - 1.0;
        double y = 2.0 * (double)rand_r(&seed) / RAND_MAX - 1.0;
        if (x * x + y * y <= 1.0) {
            local_count++;
        }
    }
    return local_count;
}

int main(int argc, char **argv) {
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoll(argv[1]); // 總點數
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // ---

    // 每個進程的點數計算
    long long tosses_per_process = tosses / world_size;
    long long local_count = compute_local_pi(tosses_per_process, world_rank);

    // 使用分層二叉樹歸約
    int step = 1; // 歸約的層級
    while (step < world_size) {
        if (world_rank % (2 * step) == 0) {
            // 接收來自相鄰節點的數據
            if (world_rank + step < world_size) {
                long long received_count;
                MPI_Recv(&received_count, 1, MPI_LONG_LONG, world_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_count += received_count; // 累加接收到的數據
            }
        } else {
            // 發送數據到上一層節點
            int destination = world_rank - step;
            MPI_Send(&local_count, 1, MPI_LONG_LONG, destination, 0, MPI_COMM_WORLD);
            break; // 完成數據傳輸後退出
        }
        step *= 2; // 進入下一層
    }

    if (world_rank == 0) {
        // 計算最終的 π 值
        pi_result = 4.0 * local_count / (double)tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
