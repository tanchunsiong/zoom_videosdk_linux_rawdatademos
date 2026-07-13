# Zoom Video SDK Linux Chat Demo

`ChatExample` demonstrates Video SDK session chat from a console application. The output binary is `ChatDemo`.

## What It Demonstrates

- checking session chat availability
- sending a message to all participants after joining
- receiving new-message, message-deletion, and chat-privilege callbacks

The sample sends the placeholder text `hello world`; edit `src/zoom_v-sdk_linux_bot.cpp` to change the message or recipient behavior.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S ChatExample/src -B ChatExample/src/build
cmake --build ChatExample/src/build -j
cd ChatExample/src/bin
./ChatDemo
```
