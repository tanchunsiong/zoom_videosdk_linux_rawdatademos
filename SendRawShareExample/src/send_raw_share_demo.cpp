#include "ConsoleVideoSDKApp.h"
#include "helpers/zoom_video_sdk_share_helper_interface.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

using namespace ZOOMVIDEOSDK;

class AnimatedShareSource : public IZoomVideoSDKShareSource
{
public:
    AnimatedShareSource(int width, int height, int frame_rate)
        : width_(std::max(2, width - width % 2)),
          height_(std::max(2, height - height % 2)),
          frame_rate_(std::max(1, std::min(30, frame_rate)))
    {
    }

    ~AnimatedShareSource() override
    {
        Stop();
    }

    void onShareSendStarted(IZoomVideoSDKShareSender* sender) override
    {
        sender_ = sender;
        if (!sender_ || sending_.exchange(true))
        {
            return;
        }
        std::cout << "Raw share sender started at "
                  << width_ << "x" << height_
                  << " @ " << frame_rate_ << " FPS" << std::endl;
        worker_ = std::thread(&AnimatedShareSource::SendLoop, this);
    }

    void onShareSendStopped() override
    {
        Stop();
        sender_ = nullptr;
        std::cout << "Raw share sender stopped" << std::endl;
    }

    void Stop()
    {
        sending_ = false;
        if (worker_.joinable())
        {
            worker_.join();
        }
    }

private:
    void SendLoop()
    {
        const int y_size = width_ * height_;
        const int chroma_size = y_size / 4;
        std::vector<char> frame(y_size + chroma_size * 2);
        auto next_frame = std::chrono::steady_clock::now();
        unsigned int frame_number = 0;

        while (sending_)
        {
            for (int row = 0; row < height_; ++row)
            {
                for (int column = 0; column < width_; ++column)
                {
                    const int bar = ((column + frame_number * 4) * 8) / width_;
                    frame[row * width_ + column] =
                        static_cast<char>(32 + (bar % 8) * 26);
                }
            }
            std::fill(
                frame.begin() + y_size,
                frame.begin() + y_size + chroma_size,
                static_cast<char>(96 + (frame_number % 64)));
            std::fill(
                frame.begin() + y_size + chroma_size,
                frame.end(),
                static_cast<char>(160 - (frame_number % 64)));

            const ZoomVideoSDKErrors error = sender_->sendShareFrame(
                frame.data(),
                width_,
                height_,
                static_cast<int>(frame.size()),
                FrameDataFormat_I420_LIMITED);
            if (error != ZoomVideoSDKErrors_Success)
            {
                std::cerr << "sendShareFrame failed with error " << error << std::endl;
                sending_ = false;
                break;
            }
            if (frame_number == 0)
            {
                std::cout << "Sending animated I420 raw share frames" << std::endl;
            }
            ++frame_number;
            next_frame += std::chrono::milliseconds(1000 / frame_rate_);
            std::this_thread::sleep_until(next_frame);
        }
    }

    int width_;
    int height_;
    int frame_rate_;
    IZoomVideoSDKShareSender* sender_ = nullptr;
    std::atomic<bool> sending_{false};
    std::thread worker_;
};

class SendRawShareFeature : public ConsoleVideoSDKFeature
{
public:
    explicit SendRawShareFeature(const FlatJsonConfig& config)
        : source_(
              config.GetInt("width", 1280),
              config.GetInt("height", 720),
              config.GetInt("frameRate", 25))
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
        const ZoomVideoSDKErrors error =
            share_helper_->startSharingExternalSource(&source_);
        std::cout << "startSharingExternalSource returned " << error << std::endl;
    }

    void OnSessionLeaving() override
    {
        source_.Stop();
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
        if (action)
        {
            std::cout << "Share status changed: user="
                      << (user ? user->getUserName() : "unknown")
                      << " status=" << action->getShareStatus()
                      << " sourceId=" << action->getShareSourceId()
                      << std::endl;
        }
    }

    void onFailedToStartShare(IZoomVideoSDKShareHelper*, IZoomVideoSDKUser*) override
    {
        std::cerr << "The SDK reported that raw sharing failed to start." << std::endl;
    }

private:
    AnimatedShareSource source_;
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
    SendRawShareFeature feature(config);
    return RunConsoleVideoSDKSample(config, feature);
}
