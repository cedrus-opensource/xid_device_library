cmake_minimum_required(VERSION 3.18)

if(NOT CMAKE_OSX_ARCHITECTURES)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS target architectures" FORCE)
endif()
if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)
endif()

project(xid_device_driver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FTD2XX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ftd2xx")

add_library(ftd2xx::ftd2xx SHARED IMPORTED GLOBAL)
set_target_properties(ftd2xx::ftd2xx PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FTD2XX_DIR}")
if(WIN32)
    set_target_properties(ftd2xx::ftd2xx PROPERTIES
        IMPORTED_LOCATION "${FTD2XX_DIR}/ftd2xx.dll"
        IMPORTED_IMPLIB   "${FTD2XX_DIR}/ftd2xx.lib"
    )
elseif(APPLE)
    set_target_properties(ftd2xx::ftd2xx PROPERTIES
        IMPORTED_LOCATION "${FTD2XX_DIR}/libftd2xx.dylib"
        IMPORTED_SONAME   "@rpath/libftd2xx.dylib"
    )
else()
    set_target_properties(ftd2xx::ftd2xx PROPERTIES IMPORTED_LOCATION "${FTD2XX_DIR}/libftd2xx.so")
endif()

# ---------------------------------------------------------------------------
# The driver library
# ---------------------------------------------------------------------------
file(GLOB XID_SOURCES CONFIGURE_DEPENDS xid_device_driver/*.cpp)

add_library(xid_device_driver SHARED ${XID_SOURCES})

# Export every symbol from the DLL on Windows (macOS/Linux already export everything by default)
set_target_properties(xid_device_driver PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(xid_device_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/xid_device_driver")

target_link_libraries(xid_device_driver PUBLIC ftd2xx::ftd2xx)