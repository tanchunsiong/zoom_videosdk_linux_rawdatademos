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
    
    // Create SDL renderer embedded in GTK widget
    bool CreateEmbeddedRenderer(Gtk::DrawingArea* drawing_area);
    void DestroyEmbeddedRenderer();
    
    // Video rendering
    void RenderVideoFrame(const char* y_data, const char* u_data, const char* v_data, 
                         int width, int height, int y_stride, int u_stride, int v_stride);
    
    // Get drawing area widget
    Gtk::DrawingArea* GetDrawingArea() const { return m_pDrawingArea; }
    
private:
    void CreateSDLContext();
    void DestroySDLContext();
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    void UpdateVideoTexture(const char* y_data, const char* u_data, const char* v_data, 
                           int width, int height, int y_stride, int u_stride, int v_stride);
    
private:
    SDL_Window* m_pSDLWindow = nullptr;
    SDL_Renderer* m_pSDLRenderer = nullptr;
    SDL_Texture* m_pSDLTexture = nullptr;
    SDL_Surface* m_pSDLSurface = nullptr;
    SDL_Rect m_rectDst;
    
    Gtk::DrawingArea* m_pDrawingArea = nullptr;
    
    int m_dataWidth = 0;
    int m_dataHeight = 0;
    bool m_bInited = false;
    bool m_bHasNewFrame = false;
    
    std::recursive_mutex m_dataLock;
    
    // Video frame data storage
    std::vector<uint8_t> m_frameBuffer;
};
