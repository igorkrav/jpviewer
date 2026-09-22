/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include "tomlx.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

std::filesystem::path tomlx::getConfigFilePath(const char* appName)
{
    // check if local config file exists in current directory
    fs::path localConfig = fs::current_path() / "config.toml";
    if (fs::exists(localConfig)) {
        std::cout << "Using local config file: " << localConfig << std::endl;
        return localConfig;
    }
    fs::path configDir;

#if defined(_WIN32)
    // --- WINDOWS PATHING ---
    // Look for C:\Users\Username\AppData\Roaming
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        configDir = fs::path(appData) / appName;
    }
    else {
        configDir = fs::path("."); // Fallback to current directory
    }
#else
    // --- LINUX PATHING ---
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfig) {
        configDir = fs::path(xdgConfig) / appName;
    }
    else {
        const char* homeDir = std::getenv("HOME");
        if (homeDir) {
            configDir = fs::path(homeDir) / ".config" / appName;
        }
        else {
            configDir = fs::path(".");
        }
    }
#endif

    // Ensure the folder structure exists
    try {
        if (configDir != ".") {
            fs::create_directories(configDir);
        }
    }
    catch (...) {
        configDir = fs::path(".");
    }

    std::filesystem::path fpath = configDir / "config.toml";
    if (!fs::exists(fpath)) {
        std::cout << "Config file not found, creating default config at: " <<
            fpath << std::endl;
        std::ofstream configFile(fpath);

        if (configFile.is_open()) {
            configFile << "# Default configuration for " << appName << "\n";
            configFile << "[window]\n";
            configFile << "width = 1100\n";
            configFile << "height = 650\n";
            configFile << "xpos = 100\n";
            configFile << "ypos = 100\n";
            configFile.close();
        }
    }
    else
    {
        std::cout << "Using config file: " << fpath << std::endl;
    }

    return configDir / "config.toml";
}
