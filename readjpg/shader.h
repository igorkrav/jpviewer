/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
class shader
{
public:
    shader() : shaderProgram(-1) {}
    virtual ~shader();

    static const char** vertex() { return &vertexShaderSource; }
    static const char** fragment() { return &fragmentShaderSource; }

    bool compileShader();
    const unsigned int getShaderProgram() const { return shaderProgram; }

protected:
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    unsigned int shaderProgram;
};

