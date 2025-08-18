#include "VideoRenderer.h"
#include <iostream>
#include <cstring>

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
    DestroyEmbeddedRenderer();
    
    if (m_bInited)
    {
        SDL_Quit();
        m_bInited = false;
    }
}

bool VideoRenderer::CreateEmbeddedRenderer(Gtk::DrawingArea* drawing_area)
{
    if (!m_bInited)
    {
        if (!Init())
            return false;
    }
    
    if (!drawing_area)
    {
        std::cerr << "Drawing area is null" << std::endl;
        return false;
    }
    
    m_pDrawingArea = drawing_area;
    
    // Connect the draw signal to our drawing function
    m_pDrawingArea->signal_draw().connect(sigc::mem_fun(*this, &VideoRenderer::on_draw));
    
    // Set minimum size for the drawing area
    m_pDrawingArea->set_size_request(640, 480);
    
    std::cout << "Video renderer embedded in GTK widget successfully" << std::endl;
    return true;
}

void VideoRenderer::DestroyEmbeddedRenderer()
{
    DestroySDLContext();
    m_pDrawingArea = nullptr;
}

void VideoRenderer::CreateSDLContext()
{
    // We'll create SDL surfaces for YUV to RGB conversion
    // but render using Cairo instead of SDL directly
}

void VideoRenderer::DestroySDLContext()
{
    if (m_pSDLTexture)
    {
        SDL_DestroyTexture(m_pSDLTexture);
        m_pSDLTexture = nullptr;
    }
    
    if (m_pSDLSurface)
    {
        SDL_FreeSurface(m_pSDLSurface);
        m_pSDLSurface = nullptr;
    }
    
    if (m_pSDLRenderer)
    {
        SDL_DestroyRenderer(m_pSDLRenderer);
        m_pSDLRenderer = nullptr;
    }
    
    if (m_pSDLWindow)
    {
        SDL_DestroyWindow(m_pSDLWindow);
        m_pSDLWindow = nullptr;
    }
}

void VideoRenderer::UpdateVideoTexture(const char* y_data, const char* u_data, const char* v_data,
                                      int width, int height, int y_stride, int u_stride, int v_stride)
{
    std::lock_guard<std::recursive_mutex> lock(m_dataLock);
    
    // Store frame dimensions
    m_dataWidth = width;
    m_dataHeight = height;
    
    // Calculate RGB buffer size
    int rgb_size = width * height * 3; // RGB24 format
    m_frameBuffer.resize(rgb_size);
    
    // Convert YUV420 to RGB24 using proper video range conversion
    uint8_t* rgb_data = m_frameBuffer.data();
    
    const uint8_t* y_plane = reinterpret_cast<const uint8_t*>(y_data);
    const uint8_t* u_plane = reinterpret_cast<const uint8_t*>(u_data);
    const uint8_t* v_plane = reinterpret_cast<const uint8_t*>(v_data);
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Get YUV values with proper stride handling
            int y_val = y_plane[y * y_stride + x];
            int u_val = u_plane[(y / 2) * u_stride + (x / 2)];
            int v_val = v_plane[(y / 2) * v_stride + (x / 2)];
            
            // Convert YUV to RGB using ITU-R BT.601 coefficients for video range
            // Y range: 16-235, UV range: 16-240 (centered at 128)
            int c = y_val - 16;
            int d = u_val - 128;
            int e = v_val - 128;
            
            // Use proper BT.601 conversion matrix
            int r = (298 * c + 409 * e + 128) >> 8;
            int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
            int b = (298 * c + 516 * d + 128) >> 8;
            
            // Clamp values to valid RGB range
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            
            // Store RGB values
            int rgb_index = (y * width + x) * 3;
            rgb_data[rgb_index] = r;
            rgb_data[rgb_index + 1] = g;
            rgb_data[rgb_index + 2] = b;
        }
    }
    
    m_bHasNewFrame = true;
}

bool VideoRenderer::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    std::lock_guard<std::recursive_mutex> lock(m_dataLock);
    
    if (!m_bHasNewFrame || m_frameBuffer.empty())
    {
        // Draw a black background if no video frame
        cr->set_source_rgb(0.0, 0.0, 0.0);
        cr->paint();
        
        // Draw placeholder text
        cr->set_source_rgb(1.0, 1.0, 1.0);
        cr->move_to(10, 30);
        cr->show_text("Waiting for video...");
        return true;
    }
    
    // Get widget dimensions
    Gtk::Allocation allocation = m_pDrawingArea->get_allocation();
    int widget_width = allocation.get_width();
    int widget_height = allocation.get_height();
    
    if (widget_width <= 0 || widget_height <= 0)
        return true;
    
    // Calculate scaling to maintain aspect ratio
    double scale_x = (double)widget_width / m_dataWidth;
    double scale_y = (double)widget_height / m_dataHeight;
    double scale = std::min(scale_x, scale_y);
    
    int scaled_width = (int)(m_dataWidth * scale);
    int scaled_height = (int)(m_dataHeight * scale);
    
    // Center the video
    int offset_x = (widget_width - scaled_width) / 2;
    int offset_y = (widget_height - scaled_height) / 2;
    
    // Create Cairo surface from RGB data
    Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(
        Cairo::FORMAT_RGB24, m_dataWidth, m_dataHeight);
    
    unsigned char* surface_data = surface->get_data();
    int surface_stride = surface->get_stride();
    
    // Copy RGB data to Cairo surface (convert RGB to BGRA)
    for (int y = 0; y < m_dataHeight; y++)
    {
        for (int x = 0; x < m_dataWidth; x++)
        {
            int src_index = (y * m_dataWidth + x) * 3;
            int dst_index = y * surface_stride + x * 4;
            
            surface_data[dst_index + 0] = m_frameBuffer[src_index + 2]; // B
            surface_data[dst_index + 1] = m_frameBuffer[src_index + 1]; // G
            surface_data[dst_index + 2] = m_frameBuffer[src_index + 0]; // R
            surface_data[dst_index + 3] = 255; // A
        }
    }
    
    surface->mark_dirty();
    
    // Draw black background
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->paint();
    
    // Scale and draw the video frame
    cr->save();
    cr->translate(offset_x, offset_y);
    cr->scale(scale, scale);
    cr->set_source(surface, 0, 0);
    cr->paint();
    cr->restore();
    
    return true;
}

void VideoRenderer::RenderVideoFrame(const char* y_data, const char* u_data, const char* v_data,
                                   int width, int height, int y_stride, int u_stride, int v_stride)
{
    if (!m_pDrawingArea)
        return;
    
    // Update the video texture data
    UpdateVideoTexture(y_data, u_data, v_data, width, height, y_stride, u_stride, v_stride);
    
    // Queue a redraw on the GTK main thread
    Glib::signal_idle().connect_once([this]() {
        if (m_pDrawingArea)
            m_pDrawingArea->queue_draw();
    });
}
