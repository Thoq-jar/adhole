cmake_minimum_required(VERSION 3.20)
project(adhole C)
set(CMAKE_C_STANDARD 11)

include_directories(include)
file(GLOB SOURCES src/*.c)

add_executable(adhole ${SOURCES})
