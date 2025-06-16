#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <vector>
#include <thread>
#include <chrono>

// BNO080 SPI接口类（简化）
class BNO080_SPI {
public:
    BNO080_SPI(const char* device = "/dev/spidev0.0", uint32_t speed = 1000000)
        : fd(-1), spi_speed(speed) {
        strncpy(spi_device, device, sizeof(spi_device));
        spi_device[sizeof(spi_device)-1] = '\0';
    }

    ~BNO080_SPI() {
        if (fd >= 0) close(fd);
    }

    bool openDevice() {
        fd = open(spi_device, O_RDWR);
        if (fd < 0) {
            std::cerr << "Failed to open SPI device: " << spi_device << " Error: " << strerror(errno) << "\n";
            return false;
        }

        uint8_t mode = SPI_MODE_0;
        if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
            perror("Can't set SPI mode");
            return false;
        }

        uint8_t bits = 8;
        if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
            perror("Can't set bits per word");
            return false;
        }

        if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0) {
            perror("Can't set max speed hz");
            return false;
        }

        return true;
    }

    // SPI 全双工传输
    bool transfer(const uint8_t* tx_buf, uint8_t* rx_buf, size_t len) {
        struct spi_ioc_transfer tr;
        memset(&tr, 0, sizeof(tr));
        tr.tx_buf = reinterpret_cast<uint64_t>(tx_buf);
        tr.rx_buf = reinterpret_cast<uint64_t>(rx_buf);
        tr.len = len;
        tr.speed_hz = spi_speed;
        tr.bits_per_word = 8;
        tr.delay_usecs = 0;

        int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
            perror("SPI transfer failed");
            return false;
        }
        return true;
    }

    // 读取SHTP包头，返回包长度，通道号，序号
    bool readHeader(uint16_t& length, uint8_t& channel, uint8_t& seq) {
        uint8_t tx[4] = {0, 0, 0, 0};
        uint8_t rx[4] = {0};
        if (!transfer(tx, rx, 4)) return false;

        length = rx[0] | (rx[1] << 8);
        channel = rx[2];
        seq = rx[3];

        return true;
    }

    // 读取包体数据
    bool readPayload(uint8_t* buffer, size_t len) {
        std::vector<uint8_t> tx(len, 0);
        std::vector<uint8_t> rx(len, 0);

        if (!transfer(tx.data(), rx.data(), len)) return false;

        memcpy(buffer, rx.data(), len);
        return true;
    }

private:
    char spi_device[64];
    int fd;
    uint32_t spi_speed;
};


// 示例程序：解析SHTP数据包，打印内容
int main() {
    BNO080_SPI bno;
    if (!bno.openDevice()) {
        std::cerr << "Failed to open SPI device\n";
        return -1;
    }

    while (true) {
        uint16_t length = 0;
        uint8_t channel = 0;
        uint8_t seq = 0;

        // 读包头
        if (!bno.readHeader(length, channel, seq)) {
            std::cerr << "Failed to read header\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (length == 0) {
            // 没有数据，短暂等待
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // 包长度包含头4字节，减去头部得到数据长度
        uint16_t payload_len = length - 4;
        std::vector<uint8_t> payload(payload_len);

        if (!bno.readPayload(payload.data(), payload_len)) {
            std::cerr << "Failed to read payload\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // 简单打印包信息
        std::cout << "Packet from channel " << (int)channel << " seq " << (int)seq << " length " << length << " payload:";
        for (auto b : payload) {
            printf(" %02X", b);
        }
        std::cout << std::endl;

        // TODO: 根据 channel 和 payload 解析具体传感器数据
        // 例如：channel 2 通常是传感器报告，解析其格式

        // 适当延时，避免一直满循环
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}
