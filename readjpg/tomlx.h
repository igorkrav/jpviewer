/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <iostream>
#include <string>
#include <filesystem>

class tomlx
{
public:
    static std::filesystem::path getConfigFilePath(const char* appName);
};

