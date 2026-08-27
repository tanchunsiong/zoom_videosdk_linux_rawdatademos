# Zoom Video SDK Linux Raw Data Demos

This repository contains Linux Zoom Video SDK sample applications for:

- raw audio and video receive
- raw audio and video send
- screen sharing, raw share sending, and annotation
- file transfer through session chat
- chat and command channel features
- callout, cloud recording, live streaming, and statistics
- a skeleton sample with optional GUI support

## Sample List

- `AllInOneExample` -> `VideoSDKSessionRecorder`: combines raw-data receive/send and optional chat, command channel, streaming, recording, callout, transcription, and statistics controls
- `AnnotationExample` -> `AnnotationDemo`: starts a display share and demonstrates annotation helper lifecycle, permissions, and tool configuration
- `CalloutExample` -> `CalloutDemo`: demonstrates inviting a participant by phone
- `ChatExample` -> `ChatDemo`: sends chat messages and handles chat callbacks
- `CloudRecordingExample` -> `CloudRecordingDemo`: starts cloud recording and handles recording status callbacks
- `CommandChannelExample` -> `CommandChannelDemo`: connects to the command channel and sends and receives commands
- `FileTransferExample` -> `FileTransferDemo`: sends files to session participants and safely receives incoming files
- `GetRawVideoAndAudioExample` -> `GetRawVideoAndAudioDemo`: subscribes to raw audio, video, and share data and writes received media to files
- `GetRawVideoAndAudioAPICallExample` -> `GetRawVideoAndAudioCallAPIDemo`: captures raw media and demonstrates sending recorded audio to the Deepgram API
- `LanguageTranscriptionAndTranslationExample` -> `LanguageTranscriptionAndTranslationDemo`: starts live transcription and handles transcription callbacks; translation language selection is present but currently disabled
- `LiveStreamingExample` -> `LiveStreamingDemo`: starts a custom RTMP live stream and handles streaming status callbacks
- `ScreenShareExample` -> `ScreenShareDemo`: shares a Linux display or X11 window and demonstrates pause, resume, stop, and share-status callbacks
- `SendRawShareExample` -> `SendRawShareDemo`: sends generated, animated I420 frames as shared content
- `SendRawVideoAndAudioExample` -> `SendRawVideoAndAudioDemo`: sends raw video and generated PCM audio through virtual sources; it does not send raw share
- `SkeletonExample` -> `SkeletonDemo`: provides a starter application with optional GTK/SDL video rendering and a console fallback
- `StatisticsExample` -> `StatisticsDemo`: queries session audio, video, and share statistics

Each sample builds from its own `src/` directory and writes its executable to `src/bin/`.

## Fresh Clone Notes

A fresh clone does not contain every local SDK artifact needed for every sample build.

Tracked in git:

- sample source code
- sample `config.json.example` templates
- legacy `ffmpeg.tar.gz` archives in the sample `src/lib/` directories

The Zoom Video SDK itself is now expected from a shared extracted SDK root:

- default path: `SDK/` at the repository root
- override with `-DZOOM_VIDEO_SDK_ROOT=/path/to/SDK`

The raw-data recording samples build against system FFmpeg development packages via `pkg-config`.

## Prerequisites

Base packages used by most samples:

```bash
sudo apt update
sudo apt install -y build-essential gcc g++ cmake pkg-config
sudo apt install -y libglib2.0-dev libcurl4-openssl-dev liblzma-dev
sudo apt install -y libxcb-image0 libxcb-keysyms1 libxcb-xfixes0 libxcb-xkb1
sudo apt install -y libxcb-shape0 libxcb-shm0 libxcb-randr0 libxcb-xtest0
sudo apt install -y libgbm1 libxtst6 libgl1 libnss3 libasound2-dev libpulse0
mkdir -p SDK
tar -xf /path/to/zoom-video-sdk-linux_x86_64-*.tar.xz -C SDK --strip-components=1
```

The Zoom archive contains a versioned top-level directory. `--strip-components=1` places the SDK contents directly in this repo's `SDK/` folder instead of creating another directory inside it.

After extraction, this repo expects these paths to exist:

- `SDK/h/zoom_video_sdk_api.h`
- `SDK/libvideosdk.so`
- `SDK/libcml.so`
- `SDK/libmpg123.so`

Keep the other package contents, including `qt_libs/` and `cpthost` when supplied by the SDK version, in the same `SDK/` folder.

Extra packages for some samples:

- raw-data samples: FFmpeg development packages
- `SendRawVideoAndAudioExample`: OpenCV
- `ScreenShareExample` and `AnnotationExample`: an X11 display or equivalent virtual display at runtime
- `SkeletonExample` GUI mode: `gtkmm-3.0` and `SDL2`

Example:

```bash
sudo apt install -y libavfilter-dev libavformat-dev libavcodec-dev
sudo apt install -y libavutil-dev libswscale-dev libswresample-dev
sudo apt install -y libgtkmm-3.0-dev libsdl2-dev libopencv-dev
```

## Build

Build the whole repo:

```bash
cmake -S . -B build
cmake --build build -j
```

Build one sample:

```bash
cmake -S GetRawVideoAndAudioExample/src -B GetRawVideoAndAudioExample/src/build
cmake --build GetRawVideoAndAudioExample/src/build -j
```

If the SDK was extracted somewhere else:

```bash
cmake -S . -B build -DZOOM_VIDEO_SDK_ROOT=/path/to/SDK
```

## Configure

Each sample expects its own `src/config.json`. Start by copying that sample's `src/config.json.example` to `src/config.json`. Example:

```json
{
    "session_name": "my-session",
    "token": "your-video-sdk-token",
    "session_psw": "",
    "getSignatureFromWebService": false,
    "signatureUrl": ""
}
```

Create or update `config.json` before configuring the sample. CMake copies it into `src/bin/`; if no local config exists, it copies `config.json.example` as the runtime `config.json` instead.

For direct-token mode, leave `getSignatureFromWebService` set to `false` and put the Video SDK JWT in `token`.

For signature-service mode, configure:

```json
{
    "session_name": "my-session",
    "token": "",
    "session_psw": "",
    "getSignatureFromWebService": true,
    "signatureUrl": "https://your-token-service.example.com/video"
}
```

Each executable reads `src/bin/config.json` when it starts. You can change `token`, `getSignatureFromWebService`, or `signatureUrl` in that runtime file and restart the executable without rebuilding. If you instead edit the source `src/config.json`, rerun the CMake configure step to copy it into `src/bin/`.

## Run

Run from the sample `src/bin/` directory. For example:

```bash
cp GetRawVideoAndAudioExample/src/config.json.example GetRawVideoAndAudioExample/src/config.json
cmake -S GetRawVideoAndAudioExample/src -B GetRawVideoAndAudioExample/src/build
cmake --build GetRawVideoAndAudioExample/src/build -j
cd GetRawVideoAndAudioExample/src/bin
./GetRawVideoAndAudioDemo
```

## JWT

You need a Zoom Video SDK token for the target session.

- create or use a Zoom Video SDK app
- get the SDK key and secret
- generate a Video SDK JWT/token for your session
- place the token into the sample `config.json`

## Notes

- `SkeletonExample` builds with a GTK/SDL GUI when those dependencies are present, and falls back to a console build otherwise.
- ALSA warnings are common on headless Linux systems and may indicate missing audio devices.
- Raw audio access may require a virtual speaker or PulseAudio on headless machines.
- If you add new `.cpp` files to a sample, update that sample's `CMakeLists.txt`.
