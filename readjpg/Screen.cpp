/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Screen.h"
#include "ScreenModel.h"
#include "ScreenView.h"
#include "ScreenController.h"
#include "jpegImage.h"

Screen::Screen(int width, int height, int xpos, int ypos, const wchar_t* name)
{
    // Initialize MVC components in order
    model = std::make_unique<ScreenModel>(width, height, name);
    view = std::make_unique<ScreenView>(width, height, xpos, ypos, L"JPEG OpenGL Renderer");
    controller = std::make_unique<ScreenController>(*model, *view);
    
    // Setup input callbacks
    controller->setupCallbacks();
}

Screen::~Screen()
{
    // Cleanup in reverse order of initialization
    controller.reset();
    model->shutdown();
    model.reset();
    view.reset();
}

bool Screen::isInitialized() const
{
    if (!view || !view->isInitialized()) return false;
    if (!model || !model->getCurrentImage()) return false;
    return true;
}

bool Screen::isOk() const
{
    if (!view || !view->isValid()) return false;
    if (!model) return false;
    
    const auto* jpeg = model->getCurrentImage();
    return jpeg;//&& jpeg->is_texture();
}

void Screen::run()
{
    if (!isInitialized()) return;

    const auto* jpeg = model->getCurrentImage();
    if (!jpeg || !jpeg->is_loaded()) return;

    // Setup initial viewport
    model->updateViewport(view->getWidth(), view->getHeight(), 
                         jpeg->get_width(), jpeg->get_height());

    // Run main application loop
    mainLoop();
}

void Screen::mainLoop()
{
    while (!shouldClose()) {
        onFrame();
    }
}

void Screen::onFrame()
{
    // Render current frame
    if (isOk()) {
        view->render(*model);
    }
    else {
        // Fallback if not ready
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(view->getWindow());
    }

    // Poll input events (Controller will handle them)
    glfwPollEvents();
}

bool Screen::shouldClose() const
{
    return glfwWindowShouldClose(view->getWindow());
}
