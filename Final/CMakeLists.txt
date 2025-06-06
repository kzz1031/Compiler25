# CMake最低版本号要求
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# 项目信息
project(FDMJ2025 VERSION 0.1)

# 添加包含路径
include_directories(include/ir)
include_directories(include/quad)
include_directories(include/quadflow)
include_directories(include/rpi)
include_directories(include/util)
include_directories(vendor/tinyxml2)

# 递归构建
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(vendor)