cmake_minimum_required(VERSION 3.14)
project(banditutil C CXX)

# External
add_subdirectory(pdcurses)
add_subdirectory(stb_image)
add_subdirectory(stb_image_write)

# Lib
add_subdirectory(banditutil)

# Command-lines
add_executable(unpack_title
    src/main.cc
    )
target_link_libraries(unpack_title
    banditutil
    pdcurses
    stb_image
    stb_image_write
    )
