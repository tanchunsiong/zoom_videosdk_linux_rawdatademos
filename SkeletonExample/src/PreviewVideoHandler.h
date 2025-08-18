#pragma once

#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "VideoRenderer.h"

USING_ZOOM_VIDEO_SDK_NAMESPACE

class PreviewVideoHandler : private IZoomVideoSDKRawDataPipeDelegate
{
public:
    PreviewVideoHandler(VideoRenderer* renderer);
    ~PreviewVideoHandler();

    bool StartPreview();
    void StopPreview();
    bool IsPreviewActive() const { return is_preview_active_; }

private:
    virtual void onRawDataFrameReceived(YUVRawDataI420* data) override;
    virtual void onRawDataStatusChanged(RawDataStatus status) override;
    
private:
    VideoRenderer* video_renderer_;
    bool is_preview_active_;
};
