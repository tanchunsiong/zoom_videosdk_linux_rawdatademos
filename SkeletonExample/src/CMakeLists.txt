cmake_minimum_required(VERSION 3.14)

project(SkeletonDemo CXX)
set(TARGET_NAME SkeletonDemo)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

# Find ALSA library for audio playback
find_package(ALSA REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/ZoomVideoSDK.cmake)

# Try to find GTKmm and SDL2 (optional for GUI)
pkg_check_modules(GTKMM gtkmm-3.0)
find_package(SDL2)

# Check if GUI dependencies are available
if(GTKMM_FOUND AND SDL2_FOUND)
    message(STATUS "GUI dependencies found - building with GTKmm interface")
    set(BUILD_GUI TRUE)
else()
    message(STATUS "GUI dependencies not found - building console version")
    set(BUILD_GUI FALSE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
zoom_video_sdk_use()

link_directories(${GLIB_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS})
include_directories(${GLIB_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER} ${GIO_CFLAGS_OTHER})

# Conditionally add GUI components
if(BUILD_GUI)
    # Add GTKmm include directories and compile options
    include_directories(${GTKMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
    add_definitions(${GTKMM_CFLAGS_OTHER})
    add_definitions(-DBUILD_GUI=1)
    
    set(GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/VideoRenderer.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/VideoDisplayBridge.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/ZoomVideoRenderer.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/PreviewVideoHandler.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/RemoteVideoRawDataHandler.cpp)
else()
    add_definitions(-DBUILD_GUI=0)
    set(GUI_SOURCES "")
endif()

add_executable(${TARGET_NAME} 
                                 ${CMAKE_CURRENT_SOURCE_DIR}/zoom_v-sdk_linux_bot.cpp  
                                 ${CMAKE_CURRENT_SOURCE_DIR}/WebService.cpp
                                 ${GUI_SOURCES})

target_link_libraries(${TARGET_NAME}  PkgConfig::deps)
target_link_libraries(${TARGET_NAME}  videosdk)
target_link_libraries(${TARGET_NAME}  curl)
target_link_libraries(${TARGET_NAME}  ${GLIB_LIBRARIES} ${GIO_LIBRARIES})
target_link_libraries(${TARGET_NAME}  ${ALSA_LIBRARIES})

# Conditionally link GUI libraries
if(BUILD_GUI)
    target_link_libraries(${TARGET_NAME}  ${GTKMM_LIBRARIES})
    target_link_libraries(${TARGET_NAME}  ${SDL2_LIBRARIES})
endif()

zoom_video_sdk_stage_config(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bin)

zoom_video_sdk_stage_runtime(${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg")
    execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg COMMAND tar -xf ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg.tar.gz -C lib/ffmpeg WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

execute_process(COMMAND mkdir -p .zoom/logs WORKING_DIRECTORY $ENV{HOME})
