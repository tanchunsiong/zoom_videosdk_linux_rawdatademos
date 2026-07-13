# Zoom Video SDK Linux Command Channel Demo

`CommandChannelExample` demonstrates exchanging application-defined data over the Video SDK command channel. The output binary is `CommandChannelDemo`.

## What It Demonstrates

- obtaining the session command channel
- sending a command to all participants after joining
- handling command-channel connection results
- receiving commands and identifying their sender

The sample sends the placeholder command `hello world`; edit `src/zoom_v-sdk_linux_bot.cpp` to send application-specific data or target a participant.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S CommandChannelExample/src -B CommandChannelExample/src/build
cmake --build CommandChannelExample/src/build -j
cd CommandChannelExample/src/bin
./CommandChannelDemo
```
