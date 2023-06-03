# vcpkg-qt example

Simplistic project to show how to use vcpkg to build a Qt application without using Qt Creator (Qt Designer was used to create the ui file though).

## How to use

### Setup

- [cmake](https://cmake.org/download/)
- [vcpkg](https://vcpkg.io/en/getting-started.html)

Then set the following environment variable :

    VCPKG_ROOT=<path to vcpkg>

This variable is used in `CMakeLists.txt` to know the location of vcpkg.

### Build and run the project

Now go to the project directory and build the example program :

    cmake --preset=debug
    cd build/debug
    cmake --build .

During the process you should see vcpkg pulling dependencies and installing them into the build directory (it will take a long time since it has to compile qt5-base).

Now run the program :

    .\vcpkg-qt.exe