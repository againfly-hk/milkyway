#include <pigpio.h>
#include <thread>
#include <atomic>
#include <iostream>

std::atomic<float> servo_angle[4];

int servoPins[4] = {17, 18, 22, 23};

void servoController() {
    gpioInitialise();
    while (true) {
        int pulseWidth = 1500;
        for (int pin : servoPins) {
            gpioServo(pin, pulseWidth);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    gpioTerminate();
}

int main() {
    // std::thread spiThread(spiReadLoop);
    std::thread servoThread(servoController);

    // spiThread.join();
    servoThread.join();
    return 0;
}
