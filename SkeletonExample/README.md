# Zoom Video SDK Linux - Skeleton Demo

A comprehensive learning guide and demonstration application for new developers getting started with the Zoom Video SDK on Linux.

## 📚 What You'll Learn

This skeleton demo teaches you how to:
- **Start and join video sessions** using the Zoom Video SDK
- **Render video streams** from yourself and other participants
- **Handle audio controls** (mute/unmute microphone)
- **Manage video controls** (start/stop camera)
- **Select different devices** (cameras, microphones, speakers)
- **Build a complete video conferencing application** with GTK GUI

## 🎯 Target Audience

This demo is perfect for:
- **New developers** learning video SDK integration
- **Linux developers** building video conferencing applications
- **Students** studying real-time communication technologies
- **Engineers** prototyping video features

## 🚀 Quick Start

### Prerequisites

Install the required development libraries:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install libgtkmm-3.0-dev libsdl2-dev
sudo apt install libglib2.0-dev libjsoncpp-dev
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake gtkmm30-devel SDL2-devel
sudo yum install glib2-devel jsoncpp-devel
```

### Build and Run

1. **Clone and navigate to the project:**
   ```bash
   git clone https://github.com/tanchunsiong/zoom_videosdk_linux_rawdatademos.git
   cd zoom_videosdk_linux_rawdatademos/SkeletonExample
   ```

2. **Build the application:**
   ```bash
   cd src
   mkdir -p build
   cd build
   cmake ..
   make
   ```

3. **Configure your session:**
   Edit `src/bin/config.json` with your session details:
   ```json
   {
       "session_name": "my-test-session",
       "session_psw": "",
       "token": "your-zoom-sdk-jwt-token"
   }
   ```

4. **Run the demo:**
   ```bash
   cd ../bin
   ./SkeletonDemo
   ```

## 🎥 Features Overview

### Core Video Conferencing Features

#### 📹 **Video Management**
- **Self Video Display**: See your own camera feed in a dedicated panel
- **Remote Video Display**: View other participants in a separate panel
- **Separate Video Controls**: Independent start/stop controls for self and remote video
- **Side-by-Side Layout**: Clean separation between your video and others

#### 🎤 **Audio Controls**
- **Mute/Unmute Microphone**: Toggle your audio transmission
- **Audio Status Indicators**: Visual feedback for audio state
- **Real-time Audio Control**: Instant mute/unmute during sessions

#### 🔧 **Device Selection**
- **Camera Selection**: Choose from available cameras with dropdown menu
- **Microphone Selection**: Select your preferred microphone
- **Speaker Selection**: Pick your audio output device
- **Hot-swapping**: Change devices during active sessions

#### 🏠 **Session Management**
- **Join Sessions**: Connect to Zoom Video SDK sessions
- **Leave Sessions**: Gracefully disconnect with proper cleanup
- **Session Status**: Real-time status updates and error handling
- **Automatic Setup**: Device enumeration and video initialization

## 🏗️ Application Architecture

### Key Components

```
┌─────────────────────────────────────────────────────────────┐
│                    GTK User Interface                       │
├─────────────────────────────────────────────────────────────┤
│  Session Controls  │  Device Selection  │  Video Controls   │
├─────────────────────────────────────────────────────────────┤
│     Self Video      │                   │   Remote Video    │
│    (Left Panel)     │    Status Area    │   (Right Panel)   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Video Processing Layer                    │
├─────────────────────────────────────────────────────────────┤
│  VideoRenderer     │  VideoDisplayBridge  │  Device Manager │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Zoom Video SDK                          │
└─────────────────────────────────────────────────────────────┘
```

### Core Classes

#### **VideoRenderer**
- Handles video frame rendering using SDL2 and GTK
- Converts YUV420 video data to RGB for display
- Manages embedded video widgets in GTK interface

#### **VideoDisplayBridge**
- Connects Zoom SDK video pipes to renderers
- Manages video stream lifecycle (subscribe/unsubscribe)
- Routes video data from SDK to appropriate display panels

#### **Device Management**
- Enumerates available cameras, microphones, and speakers
- Provides dropdown selection interfaces
- Handles device switching during active sessions

## 📖 Learning Guide

### Step 1: Understanding Session Management

The application demonstrates how to:

```cpp
// Initialize the SDK
ZoomVideoSDKInitParams init_params;
init_params.domain = "https://go.zoom.us";
init_params.enableLog = true;
video_sdk_obj->initialize(init_params);

// Join a session
ZoomVideoSDKSessionContext session_context;
session_context.sessionName = session_name.c_str();
session_context.userName = username.c_str();
session_context.token = signature.c_str();
video_sdk_obj->joinSession(session_context);
```

### Step 2: Video Rendering Pipeline

Learn how video flows through the system:

1. **SDK captures video** from camera or receives from network
2. **VideoDisplayBridge subscribes** to video pipe for specific users
3. **Raw YUV420 frames** are received via delegate callbacks
4. **VideoRenderer converts** YUV to RGB and displays in GTK widget

### Step 3: Device Selection

Understand device enumeration and selection:

```cpp
// Get available cameras
IZoomVideoSDKVideoHelper* videoHelper = video_sdk_obj->getVideoHelper();
IVideoSDKVector<IZoomVideoSDKCameraDevice*>* cameraList = videoHelper->getCameraList();

