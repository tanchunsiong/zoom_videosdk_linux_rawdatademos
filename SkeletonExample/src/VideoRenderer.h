#pragma once

#include <SDL2/SDL.h>
#include "zoom_video_sdk_delegate_interface.h"
#include "zoom_video_sdk_def.h"
#include <mutex>
#include <gtkmm.h>

USING_ZOOM_VIDEO_SDK_NAMESPACE

class VideoRenderer
{
public:
    VideoRenderer();
    ~VideoRenderer();

    bool Init();
    void Cleanup();
    
    // Create SDL window embedded in GTK widget
    bool CreateSDLWindow(Gtk::Widget* parent_widget);
    void DestroySDLWindow();
    
    // Video rendering
    void RenderVideoFrame(const char* y_data, const char* u_data, const char* v_data, 
                         int width, int height, int y_stride, int u_stride, int v_stride);
    
    // Get SDL window info for embedding
    SDL_Window* GetSDLWindow() const { return m_pSDLWindow; }
    
private:
    void CreateSDLContext();
    void DestroySDLContext();
    
private:
    SDL_Window* m_pSDLWindow = nullptr;
    SDL_Renderer* m_pSDLRenderer = nullptr;
    SDL_Texture* m_pSDLTexture = nullptr;
    SDL_Rect m_rectDst;
    
    int m_dataWidth = 0;
    int m_dataHeight = 0;
    bool m_bInited = false;
    
    std::recursive_mutex m_dataLock;
};
