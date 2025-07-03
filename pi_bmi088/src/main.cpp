#include "bmi088.h"
#include <iostream>

int main() {
    // try {
    //     BMI088 imu;
    // } catch (const std::exception& e) {
    //     std::cerr << "[ERROR] " << e.what() << std::endl;
    //     return 1;
    // }
    BMI088 imu;

    while(1) {
        imu.readAccel();
        imu.readGyro();
        imu.readTempture();

        // 打印数据log
        const auto& raw_data = imu.getRawData();
        const auto& real_data = imu.getRealData();
        std::cout << "Raw Data: ";
        std::cout << "Accel: (" << raw_data.accel_x << ", " << raw_data.accel_y << ", " << raw_data.accel_z << ") "
                  << "Gyro: (" << raw_data.gyro_x << ", " << raw_data.gyro_y << ", " << raw_data.gyro_z << ") "
                  << "Temp: " << raw_data.temperature << std::endl;
        usleep(10000); 
    }

    return 0;
}
