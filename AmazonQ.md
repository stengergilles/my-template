# Cross-Platform C++ Application with Gradle Build System

This document explains how the Gradle build system is set up for the Linux version of the ImGui application.

## Overview

The project now has a Gradle-based build system for the Linux version, similar to the Android version. This allows for consistent build processes across platforms and makes it easier to integrate with CI/CD systems.

## Directory Structure

```
.
├── android/              # Android Studio project for the Android app
├── build/                # (Generated) Build output files
├── external/             # External libraries (ImGui, Curl, OpenSSL, etc.)
├── include/              # C++ header files for the core application
├── linux/                # Linux-specific build files
│   ├── app/              # Linux app module
│   │   └── build.gradle  # Gradle build script for Linux app
│   ├── build.gradle      # Root Gradle build script for Linux
│   ├── gradle/           # Gradle wrapper files
│   ├── gradlew           # Gradle wrapper script
│   └── settings.gradle   # Gradle settings for Linux
├── src/                  # C++ source code implementation
└── CMakeLists.txt        # Main CMake build script for the C++ core
```

## Gradle Build System for Linux

The Linux build system uses Gradle's native support for C++ projects, while also integrating with the existing CMake build system. This provides the best of both worlds:

1. **Gradle** provides a modern build system with dependency management, task automation, and integration with CI/CD systems.
2. **CMake** handles the complex C++ build process, including cross-platform support and library integration.

### Key Components

1. **settings.gradle**: Defines the project structure and includes the app module.
2. **build.gradle (root)**: Configures repositories and global settings.
3. **app/build.gradle**: Configures the C++ application build, including:
   - C++ compiler and linker settings
   - Integration with CMake
   - Tasks for building, running, and packaging the application

### Key Tasks

- `buildWithCMake`: Builds the application using CMake
- `run`: Runs the built application
- `downloadDependencies`: Downloads and builds dependencies if not already available
- `createDistribution`: Creates a distributable package

## Using the Build System

### Building the Application

```bash
cd linux
./gradlew buildWithCMake
```

### Running the Application

```bash
cd linux
./gradlew run
```

### Creating a Distribution Package

```bash
cd linux
./gradlew createDistribution
```

## Integration with Existing CMake Build

The Gradle build system doesn't replace the existing CMake build system but rather wraps around it. This means:

1. You can still use CMake directly if needed
2. All existing CMake configurations are preserved
3. The Gradle build system provides additional features and integration points

### Dependencies Management

The Gradle build system is configured to download and build all dependencies from source. The dependencies are stored in the `external` directory and include:

1. **GLFW**: For window creation and input handling
2. **ImGui**: For the user interface
3. **cURL**: For HTTP requests
4. **OpenSSL**: For HTTPS support
5. **nlohmann/json**: For JSON parsing

The `downloadDependencies` task will clone and build these dependencies if they don't already exist. This ensures that the application can be built on any Linux system without requiring system-installed libraries.

## Build Types

The Gradle build system supports both Debug and Release build types:

```bash
# Build in Debug mode
./gradlew buildWithCMake -PbuildType=Debug

# Build in Release mode (default)
./gradlew buildWithCMake -PbuildType=Release
```

You can also use the convenience script:

```bash
# Build in Debug mode
./build-and-run.sh --debug

# Build in Release mode (default)
./build-and-run.sh
```

## Handling X11 Dependencies

The Linux build system requires X11 development libraries for GLFW to work properly. The `download-dependencies.sh` script will attempt to install these dependencies automatically on supported distributions.

If you encounter errors related to missing X11 libraries, you may need to install them manually:

```bash
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev libxfixes-dev libgl1-mesa-dev xorg-dev
```

## Future Improvements

1. Integrate with CI/CD systems
2. Add support for automated testing
3. Improve dependency management
4. Add support for packaging with different formats (deb, rpm, etc.)
