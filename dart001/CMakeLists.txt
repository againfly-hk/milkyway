cmake_minimum_required(VERSION 3.10)
project(dart001)

find_package(raspicam REQUIRED)
find_package(OpenCV)

IF(OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")
add_executable(dart001 dart001.cpp)  
target_link_libraries(dart001 ${raspicam_CV_LIBS})
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()