# WNFramework

[![License][License]](LICENSE)
[![C++ Version][C++ Version]][C++ Reference]
[![CMake Version][CMake Version]][CMake Reference]
[![CI][CI Badge]][CI Workflow]

A set of cross platform libraries and tools built for game development.

## Development

Currently development is supported on **Windows** and **Linux**. Only **Windows
10** and **Ubuntu 18+** have been tested with the steps laid out in this guide.

### Environment

All environment setups assumes you will be developing for all supported targets
for that host.

#### Linux

The following packages are required.

* **gcc** (currently only `gcc-9`, `gcc-10` and `gcc-11` are supported)
* **g++** (currently only `g++-9`, `g++-10` and `g++-11` are supported)
* **git**
* **clang** (currently only `clang-10`, `clang-11` and `clang-12` are supported)
* **cmake** (minimum of **CMake 3.16.3** required)
* **ninja-build**
* **python3**
* **libx11-xcb-dev**
* **libxcb-keysyms1-dev**

If you want to use **Unix Makefiles** as a generator you will also need the
following.

* **make**

### Configuration

All configuration is done via **CMake**. All steps below assume you are within a
subfolder called `build` within the root of the checked out code-base.

#### Linux

```console
cmake \
  -G[Ninja|"Unix Makefiles"] \
  -DCMAKE_C_COMPILER=[gcc|clang] \ # optional, default cc will be used
  -DCMAKE_CXX_COMPILER=[g++|clang++] \ # optional, default c++ will be used
  -DCMAKE_BUILD_TYPE=[Debug|Release] \
  ../
```

Note that `Ninja` is the only offically supported generator for **Linux**. If
`CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILE` are omitted **CMake** will configure
against the default system configured compilers. You can also specify specific
versions of `gcc` or `clang` (`gcc-10.0`, `clang++-11.0`, etc.) if you have more
then a single version installed and want to target a certain version. Currently
only `gcc/g++` **9**, **10** and **11** are supported along with `clang` **10**,
**11** and **12**.

#### Windows

```console
cmake \
  -G[Ninja|"Visual Studio 15 2017"|"Visual Studio 16 2019"] \
  -Thost=x64 \  # Not needed for Ninja generator
  -Ax64 \ # Not needed for Ninja generator
  -DCMAKE_BUILD_TYPE=[Debug|Release] \ # only needed when using Ninja
  ../
```

Note that only **64-bit** builds are supported and that `Ninja` is the only
offically supported generator for **Windows**. For **Visual Studio 15 2017** and
**Visual Studio 16 2019** you must specify the architecture through the `-A`
command. The `CMAKE_BUILD_TYPE` is only needed if using `Ninja` as the generator
type. For the `Ninja` generator you will also need to run the appropriate
**Visual Studio** environment setup script in the same command prompt you intend
to use for configuration before running a configuration. After you have
completed configuration you don't need to re-run this script for
subsequent commands. See [here][Visual Studio Command Line Tools] for location
of and options for **Visual Studio** command line tools. This step may not be
required if you only have a single version of **Visual Studio** installed.

#### Android

```console
cmake \
  -G[Ninja|"Unix Makefiles"] \
  -DCMAKE_TOOLCHAIN_FILE=[WNFramework Root]/Overlays/Posix/Overlays/Android/android.toolchain.cmake \
  -DWN_ANDROID_ABIS=[arm|arm64|x86|x86_64] \
  -DWN_ANDROID_SDK=[Android SDK Root] \
  -DCMAKE_BUILD_TYPE=[Debug|Release] \
  ../
```

Note that `Ninja` is the only offically supported generator for **Android**.
Both the `[WNFramework Root]` and `[Android SDK Root]` must be absolute paths.
Currently only `x86`, `arm` and `arm64` toolchains for **Android API 22** are
supported.

### Build

All building is done via **CMake**. All steps below assume you are within a
subfolder called `build` within the root of the code-base and a configuration
has already completed successfully

```console
cmake --build .
```

#### Visual Studio

If using the command line you will also want to specify
`--config [Debug|Release]`. If omitted it will build `Debug` by default. Inside
the `build` folder generated by the [Configuration](#windows) step there is also
a `WNFramework.sln` file that can be used to perform the build but isn't
required.

### Test

All testing is done via **CMake**. All steps below assume you are within a
subfolder called `build` within the root of the code-base and a build has
already completed successfully.

```console
ctest
```

#### Visual Studio

If using the command line you will also need to specify `-C [Debug|Release]`.
If omitted it will run `Debug` by default. Inside the `build` folder generated
by the [Configuration](#windows) step there is also a `WNFramework.sln` file
containing a project named `RUN_TESTS` that can be used to run tests but isn't
required.

## Coding Conventions

This project uses a number of different languages, for language specific coding
conventions check the guides below.

* [C/C++](documents/coding-conventions/c-cpp.md)

<!-- external links -->
[License]: https://img.shields.io/github/license/WNProject/WNFramework?label=License
[C++ Version]: https://img.shields.io/badge/C%2B%2B-17-blue
[C++ Reference]: https://en.cppreference.com/w/cpp/17
[CMake Version]: https://img.shields.io/badge/CMake-3.16.3-blue
[CMake Reference]: https://cmake.org/cmake/help/v3.16
[CI Badge]: https://github.com/WNProject/WNFramework/actions/workflows/push.yml/badge.svg?branch=master
[CI Workflow]: https://github.com/WNProject/WNFramework/actions/workflows/push.yml?query=branch%3Amaster
[Visual Studio Command Line Tools]: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line
