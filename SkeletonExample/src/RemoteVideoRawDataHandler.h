#pragma once

#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "VideoRenderer.h"

USING_ZOOM_VIDEO_SDK_NAMESPACE

class RemoteVideoRawDataHandler : private IZoomVideoSDKRawDataPipeDelegate
{
public:
    RemoteVideoRawDataHandler(VideoRenderer* renderer);
    ~RemoteVideoRawDataHandler();

    bool SubscribeToUser(IZoomVideoSDKUser* user);
    void Unsubscribe();
    bool IsSubscribed() const { return is_subscribed_; }
    IZoomVideoSDKUser* GetCurrentUser() const { return current_user_; }

private:
    virtual void onRawDataFrameReceived(YUVRawDataI420* data) override;
    virtual void onRawDataStatusChanged(RawDataStatus status) override;
    
private:
    VideoRenderer* video_renderer_;
    IZoomVideoSDKUser* current_user_;
    IZoomVideoSDKRawDataPipe* video_pipe_;
    bool is_subscribed_;
};
