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

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT}
    PRIVATE spdlog::spdlog
    PRIVATE Qt::Widgets
)

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_property(TARGET ${PROJECT} PROPERTY WIN32_EXECUTABLE true)
    endif()

    install(TARGETS ${PROJECT}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    qt_generate_deploy_app_script(
        TARGET ${PROJECT}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
