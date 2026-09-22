cmake_minimum_required(VERSION 3.15)
project(RJP_Project VERSION 1.0 LANGUAGES CXX)

# Enforce C++17 standard (-std=c++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Find System Dependencies
# -----------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

# 1. Core Graphics Libraries (-lGL, -lGLEW, -lglfw)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED) # System package might use lowercase 'glfw3'

# 2. Imaging Libraries (-lturbojpeg, -ljxl, -ljxl_threads)
pkg_check_modules(TURBOJPEG REQUIRED IMPORTED_TARGET libturbojpeg)
pkg_check_modules(JXL REQUIRED IMPORTED_TARGET libjxl)
pkg_check_modules(JXL_THREADS REQUIRED IMPORTED_TARGET libjxl_threads)

# -----------------------------------------------------------------------------
# Define Executable and Source Files
# -----------------------------------------------------------------------------
# Maps your input source files to the output binary target named 'rjp' (-o rjp)
add_executable(rjp
    readjpg/tomlx.cpp
    readjpg/jpegImage.cpp
    readjpg/file_list.cpp
    readjpg/readjpg.cpp
    readjpg/Screen.cpp
    readjpg/shader.cpp
    readjpg/Texture.cpp
    readjpg/strconv.cpp
    readjpg/ScreenController.cpp
    readjpg/ScreenModel.cpp
    readjpg/ScreenView.cpp
)

# -----------------------------------------------------------------------------
# Link Libraries to the Target
# -----------------------------------------------------------------------------
target_link_libraries(rjp
    PRIVATE
        # Graphics
        OpenGL::GL
        GLEW::GLEW
        glfw
        # Imaging
        PkgConfig::TURBOJPEG
        PkgConfig::JXL
        PkgConfig::JXL_THREADS
)
