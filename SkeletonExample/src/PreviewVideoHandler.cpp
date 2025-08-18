#include "PreviewVideoHandler.h"
#include <iostream>
#include "zoom_video_sdk_api.h"
#include "zoom_video_sdk_interface.h"
#include "helpers/zoom_video_sdk_video_helper_interface.h"

extern IZoomVideoSDK* video_sdk_obj;

PreviewVideoHandler::PreviewVideoHandler(VideoRenderer* renderer)
    : video_renderer_(renderer), is_preview_active_(false)
{
}

PreviewVideoHandler::~PreviewVideoHandler()
{
    StopPreview();
}

bool PreviewVideoHandler::StartPreview()
{
    if (!video_sdk_obj || !video_renderer_)
    {
        std::cout << "PreviewVideoHandler: SDK or renderer not available" << std::endl;
        return false;
    }

    IZoomVideoSDKVideoHelper* videoHelper = video_sdk_obj->getVideoHelper();
    if (!videoHelper)
    {
        std::cout << "PreviewVideoHandler: Video helper not available" << std::endl;
        return false;
    }

    ZoomVideoSDKErrors err = videoHelper->startVideoPreview(this);
    if (err == ZoomVideoSDKErrors_Success)
    {
        is_preview_active_ = true;
        std::cout << "PreviewVideoHandler: Preview started successfully" << std::endl;
        return true;
    }
    else
    {
        std::cout << "PreviewVideoHandler: Failed to start preview, error: " << (int)err << std::endl;
        return false;
    }
}

void PreviewVideoHandler::StopPreview()
{
    if (!video_sdk_obj || !is_preview_active_)
        return;

    IZoomVideoSDKVideoHelper* videoHelper = video_sdk_obj->getVideoHelper();
    if (videoHelper)
    {
        ZoomVideoSDKErrors err = videoHelper->stopVideoPreview(this);
        if (err == ZoomVideoSDKErrors_Success)
        {
            std::cout << "PreviewVideoHandler: Preview stopped successfully" << std::endl;
        }
        else
        {
            std::cout << "PreviewVideoHandler: Failed to stop preview, error: " << (int)err << std::endl;
        }
    }
    
    is_preview_active_ = false;
}

void PreviewVideoHandler::onRawDataFrameReceived(YUVRawDataI420* data)
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
    
    // Render the preview frame
    video_renderer_->RenderVideoFrame(y_data, u_data, v_data, width, height, y_stride, u_stride, v_stride);
    
    // Debug output (can be removed later)
    static int frame_count = 0;
    if (++frame_count % 30 == 0) // Print every 30 frames
    {
        std::cout << "PreviewVideoHandler: Rendered preview frame " << frame_count 
                  << " (" << width << "x" << height << ")" << std::endl;
    }
}

void PreviewVideoHandler::onRawDataStatusChanged(RawDataStatus status)
{
    is_preview_active_ = (status == RawData_On);
    const char* status_str = (status == RawData_On) ? "ON" : "OFF";
    std::cout << "PreviewVideoHandler: Preview status changed to " << status_str << std::endl;
}
