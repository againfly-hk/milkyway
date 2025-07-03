#ifndef BMI088_H
#define BMI088_H

#include <string>

class BMI088 {
public:
    BMI088(int cs_accel_gpio, int cs_gyro_gpio, const std::string& spi_dev);
    ~BMI088();

    bool init();
    bool read_accel(int16_t& ax, int16_t& ay, int16_t& az);
    bool read_gyro(int16_t& gx, int16_t& gy, int16_t& gz);

private:
    int spi_fd;
    int cs_accel;
    int cs_gyro;

    void select(int gpio);
    void deselect(int gpio);

    bool spi_transfer(uint8_t* tx, uint8_t* rx, size_t len);
    bool write_reg(int cs, uint8_t reg, uint8_t val);
    bool read_regs(int cs, uint8_t reg, uint8_t* buf, size_t len);
};

#endif
