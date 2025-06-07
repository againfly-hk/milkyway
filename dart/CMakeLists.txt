cmake_minimum_required(VERSION 3.10)
project(CameraDisplay)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(camera_display main.cpp)
target_link_libraries(camera_display ${OpenCV_LIBS})