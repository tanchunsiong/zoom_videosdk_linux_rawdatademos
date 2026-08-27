#pragma once

#include "VideoSDKDelegateAdapter.h"
#include "zoom_video_sdk_interface.h"

#include <map>
#include <string>

class FlatJsonConfig
{
public:
    bool Load(const std::string& path, std::string& error);
    bool Parse(const std::string& content, std::string& error);
    std::string GetString(const std::string& key, const std::string& fallback = {}) const;
    bool GetBool(const std::string& key, bool fallback = false) const;
    int GetInt(const std::string& key, int fallback = 0) const;

private:
    std::map<std::string, std::string> values_;
};

class ConsoleVideoSDKFeature : public VideoSDKDelegateAdapter
{
public:
    void Attach(ZOOMVIDEOSDK::IZoomVideoSDK* sdk);
    void onSessionJoin() final;
    void onSessionLeave() final;
    void onSessionLeave(ZOOMVIDEOSDK::ZoomVideoSDKSessionLeaveReason reason) final;
    void onError(ZOOMVIDEOSDK::ZoomVideoSDKErrors error, int detail_error) final;

    virtual void ConfigureSession(ZOOMVIDEOSDK::ZoomVideoSDKSessionContext& context);
    virtual void OnSessionJoined() = 0;
    virtual void OnSessionLeaving();
    virtual void OnSessionError(ZOOMVIDEOSDK::ZoomVideoSDKErrors error, int detail_error);

protected:
    ZOOMVIDEOSDK::IZoomVideoSDK* sdk() const;

private:
    void HandleSessionLeave();

    ZOOMVIDEOSDK::IZoomVideoSDK* sdk_ = nullptr;
    bool leave_handled_ = false;
};

std::string GetExecutableDirectory();
bool LoadRuntimeConfig(FlatJsonConfig& config, std::string& error);
int RunConsoleVideoSDKSample(const FlatJsonConfig& config, ConsoleVideoSDKFeature& feature);
