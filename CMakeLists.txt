cmake_minimum_required(VERSION 3.10)
project(mygame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Raylib path
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(RAYLIB_DIR "../raylib/raylib_win")
else()
    set(RAYLIB_DIR "/usr/local")
endif()

include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

add_executable(mygame src/main.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(mygame raylib winmm gdi32 opengl32)
else()
    target_link_libraries(mygame raylib m pthread dl GL X11 Xrandr Xinerama Xcursor Xi)
endif()
