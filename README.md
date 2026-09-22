## Readjpeg image viewer.
Version 0.55

[![License: MIT](https://shields.io)](https://opensource.org)


A high-performance image processing app built with OpenGL and TurboJPEG.

## Third-Party Libraries & Dependencies

This project relies on the following open-source libraries. We are grateful to the maintainers of these projects for making their software available.

| Library | Purpose | License | Link |
| :--- | :--- | :--- | :--- |
| **OpenGL** | Core 2D/3D graphics rendering API | Varies (Driver-implemented) | [khronos.org](https://khronos.org) |
| **GLEW** (The OpenGL Extension Wrangler Library) | Querying and loading OpenGL extensions | [Modified BSD / MIT](https://github.com) | [glew.sourceforge.net](http://sourceforge.net) |
| **GLFW** | Window creation, context management, and input handling | [zlib/libpng](https://glfw.org) | [glfw.org](https://glfw.org) |
| **TurboJPEG** (libjpeg-turbo) | High-speed JPEG image compression and decompression | [BSD-3-Clause / IJG](https://github.com) | [libjpeg-turbo.org](https://libjpeg-turbo.org) |
| **libjxl** (JPEG XL) | Next-generation image coding system and compression | [BSD-3-Clause](https://github.com) | [jpegxl.info](https://jpegxl.info) |
| **toml++ v3.4.0** | Header-only TOML configuration file parser for C++17 | [MIT](https://github.com) | [github.com/marzer/tomlplusplus](https://github.com/marzer/tomlplusplus) |


### License Compliance Note
All trademarks and copyrights belong to their respective owners. Please refer to the specific license links provided in the table above for individual terms of redistribution and modifications.


## Linux build
C++ 17 Standard
For dependencies check doc/linux-setup.sh

To build:
mkdir build
cd build
cmake ..
make

## Windows build
C++ 20 Standard

To build use readjpg.sln

Dependencies:
Install GLEW, glfw, TurboJPEG and set the Include and Lib pathes in the project.

Check doc/windows.txt for instructions how to install JXL library
