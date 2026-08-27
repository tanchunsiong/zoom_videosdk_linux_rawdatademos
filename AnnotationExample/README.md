# Zoom Video SDK Linux Annotation Demo

`AnnotationExample` demonstrates annotation setup and tool control for a locally shared Linux display. The output binary is `AnnotationDemo`.

## What It Demonstrates

- checking whether annotation is supported
- starting a display share for annotation
- enabling or disabling viewer annotation
- creating and destroying an annotation helper
- starting and stopping annotation
- selecting the pen tool, color, and width
- handling annotation cleanup, privilege, and tool callbacks

Set `monitorId` to an available Linux display. `toolColor` is the SDK numeric color value. The sample configures annotation through the SDK; interactive drawing still requires a graphical environment and pointer input.

## Build And Run

Follow the repository-level [setup and configuration instructions](../README.md), then run:

```bash
cmake -S AnnotationExample/src -B AnnotationExample/src/build
cmake --build AnnotationExample/src/build -j
cd AnnotationExample/src/bin
./AnnotationDemo
```
