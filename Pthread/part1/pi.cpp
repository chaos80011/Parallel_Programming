#include <iostream>
#include <pthread.h>
#include <cstdlib>  // rand, RAND_MAX
#include <ctime>    // time
#include <cmath>    // sqrt
#include <iomanip>  // fixed, setprecision

// 全域變數來儲存命中圓內的總次數
long long global_number_in_circle = 0;
long long number_of_tosses_per_thread = 0;
pthread_mutex_t mutex;

// 線程的工作函數
void* thread_toss(void* arg) {
    long long local_number_in_circle = 0;
    unsigned int seed = time(NULL) ^ pthread_self();  // 使用不同的種子來生成隨機數
    
    for (long long toss = 0; toss < number_of_tosses_per_thread; toss++) {
        double x = (static_cast<double>(rand_r(&seed)) / RAND_MAX) * 2.0 - 1.0;
        double y = (static_cast<double>(rand_r(&seed)) / RAND_MAX) * 2.0 - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1.0) {
            local_number_in_circle++;
        }
    }

    // 使用互斥鎖來保護全域變數的更新
    pthread_mutex_lock(&mutex);
    global_number_in_circle += local_number_in_circle;
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads> <number_of_tosses>" << std::endl;
        return -1;
    }

    int number_of_threads = std::atoi(argv[1]);
    long long total_number_of_tosses = std::atoll(argv[2]);

    if (number_of_threads <= 0 || total_number_of_tosses <= 0) {
        std::cerr << "Error: Both number of threads and tosses must be positive." << std::endl;
        return -1;
    }

    pthread_t threads[number_of_threads];
    number_of_tosses_per_thread = total_number_of_tosses / number_of_threads;

    // 初始化互斥鎖
    pthread_mutex_init(&mutex, NULL);

    // 創建線程
    for (int i = 0; i < number_of_threads; i++) {
        pthread_create(&threads[i], NULL, thread_toss, NULL);
    }

    // 等待所有線程完成
    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // 計算 PI 的估算值
    double pi_estimate = 4.0 * global_number_in_circle / static_cast<double>(total_number_of_tosses);

    // 輸出結果，設定為小數點後三位
    std::cout << std::fixed << std::setprecision(3);
    std::cout << pi_estimate << std::endl;

    // 清理互斥鎖
    pthread_mutex_destroy(&mutex);

    return 0;
}
