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
    #include "raw_data_ffmpeg_encoder.h"

    //needed for audio
    #include "ZoomVideoSDKVirtualAudioMic.h"
    #include "ZoomVideoSDKVirtualAudioSpeaker.h"
    #include "helpers/zoom_video_sdk_audio_send_rawdata_interface.h"

    //needed for share screen
    #include "ZoomVideoSDKShareSource.h"
    #include "helpers/zoom_video_sdk_share_helper_interface.h"

    //needed for get raw video
    #include "ZoomVideoSDKVideoSource.h"
    #include "helpers/zoom_video_sdk_video_helper_interface.h"

    using Json = nlohmann::json;
    USING_ZOOM_VIDEO_SDK_NAMESPACE
    IZoomVideoSDK *video_sdk_obj;
    GMainLoop *loop;

    //these are controls to demonstrate the flow
    bool getRawAudio = true;
    bool getRawVideo = true;
    bool getRawAudio = true;
    bool sendRawVideo = true;
    bool sendRawAudio = true;
    bool sendRawShare = true;

    std::string getSelfDirPath()
    {
        char dest[PATH_MAX];
        memset(dest, 0, sizeof(dest)); // readlink does not null terminate!
        if (readlink("/proc/self/exe", dest, PATH_MAX) == -1)
        {
        }

        char *tmp = strrchr(dest, '/');
        if (tmp)
            *tmp = 0;
        printf("getpath\n");
        return std::string(dest);
    }

    class ZoomVideoSDKDelegate : public IZoomVideoSDKDelegate
    {
    public:
        /// \brief Triggered when user enter the session.
        virtual void onSessionJoin()
        {
        printf("Joined session successfully\n");
        
        if (getRawAudio){
            IZoomVideoSDKAudioHelper* m_pAudiohelper=  video_sdk_obj->getAudioHelper();
            if (m_pAudiohelper) {
            //needed for getting raw audio
            ZoomVideoSDKErrors err=  m_pAudiohelper->subscribe();
            printf("subscribe status is %d\n", err);
            }
        }

        if (sendRawAudio){
             //needed for audio
            IZoomVideoSDKAudioHelper* m_pAudiohelper=  video_sdk_obj->getAudioHelper();
            if (m_pAudiohelper) {
                            // Connect User's audio.
                            printf("Starting Audio\n");
                            m_pAudiohelper->startAudio();
                        }
        }
      
        //checking the use for this
        //IZoomVideoSDKRecordingHelper* m_pRecordhelper =  video_sdk_obj->getRecordingHelper();
        

        if (sendRawShare){
            
            //needed for share source
            //this needs to be done after joing session
            ZoomVideoSDKShareSource* virtual_share_source = new ZoomVideoSDKShareSource();
            ZoomVideoSDKErrors err2= video_sdk_obj->getShareHelper()->startSharingExternalSource(virtual_share_source);    
                if (err2==ZoomVideoSDKErrors::ZoomVideoSDKErrors_Success){
                }
                else{
                   printf("Error setting external source %s\n", err2);
                }            
        };
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


        virtual void onUserJoin(IZoomVideoSDKUserHelper *pUserHelper, IVideoSDKVector<IZoomVideoSDKUser *> *userList)
        {
            if (getRawVideo){
                if (userList)
                {
                    int count = userList->GetCount();
                    for (int index = 0; index < count; index++)
                    {
                        IZoomVideoSDKUser *user = userList->GetItem(index);
                        if (user)
                        {
                            RawDataFFMPEGEncoder *encoder = new RawDataFFMPEGEncoder(user);
                        }

                    }
                }
            }
        };

        virtual void onUserLeave(IZoomVideoSDKUserHelper *pUserHelper, IVideoSDKVector<IZoomVideoSDKUser *> *userList)
        {
               if (getRawVideo){
                    if (userList)
                    {
                        int count = userList->GetCount();
                        for (int index = 0; index < count; index++)
                        {
                            IZoomVideoSDKUser *user = userList->GetItem(index);
                            if (user)
                            {
                                RawDataFFMPEGEncoder::stop_encoding_for(user);
                            }
                        }
                    }
               }
        
        };

   
        virtual void onUserVideoStatusChanged(IZoomVideoSDKVideoHelper *pVideoHelper,
                                            IVideoSDKVector<IZoomVideoSDKUser *> *userList){};


        virtual void onUserAudioStatusChanged(IZoomVideoSDKAudioHelper *pAudioHelper,
                                            IVideoSDKVector<IZoomVideoSDKUser *> *userList){
 };

   
        virtual void onUserShareStatusChanged(IZoomVideoSDKShareHelper *pShareHelper,
                                            IZoomVideoSDKUser *pUser,
                                            ZoomVideoSDKShareStatus status){};

 
        virtual void onLiveStreamStatusChanged(IZoomVideoSDKLiveStreamHelper *pLiveStreamHelper,
                                            ZoomVideoSDKLiveStreamStatus status){};


        virtual void onChatNewMessageNotify(IZoomVideoSDKChatHelper *pChatHelper, IZoomVideoSDKChatMessage *messageItem){};

     
        virtual void onUserHostChanged(IZoomVideoSDKUserHelper *pUserHelper, IZoomVideoSDKUser *pUser){};

   
        virtual void onUserActiveAudioChanged(IZoomVideoSDKAudioHelper *pAudioHelper,
                                            IVideoSDKVector<IZoomVideoSDKUser *> *list){};

     
        virtual void onSessionNeedPassword(IZoomVideoSDKPasswordHandler *handler){};

      
        virtual void onSessionPasswordWrong(IZoomVideoSDKPasswordHandler *handler){};


        virtual void onMixedAudioRawDataReceived(AudioRawData *data_){
            if (getRawAudio){
                    printf("onMixedAudioRawDataReceived\n");
                    if (data_){
                        printf("Length is : %d\n",data_->GetBufferLen());
                        printf("Data buffer: %s\n", data_->GetBuffer());
                        }

            }   
        };


        virtual void onOneWayAudioRawDataReceived(AudioRawData *data_, IZoomVideoSDKUser *pUser){
          if (getRawAudio){
                printf("onOneWayAudioRawDataReceived\n");
                if (data_){
                printf("Data buffer: %s\n", data_->GetBuffer());
                printf("Length is : %d\n",data_->GetBufferLen());
                }
          }
        };

        virtual void onSharedAudioRawDataReceived(AudioRawData *data_){};


        virtual void onUserManagerChanged(IZoomVideoSDKUser *pUser){};


        virtual void onUserNameChanged(IZoomVideoSDKUser *pUser){};


        virtual void onCameraControlRequestResult(IZoomVideoSDKUser *pUser, bool isApproved){};

        virtual void onCameraControlRequestReceived(
            IZoomVideoSDKUser *pUser,
            ZoomVideoSDKCameraControlRequestType requestType,
            IZoomVideoSDKCameraControlRequestHandler *pCameraControlRequestHandler){};


        virtual void onCommandReceived(IZoomVideoSDKUser *sender, const zchar_t *strCmd){
            
  printf("onCommandReceived() Message: %s\n", strCmd);    

        }
        virtual void onCommandChannelConnectResult(bool isSuccess){};
        virtual void onInviteByPhoneStatus(PhoneStatus status, PhoneFailedReason reason){};
        virtual void onCloudRecordingStatus(RecordingStatus status, IZoomVideoSDKRecordingConsentHandler* pHandler) {};
        virtual void onHostAskUnmute(){};
        virtual void onUserShareStatusChanged(IZoomVideoSDKShareHelper *pShareHelper, IZoomVideoSDKUser *pUser, ZoomVideoSDKShareStatus status, ZoomVideoSDKShareType type) {}
        virtual void onMultiCameraStreamStatusChanged(ZoomVideoSDKMultiCameraStreamStatus status, IZoomVideoSDKUser *pUser, IZoomVideoSDKRawDataPipe *pVideoPipe) {}
        virtual void onMicSpeakerVolumeChanged(unsigned int micVolume, unsigned int speakerVolume) {}
        virtual void onAudioDeviceStatusChanged(ZoomVideoSDKAudioDeviceType type, ZoomVideoSDKAudioDeviceStatus status) {}
        virtual void onTestMicStatusChanged(ZoomVideoSDK_TESTMIC_STATUS status) {}
        virtual void onSelectedAudioDeviceChanged() {}
        virtual void onUserRecordingConsent(IZoomVideoSDKUser* pUser) { };
        virtual void onLiveTranscriptionStatus(ZoomVideoSDKLiveTranscriptionStatus status) {};
        virtual void onLiveTranscriptionMsgReceived(const zchar_t* ltMsg, IZoomVideoSDKUser* pUser, ZoomVideoSDKLiveTranscriptionOperationType type) {};
        virtual void onLiveTranscriptionMsgInfoReceived(ILiveTranscriptionMessageInfo* messageInfo) {};
        virtual void onLiveTranscriptionMsgError(ILiveTranscriptionLanguage* spokenLanguage, ILiveTranscriptionLanguage* transcriptLanguage) {};
        virtual void onChatMsgDeleteNotification(IZoomVideoSDKChatHelper* pChatHelper, const zchar_t* msgID, ZoomVideoSDKChatMessageDeleteType deleteBy){};
        virtual void onProxyDetectComplete() {};
        virtual void onProxySettingNotification(IZoomVideoSDKProxySettingHandler* handler){};
        virtual void onSSLCertVerifiedFailNotification(IZoomVideoSDKSSLCertificateInfo* info) {};	
        virtual void onUserVideoNetworkStatusChanged(ZoomVideoSDKNetworkStatus status, IZoomVideoSDKUser* pUser){};
    
       virtual void onVirtualSpeakerMixedAudioReceived(AudioRawData* data_){

                printf("onVirtualSpeakerMixedAudioReceived() main \n");
                printf("data %s \n",  data_->GetBuffer());

        };

        virtual void onVirtualSpeakerOneWayAudioReceived(AudioRawData* data_, IZoomVideoSDKUser* pUser) {

                 printf("onVirtualSpeakerOneWayAudioReceived() main\n");
                printf("data %s \n",  data_->GetBuffer());
        };

        virtual void onVirtualSpeakerSharedAudioReceived(AudioRawData* data_) {
            printf("onVirtualSpeakerSharedAudioReceived() main\n");
                printf("data %s \n",  data_->GetBuffer());
        };
    };



    void joinVideoSDKSession(std::string &session_name, std::string &session_psw, std::string &session_token)
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
        IZoomVideoSDKDelegate *listener = new ZoomVideoSDKDelegate();
        video_sdk_obj->addListener(dynamic_cast<IZoomVideoSDKDelegate *>(listener));

        ZoomVideoSDKSessionContext session_context;
        session_context.sessionName = session_name.c_str();
        session_context.sessionPassword = session_psw.c_str();
        session_context.userName = "Linux Bot";
        session_context.token = session_token.c_str();
        session_context.videoOption.localVideoOn = true;
        session_context.audioOption.connect = false; 
        session_context.audioOption.mute = true;
 
    
        if (getRawVideo){
            //nothing much to do before joining session
        }
     

        if (getRawAudio){
            ZoomVideoSDKVirtualAudioSpeaker* vSpeaker  =new ZoomVideoSDKVirtualAudioSpeaker();
            session_context.virtualAudioSpeaker =vSpeaker;

        }

        if (sendRawVideo){
            //needed for send raw video
            //the sdk uses a Video Source to send raw video
            //this needs to be done before joining session
            ZoomVideoSDKVideoSource* virtual_video_source = new ZoomVideoSDKVideoSource();
            session_context.externalVideoSource=virtual_video_source;
        }


         if (sendRawAudio){
            session_context.audioOption.connect = true; //needed for sending raw audio data
            session_context.audioOption.mute = false; //needed for sending raw audio data

            ZoomVideoSDKVirtualAudioMic* vMic  =new ZoomVideoSDKVirtualAudioMic();
            session_context.virtualAudioMic=vMic;
        
        }
        IZoomVideoSDKSession *session = NULL;
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
        video_sdk_obj->leaveSession(false);
        printf("Leaving session.\n");
    }

    int main(int argc, char *argv[])
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
            catch (Json::parse_error &ex)
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