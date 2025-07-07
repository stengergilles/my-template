#!/bin/bash

# Script to download and build dependencies for Linux

# Change to the project root directory
cd "$(dirname "$0")/.."

# Create external directory if it doesn't exist
mkdir -p external

# Check for and install required system packages
install_system_packages() {
    echo "Checking for required system packages..."
    
    # Check if we have sudo access
    if command -v sudo &> /dev/null; then
        SUDO="sudo"
    else
        SUDO=""
    fi
    
    # Detect package manager
    if command -v apt-get &> /dev/null; then
        echo "Detected apt package manager"
        $SUDO apt-get update
        $SUDO apt-get install -y \
            build-essential \
            cmake \
            libx11-dev \
            libxrandr-dev \
            libxinerama-dev \
            libxcursor-dev \
            libxi-dev \
            libxext-dev \
            libxfixes-dev \
            libxkbcommon-dev \
            libxkbcommon-x11-dev \
            libgl1-mesa-dev \
            xorg-dev
    elif command -v dnf &> /dev/null; then
        echo "Detected dnf package manager"
        $SUDO dnf install -y \
            gcc gcc-c++ \
            cmake \
            libX11-devel \
            libXrandr-devel \
            libXinerama-devel \
            libXcursor-devel \
            libXi-devel \
            libXext-devel \
            libXfixes-devel \
            libxkbcommon-devel \
            libxkbcommon-x11-devel \
            mesa-libGL-devel
    elif command -v pacman &> /dev/null; then
        echo "Detected pacman package manager"
        $SUDO pacman -Sy --noconfirm \
            gcc \
            cmake \
            libx11 \
            libxrandr \
            libxinerama \
            libxcursor \
            libxi \
            libxext \
            libxfixes \
            libxkbcommon \
            libxkbcommon-x11 \
            mesa
    else
        echo "WARNING: Could not detect package manager. Please install X11 development packages manually."
        echo "Required packages: libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev"
        echo "                  libxext-dev libxfixes-dev libxkbcommon-dev libxkbcommon-x11-dev"
    fi
}

# Install required system packages
install_system_packages

# Function to clone a repository if it doesn't exist
clone_repo() {
    local repo_url=$1
    local target_dir=$2
    local branch_or_tag=$3

    if [ ! -d "$target_dir" ]; then
        echo "Cloning $repo_url to $target_dir..."
        git clone $repo_url $target_dir
        if [ ! -z "$branch_or_tag" ]; then
            cd $target_dir
            git checkout $branch_or_tag
            cd - > /dev/null
        fi
    else
        echo "Directory $target_dir already exists, skipping clone."
    fi
}

# Function to build GLFW
build_glfw() {
    if [ ! -d "external/glfw" ]; then
        echo "GLFW not found, cloning..."
        clone_repo "https://github.com/glfw/glfw.git" "external/glfw" "3.3.8"
    fi

    if [ ! -f "external/glfw/build/src/libglfw3.a" ]; then
        echo "Building GLFW..."
        mkdir -p external/glfw/build
        cd external/glfw/build
        cmake .. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
        make -j$(nproc)
        cd ../../..
        echo "GLFW build complete."
    else
        echo "GLFW already built, skipping."
    fi
}

# Function to build ImGui
build_imgui() {
    if [ ! -d "external/imgui" ]; then
        echo "ImGui not found, cloning..."
        clone_repo "https://github.com/ocornut/imgui.git" "external/imgui" "v1.89.9"
    else
        echo "ImGui already exists, skipping clone."
    fi
    # ImGui is a header-only library, no need to build
}

# Function to build JSON
build_json() {
    if [ ! -d "external/json" ]; then
        echo "JSON library not found, cloning..."
        clone_repo "https://github.com/nlohmann/json.git" "external/json" "v3.11.2"
    else
        echo "JSON library already exists, skipping clone."
    fi
    # JSON is a header-only library, no need to build
}

# Build all dependencies
echo "Building all dependencies..."
build_glfw
build_imgui
build_json

echo "All dependencies built successfully!"
# Function to clone a repository if it doesn't exist
clone_repo() {
    local repo_url=$1
    local target_dir=$2
    local branch_or_tag=$3

    if [ ! -d "$target_dir" ]; then
        echo "Cloning $repo_url to $target_dir..."
        git clone $repo_url $target_dir
        if [ ! -z "$branch_or_tag" ]; then
            cd $target_dir
            git checkout $branch_or_tag
            cd - > /dev/null
        fi
    else
        echo "Directory $target_dir already exists, skipping clone."
    fi
}

# Function to build GLFW
build_glfw() {
    if [ ! -d "external/glfw" ]; then
        echo "GLFW not found, cloning..."
        clone_repo "https://github.com/glfw/glfw.git" "external/glfw" "3.3.8"
    fi

    if [ ! -f "external/glfw/build/src/libglfw3.a" ]; then
        echo "Building GLFW..."
        mkdir -p external/glfw/build
        cd external/glfw/build
        cmake .. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
        make -j$(nproc)
        cd ../../..
        echo "GLFW build complete."
    else
        echo "GLFW already built, skipping."
    fi
}

# Function to build ImGui
build_imgui() {
    if [ ! -d "external/imgui" ]; then
        echo "ImGui not found, cloning..."
        clone_repo "https://github.com/ocornut/imgui.git" "external/imgui" "v1.89.9"
    else
        echo "ImGui already exists, skipping clone."
    fi
    # ImGui is a header-only library, no need to build
}

# Function to build JSON
build_json() {
    if [ ! -d "external/json" ]; then
        echo "JSON library not found, cloning..."
        clone_repo "https://github.com/nlohmann/json.git" "external/json" "v3.11.2"
    else
        echo "JSON library already exists, skipping clone."
    fi
    # JSON is a header-only library, no need to build
}

# Build all dependencies
echo "Building all dependencies..."
build_glfw
build_imgui
build_json

echo "All dependencies built successfully!"
