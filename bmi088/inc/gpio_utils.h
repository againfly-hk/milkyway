#ifndef GPIO_UTILS_H
#define GPIO_UTILS_H

#include <string>

bool export_gpio(int gpio);
bool set_gpio_direction(int gpio, const std::string& direction);
bool set_gpio_edge(int gpio, const std::string& edge);
int open_gpio_fd(int gpio);

#endif
