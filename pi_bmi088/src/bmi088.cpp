#include "bmi088.h"
#include <pigpio.h>
#include <stdexcept>
#include <unistd.h>

BMI088::BMI088() {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("pigpio initialization failed");
    }

    // 配置 CS 引脚
    gpioSetMode(csGyro, PI_OUTPUT);
    gpioSetMode(csAccel, PI_OUTPUT);
    gpioWrite(csGyro, 1);   // 拉高，非选中
    gpioWrite(csAccel, 1);  // 拉高，非选中

    // 打开 SPI0, 10 MHz, mode 0
    spiHandle = spiOpen(0, 10000000, 0);
    if (spiHandle < 0) {
        throw std::runtime_error("spiOpen failed");
    }

    // 等待 BMI088 稳定
    usleep(10000);
}

BMI088::~BMI088() {
    spiClose(spiHandle);
    gpioTerminate();
}

uint8_t BMI088::readRegister(int csPin, uint8_t reg) {
    char tx[2] = { static_cast<char>(reg | 0x80), 0x00 };
    char rx[2] = {0};

    gpioWrite(csPin, 0); // 片选拉低
    spiXfer(spiHandle, tx, rx, 2);
    gpioWrite(csPin, 1); // 片选拉高

    return static_cast<uint8_t>(rx[1]);
}

uint8_t BMI088::readAccelRegister(uint8_t reg) {
    return readRegister(csAccel, reg);
}

uint8_t BMI088::readGyroRegister(uint8_t reg) {
    return readRegister(csGyro, reg);
}
