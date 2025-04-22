# MIT License

# Copyright (c) 2025 Shults Bogdan aka K1joL

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.18)
project(ioteyeserver VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define options
option(ASIO_TAG "Asio git tag. Format: asio-*-**-*" "asio-1-31-0")
option(ASIO_ENABLE_INSTALL "Install asio target." ON)
option(ASIO_USE_CPM "Download Asio with CPM instead of FetchContent" ON)
option(
    ASIO_CPM_FETCHCONTENT_COMPAT
    "Should asio be declared with FetchContent functions to be compatible. This doesn't not allow CPM cache to work."
    OFF
)
option(IOTEYE_ENABLE_TESTS "Enable tests compilation" OFF)


if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)
include_directories(include/ioteyeserver)
include_directories(include/ioteyeserver/httpserver)
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
    if(ASIO_ENABLE_INSTALL)
        message(STATUS "ASIO library will be installed. Use ASIO_ENABLE_INSTALL=OFF to off this option")
    endif()
    # Download this repository
    fetch_asio()
endif()
# --- End Download ASIO ---

add_library(${PROJECT_NAME}
    src/ioteyeserver/httpserver/http_resource.cpp
    src/ioteyeserver/httpserver/http_request.cpp
    src/ioteyeserver/httpserver/http_response.cpp
    src/ioteyeserver/httpserver/webserver.cpp
    src/ioteyeserver/utils.cpp
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

if(IOTEYE_ENABLE_TESTS)

    if(MSVC)
        message(WARNING "Tests cannot be built with MSVC!")
    else()
        # --- Tests ---
        message(STATUS "Tests compilation is ON!")
        enable_testing()

        # Add GoogleTest (you might need to adjust the path based on your installation)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest
            GIT_TAG v1.14.0 # Or the version you prefer
        )
        FetchContent_MakeAvailable(googletest)

        # Function to add a test
        function(add_test_executable TEST_FILE)
            get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
            add_executable(${TEST_NAME} ${TEST_FILE})
            target_link_libraries(${TEST_NAME}
                ${PROJECT_NAME}
                GTest::gtest_main
                asio::asio
            )
            add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        endfunction()

        # Add individual tests
        add_test_executable(tests/utils_test.cpp)
        add_test_executable(tests/http_resource_test.cpp)
        add_test_executable(tests/http_response_test.cpp)
        add_test_executable(tests/http_request_test.cpp)
        add_test_executable(tests/webserver_test.cpp)
    endif()
endif()
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

install(FILES include/ioteyeserver.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include) # Install general header to include directly
install(DIRECTORY include/ioteyeserver/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}) # Install rest of headers to ioteyeserver

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
