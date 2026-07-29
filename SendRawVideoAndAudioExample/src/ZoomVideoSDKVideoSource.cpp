#include "ZoomVideoSDKVideoSource.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace ZOOMVIDEOSDK;

ZoomVideoSDKVideoSource::ZoomVideoSDKVideoSource()
    : video_sender_(nullptr),
      width_(640),
      height_(360),
      frame_rate_(30),
      sending_(false),
      video_source_("Big_Buck_Bunny_720_10s_1MB.mp4")
{
}

ZoomVideoSDKVideoSource::~ZoomVideoSDKVideoSource()
{
    onStopSend();
}

void ZoomVideoSDKVideoSource::onInitialize(
    IZoomVideoSDKVideoSender* sender,
    IVideoSDKVector<VideoSourceCapability>* support_cap_list,
    VideoSourceCapability& suggest_cap)
{
    video_sender_ = sender;
    UpdateCapability(suggest_cap);
    std::cout << "Video source initialized at "
              << width_ << "x" << height_ << " @ " << frame_rate_ << " FPS" << std::endl;
}

void ZoomVideoSDKVideoSource::onPropertyChange(
    IVideoSDKVector<VideoSourceCapability>* support_cap_list,
    VideoSourceCapability suggest_cap)
{
    UpdateCapability(suggest_cap);
    std::cout << "Video source changed to "
              << width_ << "x" << height_ << " @ " << frame_rate_ << " FPS" << std::endl;
}

void ZoomVideoSDKVideoSource::onStartSend()
{
    if (!video_sender_)
    {
        std::cerr << "Cannot start raw video: SDK video sender is unavailable." << std::endl;
        return;
    }
    if (sending_.exchange(true))
    {
        return;
    }

    std::cout << "Starting raw video worker" << std::endl;
    send_thread_ = std::thread(&ZoomVideoSDKVideoSource::SendVideoLoop, this);
}

void ZoomVideoSDKVideoSource::onStopSend()
{
    sending_ = false;
    if (send_thread_.joinable())
    {
        send_thread_.join();
    }
}

void ZoomVideoSDKVideoSource::onUninitialized()
{
    onStopSend();
    video_sender_ = nullptr;
}

void ZoomVideoSDKVideoSource::UpdateCapability(const VideoSourceCapability& capability)
{
    int width = capability.width > 0 ? static_cast<int>(capability.width) : 640;
    int height = capability.height > 0 ? static_cast<int>(capability.height) : 360;
    width_ = width - (width % 2);
    height_ = height - (height % 2);
    frame_rate_ = capability.frame > 0 ? static_cast<int>(capability.frame) : 30;
}

void ZoomVideoSDKVideoSource::SendVideoLoop()
{
    cv::VideoCapture capture(video_source_);
    if (!capture.isOpened())
    {
        std::cerr << "Unable to open video source: " << video_source_ << std::endl;
        sending_ = false;
        return;
    }

    std::chrono::steady_clock::time_point next_frame = std::chrono::steady_clock::now();
    bool first_frame_sent = false;
    while (sending_)
    {
        cv::Mat frame;
        if (!capture.read(frame) || frame.empty())
        {
            capture.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        const int width = width_;
        const int height = height_;
        const int frame_rate = std::max(1, frame_rate_.load());
        cv::Mat resized_frame;
        cv::resize(frame, resized_frame, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);

        cv::Mat i420_frame;
        cv::cvtColor(resized_frame, i420_frame, cv::COLOR_BGR2YUV_I420);
        const int frame_length = width * height * 3 / 2;
        video_sender_->sendVideoFrame(
            reinterpret_cast<char*>(i420_frame.data),
            width,
            height,
            frame_length,
            0,
            FrameDataFormat_I420_LIMITED);
        if (!first_frame_sent)
        {
            std::cout << "Sending I420 raw video frames" << std::endl;
            first_frame_sent = true;
        }

        next_frame += std::chrono::milliseconds(1000 / frame_rate);
        std::this_thread::sleep_until(next_frame);
    }
}
