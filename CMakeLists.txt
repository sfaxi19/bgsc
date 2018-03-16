cmake_minimum_required(VERSION 3.8)
project(bgsc)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/psnr_ssim.hpp src/psnr_ssim.cpp src/bgsc.cpp src/bgsc.hpp)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(src)

add_executable(bgsc ${SOURCE_FILES})

target_link_libraries(bgsc ${OpenCV_LIBS})
