/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

class Texture;

class jpegImage
{
public:
    jpegImage(const wchar_t* filename, bool delay_load = false);
    virtual ~jpegImage();

    void load_texture();
    const std::unique_ptr<Texture>& get_texture() const;

    int is_ready2load() const {
        return
            (get_texture() == nullptr) &&  // texture not loaded yet
            (imgBuf == nullptr) &&         // image buffer not loaded yet
            !filename.empty();             // filename is valid
    }
    int is_loaded() const { return imgBuf != nullptr; }  // image buffer is loaded
    int is_texture() const { return tx != nullptr; }     // texture is loaded
    int get_width() const { return width; }
    int get_height() const { return height; }
    int get_subsamp() const { return subsamp; }
    unsigned char* get_buffer() const { return imgBuf; }
    const std::wstring& get_filename() const { return filename; }
    const std::wstring get_filename_only() const;

    void free_buffer();
    void detect_and_load();

protected:
    int decode_jxl();

    int load_jpeg();
    void load_jxl();

    // read file into jpegBuf and set jpegSize
    int  load_file();

protected:
    std::unique_ptr<Texture> tx;
    std::vector<unsigned char> imageData;
    unsigned char* imgBuf;

    long jpegSize;
    unsigned char* jpegBuf;
    std::wstring filename;
    int width;
    int height;
    int subsamp;
    int channels;
};

