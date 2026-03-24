get_filename_component(ZOOM_VIDEO_SDK_DEFAULT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../SDK" ABSOLUTE)
get_filename_component(ZOOM_VIDEO_SDK_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/../common" ABSOLUTE)
set(ZOOM_VIDEO_SDK_ROOT "${ZOOM_VIDEO_SDK_DEFAULT_ROOT}" CACHE PATH "Path to the extracted Zoom Video SDK")

if(NOT EXISTS "${ZOOM_VIDEO_SDK_ROOT}/h/zoom_video_sdk_api.h")
    message(FATAL_ERROR "Zoom Video SDK headers not found under ${ZOOM_VIDEO_SDK_ROOT}. Extract the Linux Video SDK into SDK/ at the repo root or set ZOOM_VIDEO_SDK_ROOT.")
endif()

if(NOT EXISTS "${ZOOM_VIDEO_SDK_ROOT}/libvideosdk.so")
    message(FATAL_ERROR "libvideosdk.so not found under ${ZOOM_VIDEO_SDK_ROOT}.")
endif()

function(zoom_video_sdk_use)
    include_directories(${ZOOM_VIDEO_SDK_COMMON_DIR})
    include_directories(${ZOOM_VIDEO_SDK_ROOT}/h)
    link_directories(${ZOOM_VIDEO_SDK_ROOT})
endfunction()

function(zoom_video_sdk_stage_runtime runtime_dir)
    file(COPY ${ZOOM_VIDEO_SDK_ROOT}/libcml.so DESTINATION ${runtime_dir})
    file(COPY ${ZOOM_VIDEO_SDK_ROOT}/libmpg123.so DESTINATION ${runtime_dir})
    file(COPY ${ZOOM_VIDEO_SDK_ROOT}/libvideosdk.so DESTINATION ${runtime_dir})

    if(EXISTS "${ZOOM_VIDEO_SDK_ROOT}/qt_libs")
        file(COPY ${ZOOM_VIDEO_SDK_ROOT}/qt_libs DESTINATION ${runtime_dir})
    endif()

    if(EXISTS "${ZOOM_VIDEO_SDK_ROOT}/cpthost")
        file(COPY ${ZOOM_VIDEO_SDK_ROOT}/cpthost DESTINATION ${runtime_dir})
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ZOOM_VIDEO_SDK_ROOT}/libvideosdk.so
        ${runtime_dir}/libvideosdk.so.1
    )
endfunction()

function(zoom_video_sdk_stage_config source_dir runtime_dir)
    if(EXISTS "${source_dir}/config.json")
        set(config_source "${source_dir}/config.json")
    elseif(EXISTS "${source_dir}/config.json.example")
        set(config_source "${source_dir}/config.json.example")
    else()
        message(FATAL_ERROR "Neither config.json nor config.json.example was found under ${source_dir}.")
    endif()

    file(COPY "${config_source}" DESTINATION "${runtime_dir}")
    if(NOT "${config_source}" MATCHES "/config\\.json$")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${config_source}"
            "${runtime_dir}/config.json"
        )
    endif()
endfunction()
