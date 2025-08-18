#include "VideoDisplayBridge.h"
#include <iostream>
#include "zoom_video_sdk_api.h"
#include "zoom_video_sdk_interface.h"
#include "zoom_video_sdk_session_info_interface.h"

std::vector<VideoDisplayBridge*> VideoDisplayBridge::list_;
int VideoDisplayBridge::instance_count = 0;

VideoDisplayBridge::VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer)
    : user_(user), video_renderer_(renderer), is_share_screen_(false)
{
    instance_id_ = instance_count++;
    
    // Only subscribe to remote users' video, not local user
    extern IZoomVideoSDK* video_sdk_obj;
    bool isLocalUser = false;
    if (video_sdk_obj) {
        IZoomVideoSDKSession* session = video_sdk_obj->getSessionInfo();
        if (session) {
            IZoomVideoSDKUser* myself = session->getMyself();
            isLocalUser = (user_ == myself);
        }
    }
    
    if (!isLocalUser && user_ && user_->GetVideoPipe()) {
        user_->GetVideoPipe()->subscribe(ZoomVideoSDKResolution_720P, this);
        list_.push_back(this);
        std::cout << "VideoDisplayBridge: Subscribed to remote video for user " << user_->getUserName() << std::endl;
    } else if (isLocalUser) {
        std::cout << "VideoDisplayBridge: Skipping local user " << user_->getUserName() << " - use PreviewVideoHandler instead" << std::endl;
    }
}

VideoDisplayBridge::VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer, bool isShareScreen)
    : user_(user), video_renderer_(renderer), is_share_screen_(isShareScreen)
{
    instance_id_ = instance_count++;
    if (user_ && user_->GetSharePipe()) {
        user_->GetSharePipe()->subscribe(ZoomVideoSDKResolution_720P, this);
        list_.push_back(this);
        std::cout << "VideoDisplayBridge: Subscribed to share screen for user " << user_->getUserName() << std::endl;
    }
}

VideoDisplayBridge::~VideoDisplayBridge()
{
    if (user_) {
        if (is_share_screen_ && user_->GetSharePipe()) {
            user_->GetSharePipe()->unSubscribe(this);
        } else if (user_->GetVideoPipe()) {
            user_->GetVideoPipe()->unSubscribe(this);
        }
        std::cout << "VideoDisplayBridge: Unsubscribed from user " << user_->getUserName() << std::endl;
    }
    
    // Remove from list
    for (auto iter = list_.begin(); iter != list_.end(); iter++) {
        if (*iter == this) {
            list_.erase(iter);
            break;
        }
    }
    
    instance_count--;
    user_ = nullptr;
    video_renderer_ = nullptr;
}

VideoDisplayBridge* VideoDisplayBridge::find_instance(IZoomVideoSDKUser* user)
{
    for (auto iter = list_.begin(); iter != list_.end(); iter++) {
        VideoDisplayBridge* item = *iter;
        if (item->user_ == user) {
            return item;
        }
    }
    return nullptr;
}

void VideoDisplayBridge::stop_display_for(IZoomVideoSDKUser* user)
{
    VideoDisplayBridge* bridge = VideoDisplayBridge::find_instance(user);
    if (bridge) {
        delete bridge;
    }
}

void VideoDisplayBridge::stop_display_for(IZoomVideoSDKUser* user, bool isShareScreen)
{
    VideoDisplayBridge* bridge = VideoDisplayBridge::find_instance(user);
    if (bridge && bridge->is_share_screen_ == isShareScreen) {
        delete bridge;
    }
}

void VideoDisplayBridge::onRawDataFrameReceived(YUVRawDataI420* data)
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
    
    // Debug output (can be removed later)
    static int frame_count = 0;
    if (++frame_count % 30 == 0) { // Print every 30 frames
        std::cout << "VideoDisplayBridge: Rendered frame " << frame_count 
                  << " for user " << user_->getUserName() 
                  << " (" << width << "x" << height << ")" << std::endl;
    }
}

void VideoDisplayBridge::onRawDataStatusChanged(RawDataStatus status)
{
    const char* status_str = (status == RawData_On) ? "ON" : "OFF";
    std::cout << "VideoDisplayBridge: Raw data status changed to " << status_str 
              << " for user " << user_->getUserName() << std::endl;
}
