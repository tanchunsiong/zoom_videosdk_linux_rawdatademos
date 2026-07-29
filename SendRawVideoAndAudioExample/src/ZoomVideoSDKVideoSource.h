// Zoom Video SDK
#include "zoom_sdk_raw_data_def.h"
#include "zoom_video_sdk_def.h"
#include "helpers/zoom_video_sdk_video_source_helper_interface.h"

#include <atomic>
#include <string>
#include <thread>

using namespace ZOOMVIDEOSDK;

class ZoomVideoSDKVideoSource :
  public IZoomVideoSDKVideoSource 
{
public:
    ZoomVideoSDKVideoSource();
    ~ZoomVideoSDKVideoSource() override;

    void onInitialize(
        IZoomVideoSDKVideoSender* sender,
        IVideoSDKVector<VideoSourceCapability>* support_cap_list,
        VideoSourceCapability& suggest_cap) override;
    void onPropertyChange(
        IVideoSDKVector<VideoSourceCapability>* support_cap_list,
        VideoSourceCapability suggest_cap) override;
    void onStartSend() override;
    void onStopSend() override;
    void onUninitialized() override;

private:
    void SendVideoLoop();
    void UpdateCapability(const VideoSourceCapability& capability);

    IZoomVideoSDKVideoSender* video_sender_;
    std::atomic<int> width_;
    std::atomic<int> height_;
    std::atomic<int> frame_rate_;
    std::atomic<bool> sending_;
    std::thread send_thread_;
    std::string video_source_;
};


