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

//needed for audio

#include "ZoomVideoSDKVirtualAudioSpeaker.h"




//needed for get raw video
#include "ZoomVideoSDKRawDataPipeDelegate.h"
#include "helpers/zoom_video_sdk_video_helper_interface.h"




#include "WebService.h"

using Json = nlohmann::json;
USING_ZOOM_VIDEO_SDK_NAMESPACE
IZoomVideoSDK* video_sdk_obj;
GMainLoop* loop;


int counter = 0;

//these are controls to demonstrate the flow
bool getRawAudio = true;
bool getRawVideo = false;
bool getRawShare = false;


bool getSignatureFromWebService = true;


std::string audioFile = "output.wav";



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
		if (getRawVideo) {
			if (userList)
			{
				int count = userList->GetCount();
				for (int index = 0; index < count; index++)
				{
					IZoomVideoSDKUser* user = userList->GetItem(index);
					if (user)
					{
						ZoomVideoSDKRawDataPipeDelegate* encoder = new ZoomVideoSDKRawDataPipeDelegate(user);
					}

				}
			}
		}


	};

	virtual void onUserLeave(IZoomVideoSDKUserHelper* pUserHelper, IVideoSDKVector<IZoomVideoSDKUser*>* userList)
	{
		if (getRawVideo) {
			if (userList)
			{
				int count = userList->GetCount();
				for (int index = 0; index < count; index++)
				{
					IZoomVideoSDKUser* user = userList->GetItem(index);
					if (user)
					{
						ZoomVideoSDKRawDataPipeDelegate::stop_encoding_for(user);
					}
				}
			}
		}

	};


	virtual void onUserVideoStatusChanged(IZoomVideoSDKVideoHelper* pVideoHelper,
		IVideoSDKVector<IZoomVideoSDKUser*>* userList) {};


	virtual void onUserAudioStatusChanged(IZoomVideoSDKAudioHelper* pAudioHelper,
		IVideoSDKVector<IZoomVideoSDKUser*>* userList) {

		if (getRawAudio) {
			IZoomVideoSDKAudioHelper* m_pAudiohelper = video_sdk_obj->getAudioHelper();
			if (m_pAudiohelper) {
				//needed for getting raw audio
				ZoomVideoSDKErrors err = m_pAudiohelper->subscribe();
				printf("subscribe status is %d\n", err);
			}
		}

	};
	virtual void onUserShareStatusChanged(IZoomVideoSDKShareHelper* pShareHelper, IZoomVideoSDKUser* pUser, ZoomVideoSDKShareStatus status, ZoomVideoSDKShareType type) {

		if (getRawShare) {
			if (status == ZoomVideoSDKShareStatus_Start) {
				ZoomVideoSDKRawDataPipeDelegate* encoder = new ZoomVideoSDKRawDataPipeDelegate(pUser, true);
			}
			else if (status == ZoomVideoSDKShareStatus_Stop) {
				ZoomVideoSDKRawDataPipeDelegate::stop_encoding_for(pUser, true);
			}

		}

	}

	virtual void onUserRecordingConsent(IZoomVideoSDKUser* pUser) { };


	virtual void onLiveStreamStatusChanged(IZoomVideoSDKLiveStreamHelper* pLiveStreamHelper,
		ZoomVideoSDKLiveStreamStatus status) {};


	virtual void onChatNewMessageNotify(IZoomVideoSDKChatHelper* pChatHelper, IZoomVideoSDKChatMessage* messageItem) {

	
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


#include <iostream>

#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"

// WAV header structure
struct WavHeader {
    char riff[4];                // "RIFF"
    uint32_t overall_size;        // File size minus 8 bytes (4 bytes each for "RIFF" and file size fields)
    char wave[4];                // "WAVE"
    char fmt_chunk_marker[4];    // "fmt " chunk
    uint32_t length_of_fmt;      // Length of the format data (usually 16 for PCM)
    uint16_t format_type;        // Format type (1 for PCM)
    uint16_t channels;           // Number of channels (1 for mono, 2 for stereo)
    uint32_t sample_rate;        // Sample rate (e.g., 44100)
    uint32_t byterate;           // Sample rate * Number of channels * Bits per sample / 8
    uint16_t block_align;        // Number of channels * Bits per sample / 8
    uint16_t bits_per_sample;    // Bits per sample (usually 16 for PCM)
    char data_chunk_header[4];   // "data" chunk
    uint32_t data_size;          // Size of the data section (number of samples * channels * bits per sample / 8)
};

// Helper function to read WAV header
bool readWavHeader(std::ifstream &file, WavHeader &header) {
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file format.\n";
        return false;
    }
    return true;
}

// Helper function to write WAV header
void writeWavHeader(std::ofstream &file, const WavHeader &header) {
    file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
}


