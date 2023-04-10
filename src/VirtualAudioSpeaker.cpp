#include "VirtualAudioSpeaker.h"
#include "helpers/zoom_video_sdk_user_helper_interface.h"

using namespace ZOOMVIDEOSDK;

//needed for audio, applies to this entire .cpp file

void VirtualAudioSpeaker::SendRawAudio(char* data, unsigned int data_length, int sample_rate)
{
    if (!virtual_audio_sender_)
    {
        return;
    }

    // See zoom_video_sdk_audio_send_rawdata_interface.h for raw audio data information
    virtual_audio_sender_->Send(data, data_length, sample_rate);
}

void VirtualAudioSpeaker::onMicInitialize(IZoomVideoSDKAudioSender* rawdata_sender)
{
    // Once the sender has been recieved from this callback, then Send can be called
    virtual_audio_sender_ = rawdata_sender;
}

void VirtualAudioSpeaker::onMicStartSend()
{
    if (!virtual_audio_sender_)
    {
        return;
    }

    // Virtual Mic began sending raw audio
}

void VirtualAudioSpeaker::onMicStopSend()
{
    if (!virtual_audio_sender_)
    {
        return;
    }

    // Virtual Mic stopped sending raw audio
}

void VirtualAudioSpeaker::onMicUninitialized()
{
    virtual_audio_sender_ = nullptr;
}
void VirtualAudioSpeaker::onVirtualSpeakerMixedAudioReceived(AudioRawData* data_)
{
    // Handle mixed audio raw data here
    // data_->GetBuffer();
    // data_->GetBufferLen();
    // data_->GetChannelNum();
    // data_->GetSampleRate();
    	 //printf("onVirtualSpeakerMixedAudioReceived() \n");
         //	printf("data %s \n",  data_->GetBuffer());
        //     printf("length %n \n",  data_->GetBufferLen());
        //      printf("channel %n \n",  data_->GetChannelNum());
        //       printf("sample %n \n",  data_->GetSampleRate());
}

void VirtualAudioSpeaker::onVirtualSpeakerOneWayAudioReceived(AudioRawData* data_, IZoomVideoSDKUser* pUser)
{
    // Handle audio raw data from single user here
    // data_->GetBuffer();
    // data_->GetBufferLen();
    // data_->GetChannelNum();
    // data_->GetSampleRate();
    // pUser->getUserName();
    	 //printf("onVirtualSpeakerOneWayAudioReceived() \n");
     	//printf("data %s \n",  data_->GetBuffer());
}

void VirtualAudioSpeaker::onVirtualSpeakerSharedAudioReceived(AudioRawData* data_)
{
    // Handle audio raw data from share here
    // data_->GetBuffer();
    // data_->GetBufferLen();
    // data_->GetChannelNum();
    // data_->GetSampleRate();
    	//printf("onVirtualSpeakerSharedAudioReceived() \n");
}

