# Zoom Video SDK Linux Statistics Demo

`StatisticsExample` demonstrates querying media quality statistics for a Video SDK session. The output binary is `StatisticsDemo`.

## What It Demonstrates

- calling the SDK APIs for send and receive audio statistics
- calling the SDK APIs for send and receive video statistics
- calling the SDK APIs for send and receive screen-share statistics
- accessing frequency, resolution, frame rate, latency, jitter, and packet-loss fields

The sample queries the statistics immediately after joining but does not currently print the returned values. Extend `src/zoom_v-sdk_linux_bot.cpp` with logging or periodic polling if the metrics need to be observed over time.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S StatisticsExample/src -B StatisticsExample/src/build
cmake --build StatisticsExample/src/build -j
cd StatisticsExample/src/bin
./StatisticsDemo
```
