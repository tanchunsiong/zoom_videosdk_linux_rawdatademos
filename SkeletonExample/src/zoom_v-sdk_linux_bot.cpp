#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <sstream>
#include <thread>

#include "glib.h"
#include "json.hpp"

// Conditionally include GUI headers
#if BUILD_GUI
#include <gtkmm.h>
#include <SDL2/SDL.h>
#include "VideoRenderer.h"
#endif

#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "zoom_video_sdk_api.h"
#include "zoom_video_sdk_def.h"
#include "zoom_video_sdk_delegate_interface.h"
#include "zoom_video_sdk_interface.h"
#include "zoom_video_sdk_session_info_interface.h"

#include "zoom_video_sdk_platform.h"

//needed for chat
#include "helpers/zoom_video_sdk_chat_helper_interface.h"
#include "zoom_video_sdk_chat_message_interface.h"

#include "WebService.h"

using Json = nlohmann::json;
USING_ZOOM_VIDEO_SDK_NAMESPACE
IZoomVideoSDK* video_sdk_obj;
GMainLoop* loop;

//these are controls to demonstrate the flow
bool enableChat = true;
bool getSignatureFromWebService = true;

// Global variables
bool g_in_session = false;
bool g_audio_muted = true;
bool g_video_muted = false;

#if BUILD_GUI
// Global UI variables (GUI only)
Gtk::TextView* g_status_view = nullptr;
Gtk::Entry* g_username_entry = nullptr;
Gtk::Entry* g_session_name_entry = nullptr;
Gtk::Entry* g_session_password_entry = nullptr;
Gtk::Entry* g_signature_entry = nullptr;
Gtk::Button* g_join_button = nullptr;
Gtk::Button* g_leave_button = nullptr;
Gtk::Button* g_mute_audio_button = nullptr;
Gtk::Button* g_mute_video_button = nullptr;
VideoRenderer* g_video_renderer = nullptr;
#endif

std::string getSelfDirPath()
{
	char dest[PATH_MAX];
	memset(dest, 0, sizeof(dest)); // readlink does not null terminate!
	if (readlink("/proc/self/exe", dest, PATH_MAX) == -1)
	{
	}

	char* tmp = strrchr(dest, '/');
	if (tmp)
		*tmp = 0;
	printf("getpath\n");
	return std::string(dest);
}

#if BUILD_GUI
// UI Helper Functions (GUI only)
void updateStatus(const std::string& message)
{
    if (g_status_view)
    {
        Glib::RefPtr<Gtk::TextBuffer> buffer = g_status_view->get_buffer();
        buffer->set_text(message);
    }
}

void updateButtonStates()
{
    if (g_join_button) g_join_button->set_sensitive(!g_in_session);
    if (g_leave_button) g_leave_button->set_sensitive(g_in_session);
    if (g_mute_audio_button) g_mute_audio_button->set_sensitive(g_in_session);
    if (g_mute_video_button) g_mute_video_button->set_sensitive(g_in_session);
    
    if (g_mute_audio_button)
        g_mute_audio_button->set_label(g_audio_muted ? "Unmute Audio" : "Mute Audio");
    if (g_mute_video_button)
        g_mute_video_button->set_label(g_video_muted ? "Unmute Video" : "Mute Video");
}
#else
// Console versions of UI functions
void updateStatus(const std::string& message)
{
    printf("Status: %s\n", message.c_str());
}

void updateButtonStates()
{
    // No-op for console version
}
#endif

