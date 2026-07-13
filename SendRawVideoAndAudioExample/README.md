# Zoom Video SDK Linux Raw Data Send Demo

`SendRawVideoAndAudioExample` demonstrates publishing application-provided media into a Video SDK session. The output binary is `SendRawVideoAndAudioDemo`.

## What It Demonstrates

- reading a video file with OpenCV and sending its frames through an external video source
- wiring a virtual microphone that accepts application-provided audio buffers
- wiring an external screen-share source and its lifecycle callbacks
- configuring the session context for virtual raw-media sources

Raw video, audio, and share setup is enabled by default through `sendRawVideo`, `sendRawAudio`, and `sendRawShare` in `src/zoom_v-sdk_linux_bot.cpp`. The video source expects `Big_Buck_Bunny_1080_10s_1MB.mp4` in the runtime directory. The virtual microphone and share source do not generate buffers themselves; extend them with application media before expecting audio or share content. The video source uses OpenCV, so this sample also requires the OpenCV development package.

## Build And Run

Install OpenCV and follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S SendRawVideoAndAudioExample/src -B SendRawVideoAndAudioExample/src/build
cmake --build SendRawVideoAndAudioExample/src/build -j
cd SendRawVideoAndAudioExample/src/bin
./SendRawVideoAndAudioDemo
```
