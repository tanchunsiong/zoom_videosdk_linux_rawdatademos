# Zoom Video SDK Files

The Zoom Video SDK files are intentionally not stored in this legacy per-sample folder. This sample uses the shared `SDK/` directory at the repository root.

Extract the full Linux SDK package directly into the repository-level `SDK/` folder. Do not leave its contents inside a versioned subdirectory. This repo expects at least:

- `SDK/h/zoom_video_sdk_api.h`
- `SDK/libvideosdk.so`
- `SDK/libcml.so`
- `SDK/libmpg123.so`

Keep `qt_libs/` and `cpthost` there when supplied by the downloaded SDK version.
