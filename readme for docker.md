# Docker Notes

The 12 original samples each have a separate `Dockerfile-Ubuntu/Dockerfile`, using the repository-level `SDK/`, `cmake/`, and `common/` directories. Most build on Ubuntu 24.04. `AllInOneExample`, `GetRawVideoAndAudioExample`, and `GetRawVideoAndAudioAPICallExample` use Ubuntu 22.04 because their recording code requires FFmpeg 4 APIs. The four newer sharing, annotation, and file-transfer samples do not currently have Dockerfiles.

Run the following commands from the repository root after extracting the Linux Video SDK into `SDK/` (with `SDK/h/zoom_video_sdk_api.h` and `SDK/libvideosdk.so` directly inside it). Use a Docker host matching the architecture of your SDK archive.

```bash
docker build -f SendRawVideoAndAudioExample/Dockerfile-Ubuntu/Dockerfile \
  -t zoom-sendrawvideoandaudio .
cp SendRawVideoAndAudioExample/src/config.json.example \
  SendRawVideoAndAudioExample/src/config.json
```

Edit `config.json` with your session name and token, or enable `getSignatureFromWebService` and set its URL using the fields in the example. Mount this file when starting the container:

```bash
docker run --rm --init \
  --mount "type=bind,src=$(pwd)/SendRawVideoAndAudioExample/src/config.json,dst=/app/SendRawVideoAndAudioExample/src/bin/config.json,readonly" \
  zoom-sendrawvideoandaudio
```

For another sample, substitute its folder name in both commands and choose a corresponding image tag. Configuration is read beside the executable in `/app/<sample>/src/bin`. The mounted file must be readable by container UID 10001. `.dockerignore` excludes local `config.json`, `.env` files, backups, and host build outputs; only placeholder configuration is included in the image.

Each image builds its own target in `/app/build` and launches the correct executable as an unprivileged user. PulseAudio starts with a virtual speaker and microphone. The Skeleton image builds the console variant. The send-raw-media image includes the sample MP4 and the required OpenCV and FFmpeg development packages.

Virtual audio devices do not forward host microphone or speaker audio. Desktop capture and GUI features require a separately configured display connection; these containers do not start an X server. A successful image build does not verify joining a live session or media delivery.

The image contains the proprietary SDK copied from your local `SDK/`. Keep the resulting image private unless your SDK license permits redistribution.
