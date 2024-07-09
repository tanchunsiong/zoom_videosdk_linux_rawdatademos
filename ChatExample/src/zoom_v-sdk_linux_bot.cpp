#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "glib.h"
#include "json.hpp"
#include "helpers/zoom_video_sdk_user_helper_interface.h"
#include "zoom_video_sdk_api.h"
#include "zoom_video_sdk_def.h"
#include "zoom_video_sdk_delegate_interface.h"
#include "zoom_video_sdk_interface.h"

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

class ZoomVideoSDKDelegate : 
	public IZoomVideoSDKDelegate
{
public:
	/// \brief Triggered when user enter the session.
	virtual void onSessionJoin()
	{
		printf("Joined session successfully\n");



		if (enableChat) {

			IZoomVideoSDKChatHelper* pChatHelper = video_sdk_obj->getChatHelper();

			// Check if chat is enabled in this session.
			if (pChatHelper->isChatDisabled() == false && pChatHelper->isPrivateChatDisabled() == false) {

				// Send message to User.
				ZoomVideoSDKErrors err=	pChatHelper->sendChatToAll("hello world");
				printf("Sending chat message status: %s\n", err);
			}

		}

	}



	/// \brief Triggered when session leaveSession
	virtual void onSessionLeave()
	{
		g_main_loop_unref(loop);
		printf("Already left session.\n");
		exit(1);
	};


	virtual void onError(ZoomVideoSDKErrors errorCode, int detailErrorCode)
	{
		printf("join session errorCode : %d  detailErrorCode: %d\n", errorCode, detailErrorCode);
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

gboolean timeout_callback(gpointer data)
{
	return TRUE;
}

void my_handler(int s)
{
	printf("\nCaught signal %d\n", s);
	
	printf("\Leaving Session\n");
	video_sdk_obj->leaveSession(false);
	printf("\Left Session\n");
	
	printf("\Cleaning up SDK\n");
	video_sdk_obj->cleanup();
	printf("\Cleaned up SDK\n");

	printf("\Destroying SDK Object\n");
	DestroyZoomVideoSDKObj();
	printf("\Destroyed SDK Object\n");



}

int main(int argc, char* argv[])
{
	std::string self_dir = getSelfDirPath();
	printf("self path: %s\n", self_dir.c_str());
	self_dir.append("/config.json");

	std::ifstream t(self_dir.c_str());
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);

	std::string session_name, session_psw, session_token;
	do
	{
		Json config_json;
		try
		{
			config_json = Json::parse(buffer);
			printf("config all_content: %s\n", buffer.c_str());
		}
		catch (Json::parse_error& ex)
		{
			break;
		}

		if (config_json.is_null())
		{
			break;
		}

		Json json_name = config_json["session_name"];
		Json json_psw = config_json["session_psw"];
		Json json_token = config_json["token"];
		if (!json_name.is_null())
		{
			session_name = json_name.get<std::string>();
			printf("config session_name: %s\n", session_name.c_str());
		}
		if (!json_psw.is_null())
		{
			session_psw = json_psw.get<std::string>();
			printf("config session_psw: %s\n", session_psw.c_str());
		}
		if (!json_token.is_null())
		{
			session_token = json_token.get<std::string>();
			printf("config session_token: %s\n", session_token.c_str());
		}
	} while (false);

	if (session_name.size() == 0 || session_token.size() == 0)
	{
		return 0;
	}

	if (getSignatureFromWebService) {
		session_token = GetSignatureFromWebService("https://asdc.cc/video", session_name,"1");
		printf("JWT from webservice is %s\n", session_token.c_str());
	
	}

	printf("begin to join: %s\n", self_dir.c_str());
	joinVideoSDKSession(session_name, session_psw, session_token);

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	loop = g_main_loop_new(NULL, FALSE);

	// add source to default context
	g_timeout_add(100, timeout_callback, loop);
	g_main_loop_run(loop);
	return 0;
}