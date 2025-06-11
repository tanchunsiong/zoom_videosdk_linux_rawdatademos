#include "ZoomVideoSDKVideoSource.h"
#include "helpers/zoom_video_sdk_user_helper_interface.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>


using namespace cv;
using namespace std;

using namespace ZOOMVIDEOSDK;

int video_play_flag = -1;



	void ZoomVideoSDKVideoSource::onInitialize(IZoomVideoSDKVideoSender* sender, IVideoSDKVector<VideoSourceCapability >* support_cap_list, VideoSourceCapability& suggest_cap) {

          this->video_sender_=sender;
		  width_ = suggest_cap.width;
		  height_ = suggest_cap.height;
     }

	
	 void ZoomVideoSDKVideoSource::onPropertyChange(IVideoSDKVector<VideoSourceCapability >* support_cap_list, VideoSourceCapability suggest_cap) {}
	
	 void ZoomVideoSDKVideoSource::onStartSend() {

		 printf("ZoomVideoSDKVideoSource::onStartSend() fired \n");


		 // ******************* Use OpenCV to send video *******************************
		 video_play_flag = 1;
		 sendVideoToVideoSource(video_sender_, "Big_Buck_Bunny_1080_10s_1MB.mp4", width_, height_);
	 }
  
	
	 void ZoomVideoSDKVideoSource::onStopSend()  {
		 video_play_flag = -1;
	 }
	
	 void ZoomVideoSDKVideoSource::onUninitialized() {}

	 void ZoomVideoSDKVideoSource::sendVideoToVideoSource(IZoomVideoSDKVideoSender* video_sender, std::string video_source, int width, int height)
	 {

		 char* frameBuffer;
		 int frameLen = height / 2 * 3 * width;
		 frameBuffer = (char*)malloc(frameLen);

		 // execute in a thread.
		 while (video_play_flag > 0 && video_sender)
		 {
			 Mat frame;
			 VideoCapture cap;
			 cap.open(video_source);
			 if (!cap.isOpened())
			 {
				 cerr << "ERROR! Unable to open camera\n";
				 video_play_flag = 0;
				 break;
			 }
			 else
			 {
				 //--- GRAB AND WRITE LOOP
				 cout << "Start grabbing" << endl;
				 while (video_play_flag > 0)
				 {
					 // wait for a new frame from camera and store it into 'frame'
					 cap.read(frame);
					 // check if we succeeded
					 if (frame.empty())
					 {
						 cerr << "ERROR! blank frame grabbed\n";
						 break;
					 }
					 Mat resizedFrame;
					 resize(frame, resizedFrame, Size(width, height), 0, 0, INTER_LINEAR);

					 // covert Mat to YUV buffer
					 Mat yuv;
					 cv::cvtColor(resizedFrame, yuv, COLOR_BGRA2YUV_I420);
					 char* p;
					 for (int i = 0; i < height / 2 * 3; ++i)
					 {
						 p = yuv.ptr<char>(i);
						 for (int j = 0; j < width; ++j)
						 {
							 frameBuffer[i * width + j] = p[j];
						 }
					 }
					 video_sender->sendVideoFrame(frameBuffer, width, height, frameLen, 0);
				 }
				 cap.release();
			 }
		 }
		 video_play_flag = -1; // stop video play when video_sender is null.
	 }


