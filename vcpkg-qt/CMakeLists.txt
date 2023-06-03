cmake_minimum_required(VERSION 3.0)

set(PROJECT vcpkg-qt)
set(VERSION 0.1)
set(PRJ_VER ${PROJECT}-${VERSION})

# --- Configuration ---

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Qt variables
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

add_compile_options($<$<CONFIG:Debug>:-DDEBUG>$<$<CONFIG:Release>:-DRELEASE>)

# --- Project ---

project(
    ${PROJECT}
    VERSION ${VERSION}
    LANGUAGES CXX
)

set(
    SRC
    src/main.cpp
    src/MainWindow.cpp
)

add_executable(${PROJECT} ${SRC})

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT} PROPERTY WIN32_EXECUTABLE true)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT}
    PRIVATE spdlog::spdlog
    PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets
    $<$<CONFIG:RelWithDebInfo>:Qt5::WinMain>
    $<$<CONFIG:Release>:Qt5::WinMain>
    $<$<CONFIG:MinSizeRel>:Qt5::WinMain>
)