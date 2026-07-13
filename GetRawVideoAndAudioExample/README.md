# Zoom Video SDK Linux Raw Data Receive Demo

`GetRawVideoAndAudioExample` demonstrates receiving media from a Video SDK session without rendering a user interface. The output binary is `GetRawVideoAndAudioDemo`.

## What It Demonstrates

- subscribing to mixed and per-user raw audio callbacks
- subscribing to participant video raw-data pipes
- subscribing to screen-share raw-data pipes
- writing mixed audio to WAV and enabled video/share frames to YUV files

Raw audio is enabled by default while raw video and share are disabled. Change `getRawAudio`, `getRawVideo`, and `getRawShare` near the top of `src/zoom_v-sdk_linux_bot.cpp` to select the streams to capture. Per-user PCM writing is included as commented example code.

## Build And Run

Install the FFmpeg development packages and follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S GetRawVideoAndAudioExample/src -B GetRawVideoAndAudioExample/src/build
cmake --build GetRawVideoAndAudioExample/src/build -j
cd GetRawVideoAndAudioExample/src/bin
./GetRawVideoAndAudioDemo
```
