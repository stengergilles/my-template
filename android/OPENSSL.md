# OpenSSL for Android

This document explains how OpenSSL is built and used in the Android version of the application.

## Overview

The Android build uses a separate OpenSSL build from the Linux/Desktop version. Both builds use the same source code from `/external/openssl-src`, but they are configured and built differently:

- Android OpenSSL: Built for `android-arm64` and stored in `/external/openssl/android/arm64-v8a`
- Linux/Desktop OpenSSL: Built for the native platform and stored in `/external/openssl/native`

## Build Process

The OpenSSL build for Android is managed by Gradle through the `openssl.gradle` file. The build process:

1. Checks if the required OpenSSL libraries already exist
2. If not, it clones the OpenSSL repository (if needed)
3. Configures OpenSSL for Android ARM64
4. Builds and installs OpenSSL to the Android-specific directory

## Troubleshooting

### Mixing Linux and Android OpenSSL Builds

If you encounter issues with OpenSSL builds being mixed between Linux and Android, you can use the provided cleaning script:

```bash
./clean-openssl.sh
```

This script will:
1. Clean the OpenSSL source directory
2. Remove the Android OpenSSL build directory
3. Allow for a fresh build of OpenSSL for Android

### Manual Cleaning

You can also manually clean the OpenSSL builds:

```bash
# Clean OpenSSL source
cd ../external/openssl-src
git clean -fdx
git reset --hard

# Remove Android OpenSSL build
rm -rf ../external/openssl/android
```

### Gradle Clean

The Gradle `clean` task has been configured to also clean the OpenSSL build for Android:

```bash
./gradlew clean
```

## Build Types

OpenSSL is built in release mode for Android. If you need to build it in debug mode, you can modify the `Configure` command in `openssl.gradle`.

## Dependencies

The OpenSSL build for Android requires the Android NDK to be properly set up. The build script uses the NDK specified in your `local.properties` file or the one configured in Android Studio.

## Integration with CMake

The Android CMake build is configured to use the Android-specific OpenSSL build. The paths are set in the main `CMakeLists.txt` file:

```cmake
set(OPENSSL_ROOT_DIR "${PROJECT_ROOT}/external/openssl/android/${ANDROID_ABI}")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a")
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")
```

This ensures that the Android build uses the correct OpenSSL libraries.
