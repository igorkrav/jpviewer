/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include "ScreenModel.h"
#include "jpegImage.h"
#include "file_list.h"
#include "strconv.h"
#include <iostream>
#include <queue>
#include <cmath>

ScreenModel::ScreenModel(int windowWidth, int windowHeight, const wchar_t* directory)
    : scale0(0.0f)
    , scale(1.0f)
    , posx(0.f)
    , posy(0.f)
    , windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , windowAspect(1.0f)
    , imageAspect(1.0f)
    , jpeg(nullptr)
    , files(nullptr)
    , m_running(true)
    , image_ready(false)
{
    files = std::make_unique<file_list>();
    files->load_from_directory(directory);

    if (files->get_count() > 0)
        jpeg = std::make_unique<jpegImage>(files->get_current());

    // Start background loading thread
    loadingThread = std::thread(&ScreenModel::loadJpegThread, this);
}

ScreenModel::~ScreenModel()
{
    shutdown();
}

void ScreenModel::shutdown()
{
    m_running = false;
    m_cv.notify_one();
    if (loadingThread.joinable()) {
        loadingThread.join();
    }
    if (jpeg) jpeg->free_buffer();
    if (files) files.reset();
}

int ScreenModel::getImageCount() const
{
    return files ? files->get_total_files() : 0;
}

int ScreenModel::getCurrentImageIndex() const
{
    return files ? files->get_current_index() : -1;
}

bool ScreenModel::zoomIn()
{
    if (scale > 10.f) return false;
    scale += 0.1f;
    clampPosition();
    return true;
}

bool ScreenModel::zoomOut()
{
    if (scale < 0.1f) return false;
    scale -= 0.1f;
    clampPosition();
    return true;
}

void ScreenModel::pan(float dx, float dy)
{
    posx += dx;

    // don't allow panning beyond the image boundaries
    if (posy + dy > getMaxBottom())
        posy = getMaxBottom();
    else
        if (posy + dy < getMaxTop())
            posy = getMaxTop();
        else
        {
            posy += dy;
        }

    clampPosition();
}

void ScreenModel::setPositionY(float y)
{
    posy = y;
    clampPosition();
}

void ScreenModel::setPositionX(float x)
{
    posx = x;
    clampPosition();
}

void ScreenModel::reset()
{
    posy = posx = 0;
    scale = 1.0f;
    updateViewport(windowWidth, windowHeight, jpeg->get_width(), jpeg->get_height());
}

void ScreenModel::nextImage(bool force)
{
    updateImage(1, force);
}

void ScreenModel::previousImage(bool force)
{
    updateImage(-1, force);
}

void ScreenModel::updateViewport(int windowWidth, int windowHeight, int imageWidth, int imageHeight)
{
    if (imageHeight <= 0.f || imageWidth <= 0.f) return;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    imageAspect = (float)imageWidth / (float)imageHeight;
    windowAspect = (float)windowWidth / (float)windowHeight;

    int vpWidth(0), vpHeight(0);

    if (windowAspect > imageAspect) {
        // Window is wider than the image -> Pillarbox (bars on left/right)
        vpHeight = windowHeight;
        vpWidth = (int)(windowHeight * imageAspect);
    }
    else {
        // Window is taller than the image -> Letterbox (bars on top/bottom)
        vpWidth = windowWidth;
        vpHeight = (int)(windowWidth / imageAspect);
    }

    float scaleX = (float)vpWidth / (float)imageWidth;
    float scaleY = (float)vpHeight / (float)imageHeight;

    scale0 = scaleX < scaleY ? scaleX : scaleY;

    clampPosition();
}

float ScreenModel::getMaxLeft() const
{
    // To implement: Calculate the maximum left position based on the current scale and window dimensions
    return 0.f;
}

float ScreenModel::getMaxRight() const
{
    // To implement: Calculate the maximum right position based on the current scale and window dimensions
    return 0.f;
}

float ScreenModel::getMaxTop() const
{
    if (jpeg == nullptr) return 0.f;
    float h = jpeg->get_height() * scale0;
    auto mt = (1.f - (h * scale) / windowHeight);
    if (mt < 0.f) return mt;

    return 0.f;
}

