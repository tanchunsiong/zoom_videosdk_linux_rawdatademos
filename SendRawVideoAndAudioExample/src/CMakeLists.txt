cmake_minimum_required(VERSION 3.14)

project(SendRawVideoAndAudioDemo CXX)
set(TARGET_NAME SendRawVideoAndAudioDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavfilter libavformat libavcodec libavutil libswscale libswresample)
execute_process(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=includedir libavfilter
    OUTPUT_VARIABLE FFMPEG_INCLUDEDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(OpenCV REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/ZoomVideoSDK.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${FFMPEG_INCLUDEDIR})
zoom_video_sdk_use()

link_directories(${GLIB_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS})
include_directories(${GLIB_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER} ${GIO_CFLAGS_OTHER} ${FFMPEG_CFLAGS_OTHER})

add_executable(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/ZoomVideoSDKVirtualAudioMic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ZoomVideoSDKShareSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ZoomVideoSDKVideoSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zoom_v-sdk_linux_bot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WebService.cpp
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        PkgConfig::deps
        PkgConfig::FFMPEG
        videosdk
        curl
        ${GLIB_LIBRARIES}
        ${GIO_LIBRARIES}
        ${OpenCV_LIBS}
)

zoom_video_sdk_stage_config(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bin)

zoom_video_sdk_stage_runtime(${CMAKE_CURRENT_SOURCE_DIR}/bin)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Big_Buck_Bunny_720_10s_1MB.mp4 DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

execute_process(COMMAND mkdir -p .zoom/logs WORKING_DIRECTORY $ENV{HOME})
