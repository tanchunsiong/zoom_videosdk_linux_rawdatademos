#include "VirtualShareSource.h"
#include "helpers/zoom_video_sdk_user_helper_interface.h"

using namespace ZOOMVIDEOSDK;



   void VirtualShareSource::onShareSendStarted(IZoomVideoSDKShareSender* pSender) {
   
   printf("onShareSendStarted() invoked");

   };
   void VirtualShareSource::onShareSendStopped() {

        printf("onShareSendStopped() invoked");
   };