class ZoomVideoSDKDelegate :
	public IZoomVideoSDKDelegate
{
public:
	/// \brief Triggered when user enter the session.
	virtual void onSessionJoin()
	{
		printf("Joined session successfully\n");
		
		// Update UI on main thread
		g_idle_add([](gpointer data) -> gboolean {
			g_in_session = true;
			updateStatus("Session joined successfully");
			updateButtonStates();
			return G_SOURCE_REMOVE;
		}, nullptr);

		if (enableChat) {
			IZoomVideoSDKChatHelper* pChatHelper = video_sdk_obj->getChatHelper();
			if (pChatHelper->isChatDisabled() == false && pChatHelper->isPrivateChatDisabled() == false) {
				ZoomVideoSDKErrors err = pChatHelper->sendChatToAll("hello world");
				printf("Sending chat message status: %d\n", (int)err);
			}
		}
	}

	/// \brief Triggered when session leaveSession
	virtual void onSessionLeave()
	{
		printf("Left session.\n");
		
		// Update UI on main thread
		g_idle_add([](gpointer data) -> gboolean {
			g_in_session = false;
			updateStatus("Left session");
			updateButtonStates();
			return G_SOURCE_REMOVE;
		}, nullptr);
	};

	virtual void onError(ZoomVideoSDKErrors errorCode, int detailErrorCode)
	{
		printf("join session errorCode : %d  detailErrorCode: %d\n", errorCode, detailErrorCode);
		
		// Update UI on main thread
		g_idle_add([](gpointer data) -> gboolean {
			updateStatus("Session error occurred");
			return G_SOURCE_REMOVE;
		}, nullptr);
	};


	virtual void onUserJoin(IZoomVideoSDKUserHelper* pUserHelper, IVideoSDKVector<IZoomVideoSDKUser*>* userList)
	{
	
	};

	virtual void onUserLeave(IZoomVideoSDKUserHelper* pUserHelper, IVideoSDKVector<IZoomVideoSDKUser*>* userList)
	{
	
	};

	virtual void onUserVideoStatusChanged(IZoomVideoSDKVideoHelper* pVideoHelper,
		IVideoSDKVector<IZoomVideoSDKUser*>* userList) {};


	virtual void onUserAudioStatusChanged(IZoomVideoSDKAudioHelper* pAudioHelper,
		IVideoSDKVector<IZoomVideoSDKUser*>* userList) {

	};
	virtual void onUserShareStatusChanged(IZoomVideoSDKShareHelper* pShareHelper, IZoomVideoSDKUser* pUser, ZoomVideoSDKShareStatus status, ZoomVideoSDKShareType type) {

	}

	virtual void onUserRecordingConsent(IZoomVideoSDKUser* pUser) { };


	virtual void onLiveStreamStatusChanged(IZoomVideoSDKLiveStreamHelper* pLiveStreamHelper,
		ZoomVideoSDKLiveStreamStatus status) {};


	virtual void onChatNewMessageNotify(IZoomVideoSDKChatHelper* pChatHelper, IZoomVideoSDKChatMessage* messageItem) {

		if (enableChat) {
			if (!messageItem)
				return;


			const zchar_t* szMessageContent = messageItem->getContent();


			IZoomVideoSDKUser* pRecievingUser = messageItem->getReceiveUser();
			IZoomVideoSDKUser* pSendingUser = messageItem->getSendUser();
			const zchar_t* sendUserName = pSendingUser->getUserName();
			const zchar_t* recUserName = "all";
			if (pRecievingUser) {
				recUserName = pRecievingUser->getUserName();
			}
			printf("New message from %s to %s  %s\n", sendUserName, recUserName, szMessageContent);



			messageItem->isChatToAll(); // Returns false for private messages.
			messageItem->isSelfSend(); // Returns true if the current user sent the message.
			messageItem->getTimeStamp(); // The time at which the message was sent.
			messageItem->getReceiveUser(); // The recipient of a private message.


		}

	};


	virtual void onUserHostChanged(IZoomVideoSDKUserHelper* pUserHelper, IZoomVideoSDKUser* pUser) {};


	virtual void onUserActiveAudioChanged(IZoomVideoSDKAudioHelper* pAudioHelper,
		IVideoSDKVector<IZoomVideoSDKUser*>* list) {};


	virtual void onSessionNeedPassword(IZoomVideoSDKPasswordHandler* handler) {};


	virtual void onSessionPasswordWrong(IZoomVideoSDKPasswordHandler* handler) {};

	//this is a helper method, and not part of the implementation
	void savePcmBufferToFile(const std::string& filename, char* pcmBuffer, std::size_t bufferSize) {
		std::ofstream outfile(filename, std::ios::out | std::ios::binary | std::ios::app);
		outfile.write(reinterpret_cast<char*>(pcmBuffer), bufferSize);
		outfile.close();
		if (!outfile) {
			std::cerr << "Error writing PCM data to file!" << std::endl;
		}
		else {
			std::cout << "PCM data saved to file: " << filename << std::endl;
		}
	}

	virtual void onMixedAudioRawDataReceived(AudioRawData* data_) {
	};


	virtual void onOneWayAudioRawDataReceived(AudioRawData* data_, IZoomVideoSDKUser* pUser) {

	};

	virtual void onSharedAudioRawDataReceived(AudioRawData* data_) {};


	virtual void onUserManagerChanged(IZoomVideoSDKUser* pUser) {};


	virtual void onUserNameChanged(IZoomVideoSDKUser* pUser) {};


	virtual void onCameraControlRequestResult(IZoomVideoSDKUser* pUser, bool isApproved) {};

	virtual void onCameraControlRequestReceived(
		IZoomVideoSDKUser* pUser,
		ZoomVideoSDKCameraControlRequestType requestType,
		IZoomVideoSDKCameraControlRequestHandler* pCameraControlRequestHandler) {};


	virtual void onCommandReceived(IZoomVideoSDKUser* sender, const zchar_t* strCmd) {
	
	}
	virtual void onCommandChannelConnectResult(bool isSuccess) {

	};
	virtual void onInviteByPhoneStatus(PhoneStatus status, PhoneFailedReason reason) {};
	virtual void onCalloutJoinSuccess(IZoomVideoSDKUser* pUser, const zchar_t* phoneNumber) {};

	virtual void onCloudRecordingStatus(RecordingStatus status, IZoomVideoSDKRecordingConsentHandler* pHandler) {

	};
	virtual void onHostAskUnmute() {};


	virtual void onMultiCameraStreamStatusChanged(ZoomVideoSDKMultiCameraStreamStatus status, IZoomVideoSDKUser* pUser, IZoomVideoSDKRawDataPipe* pVideoPipe) {}
	virtual void onMicSpeakerVolumeChanged(unsigned int micVolume, unsigned int speakerVolume) {}
	virtual void onAudioDeviceStatusChanged(ZoomVideoSDKAudioDeviceType type, ZoomVideoSDKAudioDeviceStatus status) {}
	virtual void onTestMicStatusChanged(ZoomVideoSDK_TESTMIC_STATUS status) {}
	virtual void onSelectedAudioDeviceChanged() {}
	virtual void onCameraListChanged() {}
	virtual void onLiveTranscriptionStatus(ZoomVideoSDKLiveTranscriptionStatus status) {
	
	};
	virtual void onLiveTranscriptionMsgReceived(const zchar_t* ltMsg, IZoomVideoSDKUser* pUser, ZoomVideoSDKLiveTranscriptionOperationType type) {

	};
	virtual void onLiveTranscriptionMsgInfoReceived(ILiveTranscriptionMessageInfo* messageInfo) {
	
	};
	virtual void onLiveTranscriptionMsgError(ILiveTranscriptionLanguage* spokenLanguage, ILiveTranscriptionLanguage* transcriptLanguage) {
	
	};
	virtual void onChatMsgDeleteNotification(IZoomVideoSDKChatHelper* pChatHelper, const zchar_t* msgID, ZoomVideoSDKChatMessageDeleteType deleteBy) {
		if (enableChat) {


		}
	};


	virtual void onVirtualSpeakerMixedAudioReceived(AudioRawData* data_) {

		printf("onVirtualSpeakerMixedAudioReceived() main \n");
		printf("data %s \n", data_->GetBuffer());

	};

	virtual void onVirtualSpeakerOneWayAudioReceived(AudioRawData* data_, IZoomVideoSDKUser* pUser) {

		printf("onVirtualSpeakerOneWayAudioReceived() main\n");
		printf("data %s \n", data_->GetBuffer());
	};

	virtual void onVirtualSpeakerSharedAudioReceived(AudioRawData* data_) {
		printf("onVirtualSpeakerSharedAudioReceived() main\n");
		printf("data %s \n", data_->GetBuffer());
	};

	virtual void onOriginalLanguageMsgReceived(ILiveTranscriptionMessageInfo* messageInfo) {};
	virtual void onChatPrivilegeChanged(IZoomVideoSDKChatHelper* pChatHelper, ZoomVideoSDKChatPrivilegeType privilege) {
		if (enableChat) {}
	};
	virtual void onSendFileStatus(IZoomVideoSDKSendFile* file, const FileTransferStatus& status) {};
	virtual void onReceiveFileStatus(IZoomVideoSDKReceiveFile* file, const FileTransferStatus& status) {};


	virtual void onProxyDetectComplete() {};
	virtual void onProxySettingNotification(IZoomVideoSDKProxySettingHandler* handler) {};
	virtual void onSSLCertVerifiedFailNotification(IZoomVideoSDKSSLCertificateInfo* info) {};

	virtual void onUserVideoNetworkStatusChanged(ZoomVideoSDKNetworkStatus status, IZoomVideoSDKUser* pUser) {};
	virtual void onCallCRCDeviceStatusChanged(ZoomVideoSDKCRCCallStatus status) {};


	virtual void onVideoCanvasSubscribeFail(ZoomVideoSDKSubscribeFailReason fail_reason, IZoomVideoSDKUser* pUser, void* handle) {};
	virtual void onShareCanvasSubscribeFail(ZoomVideoSDKSubscribeFailReason fail_reason, IZoomVideoSDKUser* pUser, void* handle) {};
	virtual void onAnnotationHelperCleanUp(IZoomVideoSDKAnnotationHelper* helper) {};
	virtual void onAnnotationPrivilegeChange(IZoomVideoSDKUser* pUser, bool enable) {};
	virtual void onAnnotationHelperActived(void* handle) {};
	virtual void onVideoAlphaChannelStatusChanged(bool isAlphaModeOn) {};
};



