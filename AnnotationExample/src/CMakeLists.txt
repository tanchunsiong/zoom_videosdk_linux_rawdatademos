cmake_minimum_required(VERSION 3.14)

project(AnnotationDemo CXX)
set(TARGET_NAME AnnotationDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/ZoomVideoSDK.cmake)
zoom_video_sdk_use()

add_executable(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/annotation_demo.cpp
    ${ZOOM_VIDEO_SDK_COMMON_DIR}/ConsoleVideoSDKApp.cpp
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        PkgConfig::GLIB
        videosdk
        curl
)

zoom_video_sdk_stage_config(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bin)
zoom_video_sdk_stage_runtime(${CMAKE_CURRENT_SOURCE_DIR}/bin)
