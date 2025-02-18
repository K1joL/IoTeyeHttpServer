cmake_minimum_required(VERSION 3.18)
project(ioteyeserver VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)
include_directories(include/httpserver)
include(cmake/CMakeModules/FetchASIO.cmake)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# --- Download ASIO ---
find_package(asio QUIET)
if(asio_FOUND)
    message(STATUS "ASIO library found.")
else()
    message(STATUS "ASIO library not found.")
    # Force the script to use FetchContent and not CPM internally to fetch asio
    option(ASIO_USE_CPM "Download Asio with CPM instead of FetchContent" OFF)
    option(
        ASIO_CPM_FETCHCONTENT_COMPAT
        "Should asio be declared with FetchContent functions to be compatible. This doesn't not allow CPM cache to work."
        ON
    )
    option(ASIO_ENABLE_INSTALL "Install asio target." ON)
    if(NOT ASIO_ENABLE_INSTALL)
        message(STATUS "ASIO library will be downloaded and installed.\nUse ASIO_ENABLE_INSTALL=OFF to off this option")
        set(ASIO_ENABLE_INSTALL ON)
    endif()
    option(ASIO_TAG "Asio git tag. Format: asio-*-**-*" "asio-1-31-0")
    # Download this repository
    fetch_asio()
endif()
# --- End Download ASIO ---

add_library(${PROJECT_NAME}
    src/httpserver/http_resource.cpp
    src/httpserver/http_request.cpp
    src/httpserver/http_response.cpp
    src/httpserver/webserver.cpp
    src/utils.cpp
)

# Add target link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE asio::asio)

# Conditionally add logging definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Logging enabled!")
    add_compile_definitions(ENABLE_LOGGING)
endif()

# --- Examples ---
# Function to add examples automatically
function(add_example FILE_NAME)
    get_filename_component(EXAMPLE_NAME ${FILE_NAME} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${FILE_NAME})
    target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME} asio::asio)
endfunction()

# Automatically add examples
file(GLOB EXAMPLE_FILES "examples/*.cpp")
foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
    add_example(${EXAMPLE_FILE})
endforeach()

# --- Tests ---
enable_testing()

# Add GoogleTest (you might need to adjust the path based on your installation)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0 # Or the version you prefer
)
FetchContent_MakeAvailable(googletest)

# Add tests executable
add_executable(ioteye_tests
    tests/webserver_test.cpp
    src/utils.cpp
    src/httpserver/http_request.cpp
    src/httpserver/http_response.cpp
    src/httpserver/http_resource.cpp
    src/httpserver/webserver.cpp
)

target_link_libraries(ioteye_tests
    ${PROJECT_NAME}
    GTest::gtest_main
    asio::asio
)

add_test(NAME webserver_tests COMMAND ioteye_tests)

# --- Include Install/Uninstall Module ---
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    set(LIB_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/lib/)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    set(LIB_INSTALL_PATH /usr/local/lib/)
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${LIB_INSTALL_PATH}
    ARCHIVE DESTINATION ${LIB_INSTALL_PATH}
    RUNTIME DESTINATION ${LIB_INSTALL_PATH}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)

# Create and install the CMake configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ioteyeserverConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ioteyeserver
    PATH_VARS CMAKE_INSTALL_PREFIX
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ioteyeserverTargets.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverTargets.cmake"
    INSTALL_DESTINATION lib/cmake/ioteyeserver
    PATH_VARS CMAKE_INSTALL_PREFIX
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverTargets.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ioteyeserver"
)
# make uninstall
add_custom_target("uninstall" COMMENT "Uninstall installed files")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vfd < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!
)
