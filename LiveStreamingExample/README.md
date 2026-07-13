# Zoom Video SDK Linux Live Streaming Demo

`LiveStreamingExample` demonstrates starting a custom RTMP live stream from a Video SDK session. The output binary is `LiveStreamingDemo`.

## What It Demonstrates

- obtaining the live-stream helper after joining
- checking whether the current user can start a live stream
- supplying an RTMP URL, stream key, and broadcast URL
- showing where live-stream status callbacks can be implemented

The status callback is an empty hook in this sample. The YouTube RTMP values in `src/zoom_v-sdk_linux_bot.cpp` are placeholders and must be replaced before running the demo. Live streaming must be enabled for the account and session, and the joining user must have permission to start it.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S LiveStreamingExample/src -B LiveStreamingExample/src/build
cmake --build LiveStreamingExample/src/build -j
cd LiveStreamingExample/src/bin
./LiveStreamingDemo
```
