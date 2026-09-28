/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ScreenController.h"
#include "ScreenModel.h"
#include "ScreenView.h"
#include "jpegImage.h"

ScreenController::ScreenController(ScreenModel& model, ScreenView& view)
    : model(model), view(view)
{
}

ScreenController::~ScreenController()
{
}

void ScreenController::setupCallbacks()
{
    GLFWwindow* window = view.getWindow();
    if (!window) return;

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, ScreenController::keyWrapper);
    glfwSetMouseButtonCallback(window, ScreenController::mouseButtonWrapper);
    glfwSetScrollCallback(window, ScreenController::scrollWrapper);
    glfwSetFramebufferSizeCallback(window, ScreenController::framebufferSizeWrapper);
}

void ScreenController::handleMouseButton(int button, int action, int mods)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        if (action == GLFW_PRESS) {
            model.previousImage();
        }
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        if (action == GLFW_PRESS) {
            model.nextImage();
        }
        break;
    }
}

void ScreenController::handleKey(int key, int scancode, int action, int mods)
{
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            glfwSetWindowShouldClose(view.getWindow(), GLFW_TRUE);
        }
        break;

    case GLFW_KEY_LEFT:
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_UP:
    case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            handleArrowKey(key, mods);
        }
        break;

    case GLFW_KEY_0:
        if (action == GLFW_PRESS) {
            model.reset();
        }
        break;

    case GLFW_KEY_H:
        if (action == GLFW_PRESS) {
            fit2height();
        }
        break;
    case GLFW_KEY_W:
        if (action == GLFW_PRESS) {
            fit2width();
        }
        break;

    case GLFW_KEY_SPACE:
        if (action == GLFW_PRESS) {
            model.nextImage();
        }
        break;

    case GLFW_KEY_BACKSPACE:
        if (action == GLFW_PRESS) {
            model.previousImage();
        }
        break;

    case GLFW_KEY_MINUS:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (model.zoomOut()) {
                // View will be updated in the main loop
            }
        }
        break;

    case GLFW_KEY_EQUAL:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (model.zoomIn()) {
                // View will be updated in the main loop
            }
        }
        break;
    }
}

void ScreenController::handleArrowKey(int key, int mods)
{
    switch (key) {
    case GLFW_KEY_LEFT:
        if (mods == GLFW_MOD_CONTROL) {
            model.pan(model.getMaxLeft(), 0.f);
        }
        else {
            model.pan(0.01f, 0.f);
        }
        break;

    case GLFW_KEY_RIGHT:
        if (mods == GLFW_MOD_CONTROL) {
            model.pan(model.getMaxRight(), 0.f);
        }
        else {
            model.pan(-0.01f, 0.f);
        }
        break;

    case GLFW_KEY_UP:
        if (mods == GLFW_MOD_CONTROL) {
            model.setPositionY(model.getMaxTop());
        }
        else {
            float step = 0.1f;
            if (mods == GLFW_MOD_SHIFT) {
                step = 0.01f;
            }
            if (model.getPositionY() - step < model.getMaxTop()) {
                model.setPositionY(model.getMaxTop());
            }
            else
                model.pan(0.f, -step);
        }
        break;

    case GLFW_KEY_DOWN:
        if (mods == GLFW_MOD_CONTROL) {
            model.setPositionY(model.getMaxBottom());
        }
        else {
            float step = 0.1f;
            if (mods == GLFW_MOD_SHIFT) {
                step = 0.01f;
            }

            if (model.getPositionY() + step > model.getMaxBottom()) {
                model.setPositionY(model.getMaxBottom());
            }
            else
                model.pan(0.f, step);
        }
        break;
    }
}

void ScreenController::fit2width()
{
    model.fit2width();
}

void ScreenController::fit2height()
{
    model.fit2height();
}

void ScreenController::handleScroll(double xoffset, double yoffset)
{
    if (yoffset > 0) {
        model.zoomIn();
    }
    else if (yoffset < 0) {
        model.zoomOut();
    }
}

void ScreenController::handleFramebufferSize(int w, int h)
{
    const auto* jpeg = model.getCurrentImage();
    if (jpeg) {
        view.setWSize(w, h);
        model.updateViewport(w, h, jpeg->get_width(), jpeg->get_height());
    }
}

// Static wrapper methods
void ScreenController::mouseButtonWrapper(GLFWwindow* window, int button, int action, int mods)
{
    ScreenController* pThis = static_cast<ScreenController*>(glfwGetWindowUserPointer(window));
    if (pThis) {
        pThis->handleMouseButton(button, action, mods);
    }
}

void ScreenController::keyWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ScreenController* pThis = static_cast<ScreenController*>(glfwGetWindowUserPointer(window));
    if (pThis) {
        pThis->handleKey(key, scancode, action, mods);
    }
}

void ScreenController::scrollWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
    ScreenController* pThis = static_cast<ScreenController*>(glfwGetWindowUserPointer(window));
    if (pThis) {
        pThis->handleScroll(xoffset, yoffset);
    }
}

void ScreenController::framebufferSizeWrapper(GLFWwindow* window, int w, int h)
{
    ScreenController* pThis = static_cast<ScreenController*>(glfwGetWindowUserPointer(window));
    if (pThis) {
        pThis->handleFramebufferSize(w, h);
    }
}