// Function to append or create a WAV file with PCM data
// Function to append or create a WAV file with PCM data
void convertPcmBufferToWav(const char* wavFile, uint8_t* pcmBuffer, int bufferSize, int sampleRate, int channels) {
    WavHeader header;
    bool isNewFile = false;

    // Check if the file exists using fopen, a more reliable way to check file existence
    FILE *fileCheck = std::fopen(wavFile, "r");
    if (!fileCheck) {
        // If the file does not exist, create a new WAV header
        //std::cerr << "File does not exist, creating a new WAV file.\n";
        isNewFile = true;
        std::memset(&header, 0, sizeof(WavHeader));
        std::memcpy(header.riff, "RIFF", 4);
        std::memcpy(header.wave, "WAVE", 4);
        std::memcpy(header.fmt_chunk_marker, "fmt ", 4);
        header.length_of_fmt = 16;
        header.format_type = 1;  // PCM format
        header.channels = channels;
        header.sample_rate = sampleRate;
        header.bits_per_sample = 16;  // Assuming 16-bit PCM
        header.byterate = sampleRate * channels * header.bits_per_sample / 8;
        header.block_align = channels * header.bits_per_sample / 8;
        std::memcpy(header.data_chunk_header, "data", 4);
        header.data_size = 0;
        header.overall_size = 36 + header.data_size;  // 36 is the size of the rest of the header
    } else {
        // If the file exists, read the existing header
        std::fclose(fileCheck);  // Close file check
        std::ifstream wavInFile(wavFile, std::ios::binary);
        if (!readWavHeader(wavInFile, header)) {
            std::cerr << "Failed to read WAV header.\n";
            return;
        }
        wavInFile.close();
    }

    // Open the file for reading and writing (append mode or create)
    std::ofstream wavOutFile;
    if (isNewFile) {
        // Create a new file
        wavOutFile.open(wavFile, std::ios::binary | std::ios::trunc);  // Use trunc to create a new file
    } else {
        // Open the existing file for appending
        wavOutFile.open(wavFile, std::ios::binary | std::ios::in | std::ios::out);
    }

    if (!wavOutFile) {
        std::cerr << "Failed to open WAV file for writing.\n";
        return;
    }

    // Seek to the end of the current data chunk if the file exists
    if (!isNewFile) {
        wavOutFile.seekp(0, std::ios::end);
    } else {
        // Write the header if it's a new file
        writeWavHeader(wavOutFile, header);
    }

    // Append the PCM data to the file
    wavOutFile.write(reinterpret_cast<const char*>(pcmBuffer), bufferSize);

    // Update the data chunk size and overall file size
    header.data_size += bufferSize;
    header.overall_size = 36 + header.data_size;

    // Seek back to the start of the file to update the header
    wavOutFile.seekp(0, std::ios::beg);
    writeWavHeader(wavOutFile, header);

    // Close the output file
    wavOutFile.close();
}
virtual void onMixedAudioRawDataReceived(AudioRawData* data_) {
		if (getRawAudio) {

			std::string filename = "output.pcm";

			//printf("onMixedAudioRawDataReceived\n");
			if (data_) {
				;
				convertPcmBufferToWav("output.wav", reinterpret_cast<uint8_t*>(data_->GetBuffer()), data_->GetBufferLen(), data_->GetSampleRate(), data_->GetChannelNum());
				//savePcmBufferToFile(filename, data_->GetBuffer(), data_->GetBufferLen());
				//printf("Data buffer: %s\n", data_->GetBuffer());
				//printf("Length is : %d\n", data_->GetBufferLen());
				//printf("Sample is : %d\n", data_->GetSampleRate());
				//printf("Channel is : %d\n", data_->GetChannelNum());
				//
				////print counter
				//printf("Counter is : %d\n", counter);


			}

		}
	};


	virtual void onOneWayAudioRawDataReceived(AudioRawData* data_, IZoomVideoSDKUser* pUser) {
		if (getRawAudio) {
			std::string filename = pUser->getUserID();
			std::string extension = ".pcm";
			filename.append(extension);

		/*	printf("onOneWayAudioRawDataReceived\n");
			if (data_) {
				savePcmBufferToFile(filename, data_->GetBuffer(), data_->GetBufferLen());
				printf("Data buffer: %s\n", data_->GetBuffer());
				printf("Length is : %d\n", data_->GetBufferLen());
				printf("Sample is : %d\n", data_->GetSampleRate());
				printf("Channel is : %d\n", data_->GetChannelNum());
			}*/
		}
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
	virtual void onChatPrivilegeChanged(IZoomVideoSDKChatHelper* pChatHelper, ZoomVideoSDKChatPrivilegeType privilege) {};
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


	if (getRawVideo) {
		//nothing much to do before joining session
	}
	if (getRawShare) {
		//nothing much to do before joining session
	}

	if (getRawAudio) {
		//this code to load virtualaudiospeaker is needed if you are using headless linux, or linux which does not come with soundcard.
		//if you do not wish to load virtualaudiospeaker, you can alternatively install `apt install pulseaudio` on your linux distro
		//ZoomVideoSDKVirtualAudioSpeaker* vSpeaker  =new ZoomVideoSDKVirtualAudioSpeaker();
		//session_context.virtualAudioSpeaker =vSpeaker;

		session_context.audioOption.connect = true;


		//dreamtcs check if this needed
		//ZoomVideoSDKVirtualAudioMic* vMic  =new ZoomVideoSDKVirtualAudioMic();
		//session_context.virtualAudioMic=vMic;
	}

	

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