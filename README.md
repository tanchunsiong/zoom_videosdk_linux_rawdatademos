This app will join a zoom video sdk session and record each user's video to a separate video file. 
The code demostrate how to use Zoom Video SDK's Raw Data feature, and how to use FFMPEG lib to encode the Raw Data to a video file. 

## Download & Build

```
sudo apt update
sudo apt install -y build-essential gcc cmake
sudo apt install -y libglib2.0-dev liblzma-dev libxcb-image0 libxcb-keysyms1 libxcb-xfixes0 libxcb-xkb1 libxcb-shape0 libxcb-shm0 libxcb-randr0 libxcb-xtest0 libgbm1 libxtst6 libgl1 libnss3  libasound2 libpulse0


mkdir -p ~/.zoom/logs
cd /root
git clone https://github.com/tanchunsiong/zoom_v-sdk_linux_bot.git
cd zoom_v-sdk_linux_bot/
touch config.json
echo "{">> config.json
echo "    \"session_name\": \"chunsiongsession\",">> config.json
echo "    \"token\": \"xxx.yyy.zzz\",">> config.json
echo "    \"session_psw\": \"12345678\"">> config.json
echo "}">> config.json
cmake -B build
cd build/
make
```


## Adding files

you will need to add `config.json` in your root directory
its content will look something like this
```

{
    "session_name": "chunsiongsession",
    "token": "xxx.yyy.zzz",
    "session_psw": "12345678"
}


```

you will also need to add the SDK files (.so files) to /lib/zoom_video_sdk

extract /lib/ffmpeg.tar.gz to /lib/ffmpeg
extract /lib/qt_libs.tar.gz to /zoom_video_sdk/qt_libs

make a copy of /lib/zoom_video_sdk/libvideosdk.so to /lib/zoom_video_sdk/libvideosdk.so.1

you will also need to add qt_libs into  /lib/zoom_video_sdk/qt_libs
this is typically double packaged into a tar and gz

## Generate Your JWT
You much have an account [subscribed](https://marketplace.zoom.us/docs/sdk/video/developer-accounts/) Zoom Video SDK. 

Find your Video SDK [key and secret](https://marketplace.zoom.us/docs/sdk/video/auth/#get-video-sdk-key-and-secret). 

Generate your JWT with [this script](https://gist.github.com/linanw/9144d9bfc84da4076ab5beb04d4f9db1).

## Run 
Add your JWT to bin/config.json.

Run the app from bin folder:
```
./zoom_v-sdk_linux_bot
```

## error messages
ALSA error messages appears to be from the SDK, this might be caused by missing speaker / microphone


## walkthru on audio

for raw audio access to work, here are some high level requirements
//needed for audio 
the above comment in code will help you to find the code segments which are needed

- IZoomVideoSDKVirtualAudioSpeaker
  - Set this as virtual speake r and virtual mic in session_context before joining
  - getAudioHelper() when in session, and call subscribe to start callback. 
    - If you do not call subscribe, there will be no callback for onmixedaudio and ononewayaudio. 
    - If you do not set the virtual speaker and virtual mic in session_context, there will be no callback for onmixedaudio and ononewayaudio.