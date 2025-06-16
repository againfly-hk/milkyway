cmake_minimum_required(VERSION 3.10)
project(ServoController)

set(CMAKE_CXX_STANDARD 11)

add_executable(servo_controller main.cpp)
target_link_libraries(servo_controller
    pthread           # 用于 std::thread
    pigpio           # pigpio 主库
    rt               # 实时库
)
