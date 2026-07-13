# Zoom Video SDK Linux Transcription And Translation Demo

`LanguageTranscriptionAndTranslationExample` demonstrates the Video SDK live-transcription workflow. The output binary is `LanguageTranscriptionAndTranslationDemo`.

## What It Demonstrates

- connecting session audio for transcription
- checking whether live transcription can start
- starting live transcription after joining
- receiving transcription status, message, and language-error callbacks

Translation-language selection is present in `src/zoom_v-sdk_linux_bot.cpp` but currently commented out, so the sample runs live transcription without selecting translated output.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S LanguageTranscriptionAndTranslationExample/src -B LanguageTranscriptionAndTranslationExample/src/build
cmake --build LanguageTranscriptionAndTranslationExample/src/build -j
cd LanguageTranscriptionAndTranslationExample/src/bin
./LanguageTranscriptionAndTranslationDemo
```
