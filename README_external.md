# Building CoinGeckoFetcher and Dependencies for Android (C++)

This guide explains how to build the CoinGeckoFetcher and its dependencies for Android using CMake and the Android NDK.  
It assumes you are using [CMake](https://cmake.org/), [Android NDK](https://developer.android.com/ndk), and [Gradle](https://developer.android.com/studio/build).

---

## Dependencies

- **nlohmann/json** (header-only)
- **cpr** (HTTP client, depends on libcurl)
- **libcurl** (network transport)
- **OpenSSL** (for HTTPS support in libcurl)
- **zlib** (required by libcurl)
- **Android NDK** (r21 or higher recommended)
- **CMake 3.10+**

---

## 1. Clone Your Project and Dependencies

```sh
git clone https://github.com/stengergilles/my-template.git
cd my-template
git submodule add https://github.com/nlohmann/json.git external/json
git submodule add https://github.com/libcpr/cpr.git external/cpr
git submodule add https://github.com/curl/curl.git external/curl
```
*(You can also use `FetchContent` in CMake for nlohmann/json and cpr if preferred.)*

---

## 2. Set Up Your CMakeLists.txt

Add something like the following to your project's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(CoinGeckoFetcherAndroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nlohmann/json (header-only)
add_subdirectory(external/json)

# libcurl
option(BUILD_CURL_EXE OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/curl)

# cpr (depends on curl)
set(CPR_FORCE_USE_SYSTEM_CURL OFF)
set(CPR_ENABLE_SSL ON)
add_subdirectory(external/cpr)

# Your source files
add_library(coingeckofetcher STATIC
    stock_monitoring_app/fetchers/base_fetcher.cpp
    stock_monitoring_app/fetchers/coingecko_fetcher.cpp
    stock_monitoring_app/fetchers/http_client_cpr.cpp
)

target_include_directories(coingeckofetcher
    PUBLIC
        stock_monitoring_app/fetchers
)

target_link_libraries(coingeckofetcher
    PUBLIC
        cpr::cpr
        nlohmann_json::nlohmann_json
)
```

---

## 3. Configure Android Toolchain

Set up the Android toolchain in CMake and environment variables:

```sh
export ANDROID_NDK_HOME=/path/to/your/android-ndk
export PATH=$ANDROID_NDK_HOME:$PATH

cmake \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-21 \
  -DCMAKE_BUILD_TYPE=Release \
  -B build_android \
  -S .
```

---

## 4. Build

```sh
cmake --build build_android --target coingeckofetcher -- -j$(nproc)
```

---

## 5. Using in Your Android App

- Copy the generated `.a` static library and headers into your JNI or C++ layer.
- Link against it in your Android Studio/Gradle project via CMake (`externalNativeBuild`).
- Make sure all dependencies are included/linked as well.

---

## Tips

- If you need a shared library, change `STATIC` to `SHARED` in `add_library`.
- For OpenSSL and zlib, you can use prebuilt binaries or build them from source for Android.
- For network permissions, add `<uses-permission android:name="android.permission.INTERNET"/>` in your `AndroidManifest.xml`.

---

## References

- [CMake Android Toolchain](https://developer.android.com/ndk/guides/cmake)
- [cpr Android Build Docs](https://github.com/libcpr/cpr#android)
- [nlohmann/json](https://github.com/nlohmann/json)
- [libcurl Android Build](https://curl.se/docs/install.html#Android)