void joinVideoSDKSession(std::string& session_name, std::string& session_psw, std::string& session_token)
{
	ZoomVideoSDKRawDataMemoryMode heap = ZoomVideoSDKRawDataMemoryMode::ZoomVideoSDKRawDataMemoryModeHeap;
	video_sdk_obj = CreateZoomVideoSDKObj();
	ZoomVideoSDKInitParams init_params;
	init_params.domain = "https://go.zoom.us";
	init_params.enableLog = true;
	init_params.logFilePrefix = "zoom_videosdk_demo";
	init_params.videoRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
	init_params.shareRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
	init_params.audioRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
	init_params.enableIndirectRawdata = false;

	ZoomVideoSDKErrors err = video_sdk_obj->initialize(init_params);
	if (err != ZoomVideoSDKErrors_Success)
	{
		return;
	}
	IZoomVideoSDKDelegate* listener = new ZoomVideoSDKDelegate();
	video_sdk_obj->addListener(dynamic_cast<IZoomVideoSDKDelegate*>(listener));

	ZoomVideoSDKSessionContext session_context;
	session_context.sessionName = session_name.c_str();
	session_context.sessionPassword = session_psw.c_str();
	session_context.userName = "Linux Bot";
	session_context.token = session_token.c_str();
	session_context.videoOption.localVideoOn = true;
	session_context.audioOption.connect = false;
	session_context.audioOption.mute = true;


	//join the session
	IZoomVideoSDKSession* session = NULL;
	if (video_sdk_obj)
		session = video_sdk_obj->joinSession(session_context);

}

