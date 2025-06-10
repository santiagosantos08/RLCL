cmake_minimum_required(VERSION 3.10)
project(mygame C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

# Optimize for small size
set(CMAKE_C_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -s")

# Raylib minimal modules
add_definitions(
    -DGRAPHICS_API_OPENGL_33
    -DSUPPORT_MODULE_CORE        # Window, input, timing
    -DSUPPORT_MODULE_DRAWING     # Drawing shapes (rectangles)
    -DSUPPORT_MODULE_RTEXT       # Basic font/text rendering
    -DSUPPORT_DEFAULT_FONT       # Needed to draw text
    # All others OFF by default
)

# Raylib path
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(RAYLIB_DIR "../raylib/raylib_win")
else()
    set(RAYLIB_DIR "/usr/local")
endif()

include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

add_executable(mygame src/main.c)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(mygame raylib winmm gdi32 opengl32)
else()
    target_link_libraries(mygame raylib m pthread dl GL X11 Xrandr Xinerama Xcursor Xi)
endif()
