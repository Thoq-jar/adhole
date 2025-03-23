cmake_minimum_required(VERSION 3.20)
project(adhole C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(adhole
        src/main.c
        src/dns_server.c
        src/dns_string.c
        src/blocklist.c
        src/config.c
        src/logger.c
)
