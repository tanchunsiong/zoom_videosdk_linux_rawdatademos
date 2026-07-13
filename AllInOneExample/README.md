# Zoom Video SDK Linux All-in-One Demo

`AllInOneExample` combines the repository's raw-data and Video SDK feature examples in one console application. The output binary is `VideoSDKSessionRecorder`.

## What It Demonstrates

- receiving raw audio, video, and screen-share data
- configuring virtual sources for raw audio, video, and screen-share sending
- chat and command-channel messaging
- custom live streaming, cloud recording, phone callout, live transcription, and session statistics

Feature switches such as `getRawAudio`, `sendRawVideo`, and `enableChat` are defined near the top of `src/zoom_v-sdk_linux_bot.cpp`. Some optional paths contain placeholder phone or streaming values that must be replaced before use. The virtual microphone and share source are integration scaffolding; an application must supply media buffers to send useful audio or share content.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S AllInOneExample/src -B AllInOneExample/src/build
cmake --build AllInOneExample/src/build -j
cd AllInOneExample/src/bin
./VideoSDKSessionRecorder
```
