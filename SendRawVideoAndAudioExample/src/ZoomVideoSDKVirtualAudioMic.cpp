#include "ZoomVideoSDKVirtualAudioMic.h"
#include "helpers/zoom_video_sdk_user_helper_interface.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace ZOOMVIDEOSDK;

ZoomVideoSDKVirtualAudioMic::ZoomVideoSDKVirtualAudioMic()
    : virtual_audio_sender_(nullptr), sending_(false)
{
}

ZoomVideoSDKVirtualAudioMic::~ZoomVideoSDKVirtualAudioMic()
{
    onMicStopSend();
}

ZoomVideoSDKErrors ZoomVideoSDKVirtualAudioMic::SendRawAudio(
    char* data,
    unsigned int data_length,
    int sample_rate)
{
    if (!virtual_audio_sender_)
    {
        return ZoomVideoSDKErrors_Internal_Error;
    }

    return virtual_audio_sender_->Send(data, data_length, sample_rate);
}

void ZoomVideoSDKVirtualAudioMic::onMicInitialize(IZoomVideoSDKAudioSender* rawdata_sender)
{
    printf("onMicInitialize\n");
    virtual_audio_sender_ = rawdata_sender;
}

void ZoomVideoSDKVirtualAudioMic::onMicStartSend()
{
    printf("onMicStartSend\n");
    if (!virtual_audio_sender_ || sending_.exchange(true))
    {
        return;
    }

    send_thread_ = std::thread(&ZoomVideoSDKVirtualAudioMic::SendAudioLoop, this);
}

void ZoomVideoSDKVirtualAudioMic::onMicStopSend()
{
    sending_ = false;
    if (send_thread_.joinable())
    {
        send_thread_.join();
    }
}

void ZoomVideoSDKVirtualAudioMic::onMicUninitialized()
{
    onMicStopSend();
    virtual_audio_sender_ = nullptr;
}

void ZoomVideoSDKVirtualAudioMic::SendAudioLoop()
{
    const int sample_rate = 48000;
    const int samples_per_frame = sample_rate / 100;
    const double frequency = 440.0;
    const double amplitude = 0.1 * 32767.0;
    const double two_pi = 2.0 * 3.14159265358979323846;
    const double phase_step = two_pi * frequency / sample_rate;
    double phase = 0.0;
    std::vector<int16_t> frame(samples_per_frame);
    std::chrono::steady_clock::time_point next_frame = std::chrono::steady_clock::now();
    printf("Sending generated 48 kHz mono PCM audio\n");

    while (sending_)
    {
        for (int index = 0; index < samples_per_frame; ++index)
        {
            frame[index] = static_cast<int16_t>(amplitude * std::sin(phase));
            phase += phase_step;
            if (phase >= two_pi)
            {
                phase -= two_pi;
            }
        }

        const ZoomVideoSDKErrors error = SendRawAudio(
            reinterpret_cast<char*>(frame.data()),
            static_cast<unsigned int>(frame.size() * sizeof(int16_t)),
            sample_rate);
        if (error != ZoomVideoSDKErrors_Success)
        {
            std::cerr << "Failed to send raw audio frame. Error code: " << error << std::endl;
            sending_ = false;
            break;
        }

        next_frame += std::chrono::milliseconds(10);
        std::this_thread::sleep_until(next_frame);
    }
}
