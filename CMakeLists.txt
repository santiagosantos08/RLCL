cmake_minimum_required(VERSION 3.10)
project(myproject C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(raylib)
include_directories(raylib/src)

add_executable(myproject src/main.c)
target_link_libraries(myproject raylib)

# Include required Win32 libraries
if (WIN32)
    target_link_libraries(myproject winmm gdi32 opengl32)
endif()
