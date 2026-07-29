# Zoom Video SDK Linux Raw Data Send Demo

`SendRawVideoAndAudioExample` demonstrates publishing application-provided video and audio into a Video SDK session. It does not send raw screen share. The output binary is `SendRawVideoAndAudioDemo`.

## What It Demonstrates

- reading a video file with OpenCV and sending its frames through an external video source
- generating 48 kHz mono PCM tone frames and sending them through a virtual microphone
- configuring the session context for virtual raw-media sources

Raw video and audio sending are enabled by default through `sendRawVideo` and `sendRawAudio` in `src/zoom_v-sdk_linux_bot.cpp`. The video source expects `Big_Buck_Bunny_720_10s_1MB.mp4` in the runtime directory. The virtual microphone sends a generated 440 Hz test tone. The video source uses OpenCV, so this sample also requires the OpenCV development package.

## Build And Run

Install OpenCV and follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S SendRawVideoAndAudioExample/src -B SendRawVideoAndAudioExample/src/build
cmake --build SendRawVideoAndAudioExample/src/build -j
cd SendRawVideoAndAudioExample/src/bin
./SendRawVideoAndAudioDemo
```