#if BUILD_GUI
// UI Callback Functions (GUI only)
void on_join_session_clicked()
{
    if (!g_username_entry || !g_session_name_entry || !g_signature_entry)
        return;
        
    std::string username = g_username_entry->get_text();
    std::string session_name = g_session_name_entry->get_text();
    std::string session_password = g_session_password_entry ? g_session_password_entry->get_text() : "";
    std::string signature = g_signature_entry->get_text();
    
    if (username.empty() || session_name.empty() || signature.empty())
    {
        updateStatus("Please fill in all required fields");
        return;
    }
    
    updateStatus("Joining session...");
    
    // Initialize Video SDK if not already done
    if (!video_sdk_obj)
    {
        ZoomVideoSDKRawDataMemoryMode heap = ZoomVideoSDKRawDataMemoryMode::ZoomVideoSDKRawDataMemoryModeHeap;
        video_sdk_obj = CreateZoomVideoSDKObj();
        ZoomVideoSDKInitParams init_params;
        init_params.domain = "https://go.zoom.us";
        init_params.enableLog = true;
        init_params.logFilePrefix = "zoom_videosdk_demo";
        init_params.videoRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
        init_params.shareRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
        init_params.audioRawDataMemoryMode = ZoomVideoSDKRawDataMemoryModeHeap;
        init_params.enableIndirectRawdata = false;

        ZoomVideoSDKErrors err = video_sdk_obj->initialize(init_params);
        if (err != ZoomVideoSDKErrors_Success)
        {
            updateStatus("Failed to initialize Video SDK");
            return;
        }
        IZoomVideoSDKDelegate* listener = new ZoomVideoSDKDelegate();
        video_sdk_obj->addListener(dynamic_cast<IZoomVideoSDKDelegate*>(listener));
    }
    
    // Join session
    ZoomVideoSDKSessionContext session_context;
    session_context.sessionName = session_name.c_str();
    session_context.sessionPassword = session_password.c_str();
    session_context.userName = username.c_str();
    session_context.token = signature.c_str();
    session_context.videoOption.localVideoOn = !g_video_muted;
    session_context.audioOption.connect = true;
    session_context.audioOption.mute = g_audio_muted;

    IZoomVideoSDKSession* session = video_sdk_obj->joinSession(session_context);
    if (session)
    {
        g_in_session = true;
        updateButtonStates();
        updateStatus("Session joined successfully");
    }
    else
    {
        updateStatus("Failed to join session");
    }
}

