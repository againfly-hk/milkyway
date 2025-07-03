#include "gpio_utils.h"
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

bool export_gpio(int gpio) {
    std::ofstream export_file("/sys/class/gpio/export");
    if (!export_file) return false;
    export_file << gpio;
    return true;
}

bool set_gpio_direction(int gpio, const std::string& direction) {
    std::string path = "/sys/class/gpio/gpio" + std::to_string(gpio) + "/direction";
    std::ofstream dir_file(path);
    if (!dir_file) return false;
    dir_file << direction;
    return true;
}

bool set_gpio_edge(int gpio, const std::string& edge) {
    std::string path = "/sys/class/gpio/gpio" + std::to_string(gpio) + "/edge";
    std::ofstream edge_file(path);
    if (!edge_file) return false;
    edge_file << edge;
    return true;
}

int open_gpio_fd(int gpio) {
    std::string path = "/sys/class/gpio/gpio" + std::to_string(gpio) + "/value";
    return open(path.c_str(), O_RDONLY | O_NONBLOCK);
}
