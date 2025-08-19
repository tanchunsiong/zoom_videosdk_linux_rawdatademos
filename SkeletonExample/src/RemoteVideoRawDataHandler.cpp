#include "RemoteVideoRawDataHandler.h"
#include <iostream>
#include "zoom_video_sdk_api.h"
#include "zoom_video_sdk_interface.h"

RemoteVideoRawDataHandler::RemoteVideoRawDataHandler(VideoRenderer* renderer)
    : video_renderer_(renderer), current_user_(nullptr), video_pipe_(nullptr), is_subscribed_(false)
{
    std::cout << "RemoteVideoRawDataHandler: Created new handler instance" << std::endl;
}

RemoteVideoRawDataHandler::~RemoteVideoRawDataHandler()
{
    Unsubscribe();
    std::cout << "RemoteVideoRawDataHandler: Destroyed handler instance" << std::endl;
}

bool RemoteVideoRawDataHandler::SubscribeToUser(IZoomVideoSDKUser* user, ZoomVideoSDKResolution resolution)
{
    if (!user || !video_renderer_)
    {
        std::cout << "RemoteVideoRawDataHandler: Invalid user or renderer" << std::endl;
        return false;
    }

    // Unsubscribe from any existing user first
    Unsubscribe();

    // Get the user's video pipe
    video_pipe_ = user->GetVideoPipe();
    if (!video_pipe_)
    {
        std::cout << "RemoteVideoRawDataHandler: No video pipe available for user " << user->getUserName() << std::endl;
        return false;
    }

    // Subscribe to raw data from the video pipe with specified resolution
    ZoomVideoSDKErrors err = video_pipe_->subscribe(resolution, this);
    if (err == ZoomVideoSDKErrors_Success)
    {
        current_user_ = user;
        is_subscribed_ = true;
        std::cout << "RemoteVideoRawDataHandler: Successfully subscribed to raw data for user " << user->getUserName() 
                  << " at resolution " << (int)resolution << std::endl;
        return true;
    }
    else
    {
        std::cout << "RemoteVideoRawDataHandler: Failed to subscribe to raw data for user " << user->getUserName() 
                  << " at resolution " << (int)resolution << ", error: " << (int)err << std::endl;
        video_pipe_ = nullptr;
        return false;
    }
}

void RemoteVideoRawDataHandler::Unsubscribe()
{
    if (is_subscribed_ && video_pipe_)
    {
        ZoomVideoSDKErrors err = video_pipe_->unSubscribe(this);
        if (err == ZoomVideoSDKErrors_Success)
        {
            std::cout << "RemoteVideoRawDataHandler: Successfully unsubscribed from raw data" << std::endl;
        }
        else
        {
            std::cout << "RemoteVideoRawDataHandler: Failed to unsubscribe from raw data, error: " << (int)err << std::endl;
        }
    }
    
    current_user_ = nullptr;
    video_pipe_ = nullptr;
    is_subscribed_ = false;
}

void RemoteVideoRawDataHandler::onRawDataFrameReceived(YUVRawDataI420* data)
{
    if (!data || !video_renderer_)
        return;

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
    
    // Render the remote video frame
    video_renderer_->RenderVideoFrame(y_data, u_data, v_data, width, height, y_stride, u_stride, v_stride);
    
    // Debug output (can be removed later)
    static int frame_count = 0;
    if (++frame_count % 30 == 0) // Print every 30 frames
    {
        std::cout << "RemoteVideoRawDataHandler: Rendered remote video frame " << frame_count 
                  << " (" << width << "x" << height << ")";
        if (current_user_)
        {
            std::cout << " from user " << current_user_->getUserName();
        }
        std::cout << std::endl;
    }
}

void RemoteVideoRawDataHandler::onRawDataStatusChanged(RawDataStatus status)
{
    const char* status_str = (status == RawData_On) ? "ON" : "OFF";
    std::cout << "RemoteVideoRawDataHandler: Raw data status changed to " << status_str;
    if (current_user_)
    {
        std::cout << " for user " << current_user_->getUserName();
    }
    std::cout << std::endl;
    
    // Update subscription status based on raw data status
    if (status == RawData_Off && is_subscribed_)
    {
        std::cout << "RemoteVideoRawDataHandler: Raw data turned off, cleaning up subscription" << std::endl;
        Unsubscribe();
    }
}
