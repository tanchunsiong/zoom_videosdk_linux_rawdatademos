# Zoom Video SDK Linux Callout Demo

`CalloutExample` demonstrates inviting a participant into a Video SDK session by telephone. The output binary is `CalloutDemo`.

## What It Demonstrates

- checking whether the phone feature is supported for the session
- calling `inviteByPhone` after joining the session
- showing where phone invitation status and callout join callbacks can be implemented

The callback methods are empty hooks in this sample. The country code, phone number, and display name in `src/zoom_v-sdk_linux_bot.cpp` are placeholders and must be replaced before running the demo. The session user must have the permissions and account features required for callout.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S CalloutExample/src -B CalloutExample/src/build
cmake --build CalloutExample/src/build -j
cd CalloutExample/src/bin
./CalloutDemo
```
