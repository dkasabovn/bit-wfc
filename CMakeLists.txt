cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(WaveFunctionCollapse)

include_directories(include)

add_executable(main src/main.cpp src/wfc.cpp)

