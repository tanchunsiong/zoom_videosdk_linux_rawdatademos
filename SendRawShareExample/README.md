# Zoom Video SDK Linux Send Raw Share Demo

`SendRawShareExample` demonstrates sending generated I420 frames as shared content rather than as camera video. The output binary is `SendRawShareDemo`.

## What It Demonstrates

- implementing `IZoomVideoSDKShareSource`
- receiving an `IZoomVideoSDKShareSender`
- generating and pacing animated I420 limited-range frames
- sending frames with `sendShareFrame`
- starting and stopping an external share source safely

The generated moving color bars make this example self-contained and suitable for a headless machine. It sends raw share video only; it does not send camera video or microphone audio.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S SendRawShareExample/src -B SendRawShareExample/src/build
cmake --build SendRawShareExample/src/build -j
cd SendRawShareExample/src/bin
./SendRawShareDemo
```
