# Zoom Video SDK Linux Demo with Device Selection

A comprehensive Linux demonstration application for the Zoom Video SDK featuring a GTK-based GUI with complete device selection capabilities, self-video rendering, and multi-participant video conferencing.

## Features

### 🎥 **Video Conferencing**
- Join/leave Zoom Video SDK sessions
- Real-time video display for all participants
- Self-video rendering with camera preview
- Proper YUV420 to RGB color conversion (fixes green/corrupted video)
- GTK-integrated video display (no separate SDL windows)

### 🎛️ **Device Management**
- **Camera Selection**: Dropdown to select from available cameras
- **Microphone Selection**: Dropdown to select from available microphones  
- **Speaker Selection**: Dropdown to select from available speakers
- Real-time device switching during active sessions
- Automatic device enumeration when joining sessions

### 🎮 **Session Controls**
- Audio mute/unmute functionality
- Video start/stop controls
- Session join/leave with proper cleanup
- Status display with real-time updates

### 🖥️ **User Interface**
- Clean GTK-based interface optimized for low-resolution screens
- Compact 700x500 window size
- Organized layout with clearly labeled sections
- Device settings prominently displayed for easy access

## Prerequisites

### System Requirements
- Linux operating system
- GTK 3.0+ development libraries
- SDL2 development libraries
- CMake 3.10+
- GCC with C++11 support

### Dependencies Installation

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install libgtkmm-3.0-dev libsdl2-dev
sudo apt install libglib2.0-dev libjsoncpp-dev
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install gcc-c++ cmake
sudo yum install gtkmm30-devel SDL2-devel
sudo yum install glib2-devel jsoncpp-devel
```

## Building the Application

1. **Clone the repository:**
   ```bash
   git clone https://github.com/tanchunsiong/zoom_videosdk_linux_rawdatademos.git
   cd zoom_videosdk_linux_rawdatademos/SkeletonExample
   ```

2. **Build with CMake:**
   ```bash
   cd src
   mkdir -p build
   cd build
   cmake ..
   make
   ```

3. **The executable will be created at:**
   ```
   src/bin/SkeletonDemo
   ```

## Configuration

### Session Configuration
Create or edit `src/bin/config.json` with your session details:

```json
{
    "session_name": "your-session-name",
    "session_psw": "optional-password",
    "token": "your-zoom-sdk-jwt-token"
}
```

### JWT Token Generation
You need a valid Zoom Video SDK JWT token. Generate one using:
- Your Zoom Video SDK App credentials
- Session name matching your config
- Appropriate expiration time

**Example token payload:**
```json
{
    "iss": "your-api-key",
    "exp": 1234567890,
    "alg": "HS256",
    "aud": "zoom",
    "appKey": "your-api-key",
    "tokenExp": 1234567890,
    "sessionName": "your-session-name",
    "userIdentity": "user-id",
    "sessionKey": "session-key",
    "geoRegions": "US",
    "cloudRecordingOption": 0,
    "cloudRecordingElection": 0
}
```

## Running the Application

### GUI Mode (Default)
```bash
cd src
./bin/SkeletonDemo
```

### Console Mode
```bash
cd src
# Build without GUI
cmake -DBUILD_GUI=OFF ..
make
./bin/SkeletonDemo
```

## User Interface Guide

### Main Window Layout

1. **Session Information**
   - Username: Your display name in the session
   - Session Name: Name of the Zoom session to join
   - Password: Optional session password
   - Signature: JWT token for authentication

2. **Device Settings** ⭐ *New Feature*
   - **Camera**: Select from available cameras
   - **Microphone**: Select from available microphones
   - **Speaker**: Select from available speakers
   - Devices auto-populate when joining a session

3. **Session Controls**
   - Join Session: Connect to the Zoom session
   - Leave Session: Disconnect from the session
   - Unmute/Mute Audio: Toggle microphone
   - Unmute/Mute Video: Toggle camera

4. **Status Display**
   - Real-time status updates
   - Connection status and error messages

5. **Video Display**
   - Shows your own video (self-view)
   - Displays other participants' video streams
   - Proper color rendering with YUV420→RGB conversion

## Technical Architecture

### Core Components

- **VideoRenderer**: Handles video display with GTK/Cairo integration
- **VideoDisplayBridge**: Manages video pipeline between SDK and renderer
- **Device Management**: Enumerates and controls audio/video devices
- **Session Management**: Handles Zoom SDK session lifecycle

### Video Pipeline
```
Zoom SDK → VideoDisplayBridge → VideoRenderer → GTK DrawingArea
```

### Device Selection Flow
```
Session Join → Device Enumeration → Populate Dropdowns → User Selection → Device Switch
```

## Troubleshooting

### Common Issues

**Build Errors:**
- Ensure all development libraries are installed
- Check CMake version (3.10+ required)
- Verify GTK and SDL2 development packages

**Runtime Issues:**
- Check config.json format and JWT token validity
- Ensure proper audio/video device permissions
- Verify network connectivity for Zoom services

**Video Display Problems:**
- Confirm GTK and Cairo libraries are properly installed
- Check video device permissions (`/dev/video*`)
- Ensure proper graphics drivers

**Device Selection Not Working:**
- Verify audio/video devices are not locked by other applications
- Check device permissions in `/dev/`
- Ensure PulseAudio/ALSA is properly configured

### Debug Information

Enable verbose logging by setting environment variables:
```bash
export ZOOM_SDK_LOG_LEVEL=DEBUG
export GTK_DEBUG=all
./bin/SkeletonDemo
```

## Development

### Project Structure
```
SkeletonExample/
├── src/
│   ├── zoom_v-sdk_linux_bot.cpp    # Main application
│   ├── VideoRenderer.h/cpp         # Video rendering engine
│   ├── VideoDisplayBridge.h/cpp    # Video pipeline bridge
│   ├── CMakeLists.txt              # Build configuration
│   └── bin/
│       ├── config.json             # Session configuration
│       └── SkeletonDemo            # Compiled executable
└── README.md                       # This file
```

### Key Features Implementation

**Device Selection:**
- Real-time device enumeration using Zoom SDK APIs
- GTK ComboBoxText widgets for device selection
- Automatic device switching with video refresh

**Video Rendering:**
- YUV420 to RGB conversion with proper video range handling
- Cairo-based rendering for GTK integration
- Multi-participant video display support

**Session Management:**
- Thread-safe UI updates using GLib main loop
- Proper resource cleanup on session leave
- Error handling and status reporting

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project follows the same license as the original Zoom Video SDK Linux demos repository.

## Support

For issues related to:
- **Zoom SDK**: Check [Zoom Developer Documentation](https://developers.zoom.us/)
- **This Implementation**: Create an issue in the repository
- **Device-specific problems**: Check your Linux audio/video subsystem configuration

---

**Note**: This application requires valid Zoom Video SDK credentials and proper device permissions to function correctly.
