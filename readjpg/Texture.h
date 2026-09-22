/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
class Texture
{
public:
    Texture(int width, int height, unsigned char* data);
    virtual ~Texture();

    void update(int width, int height, unsigned char* data);
    bool is_valid() const { return texture != -1; }
    unsigned int get_texture() const { return texture; }

protected:
    unsigned int texture;
    int width;
    int height;
};

