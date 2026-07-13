# Zoom Video SDK Linux Raw Data API Call Demo

`GetRawVideoAndAudioAPICallExample` extends the raw-data receiver with an example HTTP integration that sends recorded audio to Deepgram. The output binary is `GetRawVideoAndAudioCallAPIDemo`.

## What It Demonstrates

- subscribing to raw audio, video, and screen-share callbacks
- writing mixed audio to WAV and enabled video/share frames to YUV output files
- submitting an audio file to the Deepgram transcription API with libcurl

Raw audio is enabled by default while raw video and share are disabled. These switches, the `output.wav` path, and the placeholder Deepgram API key are in `src/zoom_v-sdk_linux_bot.cpp`; provide your own API key before enabling the API call in a real environment.

## Build And Run

Install the FFmpeg development packages and follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S GetRawVideoAndAudioAPICallExample/src -B GetRawVideoAndAudioAPICallExample/src/build
cmake --build GetRawVideoAndAudioAPICallExample/src/build -j
cd GetRawVideoAndAudioAPICallExample/src/bin
./GetRawVideoAndAudioCallAPIDemo
```