void on_leave_session_clicked()
{
    if (video_sdk_obj && g_in_session)
    {
        video_sdk_obj->leaveSession(false);
        g_in_session = false;
        updateButtonStates();
        updateStatus("Left session");
    }
}

void on_mute_audio_clicked()
{
    if (video_sdk_obj && g_in_session)
    {
        IZoomVideoSDKAudioHelper* audioHelper = video_sdk_obj->getAudioHelper();
        if (audioHelper)
        {
            // Get current user for mute/unmute operations
           

            IZoomVideoSDKSession* m_sessionhelper = video_sdk_obj->getSessionInfo();
		
            if (m_sessionhelper)
            {
                IZoomVideoSDKUser* currentUser = m_sessionhelper->getMyself();
              
                if (currentUser)
                {
                    if (g_audio_muted)
                        audioHelper->unMuteAudio(currentUser);
                    else
                        audioHelper->muteAudio(currentUser);
                    g_audio_muted = !g_audio_muted;
                    updateButtonStates();
                }
            }
        }
    }
}

void on_mute_video_clicked()
{
    if (video_sdk_obj && g_in_session)
    {
        IZoomVideoSDKVideoHelper* videoHelper = video_sdk_obj->getVideoHelper();
        if (videoHelper)
        {
            if (g_video_muted)
                videoHelper->startVideo();
            else
                videoHelper->stopVideo();
            g_video_muted = !g_video_muted;
            updateButtonStates();
        }
    }
}
#endif

gboolean timeout_callback(gpointer data)
{
	return TRUE;
}

void my_handler(int s)
{
	printf("\nCaught signal %d\n", s);
	
	printf("\Leaving Session\n");
	if (video_sdk_obj)
	{
		video_sdk_obj->leaveSession(false);
		printf("\Left Session\n");
		
		printf("\Cleaning up SDK\n");
		video_sdk_obj->cleanup();
		printf("\Cleaned up SDK\n");

		printf("\Destroying SDK Object\n");
		DestroyZoomVideoSDKObj();
		printf("\Destroyed SDK Object\n");
	}
}

