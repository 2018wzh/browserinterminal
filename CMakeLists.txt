cmake_minimum_required(VERSION 3.10)
project(browserinterminal)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE sources "src/*.c")
add_executable(browserinterminal main.c ${sources})