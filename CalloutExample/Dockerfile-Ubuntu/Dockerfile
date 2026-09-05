# Build from the repository root; extract the Linux Video SDK into SDK/ first.
FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential cmake pkg-config ca-certificates \
        libglib2.0-dev libcurl4-openssl-dev \
        libx11-xcb1 libxcb-xfixes0 libxcb-shape0 libxcb-shm0 \
        libxcb-randr0 libxcb-image0 libxcb-keysyms1 libxcb-xtest0 \
        libxcb-xkb1 libxkbcommon-x11-0 libgbm1 libxtst6 libgl1 \
        libegl1 libnss3 libasound2t64 libpulse0 pulseaudio pulseaudio-utils dbus \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY SDK/ SDK/
COPY cmake/ cmake/
COPY common/ common/
COPY CalloutExample/src/ CalloutExample/src/

RUN cmake -S CalloutExample/src -B build \
    && cmake --build build --parallel 2 --target CalloutDemo \
    && useradd --create-home --uid 10001 demo \
    && chown -R demo:demo /app

USER demo
ENV HOME=/home/demo
ENV LD_LIBRARY_PATH=/app/CalloutExample/src/bin:/app/CalloutExample/src/bin/qt_libs/Qt/lib
WORKDIR /app/CalloutExample/src/bin

# Run PulseAudio as the same unprivileged user as the SDK, with virtual audio devices.
ENTRYPOINT ["/bin/sh", "-ec", "pulseaudio --start --exit-idle-time=-1; pactl load-module module-null-sink sink_name=SpeakerOutput >/dev/null; pactl set-default-sink SpeakerOutput; pactl set-default-source SpeakerOutput.monitor; exec \"$@\"", "--"]
CMD ["./CalloutDemo"]
