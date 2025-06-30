# Cross-Platform C++ Application Template

> [!NOTE]
> Much of this repository was generated with the help of AI.

This repository provides a template for building C++ applications that can run on multiple platforms, including desktop (Windows, macOS, Linux), Android, and WebAssembly.

It features a clean architecture that separates the core application logic from platform-specific code, making it an ideal starting point for complex projects. The user interface is built with [Dear ImGui](https://github.com/ocornut/imgui), providing a lightweight and performant GUI out of the box.

> [!IMPORTANT]
> This repository requires a specific commit of Dear ImGui to function correctly. Please ensure you are using commit `69e1fb50cacbde1c2c585ae59898e68c1818d9b7`.

## Features

*   **Cross-Platform by Design:**
    *   **Desktop:** Builds on Windows, macOS, and Linux using GLFW.
    *   **Android:** A pre-configured Android Studio project integrates the C++ core.
    *   **WebAssembly:** Ready for web deployment (requires Emscripten setup).
*   **Extensible Architecture:** The core logic is modular, allowing you to easily add your own features. The template includes example modules for data processing and handling.
*   **Common Libraries Included:** Comes with essential libraries for modern C++ development:
    *   [**Dear ImGui**](https://github.com/ocornut/imgui) for the user interface.
    *   [**cURL**](https://curl.se/) for HTTP requests.
    *   [**OpenSSL**](https://www.openssl.org/) for HTTPS support. (Requires OpenSSL source code to be cloned into `external/openssl-src`.)
    *   [**nlohmann/json**](https://github.com/nlohmann/json) for easy JSON parsing.
*   **Ready to Build:** Includes a `CMakeLists.txt` file that handles the build process for the C++ core and its dependencies.

## Project Structure

A brief overview of the main directories:

```
.
├── android/              # Android Studio project for the Android app
├── build/                # (Generated) Build output files
├── external/             # External libraries (ImGui, Curl, OpenSSL, etc.)
├── include/              # C++ header files for the core application
│   ├── app/              # Headers for custom application logic/modules
│   └── platform/         # Platform abstraction headers
├── src/                  # C++ source code implementation
│   ├── app/              # Implementation for custom modules
│   └── platform/         # Platform-specific implementations
├── tests/                # C++ unit and integration tests
├── CMakeLists.txt        # Main CMake build script for the C++ core
├── LICENSE               # Project license
└── README.md             # This file
```

### Application Main Rendering

The `src/app/app_main.cpp` file is designed to contain the custom rendering logic for your application's main frame. If this file exists, the `USE_EXTERNAL_RENDER_IMGUI` preprocessor definition is enabled during compilation, directing the `Application` class to use the `renderImGui()` implementation provided in `app_main.cpp`.

If `src/app/app_main.cpp` is not present, a default `renderImGui()` implementation, defined internally within `include/application.h`, will be used instead. This ensures a basic functional application is available even without custom rendering code.

## Getting Started

> [!WARNING]
> This repository is currently a work in progress. While the core logic for desktop and WebAssembly targets is present, only the Android target and the C++ tests are fully functional and regularly tested at this time.

### Prerequisites

*   A C++17 compatible compiler (GCC, Clang, MSVC).
*   [CMake](https://cmake.org/) (version 3.10 or higher).
*   **For Desktop:** GLFW and its dependencies.
*   **For Android:** The Android NDK and Android Studio.

### Setup Hooks

The `setup-hooks.sh` script is automatically executed by CMake during the configuration phase. This script is responsible for setting up necessary Git hooks and preparing the development environment. You do not need to run it manually.

### Desktop (Linux, macOS, Windows)

1.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```

2.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```

3.  **Build the project:**
    ```bash
    cmake --build .
    ```

4.  **Run the application:**
    The executable will be located in the `build` directory.

### Android

1.  **Open the `android` directory in Android Studio.**
2.  **Let Gradle sync and configure the project.** The `build.gradle` file is set up to execute CMake and build the native C++ code.
3.  **Build and run the application on an emulator or a physical device.**

#### Customizing App Title and Package Name (CMake)

To override the default application title and package name for the Android build, you can define the `APP_TITLE` and `APP_PACKAGE` CMake variables. This can be done by passing them as arguments to the CMake command when configuring the project:

```bash
cmake -DAPP_TITLE="My Custom App" -DAPP_PACKAGE="com.example.mycustomapp" ..
```

These variables will be used to set the `applicationId` in `app/build.gradle` and the `app_name` in `app/src/main/res/values/strings.xml`.

#### Customizing App Title and Package Name (Gradle)

Alternatively, you can override the application title and package name directly within the Gradle build system. This can be done by setting the `appTitle` and `appPackage` properties in your `gradle.properties` file (located in the `android/` directory) or by passing them as command-line arguments to Gradle:

**Via `gradle.properties`:**
```properties
appTitle=My Custom App
appPackage=com.example.mycustomapp
```

**Via command line:**
```bash
./gradlew assembleDebug -PappTitle="My Custom App" -PappPackage="com.example.mycustomapp"
```

These properties will be used by the `app/build.gradle` file to configure the Android application.

## Testing

The project includes a suite of tests for the core C++ logic, located in the `tests/` directory.

To run the tests, build the `test` target with CMake:

```bash
# From the build directory
cmake --build . --target test
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.


