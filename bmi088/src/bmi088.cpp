#include "bmi088.h"
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

BMI088::BMI088(int cs_accel_gpio, int cs_gyro_gpio, const std::string& spi_dev)
    : cs_accel(cs_accel_gpio), cs_gyro(cs_gyro_gpio) {

    spi_fd = open(spi_dev.c_str(), O_RDWR);
    if (spi_fd < 0) {
        perror("SPI open failed");
    }

    uint8_t mode = SPI_MODE_3;
    uint32_t speed = 10000000;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    // Set CS GPIOs
    export_gpio(cs_accel);
    export_gpio(cs_gyro);
    set_gpio_direction(cs_accel, "out");
    set_gpio_direction(cs_gyro, "out");
    deselect(cs_accel);
    deselect(cs_gyro);
}

BMI088::~BMI088() {
    if (spi_fd >= 0) {
        close(spi_fd);
    }
}

void BMI088::select(int gpio) {
    std::ofstream out("/sys/class/gpio/gpio" + std::to_string(gpio) + "/value");
    out << "0";
}

void BMI088::deselect(int gpio) {
    std::ofstream out("/sys/class/gpio/gpio" + std::to_string(gpio) + "/value");
    out << "1";
}

bool BMI088::spi_transfer(uint8_t* tx, uint8_t* rx, size_t len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = static_cast<uint32_t>(len),
        .speed_hz = 10000000,
        .delay_usecs = 0,
        .bits_per_word = 8,
    };
    return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) >= 0;
}

bool BMI088::write_reg(int cs, uint8_t reg, uint8_t val) {
    uint8_t tx[] = { reg & 0x7F, val };
    uint8_t rx[2];
    select(cs);
    bool ok = spi_transfer(tx, rx, 2);
    deselect(cs);
    return ok;
}

bool BMI088::read_regs(int cs, uint8_t reg, uint8_t* buf, size_t len) {
    uint8_t tx[len + 1];
    uint8_t rx[len + 1];
    tx[0] = reg | 0x80;
    memset(tx + 1, 0, len);

    select(cs);
    bool ok = spi_transfer(tx, rx, len + 1);
    deselect(cs);
    if (ok) memcpy(buf, rx + 1, len);
    return ok;
}

bool BMI088::init() {
    uint8_t chip_id;

    // Read accel chip ID
    if (!read_regs(cs_accel, 0x00, &chip_id, 1) || chip_id != 0x1E) {
        std::cerr << "Accel not detected." << std::endl;
        return false;
    }

    // Read gyro chip ID
    if (!read_regs(cs_gyro, 0x00, &chip_id, 1) || chip_id != 0x0F) {
        std::cerr << "Gyro not detected." << std::endl;
        return false;
    }

    // Optional: init settings

    return true;
}

bool BMI088::read_accel(int16_t& ax, int16_t& ay, int16_t& az) {
    uint8_t buf[6];
    if (!read_regs(cs_accel, 0x12, buf, 6)) return false;
    ax = (buf[1] << 8) | buf[0];
    ay = (buf[3] << 8) | buf[2];
    az = (buf[5] << 8) | buf[4];
    return true;
}

bool BMI088::read_gyro(int16_t& gx, int16_t& gy, int16_t& gz) {
    uint8_t buf[6];
    if (!read_regs(cs_gyro, 0x02, buf, 6)) return false;
    gx = (buf[1] << 8) | buf[0];
    gy = (buf[3] << 8) | buf[2];
    gz = (buf[5] << 8) | buf[4];
    return true;
}
