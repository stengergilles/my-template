# Cross-Platform C++ Application Template

This repository provides a template for building C++ applications that can run on multiple platforms, including desktop (Windows, macOS, Linux), Android, and WebAssembly.

It features a clean architecture that separates the core application logic from platform-specific code, making it an ideal starting point for complex projects. The user interface is built with [Dear ImGui](https://github.com/ocornut/imgui), providing a lightweight and performant GUI out of the box.

## Features

*   **Cross-Platform by Design:**
    *   **Desktop:** Builds on Windows, macOS, and Linux using GLFW.
    *   **Android:** A pre-configured Android Studio project integrates the C++ core.
    *   **WebAssembly:** Ready for web deployment (requires Emscripten setup).
*   **Extensible Architecture:** The core logic is modular, allowing you to easily add your own features. The template includes example modules for data processing and handling.
*   **Common Libraries Included:** Comes with essential libraries for modern C++ development:
    *   [**Dear ImGui**](https://github.com/ocornut/imgui) for the user interface.
    *   [**cURL**](https://curl.se/) for HTTP requests.
    *   [**OpenSSL**](https://www.openssl.org/) for HTTPS support.
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

## Getting Started

### Prerequisites

*   A C++17 compatible compiler (GCC, Clang, MSVC).
*   [CMake](https://cmake.org/) (version 3.10 or higher).
*   **For Desktop:** GLFW and its dependencies.
*   **For Android:** The Android NDK and Android Studio.

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

## Testing

The project includes a suite of tests for the core C++ logic, located in the `tests/` directory.

To run the tests, build the `test` target with CMake:

```bash
# From the build directory
cmake --build . --target test
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Additional Documentation

*   [External Dependencies](doc/README_external.md)
*   [ImGui Details](doc/README_IMGUI.md)
*   [OpenSSL Details](doc/README_openssl.md)
