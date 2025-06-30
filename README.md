# My Pricer - Cross-Platform Financial Analysis Application

> [!NOTE]
> Much of this repository was generated with the help of AI.

This repository contains **My Pricer**, a C++ application for financial analysis that runs on multiple platforms, including desktop (Windows, macOS, Linux), Android, and WebAssembly.

It features a clean architecture that separates the core financial logic from platform-specific code. The user interface is built with [Dear ImGui](https://github.com/ocornut/imgui), providing a lightweight and performant GUI.

> [!IMPORTANT]
> This repository requires a specific commit of Dear ImGui to function correctly. Please ensure you are using commit `69e1fb50cacbde1c2c585ae59898e68c1818d9b7`.

## Features

*   **Cross-Platform by Design:**
    *   **Desktop:** Builds on Windows, macOS, and Linux using GLFW.
    *   **Android:** A pre-configured Android Studio project integrates the C++ core.
    *   **WebAssembly:** Ready for web deployment (requires Emscripten setup).
*   **Financial Data Fetchers:**
    *   Integration with [CoinGecko](https://www.coingecko.com/) for cryptocurrency data.
    *   Integration with [Polygon.io](https://polygon.io/) for stock and crypto data.
*   **Technical Indicators:**
    *   Average True Range (ATR)
    *   Bollinger Bands
    *   Breakout Indicator
    *   Moving Average (MA)
    *   Moving Average Convergence Divergence (MACD)
    *   Relative Strength Index (RSI)
    *   Volume Spike Indicator
*   **Core Libraries Included:**
    *   [**Dear ImGui**](https://github.com/ocornut/imgui) for the user interface.
    *   [**cURL**](https://curl.se/) for HTTP requests.
    *   [**OpenSSL**](https://www.openssl.org/) for HTTPS support. (OpenSSL source code is automatically cloned and built by CMake.)
    *   [**nlohmann/json**](https://github.com/nlohmann/json) for easy JSON parsing.

## Project Structure

A brief overview of the main directories:

```
.
├── android/              # Android Studio project for the Android app
├── build/                # (Generated) Build output files
├── external/             # External libraries (ImGui, Curl, OpenSSL, etc.)
├── include/              # C++ header files for the core application
│   ├── app/              # Headers for financial indicators and data fetchers
│   └── platform/         # Platform abstraction headers
├── src/                  # C++ source code implementation
│   ├── app/              # Implementation for indicators and fetchers
│   └── platform/         # Platform-specific implementations
├── tests/                # C++ unit and integration tests
├── CMakeLists.txt        # Main CMake build script for the C++ core
├── LICENSE               # Project license
└── README.md             # This file
```


## Getting Started

> [!WARNING]
> This repository is currently a work in progress. While the core logic for desktop and WebAssembly targets is present, only the Android target and the C++ tests are fully functional and regularly tested at this time.

### Prerequisites

*   A C++17 compatible compiler (GCC, Clang, MSVC).
*   [CMake](https.cmake.org/) (version 3.10 or higher).
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
overrideAppTitle=My Custom App
overridePackageName=com.example.mycustomapp
```

**Via command line:**
```bash
./gradlew assembleDebug -PoverrideAppTitle="My Custom App" -PoverridePackageName="com.example.mycustomapp"
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