cmake_minimum_required(VERSION 3.0)

set(PROJECT vcpkg-qt)
set(VERSION 0.1)
set(PRJ_VER ${PROJECT}-${VERSION})

# --- Configuration ---

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Qt variables
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

add_compile_options($<$<CONFIG:Debug>:-DDEBUG>$<$<CONFIG:Release>:-DRELEASE>)

if (WIN32)
    add_compile_options(-DWINDOWS)
endif (WIN32)

if (UNIX)
    add_compile_options(-DLINUX)
endif (UNIX)

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

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT}
    PRIVATE spdlog::spdlog
    PRIVATE Qt::Widgets
)