cmake_minimum_required(VERSION 3.16)

# project(lodepng)

add_library(lodepng STATIC src/lodepng.cpp)

target_include_directories(lodepng INTERFACE include)

target_include_directories(lodepng PRIVATE include)