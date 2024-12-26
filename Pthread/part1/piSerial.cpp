#include <iostream>
#include <cstdlib> 
#include <cmath> 
#include <ctime>
#include <iomanip>

int main() {
    long long number_of_tosses = 1000000000; 
    long long number_in_circle = 0;

    std::srand(static_cast<unsigned>(std::time(0)));

    for (long long toss = 0; toss < number_of_tosses; toss++) {
        double x = (static_cast<double>(std::rand()) / RAND_MAX) * 2.0 - 1.0;
        double y = (static_cast<double>(std::rand()) / RAND_MAX) * 2.0 - 1.0;
        // std::cout << x << y << std::endl;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1.0) {
            number_in_circle++;
        }
    }

    double pi_estimate = 4.0 * number_in_circle / static_cast<double>(number_of_tosses);

    // 輸出結果
    std::cout << std::fixed << std::setprecision(4) << pi_estimate << std::endl;

    return 0;
}
