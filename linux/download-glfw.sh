#!/bin/bash

# Script to download and build GLFW for Linux

# Change to the project root directory
cd "$(dirname "$0")/.."

# Create external directory if it doesn't exist
mkdir -p external

# Check if GLFW is already downloaded
if [ ! -d "external/glfw" ]; then
    echo "Downloading GLFW..."
    git clone https://github.com/glfw/glfw.git external/glfw
    cd external/glfw
    git checkout 3.3.8
    cd ../..
else
    echo "GLFW already downloaded."
fi

# Build GLFW
echo "Building GLFW..."
mkdir -p external/glfw/build
cd external/glfw/build
cmake .. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
make -j$(nproc)
cd ../../..

echo "GLFW build complete."
