cmake_minimum_required(VERSION 3.0)

set(PROJECT vcpkg-test)
set(VERSION 0.1)
set(PRJ_VER ${PROJECT}-${VERSION})

# --- Configuration ---

add_compile_options($<$<CONFIG:Debug>:-DDEBUG>$<$<CONFIG:Release>:-DRELEASE>)

macro(set_from_environment VARIABLE)
    if (NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
        set(${VARIABLE} $ENV{${VARIABLE}})
    endif ()
endmacro()

set_from_environment(VCPKG_ROOT)
if (DEFINED VCPKG_ROOT)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    set(VCPKG_HOST_TRIPLET "x64-windows-static-md" CACHE STRING "")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static-md" CACHE STRING "")
else ()
    message(
        WARNING
        "Variable VCPKG_ROOT is not set. Continuing without vcpkg."
    )
endif ()

# --- Project ---

project(
    ${PROJECT}
    VERSION ${VERSION}
    LANGUAGES CXX
)

set(
    SRC
    main.cpp
)

add_executable(${PROJECT} ${SRC})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT} PRIVATE spdlog::spdlog)