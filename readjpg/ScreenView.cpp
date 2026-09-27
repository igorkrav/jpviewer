/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <iostream>
#include "ScreenView.h"
#include "jpegImage.h"
#include "shader.h"
#include "Texture.h"
#include "ScreenModel.h"
#include "strconv.h"

ScreenView::ScreenView(int width, int height, int xpos, int ypos, const wchar_t* windowName)
    : width(width)
    , height(height)
    , xpos(xpos)
    , ypos(ypos)
    , is_init(false)
    , window(nullptr)
    , sh(nullptr)
    , VBO(static_cast<unsigned int>(-1))
    , VAO(static_cast<unsigned int>(-1))
{
    initOpenGL();
}

ScreenView::~ScreenView()
{
    cleanup();
}

bool ScreenView::isValid() const
{
    if (sh == nullptr || !sh->getShaderProgram()) return false;
    return is_init;
}

void ScreenView::initOpenGL()
{
    is_init = false;

    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "JPEG OpenGL Renderer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        return;
    }

    glfwSetWindowPos(window, xpos, ypos);
    setupShader();

    is_init = true;
}

void ScreenView::setupShader()
{
    sh = std::make_unique<shader>();
    sh->compileShader();
}

void ScreenView::setupImage(ScreenModel& model)
{
    const jpegImage* jpeg = model.getCurrentImage();
    if (!jpeg) return;

    if (jpeg->is_loaded() && !jpeg->is_texture()) {
        // Note: Casting away const for load_texture if needed
        const_cast<jpegImage*>(jpeg)->load_texture();
    }

    if (!model.isImageReady())
    {
        auto name = jpeg->get_filename_only();
        wchar_t title_buffer[256];
        swprintf(title_buffer, 256, L"%d/%d %ls",
            model.getCurrentImageIndex() + 1, model.getImageCount(),
            name.c_str());

        setWindowTitle(title_buffer);
        model.setImageReady(true);
    }
}

void ScreenView::updateVertices(const ScreenModel& model)
{
    glViewport(0, 0, width, height);

    const jpegImage* jpeg = model.getCurrentImage();
    if (!jpeg) return;

    float scalex = model.getScale();
    float scaley = model.getScale();

    auto windowAspect = model.getWindowAspectRatio();
    auto imageAspect = model.getAspectRatio();
    if (windowAspect > imageAspect) {
        // Window is wider than the image -> Pillarbox
        scalex = model.getScale() / windowAspect * imageAspect;
    }
    else {
        scaley = model.getScale() * windowAspect / imageAspect;
    }

    float vertices[] = {
        -scalex + model.getPositionX(),  scaley + model.getPositionY(),  0.0f, 1.0f,
        -scalex + model.getPositionX(), -scaley + model.getPositionY(),  0.0f, 0.0f,
         scalex + model.getPositionX(), -scaley + model.getPositionY(),  1.0f, 0.0f,

        -scalex + model.getPositionX(),  scaley + model.getPositionY(),  0.0f, 1.0f,
         scalex + model.getPositionX(), -scaley + model.getPositionY(),  1.0f, 0.0f,
         scalex + model.getPositionX(),  scaley + model.getPositionY(),  1.0f, 1.0f
    };

    if (VAO == static_cast<unsigned int>(-1)) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void ScreenView::render(ScreenModel& model)
{
    setupImage(model);

    const jpegImage* jpeg = model.getCurrentImage();
    if (!jpeg) return;

    if (jpeg->is_texture()) {
        updateVertices(model);
    }
    if (!isValid()) return;

    renderFrame(model);
}

void ScreenView::renderFrame(const ScreenModel& model)
{
    const jpegImage* jpeg = model.getCurrentImage();
    if (!jpeg) return;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (jpeg->is_loaded() && !jpeg->is_texture()) {
        const_cast<jpegImage*>(jpeg)->load_texture();
    }

    if (isValid() && jpeg->is_texture())
    {
        glUseProgram(sh->getShaderProgram());
        glBindTexture(GL_TEXTURE_2D, jpeg->get_texture()->get_texture());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(0);
    }
    else {
        no_image();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void ScreenView::no_image()
{
    // add a simple placeholder rendering when no image is available
}

void ScreenView::setWindowTitle(const std::wstring& title)
{
    if (window) {
        glfwSetWindowTitle(window, wstring_to_string(title).c_str());
    }
}

void ScreenView::cleanup()
{
    if (VAO != static_cast<unsigned int>(-1)) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != static_cast<unsigned int>(-1)) {
        glDeleteBuffers(1, &VBO);
    }

    if (sh) sh.reset();

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}