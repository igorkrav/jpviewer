/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include <turbojpeg.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <filesystem>

#include <jxl/decode.h>
#include <jxl/codestream_header.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner_cxx.h>

#include "jpegImage.h"
#include "Texture.h"

jpegImage::jpegImage(const wchar_t* _filename, bool delay_load)
    : imgBuf(nullptr), width(0), height(0), subsamp(0) , filename(_filename)
    , jpegBuf(nullptr), jpegSize(0)
    , tx(nullptr)
    , channels(0)
{
    // std::cout << "Loading image: " << std::filesystem::path(filename).string() << " (" << width << "x" << height << ")" << std::endl;
    if (!delay_load) {
        detect_and_load();
    }
}

jpegImage::~jpegImage()
{
    if (imgBuf) {
        free(imgBuf);
        imgBuf = nullptr;
    }
}

const std::unique_ptr<Texture>& jpegImage::get_texture() const { return tx; }

void jpegImage::load_texture()
{
    if (tx == nullptr) {
        tx = std::make_unique<Texture>(get_width(), get_height(), get_buffer());
        free_buffer();
    }
}

void jpegImage::detect_and_load()
{
    // Get file extension
    std::filesystem::path filePath( filename);
    std::wstring ext = filePath.extension().wstring();

    if (ext.empty()) {
        //std::cerr << "Error: File has no extension: " << filePath.string() << std::endl;
        return;
    }

    // Convert to lowercase
    for (wchar_t& c : ext) c = tolower(c);

    if (ext == L".jxl")
    {
        load_jxl();
    }
    else if (ext == L".jpg" || ext == L".jpeg")
    {
        load_jpeg();
    }
    //std::cout << "Loaded image: " << std::filesystem::path(filename).string() << " (" << width << "x" << height << ")" << std::endl;
}

int jpegImage::load_file()
{
    std::filesystem::path filePath(filename);
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        return 0;
    }

    // Get the current position (which is the end of the file, yielding the size)
    jpegSize = static_cast<int> (file.tellg());

    // Seek back to the beginning to start reading
    file.seekg(0, std::ios::beg);

    // Allocate a vector buffer of the exact required size
    jpegBuf = (unsigned char*)malloc(jpegSize);

    // Read the contents as a continuous block of raw bytes
    if (!file.read(reinterpret_cast<char*>(jpegBuf), jpegSize)) {
        free(jpegBuf);
        jpegBuf = nullptr;
        jpegSize = 0;

        return 0;
    }

    return 1;
}

int jpegImage::load_jpeg()
{
    // DECODE JPEG USING TURBOJPEG
    load_file();

    if (jpegSize <= 0 || jpegBuf == nullptr) {
        free(jpegBuf);
        return 0;
    }

    tjhandle handle = tjInitDecompress();

    tjDecompressHeader2(handle, jpegBuf, jpegSize, &width, &height, &subsamp);

    // Use RGBA pixel format for standard OpenGL texture generation
    imgBuf = (unsigned char*)malloc(width * height * 4);
    tjDecompress2(handle, jpegBuf, jpegSize, imgBuf, width, 0, height, TJPF_RGBA, 0);
    tjDestroy(handle); free(jpegBuf);

    return 1;
}

const std::wstring jpegImage::get_filename_only() const
{
    std::filesystem::path filePath = filename;
    return filePath.filename().wstring();
}

void jpegImage::free_buffer()
{
    if (imgBuf) {
        free(imgBuf);
        imgBuf = nullptr;
    }
}

void jpegImage::load_jxl()
{

    load_file();

    if (jpegSize <= 0 || jpegBuf == nullptr) {
        free(jpegBuf);
        return;
    }

    decode_jxl();
}

int jpegImage::decode_jxl()
{

    // 1. Create the decoder instance
    auto dec = JxlDecoderMake(nullptr);
    if (!dec) {
        //std::cerr << "JxlDecoderMake failed" << std::endl;
        return 1;
    }

    // 2. Subscribe to the events we care about (Basic Info & Pixel Full Frames)
    if (JXL_DEC_SUCCESS != JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE)) {
        //std::cerr << "JxlDecoderSubscribeEvents failed" << std::endl;
        return 1;
    }

    // 3. (Optional) Set up multi-threading for faster decoding
    auto runner = JxlResizableParallelRunnerMake(nullptr);
    if (JXL_DEC_SUCCESS != JxlDecoderSetParallelRunner(dec.get(), JxlResizableParallelRunner, runner.get())) {
        //std::cerr << "JxlDecoderSetParallelRunner failed" << std::endl;
        return 1;
    }

    // Set the input data buffer
    JxlDecoderSetInput(dec.get(), jpegBuf, jpegSize);
    JxlDecoderCloseInput(dec.get());

    // 4. Configure output format (RGBA 8-bit per channel)
    JxlPixelFormat format = {
        .num_channels = 4,
        .data_type = JXL_TYPE_UINT8,
        .endianness = JXL_NATIVE_ENDIAN,
        .align = 0
    };

    JxlBasicInfo info{ 0 };

    // 5. Run the decoding state machine loop
    while (true) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());

        if (status == JXL_DEC_ERROR) {
            // std::cerr << "Decoder error occurred" << std::endl;
            return 1;
        }
        else if (status == JXL_DEC_NEED_MORE_INPUT) {
            // std::cerr << "Error: Unexpected end of input file" << std::endl;
            return 1;
        }
        else if (status == JXL_DEC_BASIC_INFO) {
            // Retrieve image dimensions
            if (JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &info)) {
                //std::cerr << "JxlDecoderGetBasicInfo failed" << std::endl;
                return 1;
            }
            //std::cout << "Image dimensions: " << info.xsize << "x" << info.ysize << std::endl;

            // Adjust parallel runner threads based on image constraints
            JxlResizableParallelRunnerSetThreads(runner.get(), JxlResizableParallelRunnerSuggestThreads(info.xsize, info.ysize));
        }
        else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            // Calculate buffer size needed for raw pixels and allocate it
            size_t buffer_size;
            if (JXL_DEC_SUCCESS != JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size)) {
                //std::cerr << "JxlDecoderImageOutBufferSize failed" << std::endl;
                return 1;
            }

            imgBuf = (unsigned char*)malloc(buffer_size);

            // Hand the memory block over to the decoder
            if (JXL_DEC_SUCCESS != JxlDecoderSetImageOutBuffer(
                dec.get(), &format, imgBuf, buffer_size)) {
                //std::cerr << "JxlDecoderSetImageOutBuffer failed" << std::endl;
                return 1;
            }
        }
        else if (status == JXL_DEC_FULL_IMAGE) {
            int a = 0;
            // This frame is fully decoded!
            //std::cout << "Successfully decoded frame to memory buffer (" << pixels.size() << " bytes)." << std::endl;
        }
        else if (status == JXL_DEC_SUCCESS) {
            // Entire decoding process is finished
            break;
        }
    }

    // At this stage, 'pixels' contains the uncompressed RGBA pixel payload
    width = info.xsize;
    height = info.ysize;
    channels = format.num_channels;
    subsamp = 0; // Not applicable for JXL, but set to 0 for consistency

    return 0;
}
