/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

class jpegImage;
class shader;
class ScreenModel;

class ScreenView
{
public:
    ScreenView(int width, int height, int xpos, int ypos, const wchar_t* windowName);
    ~ScreenView();

    GLFWwindow* getWindow() const { return window; }
    bool isInitialized() const { return is_init; }
    bool isValid() const;
    
    // Rendering
    void render(ScreenModel& model);

    // Window queries
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void setWSize(int w, int h) { width = w; height = h; }
    void setWidth (int v) { width = v; }
    void setHeight(int v) { height = v; }

    // Title management
    void setWindowTitle(const std::wstring& title);

private:
    // Window state
    int width;
    int height;
    int xpos;
    int ypos;
    bool is_init;
    GLFWwindow* window;
    
    // Graphics resources
    std::unique_ptr<shader> sh;
    unsigned int VBO;
    unsigned int VAO;

    // Initialization
    void initOpenGL();
    void setupShader();
    void setupImage(ScreenModel& model);
    void updateVertices(const ScreenModel& model);
    void cleanup();
    void renderFrame(const ScreenModel& model);
    void no_image();
};