# Zoom Video SDK Linux Cloud Recording Demo

`CloudRecordingExample` demonstrates starting cloud recording in a Video SDK session. The output binary is `CloudRecordingDemo`.

## What It Demonstrates

- obtaining the session recording helper
- checking whether the current user can start recording
- starting cloud recording after joining
- showing where cloud-recording status and recording-consent callbacks can be implemented

The status callback is an empty hook in this sample. Cloud recording must be enabled for the account and session, and the joining user must have permission to start it.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S CloudRecordingExample/src -B CloudRecordingExample/src/build
cmake --build CloudRecordingExample/src/build -j
cd CloudRecordingExample/src/bin
./CloudRecordingDemo
```
