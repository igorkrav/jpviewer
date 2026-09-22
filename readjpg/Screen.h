/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <memory>

class ScreenModel;
class ScreenView;
class ScreenController;

class Screen
{
public:
    Screen(int width, int height, int xpos, int ypos, const wchar_t* name);
    ~Screen();

    bool isInitialized() const;
    bool isOk() const;

    // Main entry point - runs the application loop
    void run();

private:
    std::unique_ptr<ScreenModel> model;
    std::unique_ptr<ScreenView> view;
    std::unique_ptr<ScreenController> controller;

    // Loop management
    void mainLoop();
    void onFrame();
    bool shouldClose() const;
};
