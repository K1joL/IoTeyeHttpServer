# cmake/FetchASIO.cmake

function(fetch_asio)
    cmake_parse_arguments(
        FETCH_ASIO
        ""
        "VERSION;VERSION_STATUS"
        ""
        ${ARGN}
    )

    if(NOT DEFINED FETCH_ASIO_VERSION)
        set(FETCH_ASIO_VERSION "1.30.2")
    endif()

    if(NOT DEFINED FETCH_ASIO_VERSION_STATUS)
        set(FETCH_ASIO_VERSION_STATUS "Stable")
    endif()

    set(ASIO_SOURCEFORGE_URL "https://sourceforge.net/projects/asio/files/asio/${FETCH_ASIO_VERSION}%20(${FETCH_ASIO_VERSION_STATUS})/asio-${FETCH_ASIO_VERSION}.tar.gz/download") #Adjust URL as necessary
    set(ASIO_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/_deps/asio-download")
    set(ASIO_EXTRACT_DIR "${CMAKE_BINARY_DIR}/_deps/asio-src")
    set(ASIO_INCLUDE_DIR "${ASIO_EXTRACT_DIR}/asio-${FETCH_ASIO_VERSION}/include")

    # Check if ASIO is already downloaded and extracted
    if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio/version.hpp")
        # Create download and extract directories
        file(MAKE_DIRECTORY "${ASIO_DOWNLOAD_DIR}")
        file(MAKE_DIRECTORY "${ASIO_EXTRACT_DIR}")

        # Download ASIO
        file(DOWNLOAD "${ASIO_SOURCEFORGE_URL}" "${ASIO_DOWNLOAD_DIR}/asio-${FETCH_ASIO_VERSION}.tar.gz"
            SHOW_PROGRESS
            STATUS download_status)

        list(GET download_status 0 download_code)
        if(NOT "${download_code}" STREQUAL "0")
            message(FATAL_ERROR "Failed to download ASIO from SourceForge: ${download_status}")
        endif()

        # Extracting ASIO
        file(ARCHIVE_EXTRACT INPUT "${ASIO_DOWNLOAD_DIR}/asio-${FETCH_ASIO_VERSION}.tar.gz" DESTINATION "${ASIO_EXTRACT_DIR}")

        # Check if extraction was successful.
        if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio/version.hpp")
            message(FATAL_ERROR "Failed to extract ASIO.  Check execute_process command.")
        endif()
    else()
        message(STATUS "ASIO already downloaded and extracted.")
    endif()

    # Add asio lib
    add_library(asio INTERFACE)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_include_directories(asio INTERFACE "${ASIO_INCLUDE_DIR}")
    target_link_libraries(asio INTERFACE pthread)
endfunction()