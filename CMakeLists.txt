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
option(IOTEYE_ENABLE_TESTS "Enable tests compilation" OFF)
option(IOTEYE_ENABLE_EXAMPLES "Enable tests compilation" OFF)
option(IOTEYE_USE_BOOST_ASIO "Use Boost.Asio instead of standalone Asio" OFF)

# Include standard install directories
include(GNUInstallDirs)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Find the requested dependency
if(IOTEYE_USE_BOOST_ASIO)
    message(STATUS "ioteyeserver: Using Boost.Asio")
    find_package(Boost REQUIRED)
else()
    message(STATUS "ioteyeserver: Using standalone Asio")
    find_package(asio REQUIRED)
endif()

add_library(${PROJECT_NAME}
    src/ioteyeserver/httpserver/http_resource.cpp
    src/ioteyeserver/httpserver/http_request.cpp
    src/ioteyeserver/httpserver/http_response.cpp
    src/ioteyeserver/httpserver/webserver.cpp
    src/ioteyeserver/utils.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Conditionally link and add compile definitions
if(IOTEYE_USE_BOOST_ASIO)
    # Modern CMake provides Boost::headers for header-only boost libraries
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::headers)
    target_compile_definitions(${PROJECT_NAME} PUBLIC IOTEYE_USE_BOOST_ASIO)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC asio::asio)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASIO_STANDALONE)
endif()

# Conditionally add logging definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Logging enabled!")
    add_compile_definitions(ENABLE_LOGGING)
endif()

if(IOTEYE_ENABLE_EXAMPLES)
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
endif()

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

# --- Installation ---
# 1. Install the target and assign it to an EXPORT set
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 2. Install your headers
install(FILES include/ioteyeserver.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ioteyeserver/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ioteyeserver
)

# 3. Generate and install the Targets file automatically
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# 4. Generate and install the Config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ioteyeserverConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ioteyeserverConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# make uninstall
add_custom_target(uninstall COMMENT "Uninstall installed files")
add_custom_command(
    TARGET uninstall
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vfd < install_manifest.txt || echo "Nothing in install_manifest.txt to be uninstalled!"
)
