#include "ZoomVideoRenderer.h"
#include <iostream>

ZoomVideoRenderer::ZoomVideoRenderer(VideoRenderer* renderer)
    : video_renderer_(renderer), subscribed_user_(nullptr), is_subscribed_(false)
{
    std::cout << "ZoomVideoRenderer: Created new renderer instance" << std::endl;
}

ZoomVideoRenderer::~ZoomVideoRenderer()
{
    Unsubscribe();
    std::cout << "ZoomVideoRenderer: Destroyed renderer instance" << std::endl;
}

bool ZoomVideoRenderer::SubscribeToUser(IZoomVideoSDKUser* user, ZoomVideoSDKResolution resolution)
{
    if (!user || !video_renderer_) {
        std::cout << "ZoomVideoRenderer: Invalid user or renderer" << std::endl;
        return false;
    }
    
    // Unsubscribe from previous user if any
    Unsubscribe();
    
    // Get the user's video pipe
    IZoomVideoSDKRawDataPipe* videoPipe = user->GetVideoPipe();
    if (!videoPipe) {
        std::cout << "ZoomVideoRenderer: No video pipe available for user " << user->getUserName() << std::endl;
        return false;
    }
    
    // Subscribe to the video pipe for display only
    bool success = videoPipe->subscribe(resolution, this);
    if (success) {
        subscribed_user_ = user;
        is_subscribed_ = true;
        std::cout << "ZoomVideoRenderer: Successfully subscribed to video for user " 
                  << user->getUserName() << " at resolution " << (int)resolution << std::endl;
    } else {
        std::cout << "ZoomVideoRenderer: Failed to subscribe to video for user " << user->getUserName() << std::endl;
    }
    
    return success;
}

void ZoomVideoRenderer::Unsubscribe()
{
    if (is_subscribed_ && subscribed_user_) {
        IZoomVideoSDKRawDataPipe* videoPipe = subscribed_user_->GetVideoPipe();
        if (videoPipe) {
            videoPipe->unSubscribe(this);
            std::cout << "ZoomVideoRenderer: Unsubscribed from user " << subscribed_user_->getUserName() << std::endl;
        }
    }
    
    subscribed_user_ = nullptr;
    is_subscribed_ = false;
}

void ZoomVideoRenderer::onRawDataFrameReceived(YUVRawDataI420* data)
{
    if (!data || !video_renderer_) {
        return;
    }

    const int width = data->GetStreamWidth();
    const int height = data->GetStreamHeight();
    
    // Get YUV data pointers
    const char* y_data = reinterpret_cast<const char*>(data->GetYBuffer());
    const char* u_data = reinterpret_cast<const char*>(data->GetUBuffer());
    const char* v_data = reinterpret_cast<const char*>(data->GetVBuffer());
    
    // Calculate strides (assuming standard YUV420 layout)
    const int y_stride = width;
    const int u_stride = width / 2;
    const int v_stride = width / 2;
    
    // Render the frame
    video_renderer_->RenderVideoFrame(y_data, u_data, v_data, width, height, y_stride, u_stride, v_stride);
    
    // Debug output (reduced frequency)
    static int frame_count = 0;
    if (++frame_count % 60 == 0) { // Print every 60 frames (about once per 2 seconds at 30fps)
        std::cout << "ZoomVideoRenderer: Rendered frame " << frame_count 
                  << " for user " << (subscribed_user_ ? subscribed_user_->getUserName() : "unknown")
                  << " (" << width << "x" << height << ")" << std::endl;
    }
}

void ZoomVideoRenderer::onRawDataStatusChanged(RawDataStatus status)
{
    const char* status_str = (status == RawData_On) ? "ON" : "OFF";
    std::cout << "ZoomVideoRenderer: Raw data status changed to " << status_str 
              << " for user " << (subscribed_user_ ? subscribed_user_->getUserName() : "unknown") << std::endl;
}
