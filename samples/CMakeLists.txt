# ---------------------------------------------------------------------------
# Sample programs for the XID device driver.
#
# Enabled by default when this repository is the top-level project, off when
# it's pulled in by another project via add_subdirectory(). Toggle with
#   -DXID_BUILD_SAMPLES=ON|OFF
# ---------------------------------------------------------------------------

function(xid_add_sample target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE xid_device_driver)

    # Windows has no rpath: put the driver and FTDI DLLs next to the .exe so
    # the samples run straight out of the build tree. On macOS/Linux, CMake's
    # build rpath already points at both libraries.
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:xid_device_driver>"
                    "$<TARGET_FILE:ftd2xx::ftd2xx>"
                    "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endif()
endfunction()

# Reads responses
xid_add_sample(responses       responses.cpp)
# Sends event markers
xid_add_sample(event_markers   event_markers.cpp)
# Sets up and executes a pulse table
xid_add_sample(pulsetable_test pulsetable_test.cpp)