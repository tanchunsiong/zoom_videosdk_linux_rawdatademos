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
    
    // Subscribe to video for both local and remote users
    extern IZoomVideoSDK* video_sdk_obj;
    bool isLocalUser = false;
    if (video_sdk_obj) {
        IZoomVideoSDKSession* session = video_sdk_obj->getSessionInfo();
        if (session) {
            IZoomVideoSDKUser* myself = session->getMyself();
            isLocalUser = (user_ == myself);
        }
    }
    
    if (user_ && user_->GetVideoPipe()) {
        // Try different resolutions for better compatibility - using 90P for remote users based on Electron reference
        ZoomVideoSDKResolution resolution = ZoomVideoSDKResolution_90P; // Start with ultra-low resolution for stability
        if (isLocalUser) {
            resolution = ZoomVideoSDKResolution_720P; // Higher resolution for self video
        }
        current_resolution_ = resolution;
        
        bool subscribed = user_->GetVideoPipe()->subscribe(resolution, this);
        if (subscribed) {
            list_.push_back(this);
            if (isLocalUser) {
                std::cout << "VideoDisplayBridge: Successfully subscribed to self video for user " << user_->getUserName() 
                         << " at resolution " << (int)resolution << std::endl;
            } else {
                std::cout << "VideoDisplayBridge: Successfully subscribed to remote video for user " << user_->getUserName() 
                         << " at resolution " << (int)resolution << std::endl;
            }
        } else {
            std::cout << "VideoDisplayBridge: Failed to subscribe to video for user " << user_->getUserName() << std::endl;
        }
    } else {
        std::cout << "VideoDisplayBridge: No video pipe available for user " << (user_ ? user_->getUserName() : "unknown") << std::endl;
    }
}

VideoDisplayBridge::VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer, bool isShareScreen)
    : user_(user), video_renderer_(renderer), is_share_screen_(isShareScreen)
{
    instance_id_ = instance_count++;
    if (user_ && user_->GetSharePipe()) {
        current_resolution_ = ZoomVideoSDKResolution_720P;
        user_->GetSharePipe()->subscribe(ZoomVideoSDKResolution_720P, this);
        list_.push_back(this);
        std::cout << "VideoDisplayBridge: Subscribed to share screen for user " << user_->getUserName() << std::endl;
    }
}

VideoDisplayBridge::VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer, ZoomVideoSDKResolution resolution)
    : user_(user), video_renderer_(renderer), is_share_screen_(false), current_resolution_(resolution)
{
    instance_id_ = instance_count++;
    
    if (user_ && user_->GetVideoPipe()) {
        bool subscribed = user_->GetVideoPipe()->subscribe(resolution, this);
        if (subscribed) {
            list_.push_back(this);
            std::cout << "VideoDisplayBridge: Successfully subscribed to video for user " << user_->getUserName() 
                     << " at custom resolution " << (int)resolution << std::endl;
        } else {
            std::cout << "VideoDisplayBridge: Failed to subscribe to video for user " << user_->getUserName() << std::endl;
        }
    } else {
        std::cout << "VideoDisplayBridge: No video pipe available for user " << (user_ ? user_->getUserName() : "unknown") << std::endl;
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

bool VideoDisplayBridge::ChangeResolution(ZoomVideoSDKResolution newResolution)
{
    if (!user_ || is_share_screen_) {
        std::cout << "VideoDisplayBridge: Cannot change resolution for share screen or invalid user" << std::endl;
        return false;
    }
    
    if (current_resolution_ == newResolution) {
        std::cout << "VideoDisplayBridge: Resolution already set to " << (int)newResolution << std::endl;
        return true;
    }
    
    // Unsubscribe from current resolution
    if (user_->GetVideoPipe()) {
        user_->GetVideoPipe()->unSubscribe(this);
        std::cout << "VideoDisplayBridge: Unsubscribed from current resolution " << (int)current_resolution_ << std::endl;
    }
    
    // Subscribe to new resolution
    if (user_->GetVideoPipe()) {
        bool subscribed = user_->GetVideoPipe()->subscribe(newResolution, this);
        if (subscribed) {
            current_resolution_ = newResolution;
            std::cout << "VideoDisplayBridge: Successfully changed resolution to " << (int)newResolution 
                     << " for user " << user_->getUserName() << std::endl;
            return true;
        } else {
            std::cout << "VideoDisplayBridge: Failed to subscribe to new resolution " << (int)newResolution 
                     << " for user " << user_->getUserName() << std::endl;
            
            // Try to resubscribe to original resolution
            user_->GetVideoPipe()->subscribe(current_resolution_, this);
            return false;
        }
    }
    
    return false;
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
