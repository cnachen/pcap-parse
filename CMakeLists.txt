# FetchContent is available in 3.11+, but 3.14+ is recommended.
cmake_minimum_required(VERSION 3.14)

project(BipEck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Check if cpr has already been populated (previously downloaded and configured)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8
)

FetchContent_GetProperties(cpr)
if(NOT cpr_POPULATED)
    FetchContent_Populate(cpr)
    add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR})
endif()

# Find the required libpcap package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(PCAP REQUIRED libpcap)

# Find the required libarchive package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(ARCHIVE REQUIRED libarchive)

# Find jsoncpp package using pkg-config, or fallback to find_package
pkg_search_module(JSONCPP jsoncpp)
if(NOT JSONCPP_FOUND)
    find_package(JsonCpp REQUIRED)
endif()

# Add executable target with source files
add_executable(bip-eck src/bip_eck.cpp
                       src/workers/mongodb_uploader.cpp
                       src/workers/obs_observer.cpp
                       src/workers/pcap_parser.cpp
                       src/workers/worker.cpp
                       src/handlers/packet_handler.cpp
                       src/handlers/protocol_handler.cpp
                       src/handlers/someip_protocol_handler.cpp
                       src/api.cpp)

# Include our headers
target_include_directories(bip-eck PRIVATE include)

# Include directories for pcap and jsoncpp for bip-eck target
target_include_directories(bip-eck PRIVATE ${PCAP_INCLUDE_DIRS}
                                           ${JSONCPP_INCLUDE_DIRS}
                                           ${ARCHIVE_INCLUDE_DIRS})

# Link libraries to bip-eck target
target_link_libraries(bip-eck PRIVATE
    ${PCAP_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${ARCHIVE_LIBRARIES}
    cpr::cpr
    ssl
    crypto
)

# Instruct CMake to use pkg-config to set the include path for the target
if(PCAP_FOUND)
    target_include_directories(bip-eck PRIVATE ${PCAP_INCLUDE_DIRS})
    target_link_directories(bip-eck PRIVATE ${PCAP_LIBRARY_DIRS})
    target_compile_options(bip-eck PRIVATE ${PCAP_CFLAGS_OTHER})
endif()

if(JSONCPP_FOUND)
    target_include_directories(bip-eck PRIVATE ${JSONCPP_INCLUDE_DIRS})
    target_link_directories(bip-eck PRIVATE ${JSONCPP_LIBRARY_DIRS})
    target_compile_options(bip-eck PRIVATE ${JSONCPP_CFLAGS_OTHER})
endif()

if(ARCHIVE_FOUND)
    target_include_directories(bip-eck PRIVATE ${ARCHIVE_INCLUDE_DIRS})
    target_link_directories(bip-eck PRIVATE ${ARCHIVE_LIBRARY_DIRS})
    target_compile_options(bip-eck PRIVATE ${ARCHIVE_CFLAGS_OTHER})
endif()
