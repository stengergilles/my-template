# Cross-Platform C++ Application Template

> [!IMPORTANT]
> This project is a fun/toy project and was largely AI-generated.

> [!NOTE]
> This repository provides a template for building C++ applications that can run on desktop (Windows, macOS, Linux), Android, and WebAssembly.

This template provides a solid foundation for developing applications that require a lightweight, immediate-mode graphical user interface. It is pre-configured with a clean architecture that separates core application logic from platform-specific code, making it easier to maintain and extend.

The user interface is built with [Dear ImGui](https://github.com/ocornut/imgui), a popular and highly performant GUI library.

## Features

*   **Cross-Platform by Design:**
    *   **Desktop:** Builds on Windows, macOS, and Linux using GLFW.
    *   **Android:** A pre-configured Android Studio project integrates the C++ core.
    *   **WebAssembly:** Ready for web deployment (requires Emscripten setup).
*   **Core Libraries Included:**
    *   [**Dear ImGui**](https://github.com/ocornut/imgui) for the user interface.
    *   [**cURL**](https://curl.se/) for HTTP requests.
    *   [**OpenSSL**](https://www.openssl.org/) for HTTPS support. (OpenSSL source code is automatically cloned and built by CMake.)
    *   [**nlohmann/json**](https://github.com/nlohmann/json) for easy JSON parsing.
    *   [**Font Awesome 6**](https://fontawesome.com/) for scalable vector icons.
*   **Asynchronous Task Execution:**
    *   A simple `Worker` class is provided for running tasks in the background, preventing the UI from freezing during long-running operations.
*   **HTTP Client:**
    *   A basic HTTP client is included, with an abstraction that can be extended to support different backends. The default implementation uses cURL.
*   **Logging:**
    *   A simple logging utility is provided for easy debugging.

## Project Structure

A brief overview of the main directories:

```
.
├── android/              # Android Studio project for the Android app
├── build/                # (Generated) Build output files
├── external/             # External libraries (ImGui, Curl, OpenSSL, Font Awesome, IconFontCppHeaders, etc.)
├── include/              # C++ header files for the core application
│   └── platform/         # Platform abstraction headers
├── src/                  # C++ source code implementation
│   └── platform/         # Platform-specific implementations
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
*   **For WebAssembly:** The Emscripten SDK.

### Initial Setup (Important!)

Before building for any platform, you need to fetch and prepare external dependencies. This includes Font Awesome and its C++ header definitions.

Run the `get-external.sh` script from the project root:

```bash
./get-external.sh
```

This script will:
*   Clone necessary repositories (cURL, ImGui, nlohmann/json, OpenSSL, IconFontCppHeaders).
*   Download and extract Font Awesome Free Web fonts.
*   Generate the `IconsFontAwesome6.h` header file (and others) from Font Awesome metadata, which defines the `ICON_FA_...` macros used in the application.

### Font Scaling and Appearance

This template uses a combination of base font size and global UI scaling to adapt to different screen densities, especially on Android. You might notice that fonts appear large or slightly blurry if the scaling isn't optimally configured for your device.

*   **Base Font Size:** Fonts are loaded with a base size of `16.0f` in `imgui_impl_android.cpp`.
*   **Display Scale:** The `ScalingManager` calculates a `displayScale` based on your device's screen density and a `scaleAdjustment` factor. You can find the calculated `Initial UI scale` in the application logs (e.g., `android/log.log`).
*   **Effective Font Size:** The `io.FontGlobalScale` in ImGui is set to this `displayScale`. This means your fonts are rendered at `16.0f * displayScale` pixels.

If fonts appear too large or blurry, consider adjusting the `scaleAdjustment` factor in `src/platform/android/android_main.cpp` (e.g., from `1.5f` to `1.0f` or a custom value) to fine-tune the overall UI size. Loading fonts at a higher base size can also improve clarity on high-DPI screens.

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
    The executable will be located in the `build/bin` directory.

### Android

1.  **Open the `android` directory in Android Studio.**
2.  **Let Gradle sync and configure the project.** The `build.gradle` file is set up to execute CMake and build the native C++ code.
3.  **Build and run the application on an emulator or a physical device.**

### WebAssembly

1.  **Configure the project with Emscripten's emcmake:**
    ```bash
    mkdir build-wasm && cd build-wasm
    emcmake cmake ..
    ```

2.  **Build the project:**
    ```bash
    cmake --build .
    ```

3.  **Run the application:**
    A web server is required to serve the generated `.html`, `.js`, and `.wasm` files.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue.

## Acknowledgments

*   The [Dear ImGui](https://github.com/ocornut/imgui) community for their excellent UI library.
*   The developers of [cURL](https://curl.se/), [OpenSSL](https://www.openssl.org/), and [nlohmann/json](https://github.com/nlohmann/json).
*   The [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) project for providing C++ headers for icon fonts.
*   The open-source community for providing the tools and libraries that make this project possible.