# Zoom Video SDK Linux Statistics Demo

`StatisticsExample` demonstrates querying media quality statistics for a Video SDK session. The output binary is `StatisticsDemo`.

## What It Demonstrates

- retrieving send and receive audio statistics
- retrieving send and receive video statistics
- retrieving send and receive screen-share statistics
- accessing frequency, resolution, frame rate, latency, jitter, and packet-loss fields

The sample queries the statistics immediately after joining. Extend `src/zoom_v-sdk_linux_bot.cpp` with logging or periodic polling if the values need to be observed over time.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S StatisticsExample/src -B StatisticsExample/src/build
cmake --build StatisticsExample/src/build -j
cd StatisticsExample/src/bin
./StatisticsDemo
```
