# vcpkg example

Simplistic project to show how to use vcpkg on Windows for pulling dependencies in a clean and automated manner. It works on other OSes too but they have proper package management already so it's not as useful.

## How to use

First [install vcpkg](https://vcpkg.io/en/getting-started.html).

Then set the following environment variable :

    VCPKG_ROOT=path to vcpkg

This variable is used by `CMakeLists.txt` to know the location of vcpkg.

Now go to the project directory and build the example program :

    cmake -B build
    cmake --build build

During the process you should see vcpkg pulling dependencies and installing them into the build directory.

## Explanation

Thanks to cmake and its vcpkg integration, it works thanks to a few variables.

In `CMakeLists.txt` these variables are set :

    CMAKE_TOOLCHAIN_FILE # Point cmake to the vcpkg install
    VCPKG_HOST_TRIPLET   # Tell vcpkg which triplet to download dependencies for
    VCPKG_TARGET_TRIPLET # Tell vcpkg how to build the dependencies

These variables, along with the other we set earlier, are all we need for a basic link between cmake and vcpkg.