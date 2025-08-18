#include "VideoRenderer.h"
#include <iostream>

VideoRenderer::VideoRenderer()
{
}

VideoRenderer::~VideoRenderer()
{
    Cleanup();
}

bool VideoRenderer::Init()
{
    if (m_bInited)
        return true;
        
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    m_bInited = true;
    return true;
}

void VideoRenderer::Cleanup()
{
    DestroySDLWindow();
    
    if (m_bInited)
    {
        SDL_Quit();
        m_bInited = false;
    }
}

bool VideoRenderer::CreateSDLWindow(Gtk::Widget* parent_widget)
{
    if (!m_bInited)
    {
        if (!Init())
            return false;
    }
    
    // Create SDL window
    m_pSDLWindow = SDL_CreateWindow("Video Display",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   640, 480,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    if (!m_pSDLWindow)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    CreateSDLContext();
    return true;
}

void VideoRenderer::DestroySDLWindow()
{
    DestroySDLContext();
    
    if (m_pSDLWindow)
    {
        SDL_DestroyWindow(m_pSDLWindow);
        m_pSDLWindow = nullptr;
    }
}

void VideoRenderer::CreateSDLContext()
{
    if (!m_pSDLWindow)
        return;
        
    m_pSDLRenderer = SDL_CreateRenderer(m_pSDLWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!m_pSDLRenderer)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Set renderer draw color to black
    SDL_SetRenderDrawColor(m_pSDLRenderer, 0x00, 0x00, 0x00, 0xFF);
}

void VideoRenderer::DestroySDLContext()
{
    if (m_pSDLTexture)
    {
        SDL_DestroyTexture(m_pSDLTexture);
        m_pSDLTexture = nullptr;
    }
    
    if (m_pSDLRenderer)
    {
        SDL_DestroyRenderer(m_pSDLRenderer);
        m_pSDLRenderer = nullptr;
    }
}

void VideoRenderer::RenderVideoFrame(const char* y_data, const char* u_data, const char* v_data,
                                   int width, int height, int y_stride, int u_stride, int v_stride)
{
    std::lock_guard<std::recursive_mutex> lock(m_dataLock);
    
    if (!m_pSDLRenderer)
        return;
        
    // Create or recreate texture if dimensions changed
    if (!m_pSDLTexture || m_dataWidth != width || m_dataHeight != height)
    {
        if (m_pSDLTexture)
        {
            SDL_DestroyTexture(m_pSDLTexture);
        }
        
        m_pSDLTexture = SDL_CreateTexture(m_pSDLRenderer,
                                        SDL_PIXELFORMAT_IYUV,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width, height);
        
        if (!m_pSDLTexture)
        {
            std::cerr << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        
        m_dataWidth = width;
        m_dataHeight = height;
        
        // Update destination rectangle
        m_rectDst.x = 0;
        m_rectDst.y = 0;
        m_rectDst.w = width;
        m_rectDst.h = height;
    }
    
    // Update texture with YUV data
    if (SDL_UpdateYUVTexture(m_pSDLTexture, nullptr,
                           (const Uint8*)y_data, y_stride,
                           (const Uint8*)u_data, u_stride,
                           (const Uint8*)v_data, v_stride) < 0)
    {
        std::cerr << "Unable to update texture! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Clear renderer
    SDL_RenderClear(m_pSDLRenderer);
    
    // Copy texture to renderer
    SDL_RenderCopy(m_pSDLRenderer, m_pSDLTexture, nullptr, &m_rectDst);
    
    // Update screen
    SDL_RenderPresent(m_pSDLRenderer);
}
