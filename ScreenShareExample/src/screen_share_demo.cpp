#include "ConsoleVideoSDKApp.h"
#include "helpers/zoom_video_sdk_share_helper_interface.h"

#include <glib.h>

#include <iostream>
#include <string>

using namespace ZOOMVIDEOSDK;

class ScreenShareFeature : public ConsoleVideoSDKFeature
{
public:
    explicit ScreenShareFeature(const FlatJsonConfig& config)
        : monitor_id_(config.GetString("monitorId")),
          window_handle_(config.GetString("windowHandle")),
          share_device_audio_(config.GetBool("shareDeviceAudio")),
          optimize_for_video_(config.GetBool("optimizeForVideo")),
          pause_after_seconds_(config.GetInt("pauseAfterSeconds")),
          resume_after_seconds_(config.GetInt("resumeAfterSeconds")),
          stop_after_seconds_(config.GetInt("stopAfterSeconds"))
    {
    }

    void OnSessionJoined() override
    {
        share_helper_ = sdk()->getShareHelper();
        if (!share_helper_)
        {
            std::cerr << "Share helper is unavailable." << std::endl;
            return;
        }

        std::cout << "enableShareDeviceAudio returned "
                  << share_helper_->enableShareDeviceAudio(share_device_audio_)
                  << std::endl;
        std::cout << "enableOptimizeForSharedVideo returned "
                  << share_helper_->enableOptimizeForSharedVideo(optimize_for_video_)
                  << std::endl;

        ZoomVideoSDKShareOption option(share_device_audio_, optimize_for_video_);
        ZoomVideoSDKErrors error = ZoomVideoSDKErrors_Invalid_Parameter;
        if (!window_handle_.empty())
        {
            error = share_helper_->startShareView(
                reinterpret_cast<void*>(const_cast<char*>(window_handle_.c_str())),
                option);
            std::cout << "startShareView returned " << error << std::endl;
        }
        else if (!monitor_id_.empty())
        {
            error = share_helper_->startShareScreen(monitor_id_.c_str(), option);
            std::cout << "startShareScreen returned " << error << std::endl;
        }
        else
        {
            std::cerr << "Set monitorId or windowHandle in config.json." << std::endl;
            return;
        }

        if (error != ZoomVideoSDKErrors_Success)
        {
            return;
        }
        ScheduleControl(pause_after_seconds_, PauseShare);
        ScheduleControl(resume_after_seconds_, ResumeShare);
        ScheduleControl(stop_after_seconds_, StopShare);
    }

    void OnSessionLeaving() override
    {
        if (share_helper_ && share_helper_->isSharingOut())
        {
            share_helper_->stopShare();
        }
    }

    void onUserShareStatusChanged(
        IZoomVideoSDKShareHelper*,
        IZoomVideoSDKUser* user,
        IZoomVideoSDKShareAction* action) override
    {
        if (!action)
        {
            return;
        }
        std::cout << "Share status changed: user="
                  << (user ? user->getUserName() : "unknown")
                  << " status=" << action->getShareStatus()
                  << " type=" << action->getShareType()
                  << " sourceId=" << action->getShareSourceId()
                  << std::endl;
    }

    void onFailedToStartShare(IZoomVideoSDKShareHelper*, IZoomVideoSDKUser*) override
    {
        std::cerr << "The SDK reported that sharing failed to start." << std::endl;
    }

private:
    using ControlCallback = gboolean (*)(gpointer);

    void ScheduleControl(int delay_seconds, ControlCallback callback)
    {
        if (delay_seconds > 0)
        {
            g_timeout_add_seconds(delay_seconds, callback, this);
        }
    }

    static gboolean PauseShare(gpointer data)
    {
        auto* self = static_cast<ScreenShareFeature*>(data);
        if (self->share_helper_ && self->share_helper_->isSharingOut())
        {
            std::cout << "pauseShare returned " << self->share_helper_->pauseShare() << std::endl;
        }
        return G_SOURCE_REMOVE;
    }

    static gboolean ResumeShare(gpointer data)
    {
        auto* self = static_cast<ScreenShareFeature*>(data);
        if (self->share_helper_ && self->share_helper_->isSharingOut())
        {
            std::cout << "resumeShare returned " << self->share_helper_->resumeShare() << std::endl;
        }
        return G_SOURCE_REMOVE;
    }

    static gboolean StopShare(gpointer data)
    {
        auto* self = static_cast<ScreenShareFeature*>(data);
        if (self->share_helper_ && self->share_helper_->isSharingOut())
        {
            std::cout << "stopShare returned " << self->share_helper_->stopShare() << std::endl;
        }
        return G_SOURCE_REMOVE;
    }

    std::string monitor_id_;
    std::string window_handle_;
    bool share_device_audio_;
    bool optimize_for_video_;
    int pause_after_seconds_;
    int resume_after_seconds_;
    int stop_after_seconds_;
    IZoomVideoSDKShareHelper* share_helper_ = nullptr;
};

int main()
{
    FlatJsonConfig config;
    std::string error;
    if (!LoadRuntimeConfig(config, error))
    {
        std::cerr << error << std::endl;
        return 1;
    }
    ScreenShareFeature feature(config);
    return RunConsoleVideoSDKSample(config, feature);
}