#if BUILD_GUI
// GUI version of main function
int main(int argc, char* argv[])
{
    // Initialize GTKmm application
    auto app = Gtk::Application::create(argc, argv);

    // Create main window
    Gtk::Window window;
    window.set_default_size(800, 600);
    window.set_title("Zoom Video SDK Demo");

    // Create main vertical layout container
    Gtk::Box main_box(Gtk::ORIENTATION_VERTICAL, 10);
    main_box.set_margin_left(10);
    main_box.set_margin_right(10);
    main_box.set_margin_top(10);
    main_box.set_margin_bottom(10);
    window.add(main_box);

    // Create input section
    Gtk::Frame input_frame("Session Information");
    Gtk::Box input_box(Gtk::ORIENTATION_VERTICAL, 5);
    input_box.set_margin_left(10);
    input_box.set_margin_right(10);
    input_box.set_margin_top(10);
    input_box.set_margin_bottom(10);
    input_frame.add(input_box);

    // Username input
    Gtk::Box username_box(Gtk::ORIENTATION_HORIZONTAL, 5);
    Gtk::Label username_label("Username:");
    username_label.set_size_request(120, -1);
    Gtk::Entry username_entry;
    username_entry.set_text("Linux User");
    g_username_entry = &username_entry;
    username_box.pack_start(username_label, Gtk::PACK_SHRINK);
    username_box.pack_start(username_entry);
    input_box.pack_start(username_box, Gtk::PACK_SHRINK);

    // Session name input
    Gtk::Box session_name_box(Gtk::ORIENTATION_HORIZONTAL, 5);
    Gtk::Label session_name_label("Session Name:");
    session_name_label.set_size_request(120, -1);
    Gtk::Entry session_name_entry;
    g_session_name_entry = &session_name_entry;
    session_name_box.pack_start(session_name_label, Gtk::PACK_SHRINK);
    session_name_box.pack_start(session_name_entry);
    input_box.pack_start(session_name_box, Gtk::PACK_SHRINK);

    // Session password input
    Gtk::Box session_password_box(Gtk::ORIENTATION_HORIZONTAL, 5);
    Gtk::Label session_password_label("Password:");
    session_password_label.set_size_request(120, -1);
    Gtk::Entry session_password_entry;
    g_session_password_entry = &session_password_entry;
    session_password_box.pack_start(session_password_label, Gtk::PACK_SHRINK);
    session_password_box.pack_start(session_password_entry);
    input_box.pack_start(session_password_box, Gtk::PACK_SHRINK);

    // Signature input
    Gtk::Box signature_box(Gtk::ORIENTATION_HORIZONTAL, 5);
    Gtk::Label signature_label("Signature:");
    signature_label.set_size_request(120, -1);
    Gtk::Entry signature_entry;
    g_signature_entry = &signature_entry;
    signature_box.pack_start(signature_label, Gtk::PACK_SHRINK);
    signature_box.pack_start(signature_entry);
    input_box.pack_start(signature_box, Gtk::PACK_SHRINK);

    main_box.pack_start(input_frame, Gtk::PACK_SHRINK);

    // Create control buttons section
    Gtk::Frame control_frame("Session Controls");
    Gtk::Box control_box(Gtk::ORIENTATION_HORIZONTAL, 10);
    control_box.set_margin_left(10);
    control_box.set_margin_right(10);
    control_box.set_margin_top(10);
    control_box.set_margin_bottom(10);
    control_frame.add(control_box);

    // Join button
    Gtk::Button join_button("Join Session");
    join_button.set_size_request(120, 40);
    g_join_button = &join_button;
    join_button.signal_clicked().connect(sigc::ptr_fun(on_join_session_clicked));
    control_box.pack_start(join_button, Gtk::PACK_SHRINK);

    // Leave button
    Gtk::Button leave_button("Leave Session");
    leave_button.set_size_request(120, 40);
    leave_button.set_sensitive(false);
    g_leave_button = &leave_button;
    leave_button.signal_clicked().connect(sigc::ptr_fun(on_leave_session_clicked));
    control_box.pack_start(leave_button, Gtk::PACK_SHRINK);

    // Mute audio button
    Gtk::Button mute_audio_button("Unmute Audio");
    mute_audio_button.set_size_request(120, 40);
    mute_audio_button.set_sensitive(false);
    g_mute_audio_button = &mute_audio_button;
    mute_audio_button.signal_clicked().connect(sigc::ptr_fun(on_mute_audio_clicked));
    control_box.pack_start(mute_audio_button, Gtk::PACK_SHRINK);

    // Mute video button
    Gtk::Button mute_video_button("Mute Video");
    mute_video_button.set_size_request(120, 40);
    mute_video_button.set_sensitive(false);
    g_mute_video_button = &mute_video_button;
    mute_video_button.signal_clicked().connect(sigc::ptr_fun(on_mute_video_clicked));
    control_box.pack_start(mute_video_button, Gtk::PACK_SHRINK);

    main_box.pack_start(control_frame, Gtk::PACK_SHRINK);

    // Create status display section
    Gtk::Frame status_frame("Status");
    Gtk::ScrolledWindow status_scrolled;
    status_scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    status_scrolled.set_size_request(-1, 100);
    Gtk::TextView status_view;
    status_view.set_editable(false);
    g_status_view = &status_view;
    status_scrolled.add(status_view);
    status_frame.add(status_scrolled);
    main_box.pack_start(status_frame, Gtk::PACK_SHRINK);

    // Create video display section
    Gtk::Frame video_frame("Video Display");
    Gtk::Box video_box(Gtk::ORIENTATION_VERTICAL);
    video_box.set_size_request(640, 480);
    Gtk::Label video_placeholder("Video will appear here when session is joined");
    video_placeholder.set_justify(Gtk::JUSTIFY_CENTER);
    video_box.pack_start(video_placeholder);
    video_frame.add(video_box);
    main_box.pack_start(video_frame);

    // Initialize video renderer
    g_video_renderer = new VideoRenderer();
    if (!g_video_renderer->Init())
    {
        std::cerr << "Failed to initialize video renderer" << std::endl;
    }

    // Load default values from config.json if available
    std::string self_dir = getSelfDirPath();
    self_dir.append("/config.json");
    std::ifstream config_file(self_dir.c_str());
    if (config_file.good())
    {
        config_file.seekg(0, std::ios::end);
        size_t size = config_file.tellg();
        std::string buffer(size, ' ');
        config_file.seekg(0);
        config_file.read(&buffer[0], size);

        try
        {
            Json config_json = Json::parse(buffer);
            if (!config_json.is_null())
            {
                Json json_name = config_json["session_name"];
                Json json_psw = config_json["session_psw"];
                Json json_token = config_json["token"];
                
                if (!json_name.is_null())
                    session_name_entry.set_text(json_name.get<std::string>());
                if (!json_psw.is_null())
                    session_password_entry.set_text(json_psw.get<std::string>());
                if (!json_token.is_null())
                    signature_entry.set_text(json_token.get<std::string>());
            }
        }
        catch (Json::parse_error& ex)
        {
            // Ignore config file parsing errors
        }
    }

    // Set up signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Initial status
    updateStatus("Ready to join session");
    updateButtonStates();

    // Show all widgets
    window.show_all();

    // Run GTKmm application
    return app->run(window);
}

