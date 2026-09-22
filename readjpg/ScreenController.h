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

class ScreenController
{
public:
    ScreenController(ScreenModel& model, ScreenView& view);
    ~ScreenController();

    void setupCallbacks();

private:
    ScreenModel& model;
    ScreenView& view;

    // Input handlers
    void handleMouseButton(int button, int action, int mods);
    void handleKey(int key, int scancode, int action, int mods);
    void handleScroll(double xoffset, double yoffset);
    void handleFramebufferSize(int w, int h);

    // Helper methods for key handling
    void handleArrowKey(int key, int mods);
    //void handleZoomKey(int key);
    void fit2width();
    void fit2height();

    // GLFW callback wrappers
    static void mouseButtonWrapper(GLFWwindow* window, int button, int action, int mods);
    static void keyWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollWrapper(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeWrapper(GLFWwindow* window, int w, int h);
};