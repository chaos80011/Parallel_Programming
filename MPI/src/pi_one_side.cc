#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

// 蒙特卡羅方法估算圓周率
double estimate_pi(long long int tosses) {
    long long int count = 0;
    for (long long int i = 0; i < tosses; i++) {
        double x = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }
    return (4.0 * count) / tosses;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result = 0.0;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // 創建 MPI Window 用來儲存 pi_result
    MPI_Win win;
    MPI_Win_create(&pi_result, sizeof(double), sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    if (world_rank == 0) {
        // 主進程負責接收所有工作進程的結果
        double local_pi = 0.0;
        long long int tosses_per_process = tosses / world_size;

        // 工作進程開始計算
        for (int i = 1; i < world_size; i++) {
            // 等待並讀取工作進程的計算結果
            MPI_Win_lock(MPI_LOCK_SHARED, i, 0, win);
            MPI_Get(&local_pi, 1, MPI_DOUBLE, i, 0, 1, MPI_DOUBLE, win);
            MPI_Win_unlock(i, win);
            // 累加每個工作進程的結果
            pi_result += local_pi;
        }

        // 計算主進程的部分
        pi_result += estimate_pi(tosses / world_size);

        // 最後印出結果
        double end_time = MPI_Wtime();
        printf("Estimated Pi: %lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
    } else {
        // 工作進程計算 Pi 值
        double local_pi = estimate_pi(tosses / world_size);
        
        // 使用 MPI_Put 將結果傳送給主進程
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&local_pi, 1, MPI_DOUBLE, 0, 0, 1, MPI_DOUBLE, win);
        MPI_Win_unlock(0, win);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}
