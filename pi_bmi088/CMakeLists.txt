cmake_minimum_required(VERSION 3.10)
project(bmi088_project)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

add_executable(bmi088_reader
    src/main.cpp
    src/bmi088.cpp
)

target_link_libraries(bmi088_reader
    pigpio
    pthread
)
