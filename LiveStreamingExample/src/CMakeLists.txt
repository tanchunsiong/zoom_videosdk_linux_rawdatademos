cmake_minimum_required(VERSION 3.14)

project(LiveStreamingDemo CXX)
set(TARGET_NAME LiveStreamingDemo)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/zoom_video_sdk)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/zoom_video_sdk)

add_executable(${TARGET_NAME} 

                                 ${CMAKE_CURRENT_SOURCE_DIR}/zoom_v-sdk_linux_bot.cpp  
                                 ${CMAKE_CURRENT_SOURCE_DIR}/WebService.cpp)

target_link_libraries(${TARGET_NAME}  PkgConfig::deps)
target_link_libraries(${TARGET_NAME}  videosdk)
target_link_libraries(${TARGET_NAME}  curl)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.json ${CMAKE_CURRENT_SOURCE_DIR}/bin/config.json COPYONLY)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/zoom_video_sdk/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg")
    execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg COMMAND tar -xf ${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg.tar.gz -C lib/ffmpeg WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/zoom_video_sdk/qt_libs")
    execute_process(COMMAND tar -xf ${CMAKE_CURRENT_SOURCE_DIR}/lib/qt_libs.tar.gz -C lib/zoom_video_sdk WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

execute_process(COMMAND mkdir -p .zoom/logs WORKING_DIRECTORY $ENV{HOME})
