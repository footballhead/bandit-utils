cmake_minimum_required(VERSION 3.14)
project(banditutil C CXX)

# Lib
add_subdirectory(banditutil)

# Command-lines
add_executable(unpack_title
    src/main.cc
    )
target_link_libraries(unpack_title
    banditutil
    )