// Select a camera
videoHelper->selectCamera(selectedCameraId);
```

### Step 4: Audio/Video Controls

Learn mute/unmute and start/stop video:

```cpp
// Audio control
IZoomVideoSDKAudioHelper* audioHelper = video_sdk_obj->getAudioHelper();
audioHelper->muteAudio(currentUser);
audioHelper->unMuteAudio(currentUser);

// Video control
IZoomVideoSDKVideoHelper* videoHelper = video_sdk_obj->getVideoHelper();
videoHelper->startVideo();
videoHelper->stopVideo();
```

## 🔍 Code Structure

### Main Application (`zoom_v-sdk_linux_bot.cpp`)
- **GUI Setup**: Creates GTK interface with all controls
- **Event Handlers**: Manages button clicks and device selection
- **SDK Integration**: Initializes and manages Zoom Video SDK
- **Video Separation**: Implements separate self and remote video displays

### Video Components
- **`VideoRenderer.h/cpp`**: SDL2-based video rendering engine
- **`VideoDisplayBridge.h/cpp`**: Video pipeline management
- **`PreviewVideoHandler.h/cpp`**: Camera preview functionality

### Key Features Implemented

#### Separate Video Controls
```cpp
// Self video (your camera)
void on_self_video_clicked() {
    if (g_self_video_enabled) {
        videoHelper->stopVideo();  // Stop transmission
        cleanupSelfVideo();        // Clean display
    } else {
        videoHelper->startVideo(); // Start transmission
        setupSelfVideo();          // Setup display
    }
}

// Remote video (other participants)
void on_remote_video_clicked() {
    g_remote_video_enabled = !g_remote_video_enabled;
    // Toggle remote video display
}
```

#### Device Hot-Swapping
```cpp
void on_camera_changed() {
    std::string selectedId = g_camera_combo->get_active_id();
    videoHelper->selectCamera(selectedId.c_str());
    
    // Refresh video after camera change
    cleanupSelfVideo();
    setupSelfVideo();
}
```

## 🎛️ User Interface Guide

### Session Information Panel
- **Username**: Your display name in the session
- **Session Name**: The session you want to join
- **Password**: Optional session password
- **Signature**: JWT token for authentication

### Device Settings Panel
- **Camera**: Select your video input device
- **Microphone**: Choose your audio input device
- **Speaker**: Pick your audio output device

### Session Controls
- **Join Session**: Connect to the video session
- **Leave Session**: Disconnect from the session
- **Unmute Audio**: Enable/disable your microphone
- **Mute Video**: Start/stop your camera

### Video Controls
- **Start Self Video**: Begin transmitting your camera
- **Stop Self Video**: Stop your video transmission
- **Start Remote Video**: Enable viewing others' video
- **Stop Remote Video**: Disable remote video display

### Video Display
- **Self Video (Left)**: Your camera feed
- **Remote Video (Right)**: Other participants' video

## 🔧 Configuration

### JWT Token Generation

You need a valid JWT token to join sessions. Generate one using:

1. **Zoom Marketplace**: Create a Video SDK app
2. **Get credentials**: Note your SDK Key and Secret
3. **Generate JWT**: Use the Zoom JWT generator or create programmatically

### Session Configuration

Edit `src/bin/config.json`:

```json
{
    "session_name": "learning-session-001",
    "session_psw": "",
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

## 🐛 Troubleshooting

### Common Issues

#### Build Errors
```bash
# Missing GTK development headers
sudo apt install libgtkmm-3.0-dev

# Missing SDL2 development headers  
sudo apt install libsdl2-dev

# Missing CMake
sudo apt install cmake
```

#### Runtime Issues
```bash
# Check config.json format
cat src/bin/config.json | python -m json.tool

# Verify JWT token
# Token should be valid and not expired

# Check device permissions
# Ensure camera/microphone access is allowed
```

#### Video Display Problems
```bash
# Enable debug logging
export ZOOM_SDK_LOG_LEVEL=DEBUG
./SkeletonDemo

# Check GTK version
pkg-config --modversion gtk+-3.0

# Verify SDL2 installation
pkg-config --modversion sdl2
```

## 📚 Learning Resources

### Next Steps
1. **Explore other demos** in the repository
2. **Read Zoom Video SDK documentation**
3. **Experiment with additional features** like screen sharing
4. **Build your own video application** using this as a foundation

### Related Examples
- **GetRawVideoAndAudioExample**: Learn raw data handling
- **ChatExample**: Add text messaging to your app
- **CloudRecordingExample**: Implement session recording

## 🤝 Contributing

This is a learning resource! Feel free to:
- **Report issues** you encounter while learning
- **Suggest improvements** to the documentation
- **Share your modifications** and enhancements
- **Ask questions** about the implementation

## 📄 License

This project follows the same license as the original Zoom Video SDK Linux demos repository.

---

**Happy Learning!** 🎉 This skeleton demo provides a solid foundation for understanding video SDK integration on Linux. Start here, experiment, and build amazing video applications!
