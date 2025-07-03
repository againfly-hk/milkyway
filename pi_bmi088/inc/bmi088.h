#ifndef BMI088_H
#define BMI088_H

#include <cstdint>

typedef struct {
    int16_t status;
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t temperature;
} bmi088_raw_data_t;

typedef struct {
    int16_t status;
    double accel_x, accel_y, accel_z;
    double gyro_x, gyro_y, gyro_z;
    float temperature;
    float time;
} bmi088_real_data_t;

class BMI088 {
public:
    BMI088();
    ~BMI088();

    uint8_t readAccelRegister(uint8_t reg);
    uint8_t writeAccelRegister(uint8_t reg, uint8_t cmd);
    uint8_t readGyroRegister(uint8_t reg);
    uint8_t writeGyroRegister(uint8_t reg, uint8_t cmd);

    uint8_t readAccelMultiRegister(uint8_t reg, uint8_t *bufp, uint8_t len);
    uint8_t readGyroMultiRegister(uint8_t reg, uint8_t *bufp, uint8_t len);

    uint8_t readAccel(void);
    uint8_t readGyro(void);
    uint8_t readTempture(void);

    uint8_t accelInit(void);
    uint8_t gyroInit(void);
    uint8_t accelSelfTest(void);
    uint8_t gyroSelfTest(void);

    const bmi088_raw_data_t& getRawData() const { return raw_data; }
    const bmi088_real_data_t& getRealData() const { return real_data; }

private:
    int spiHandle;
    int csGyro;
    int csAccel;

    bmi088_raw_data_t raw_data;
    bmi088_real_data_t real_data;

    uint8_t readRegister(int csPin, uint8_t reg);
    uint8_t writeRegister(int csPin, uint8_t reg, uint8_t cmd);
    uint8_t readMultiRegister(int csPin, uint8_t reg, uint8_t *bufp, uint8_t len);

    void bmi088SleepMs(unsigned int ms);
    void bmi088SleepUs(unsigned int us);
};

#endif