#else
// Console version of main function (original behavior)
int main(int argc, char* argv[])
{
    // Set up signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Load session parameters from config.json
    std::string session_name;
    std::string session_psw;
    std::string session_token;

    std::string self_dir = getSelfDirPath();
    self_dir.append("/config.json");
    std::ifstream config_file(self_dir.c_str());
    if (config_file.good())
    {
        config_file.seekg(0, std::ios::end);
        size_t size = config_file.tellg();
        std::string buffer(size, ' ');
        config_file.seekg(0);
        config_file.read(&buffer[0], size);

        try
        {
            Json config_json = Json::parse(buffer);
            if (!config_json.is_null())
            {
                Json json_name = config_json["session_name"];
                Json json_psw = config_json["session_psw"];
                Json json_token = config_json["token"];
                
                if (!json_name.is_null())
                    session_name = json_name.get<std::string>();
                if (!json_psw.is_null())
                    session_psw = json_psw.get<std::string>();
                if (!json_token.is_null())
                    session_token = json_token.get<std::string>();
            }
        }
        catch (Json::parse_error& ex)
        {
            printf("Error parsing config.json: %s\n", ex.what());
            return 1;
        }
    }
    else
    {
        printf("Config file not found: %s\n", self_dir.c_str());
        return 1;
    }

    if (session_name.empty() || session_token.empty())
    {
        printf("Missing required session parameters in config.json\n");
        return 1;
    }

    printf("Starting Zoom Video SDK Console Demo\n");
    printf("Session: %s\n", session_name.c_str());
    
    // Join the session
    joinVideoSDKSession(session_name, session_psw, session_token);

    // Start GLib main loop for handling SDK callbacks
    loop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(1000, timeout_callback, NULL);
    g_main_loop_run(loop);

    return 0;
}
#endif
