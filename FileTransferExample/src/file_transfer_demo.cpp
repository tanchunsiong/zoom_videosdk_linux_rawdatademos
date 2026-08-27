#include "ConsoleVideoSDKApp.h"
#include "zoom_video_sdk_session_info_interface.h"

#include <filesystem>
#include <iostream>
#include <string>

using namespace ZOOMVIDEOSDK;

class FileTransferFeature : public ConsoleVideoSDKFeature
{
public:
    explicit FileTransferFeature(const FlatJsonConfig& config)
        : file_path_(config.GetString("filePath")),
          download_directory_(config.GetString("downloadDirectory", "downloads")),
          auto_accept_(config.GetBool("autoAcceptIncomingFiles", true))
    {
    }

    void OnSessionJoined() override
    {
        IZoomVideoSDKSession* session = sdk()->getSessionInfo();
        if (!session)
        {
            std::cerr << "Session information is unavailable." << std::endl;
            return;
        }
        if (!session->isFileTransferEnable())
        {
            std::cerr << "File transfer is disabled for this session." << std::endl;
            return;
        }

        const char* allowed_types = session->getTransferFileTypeWhiteList();
        std::cout << "File transfer enabled; maximum size="
                  << session->getMaxTransferFileSize()
                  << " bytes; allowed types="
                  << (allowed_types ? allowed_types : "not reported")
                  << std::endl;

        if (!file_path_.empty())
        {
            if (!std::filesystem::is_regular_file(file_path_))
            {
                std::cerr << "Configured filePath is not a regular file: " << file_path_ << std::endl;
                return;
            }
            const ZoomVideoSDKErrors error = session->transferFile(file_path_.c_str());
            std::cout << "transferFile to all participants returned " << error << std::endl;
        }
        else
        {
            std::cout << "No filePath configured; waiting to receive files." << std::endl;
        }
    }

    void onSendFileStatus(IZoomVideoSDKSendFile* file, const FileTransferStatus& status) override
    {
        if (!file)
        {
            return;
        }
        const ZoomVideoSDKFileStatus details = file->getStatus();
        std::cout << "Send status: file=" << file->getFileName()
                  << " state=" << status
                  << " progress=" << details.transProgress.ratio << "%"
                  << " bytes=" << details.transProgress.completeSize
                  << std::endl;
    }

    void onReceiveFileStatus(IZoomVideoSDKReceiveFile* file, const FileTransferStatus& status) override
    {
        if (!file)
        {
            return;
        }

        std::cout << "Receive status: file=" << file->getFileName()
                  << " state=" << status << std::endl;
        if (status != FileTransferState_ReadyToTransfer || !auto_accept_)
        {
            return;
        }

        std::error_code filesystem_error;
        std::filesystem::create_directories(download_directory_, filesystem_error);
        if (filesystem_error)
        {
            std::cerr << "Unable to create download directory: "
                      << filesystem_error.message() << std::endl;
            return;
        }

        const char* file_name = file->getFileName();
        if (!file_name || !*file_name)
        {
            std::cerr << "Incoming file has no usable name." << std::endl;
            return;
        }
        const std::string safe_name =
            std::filesystem::path(file_name).filename().string();
        const std::filesystem::path destination =
            std::filesystem::path(download_directory_) / safe_name;
        const ZoomVideoSDKErrors error = file->startReceive(destination.c_str());
        std::cout << "startReceive(" << destination.string() << ") returned " << error << std::endl;
    }

private:
    std::string file_path_;
    std::string download_directory_;
    bool auto_accept_;
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
    FileTransferFeature feature(config);
    return RunConsoleVideoSDKSample(config, feature);
}
