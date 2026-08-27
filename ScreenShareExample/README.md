# Zoom Video SDK Linux Screen Share Demo

`ScreenShareExample` demonstrates sharing a Linux display or X11 window through the Video SDK. The output binary is `ScreenShareDemo`.

## What It Demonstrates

- starting a display share with `startShareScreen`
- starting an X11 window share with `startShareView`
- optionally sharing device audio and optimizing for video
- receiving share-status and failure callbacks
- optionally pausing, resuming, and stopping the share on configured timers

Set either `monitorId` or `windowHandle` in `src/config.json`. `windowHandle` takes precedence when both are set. A window handle uses the Linux SDK X11 handle format documented by Zoom.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S ScreenShareExample/src -B ScreenShareExample/src/build
cmake --build ScreenShareExample/src/build -j
cd ScreenShareExample/src/bin
./ScreenShareDemo
```

Screen capture requires an available graphical display. It will not work in a headless shell without X11 or an equivalent virtual display.
