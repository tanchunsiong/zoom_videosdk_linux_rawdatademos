#pragma once

#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "VideoRenderer.h"
#include <memory>

USING_ZOOM_VIDEO_SDK_NAMESPACE

// Windows-like renderer class that only handles video display (not transmission)
class ZoomVideoRenderer : private IZoomVideoSDKRawDataPipeDelegate
{
public:
    ZoomVideoRenderer(VideoRenderer* renderer);
    ~ZoomVideoRenderer();

    // Subscribe to a user's video for display only
    bool SubscribeToUser(IZoomVideoSDKUser* user, ZoomVideoSDKResolution resolution = ZoomVideoSDKResolution_720P);
    
    // Unsubscribe from current user
    void Unsubscribe();
    
    // Get current subscribed user
    IZoomVideoSDKUser* GetSubscribedUser() const { return subscribed_user_; }

private:
    // IZoomVideoSDKRawDataPipeDelegate implementation
    virtual void onRawDataFrameReceived(YUVRawDataI420* data) override;
    virtual void onRawDataStatusChanged(RawDataStatus status) override;
    
    VideoRenderer* video_renderer_;
    IZoomVideoSDKUser* subscribed_user_;
    bool is_subscribed_;
};
