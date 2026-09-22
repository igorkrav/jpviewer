/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <filesystem>
#include <iostream>

#include "shader.h"
#include "jpegImage.h"
#include "Screen.h"

#include "toml.hpp"
#include "tomlx.h"
#include "strconv.h"

int read_config();

// default window size
constexpr int WWIDTH = 1100;
constexpr int WHEIGHT = 650;

int winWidth (WWIDTH);
int winHeight (WHEIGHT);

int xpos = 100;
int ypos = 100;

int main(int argc, char* argv[])
{
    read_config();

    if (argc < 2) {
        std::cout<<"Usage:\n"
            << argv[0]
            << " <[image.jpg|image.jxl]>\n"
            << std::endl;
        return -1;
    }

    std::unique_ptr<Screen> screen =
        std::make_unique<Screen>(winWidth, winHeight,
                                 xpos, ypos, string_to_wstring(argv[1]).c_str());

    screen->run();

    return 0;
}


int read_config()
{
    try {
        std::filesystem::path path = tomlx::getConfigFilePath("rjp");
        // Load and parse the configuration file
        auto config = toml::parse_file(path.c_str());

        // Read specific values (with fallbacks if they don't exist)
        winWidth = config["window"]["width"].value_or(winWidth);
        winHeight = config["window"]["height"].value_or(winHeight);
        xpos = config["window"]["xpos"].value_or(xpos);
        ypos = config["window"]["ypos"].value_or(ypos);
    }
    catch (const toml::parse_error& err) {
        (void)err; // Suppress unused variable warning
        return 1;
    }
    return 0;
}
