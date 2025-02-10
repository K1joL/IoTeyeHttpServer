cmake_minimum_required(VERSION 3.18)
project(ioteyeserver_lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)
include_directories(include/httpserver)

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Option to specify ASIO version
option(ASIO_VERSION "Specify the ASIO version to use" "1.30.2")

# --- Download ASIO ---
include(${CMAKE_CURRENT_LIST_DIR}/CMakeModules/FetchASIO.cmake)
if(ASIO_VERSION)
    # Specified ASIO VERSION
    message(STATUS "ASIO_VERSION: ${ASIO_VERSION}")
    fetch_asio(VERSION ${ASIO_VERSION})
else()
    # Default ASIO VERSION 1.30.2
    message(STATUS "ASIO_VERSION: Default version 1.30.2")
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
target_link_libraries(${PROJECT_NAME} PUBLIC asio)

# Function to add examples automatically
function(add_example FILE_NAME)
    get_filename_component(EXAMPLE_NAME ${FILE_NAME} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${FILE_NAME})
    target_link_libraries(${EXAMPLE_NAME} ${PROJECT_NAME})
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
    asio # link asio here as well
    pthread
)

add_test(NAME webserver_tests COMMAND ioteye_tests)
