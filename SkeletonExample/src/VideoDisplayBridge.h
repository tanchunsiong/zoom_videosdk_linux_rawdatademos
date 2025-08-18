#pragma once

#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "VideoRenderer.h"
#include <vector>

USING_ZOOM_VIDEO_SDK_NAMESPACE

class VideoDisplayBridge : private IZoomVideoSDKRawDataPipeDelegate
{
public:
    VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer);
    VideoDisplayBridge(IZoomVideoSDKUser* user, VideoRenderer* renderer, bool isShareScreen);
    ~VideoDisplayBridge();

    static void stop_display_for(IZoomVideoSDKUser* user);
    static void stop_display_for(IZoomVideoSDKUser* user, bool isShareScreen);

private:
    virtual void onRawDataFrameReceived(YUVRawDataI420* data) override;
    virtual void onRawDataStatusChanged(RawDataStatus status) override;
    
    static VideoDisplayBridge* find_instance(IZoomVideoSDKUser* user);
    
    int instance_id_;
    static int instance_count;
    static std::vector<VideoDisplayBridge*> list_;
    IZoomVideoSDKUser* user_;
    VideoRenderer* video_renderer_;
    bool is_share_screen_;
};
