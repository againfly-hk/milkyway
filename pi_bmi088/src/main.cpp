#include "bmi088.h"
#include <iostream>

int main() {
    try {
        BMI088 imu;

        uint8_t accel_id = imu.readAccelRegister(0x00);  // 加速度计芯片 ID
        uint8_t gyro_id = imu.readGyroRegister(0x00);    // 陀螺仪芯片 ID

        std::cout << "Accel Chip ID: 0x" << std::hex << (int)accel_id << std::endl;
        std::cout << "Gyro Chip ID:  0x" << std::hex << (int)gyro_id << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
