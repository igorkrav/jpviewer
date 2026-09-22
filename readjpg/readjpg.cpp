/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "Screen.h"

#include "toml.hpp"
#include "tomlx.h"
#include "strconv.h"

int read_config();
void help(const char *name);

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

    help(argv[0]);
    if (argc < 2) {

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

void help(const char *name)
{
    std::cout << "OGL viewer v0.55\n"
        << std::endl;
    std::cout << "Usage:\n"
        << name
        << " <[image.jpg|image.jxl]>\n"
        << std::endl;

    std::cout << "Viewer controls:" << std::endl;
    std::cout << "Keyboard" << std::endl;
    std::cout << "Up, Down, Left, Right - scroll image"<< std::endl;
    std::cout << "Shift-Up, Shift-Down, Left, Right - scroll image smoothly"<< std::endl;
    std::cout << "Ctrl-Up    - scroll to image top"<< std::endl;
    std::cout << "Ctrl-Down  - scroll to image bottom"<< std::endl;
    std::cout << "Spacebar   - show next image"<< std::endl;
    std::cout << "Backspace  - show prev image"<< std::endl;
    std::cout << "W          - fit image to window width"<< std::endl;
    std::cout << "H          - fit image to window height"<< std::endl;
    std::cout << "0          - reload image with default setting"<< std::endl;
    std::cout << "=/+        - zoom in" << std::endl;
    std::cout << "-          - zoom out" << std::endl;
    std::cout << "Esc        - exit" << std::endl << std::endl;
    std::cout << "Mouse" << std::endl;
    std::cout << "Right      - show next image" << std::endl;
    std::cout << "Left       - show prev image" << std::endl;
    std::cout << "Scroll     - zoom" << std::endl;
}
