#ifndef BMI088_H
#define BMI088_H

#include <cstdint>

class BMI088 {
public:
    BMI088();
    ~BMI088();

    uint8_t readAccelRegister(uint8_t reg);
    uint8_t readGyroRegister(uint8_t reg);

private:
    int spiHandle;
    const int csGyro = 7;   // GPIO7 (BCM)
    const int csAccel = 8;  // GPIO8 (BCM)

    uint8_t readRegister(int csPin, uint8_t reg);
};

#endif
