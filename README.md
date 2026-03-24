# Zoom Video SDK Linux Raw Data Demos

This repository contains Linux Zoom Video SDK sample applications for:

- raw audio and video receive
- raw audio and video send
- chat and command channel features
- callout, cloud recording, live streaming, and statistics
- a skeleton sample with optional GUI support

## Sample List

- `AllInOneExample` -> `VideoSDKSessionRecorder`
- `CalloutExample` -> `CalloutDemo`
- `ChatExample` -> `ChatDemo`
- `CloudRecordingExample` -> `CloudRecordingDemo`
- `CommandChannelExample` -> `CommandChannelDemo`
- `GetRawVideoAndAudioExample` -> `GetRawVideoAndAudioDemo`
- `GetRawVideoAndAudioAPICallExample` -> `GetRawVideoAndAudioCallAPIDemo`
- `LanguageTranscriptionAndTranslationExample` -> `LanguageTranscriptionAndTranslationDemo`
- `LiveStreamingExample` -> `LiveStreamingDemo`
- `SendRawVideoAndAudioExample` -> `SendRawVideoAndAudioDemo`
- `SkeletonExample` -> `SkeletonDemo`
- `StatisticsExample` -> `StatisticsDemo`

Each sample builds from its own `src/` directory and writes its executable to `src/bin/`.

## Fresh Clone Notes

A fresh clone does not contain every local SDK artifact needed for every sample build.

Tracked in git:

- sample source code
- sample `config.json` templates
- legacy `ffmpeg.tar.gz` archives in some sample `src/lib/` directories

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
sudo apt install -y libgbm1 libxtst6 libgl1 libnss3 libasound2 libpulse0
mkdir -p SDK
tar -xf /path/to/zoom-video-sdk-linux_x86_64-*.tar.xz -C SDK
```

After extraction, the repo expects these paths to exist:

- `SDK/h/zoom_video_sdk_api.h`
- `SDK/libvideosdk.so`

Extra packages for some samples:

- raw-data samples: FFmpeg development packages
- `SendRawVideoAndAudioExample`: OpenCV
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

Each sample expects its own `src/config.json`. Start by copying `src/config.json.example` to `src/config.json`. Example:

```json
{
    "session_name": "my-session",
    "token": "your-video-sdk-token",
    "session_psw": ""
}
```

If you use the optional web-service token flow, add a repo-level `.env` file:

```bash
cat > .env <<'EOF'
ZOOM_VIDEO_SDK_SIGNATURE_URL=https://your-token-service.example.com/video
EOF
```

The samples first read `ZOOM_VIDEO_SDK_SIGNATURE_URL` from the process environment, then fall back to a `.env` file by walking upward from the current working directory.

## Run

Run from the sample `src/bin/` directory. For example:

```bash
cp GetRawVideoAndAudioExample/src/config.json.example GetRawVideoAndAudioExample/src/config.json
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
