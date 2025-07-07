#!/bin/bash

# Script to properly clean OpenSSL before rebuilding for Android

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Clean the OpenSSL source directory
if [ -d "$PROJECT_ROOT/external/openssl-src" ]; then
    echo "Cleaning OpenSSL source directory..."
    cd "$PROJECT_ROOT/external/openssl-src"
    git clean -fdx
    git reset --hard
fi

# Remove the Android OpenSSL build directory
if [ -d "$PROJECT_ROOT/external/openssl/android" ]; then
    echo "Removing Android OpenSSL build directory..."
    rm -rf "$PROJECT_ROOT/external/openssl/android"
fi

echo "OpenSSL cleaned successfully. You can now rebuild the Android app."
