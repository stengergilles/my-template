# Linux Build Instructions

This directory contains the build system for the Linux version of the application. The build system uses Gradle to wrap around CMake, providing a consistent build experience across platforms.

## Prerequisites

Before building the application, make sure you have the following dependencies installed:

- C++17 compatible compiler (GCC or Clang)
- CMake (version 3.10 or higher)
- X11 development libraries
- OpenGL development libraries

The `download-dependencies.sh` script will attempt to install these dependencies automatically on supported distributions (Ubuntu, Fedora, Arch Linux).

## Building the Application

### 1. Download and Build Dependencies

Run the following command to download and build all required dependencies:

```bash
./download-dependencies.sh
```

This script will:
- Install required system packages (if possible)
- Clone and build GLFW
- Clone ImGui
- Clone nlohmann/json

### 2. Build the Application

To build the application, run:

```bash
./gradlew buildWithCMake
```

By default, this will build the application in Release mode. To build in Debug mode, use:

```bash
./gradlew buildWithCMake -PbuildType=Debug
```

### 3. Run the Application

To run the application, use:

```bash
./gradlew run
```

Or to run in Debug mode:

```bash
./gradlew run -PbuildType=Debug
```

### 4. Create a Distribution Package

To create a distributable package, run:

```bash
./gradlew createDistribution
```

This will create a tar.gz file in the `dist` directory containing the executable.

## Build System Structure

- `build.gradle`: Root Gradle build script
- `settings.gradle`: Gradle settings file
- `app/build.gradle`: Application-specific build script
- `download-dependencies.sh`: Script to download and build dependencies
- `build-and-run.sh`: Convenience script to build and run the application

## Troubleshooting

### Missing X11 Libraries

If you encounter errors related to missing X11 libraries, make sure you have the X11 development packages installed. On Ubuntu, you can install them with:

```bash
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev libxfixes-dev libgl1-mesa-dev xorg-dev
```

### GLFW Build Errors

If GLFW fails to build, try removing the build directory and rebuilding:

```bash
rm -rf ../external/glfw/build
./download-dependencies.sh
```

### OpenGL Errors

If you encounter OpenGL-related errors, make sure you have the OpenGL development libraries installed:

```bash
sudo apt-get install libgl1-mesa-dev
```

### Running in Headless Environment

This application requires a graphical environment to run properly. If you're running in a headless environment (like a CI server), the application will crash when trying to create a window. This is expected behavior.
