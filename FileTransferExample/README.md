# Zoom Video SDK Linux File Transfer Demo

`FileTransferExample` demonstrates sending and receiving files through Video SDK session chat. The output binary is `FileTransferDemo`.

## What It Demonstrates

- checking whether file transfer is enabled
- reading the maximum transfer size and allowed file types
- sending a configured file to all session participants
- tracking send and receive status callbacks
- safely accepting incoming files into a configured download directory

Leave `filePath` empty to run as a receive-only participant. Set `autoAcceptIncomingFiles` to `false` to observe incoming transfer requests without downloading them.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S FileTransferExample/src -B FileTransferExample/src/build
cmake --build FileTransferExample/src/build -j
cd FileTransferExample/src/bin
./FileTransferDemo
```
