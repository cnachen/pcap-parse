cmake_minimum_required(VERSION 3.14)  # FetchContent is available in 3.11+, but 3.14+ is recommended.

project(PcapParseProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8  # Replace with your desired git commit
)

# Check if cpr has already been populated (previously downloaded and configured)
FetchContent_GetProperties(cpr)
if(NOT cpr_POPULATED)
    FetchContent_Populate(cpr)
    add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR})
endif()

# Find the required libpcap package using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(PCAP REQUIRED libpcap)

# Find jsoncpp package using pkg-config, or fallback to find_package
pkg_search_module(JSONCPP jsoncpp)
if(NOT JSONCPP_FOUND)
    find_package(JsonCpp REQUIRED)
endif()

# Add executable target with source files
add_executable(pcap_parse pcap_parse.cpp)

# Include directories for pcap and jsoncpp for pcap_parse target
target_include_directories(pcap_parse PRIVATE ${PCAP_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})

# Link libraries to pcap_parse target
target_link_libraries(pcap_parse PRIVATE
    ${PCAP_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    cpr::cpr
    ssl
    crypto
)

# Instruct CMake to use pkg-config to set the include path for the target
if(PCAP_FOUND)
    target_include_directories(pcap_parse PRIVATE ${PCAP_INCLUDE_DIRS})
    target_link_directories(pcap_parse PRIVATE ${PCAP_LIBRARY_DIRS})
    target_compile_options(pcap_parse PRIVATE ${PCAP_CFLAGS_OTHER})
endif()

if(JSONCPP_FOUND)
    target_include_directories(pcap_parse PRIVATE ${JSONCPP_INCLUDE_DIRS})
    target_link_directories(pcap_parse PRIVATE ${JSONCPP_LIBRARY_DIRS})
    target_compile_options(pcap_parse PRIVATE ${JSONCPP_CFLAGS_OTHER})
endif()
