#include "ConsoleVideoSDKApp.h"
#include "helpers/zoom_video_sdk_annotation_helper_interface.h"
#include "helpers/zoom_video_sdk_share_helper_interface.h"
#include "zoom_video_sdk_session_info_interface.h"

#include <iostream>
#include <string>

using namespace ZOOMVIDEOSDK;

class AnnotationFeature : public ConsoleVideoSDKFeature
{
public:
    explicit AnnotationFeature(const FlatJsonConfig& config)
        : monitor_id_(config.GetString("monitorId")),
          tool_color_(static_cast<unsigned long>(config.GetInt("toolColor", 255))),
          tool_width_(config.GetInt("toolWidth", 3)),
          disable_viewer_annotation_(config.GetBool("disableViewerAnnotation"))
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
        if (!share_helper_->isAnnotationFeatureSupport())
        {
            std::cerr << "Annotation is not supported in this session." << std::endl;
            return;
        }
        if (monitor_id_.empty())
        {
            std::cerr << "monitorId is required in config.json." << std::endl;
            return;
        }

        ZoomVideoSDKShareOption option(false, false);
        const ZoomVideoSDKErrors error =
            share_helper_->startShareScreen(monitor_id_.c_str(), option);
        std::cout << "startShareScreen returned " << error << std::endl;
    }

    void OnSessionLeaving() override
    {
        DestroyAnnotationHelper();
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
        std::cout << "Share status changed: "
                  << action->getShareStatus() << std::endl;
        if (!IsSelf(user))
        {
            return;
        }
        if (action->getShareStatus() == ZoomVideoSDKShareStatus_Start)
        {
            StartAnnotation();
        }
        else if (action->getShareStatus() == ZoomVideoSDKShareStatus_Stop)
        {
            DestroyAnnotationHelper();
        }
    }

    void onAnnotationHelperCleanUp(IZoomVideoSDKAnnotationHelper* helper) override
    {
        if (helper == annotation_helper_)
        {
            annotation_helper_ = nullptr;
        }
        std::cout << "Annotation helper cleaned up by SDK" << std::endl;
    }

    void onAnnotationPrivilegeChange(
        IZoomVideoSDKUser* user,
        IZoomVideoSDKShareAction*) override
    {
        std::cout << "Annotation privilege changed for "
                  << (user ? user->getUserName() : "unknown") << std::endl;
    }

    void onAnnotationToolTypeChanged(
        IZoomVideoSDKAnnotationHelper*,
        void*,
        ZoomVideoSDKAnnotationToolType tool_type) override
    {
        std::cout << "Annotation tool changed to " << tool_type << std::endl;
    }

private:
    bool IsSelf(IZoomVideoSDKUser* user) const
    {
        IZoomVideoSDKSession* session = sdk()->getSessionInfo();
        return session && user && user == session->getMyself();
    }

    void StartAnnotation()
    {
        if (annotation_helper_)
        {
            return;
        }
        const ZoomVideoSDKErrors permission_error =
            share_helper_->disableViewerAnnotation(disable_viewer_annotation_);
        std::cout << "disableViewerAnnotation returned "
                  << permission_error << std::endl;

        annotation_helper_ = share_helper_->createAnnotationHelper(nullptr);
        if (!annotation_helper_)
        {
            std::cerr << "createAnnotationHelper returned null." << std::endl;
            return;
        }
        if (!annotation_helper_->canDoAnnotation())
        {
            std::cerr << "Current user cannot annotate this share." << std::endl;
            DestroyAnnotationHelper();
            return;
        }

        std::cout << "startAnnotation returned "
                  << annotation_helper_->startAnnotation() << std::endl;
        std::cout << "setToolType(Pen) returned "
                  << annotation_helper_->setToolType(ZoomVideoSDKAnnotationToolType_Pen)
                  << std::endl;
        std::cout << "setToolColor returned "
                  << annotation_helper_->setToolColor(tool_color_) << std::endl;
        std::cout << "setToolWidth returned "
                  << annotation_helper_->setToolWidth(tool_width_) << std::endl;
    }

    void DestroyAnnotationHelper()
    {
        if (!annotation_helper_)
        {
            return;
        }
        annotation_helper_->stopAnnotation();
        IZoomVideoSDKAnnotationHelper* helper = annotation_helper_;
        annotation_helper_ = nullptr;
        if (share_helper_)
        {
            std::cout << "destroyAnnotationHelper returned "
                      << share_helper_->destroyAnnotationHelper(helper) << std::endl;
        }
    }

    std::string monitor_id_;
    unsigned long tool_color_;
    long tool_width_;
    bool disable_viewer_annotation_;
    IZoomVideoSDKShareHelper* share_helper_ = nullptr;
    IZoomVideoSDKAnnotationHelper* annotation_helper_ = nullptr;
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
    AnnotationFeature feature(config);
    return RunConsoleVideoSDKSample(config, feature);
}
