# Zoom Video SDK Linux Skeleton Demo

`SkeletonExample` is the general-purpose starter sample in this repo. It can build in two modes:

- GUI mode when `gtkmm-3.0` and `SDL2` are available
- console mode when those GUI dependencies are missing

The output binary is `SkeletonDemo`.

## Prerequisites

From the repo root, extract the Linux Video SDK so these paths exist:

- `SDK/h/zoom_video_sdk_api.h`
- `SDK/libvideosdk.so`

Minimum packages:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config
sudo apt install -y libglib2.0-dev libcurl4-openssl-dev libasound2-dev
sudo apt install -y liblzma-dev
```

Optional GUI packages:

```bash
sudo apt install -y libgtkmm-3.0-dev libsdl2-dev
```

## Build

From the repo root:

```bash
cmake -S SkeletonExample/src -B SkeletonExample/src/build
cmake --build SkeletonExample/src/build -j
```

The binary and copied runtime files are written to `SkeletonExample/src/bin/`.

## Configure

Copy `SkeletonExample/src/config.json.example` to `SkeletonExample/src/config.json`, then edit it:

```json
{
    "session_name": "my-session",
    "session_psw": "",
    "token": "your-video-sdk-token"
}
```

## Run

```bash
cp SkeletonExample/src/config.json.example SkeletonExample/src/config.json
cd SkeletonExample/src/bin
./SkeletonDemo
```

## Notes

- GUI mode is selected automatically when both `gtkmm-3.0` and `SDL2` are available at configure time.
- Without those packages, the same sample builds as a console application.
- The sample reads `config.json` from the `bin/` directory at runtime.
- If you extract the SDK somewhere else, pass `-DZOOM_VIDEO_SDK_ROOT=/path/to/SDK` to CMake.
