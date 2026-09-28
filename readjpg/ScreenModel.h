/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <memory>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>

class jpegImage;
class file_list;

class ScreenModel
{
public:
    ScreenModel(int windowWidth, int windowHeight, const wchar_t* directory);
    ~ScreenModel();

    // State accessors
    float getScale() const { return scale; }
    float getPositionX() const { return posx; }
    float getPositionY() const { return posy; }
    float getScale0() const { return scale0; }
    float getAspectRatio() const { return imageAspect; }
    float getWindowAspectRatio() const { return windowAspect; }

    void setScale(float v) { scale = v; }
    
    jpegImage* getCurrentImage() const { return jpeg.get(); }
    int getImageCount() const;
    int getCurrentImageIndex() const;

    // Camera operations
    bool zoomIn();
    bool zoomOut();
    void pan(float dx, float dy);
    void setPositionY(float y);
    void setPositionX(float x);
    void reset();
    
    // Navigation
    void nextImage();
    void previousImage();
    
    // Viewport management
    void updateViewport(int windowWidth, int windowHeight, int imageWidth, int imageHeight);
    float getMaxLeft() const;
    float getMaxRight() const;
    float getMaxTop() const;
    float getMaxBottom() const;
    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }
    bool isImageReady() const { return image_ready; }
    void setImageReady(bool ready) { image_ready = ready; }
    
    // Threading control
    bool isRunning() const { return m_running; }
    void shutdown();

private:
    // Display state
    float scale0;
    float scale;
    float posx;
    float posy;
    float windowAspect;
    float imageAspect;
    
    int windowWidth;
    int windowHeight;

    // Resources
    std::unique_ptr<jpegImage> jpeg;
    std::unique_ptr<file_list> files;
    std::map<int, std::unique_ptr<jpegImage>> jpegCache;
    
    // Threading
    std::thread loadingThread;
    std::mutex cacheMutex;
    std::condition_variable m_cv;
    bool m_running;
    bool image_ready;
    
    // Internal methods
    void loadJpegThread();
    void scheduleJpeg(int idx);
    void updateImage(int step);
    void clampPosition();
};