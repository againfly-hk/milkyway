#include "bmi088.h"
#include "gpio_utils.h"
#include <iostream>
#include <thread>
#include <chrono>

#define SPI_DEV "/dev/spidev0.0"
#define CS_ACCEL 8  // IO8
#define CS_GYRO  7  // IO7

int main() {
    BMI088 imu(CS_ACCEL, CS_GYRO, SPI_DEV);
    if (!imu.init()) {
        std::cerr << "BMI088 init failed!" << std::endl;
        return 1;
    }

    std::cout << "Polling sensor data..." << std::endl;

    while (true) {
        int16_t ax, ay, az, gx, gy, gz;
        if (imu.read_accel(ax, ay, az) && imu.read_gyro(gx, gy, gz)) {
            std::cout << "ACCEL: " << ax << "," << ay << "," << az
                      << " | GYRO: " << gx << "," << gy << "," << gz << std::endl;
        } else {
            std::cerr << "Failed to read sensor" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100Hz
    }

    return 0;
}