float ScreenModel::getMaxBottom() const
{
    if (jpeg == nullptr) return 0.f;
    float h = jpeg->get_height() * scale0;
    auto mt = (h * scale) / windowHeight - 1.f;
    if (mt > 0.f) return mt;

    return 0.f;
}

void ScreenModel::loadJpegThread()
{
    while (true) {
        {
            std::unique_lock<std::mutex> lk(cacheMutex);
            m_cv.wait(lk);

            // Check if the thread should exit
            if (!m_running) return;

            // Load jpeg images in the cache
            for (const auto& [idx, jpegPtr] : jpegCache) {
                if (!m_running) return;
                if (jpegPtr && jpegPtr->is_ready2load()) {
                    jpegPtr->detect_and_load();
                }
            }
        }
    }
}

void ScreenModel::updateImage(int step, bool force)
{
    if (!files) return;

    int idx = files->get_current_index();
    int idx_next = idx + step;
    //if (idx_next < 0 || idx_next >= files->get_total_files()) return;

    // Update current index
    if (step > 0) {
        files->next(force);
    }
    else {
        files->prev(force);
    }

    // Check if the current image is already in the cache
    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        jpegCache.try_emplace(idx, std::move(jpeg));

        auto it = jpegCache.find(idx_next);
        if (it != jpegCache.end()) {
            jpeg = std::move(it->second);
            jpegCache.erase(it);
        }
        else {
            jpeg = std::make_unique<jpegImage>(files->get_current());
        }
    }
    image_ready = false;

    // Reset position when loading new image
    posx = posy = 0;
    scale = 1.0f;

    if (jpeg && (jpeg->is_loaded() || jpeg->is_texture())) {
        updateViewport(windowWidth, windowHeight, jpeg->get_width(), jpeg->get_height());
    }

    // Manage cache size
    {
        std::unique_lock<std::mutex> lk(cacheMutex);
        if (jpegCache.size() > 5) {
            std::queue<int> keysToRemove;
            for (const auto& [cacheIdx, jpegPtr] : jpegCache) {
                if (std::abs(files->get_current_index() - cacheIdx) > 2) {
                    keysToRemove.push(cacheIdx);
                }
            }
            while (!keysToRemove.empty()) {
                jpegCache.erase(keysToRemove.front());
                keysToRemove.pop();
            }
        }
    }

    // Preload the next image
    scheduleJpeg(idx_next + step);
}

void ScreenModel::scheduleJpeg(int idx)
{
    if (!files || idx < 0 || idx >= files->get_total_files()) return;

    bool notify = false;
    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = jpegCache.find(idx);

        if (it == jpegCache.end()) {
            jpegCache.try_emplace(idx, std::make_unique<jpegImage>(files->get_file(idx), true));
            notify = true;
        }
    }

    if (notify) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        m_cv.notify_one();
    }
}

void ScreenModel::clampPosition()
{
    if (jpeg == nullptr) return;

    float scalex = scale;
    float scaley = scale;

    if (windowAspect > imageAspect) {
        // Window is wider than the image -> Pillarbox
        scalex = scale / windowAspect * imageAspect;
    }
    else {
        scaley = scale * windowAspect / imageAspect;
    }

    // Clamp vertical position
    if (posy - scaley > 1.0f) posy = 1.0f + scaley;
    if (posy + scaley < -1.0f) posy = -1.0f - scaley;

    // Clamp horizontal position
    if (posx - scalex > 1.0f) posx = 1.0f + scalex;
    if (posx + scalex < -1.0f) posx = -1.0f - scalex;
}

void ScreenModel::fit2width()
{
    // Fit to window width
    const auto* jpeg = getCurrentImage();
    if (jpeg) {
        float w = jpeg->get_width() * getScale0();
        float newScale = windowWidth / w;
        setScale(newScale);

        setPositionX(0);
    }
}

void ScreenModel::fit2height()
{
    // Fit to window height
    const auto* jpeg = getCurrentImage();
    if (jpeg) {
        float h = jpeg->get_height() * getScale0();
        float newScale = windowHeight / h;
        setScale(newScale);

        // Pan to top
        setPositionX(0);
        setPositionY(getMaxTop());
    }
}
