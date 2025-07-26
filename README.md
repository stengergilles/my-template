# Cross-Platform C++ Application Template

> [!IMPORTANT]
> This project is a fun/toy project and was largely AI-generated.

> [!NOTE]
> This repository provides a template for building C++ applications that can run on desktop (Windows, macOS, Linux), and Android.

This template provides a solid foundation for developing applications that require a lightweight, immediate-mode graphical user interface. It is pre-configured with a clean architecture that separates core application logic from platform-specific code, making it easier to maintain and extend.

The user interface is built with [Dear ImGui](https://github.com/ocornut/imgui), a popular and highly performant GUI library.

## Features

*   **Cross-Platform by Design:**
    *   **Desktop:** Builds on Windows, macOS, and Linux using GLFW.
    *   **Android:** A pre-configured Android Studio project integrates the C++ core.
*   **Core Libraries Included:**
    *   [**Dear ImGui**](https://github.com/ocornut/imgui) for the user interface.
    *   [**cURL**](https://curl.se/) for HTTP requests.
    *   [**OpenSSL**](https://www.openssl.org/) for HTTPS support. (OpenSSL source code is automatically cloned and built by CMake.)
    *   [**nlohmann/json**](https://github.com/nlohmann/json) for easy JSON parsing.
    *   [**Font Awesome 6**](https://fontawesome.com/) for scalable vector icons.
*   **Asynchronous Task Execution:**
    *   A simple `Worker` class is provided for running tasks in the background. This is crucial for preventing the UI from freezing during long-running operations such as network requests or heavy computations. By offloading work to a separate thread, the main application thread remains responsive, ensuring a smooth user experience.
*   **HTTP Client:**
    *   A basic HTTP client is included, with an abstraction that can be extended to support different backends. The default implementation uses cURL.
*   **Logging:**
    *   A simple logging utility is provided for easy debugging, with an in-app log viewer.
*   **Settings Management:**
    *   A `SettingsManager` class allows for easy persistence of application settings.
*   **Dynamic Font Loading:**
    *   The `FontManager` class supports loading custom fonts at runtime.

## Project Structure

A brief overview of the main directories:

```
.
├── android/              # Android Studio project for the Android app
├── external/             # External libraries (ImGui, Curl, OpenSSL, etc.)
├── include/              # C++ header files for the core application
│   └── platform/         # Platform abstraction headers
├── linux/                # Build scripts for Linux
├── src/                  # C++ source code implementation
│   └── platform/         # Platform-specific implementations
├── CMakeLists.txt        # Main CMake build script for the C++ core
├── get-external.sh       # Script to download external dependencies
├── LICENSE               # Project license
└── README.md             # This file
```

## Getting Started

### Prerequisites

*   A C++17 compatible compiler (GCC, Clang, MSVC).
*   [CMake](https://cmake.org/) (version 3.10 or higher).
*   **For Desktop:** GLFW and its dependencies.
*   **For Android:** The Android NDK and Android Studio.

### Initial Setup (Important!)

Before building for any platform, you need to fetch and prepare external dependencies.

Run the `get-external.sh` script from the project root:

```bash
./get-external.sh
```

This script will clone necessary repositories (cURL, ImGui, nlohmann/json, OpenSSL, etc.) and download Font Awesome fonts.

### Desktop (Linux)

1.  **Run the build script:**
    ```bash
    ./linux/build-and-run.sh
    ```
    This script will download dependencies, configure CMake, build the project, and run the application.

### Android

1.  **Open the `android` directory in Android Studio.**
2.  **Let Gradle sync and configure the project.** The `build.gradle` file is set up to execute CMake and build the native C++ code.
3.  **Build and run the application on an emulator or a physical device.**

## How to Start Coding

The core of your application's user interface logic is implemented by overriding the `Application::renderImGui()` method. This method is called every frame to draw your application's UI using Dear ImGui.

You can modify the `Application::renderImGui()` method in `src/application.cpp` to add your own UI elements.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue.

## Acknowledgments

*   The [Dear ImGui](https://github.com/ocornut/imgui) community for their excellent UI library.
*   The developers of [cURL](https://curl.se/), [OpenSSL](https://www.openssl.org/), and [nlohmann/json](https://github.com/nlohmann/json).
*   The [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) project for providing C++ headers for icon fonts.
*   The open-source community for providing the tools and libraries that make this project possible.
