// Zoom Video SDK
#include "zoom_sdk_raw_data_def.h"
#include "zoom_video_sdk_def.h"
#include "helpers/zoom_video_sdk_video_source_helper_interface.h"

using namespace ZOOMVIDEOSDK;

//needed for audio, applies to this entire .h file

class ZoomVideoSDKVideoSource :
  public IZoomVideoSDKVideoSource 
{
public: 

	IZoomVideoSDKVideoSender* video_sender_;
	int width_;
	int height_;

 

	virtual	void onInitialize(IZoomVideoSDKVideoSender* sender, IVideoSDKVector<VideoSourceCapability >* support_cap_list, VideoSourceCapability& suggest_cap) ;

	
	virtual void onPropertyChange(IVideoSDKVector<VideoSourceCapability >* support_cap_list, VideoSourceCapability suggest_cap) ;
	
	virtual void onStartSend();
	virtual void onStopSend() ;
	
	virtual void onUninitialized();

	virtual void sendVideoToVideoSource(IZoomVideoSDKVideoSender* video_sender, std::string video_source, int width, int height);

	


};



