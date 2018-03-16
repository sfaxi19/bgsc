cmake_minimum_required(VERSION 3.8)
project(background_subtracted)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp psnr_ssim.hpp psnr_ssim.cpp bgsc.cpp bgsc.hpp)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(background_subtracted ${SOURCE_FILES})

target_link_libraries(background_subtracted ${OpenCV_LIBS})
