
// Zoom Video SDK
#include "helpers/zoom_video_sdk_audio_send_rawdata_interface.h"
using namespace ZOOMVIDEOSDK;

//needed for audio, applies to this entire .h file

class VirtualAudioSpeaker :
  public IZoomVideoSDKVirtualAudioMic , public IZoomVideoSDKVirtualAudioSpeaker
{
public: 


   void SendRawAudio(char* data, unsigned int data_length, int sample_rate);


    //IZoomVideoSDKVirtualAudioMic
    virtual void onMicInitialize(ZOOM_VIDEO_SDK_NAMESPACE::IZoomVideoSDKAudioSender* rawdata_sender);
    virtual void onMicStartSend();
    virtual void onMicStopSend();
    virtual void onMicUninitialized();

    //IZoomVideoSDKVirtualAudioSpeaker
    virtual void onVirtualSpeakerMixedAudioReceived(AudioRawData* data_);
    virtual void onVirtualSpeakerOneWayAudioReceived(AudioRawData* data_, ZOOM_VIDEO_SDK_NAMESPACE::IZoomVideoSDKUser* pUser);
    virtual void onVirtualSpeakerSharedAudioReceived(AudioRawData* data_);
    

protected:
    ZOOM_VIDEO_SDK_NAMESPACE::IZoomVideoSDKAudioSender* virtual_audio_sender_;

};



