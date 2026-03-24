# Visual Studio 2022 on WSL

For Visual Studio 2022 with a Ubuntu 22 WSL target, install:

```bash
sudo apt-get update
sudo apt-get install -y g++ gdb make ninja-build rsync zip cmake pkg-config
```

Then:

1. Open the repo folder in Visual Studio.
2. Create a Linux or WSL CMake configuration such as `WSL-GCC-Debug`.
3. Make sure the Zoom Video SDK is extracted to `SDK/` at the repo root, or set `ZOOM_VIDEO_SDK_ROOT` in the CMake cache.
4. Configure and build either the whole repo or an individual sample `src/` directory.
