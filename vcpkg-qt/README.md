# vcpkg-qt example

Simplistic project to show how to use vcpkg to build a Qt application without using Qt Creator (Qt Designer was used to create the ui file though).

## How to use

### Setup

- [cmake](https://cmake.org/download/)
- [vcpkg](https://vcpkg.io/en/getting-started.html)

Then set the following environment variable :

    VCPKG_ROOT=<path to vcpkg>

This variable is used to locate vcpkg in `CMakePresets.json`.

### Build

Now go to the project directory and build the example program :

    cmake --preset=release
    cmake --build build/release

If on Windows append `win` to the preset name.

During the process you should see vcpkg pulling dependencies and installing them into the build directory (it will take a long time on the first time since it has to compile Qt).