#!/bin/bash

# Script to build and run the application

# Parse command line arguments
BUILD_TYPE="Release"
RUN_APP=true

for arg in "$@"; do
  case $arg in
    --debug)
      BUILD_TYPE="Debug"
      shift
      ;;
    --no-run)
      RUN_APP=false
      shift
      ;;
    *)
      # Unknown option
      echo "Unknown option: $arg"
      echo "Usage: $0 [--debug] [--no-run]"
      exit 1
      ;;
  esac
done

# Change to the script directory
cd "$(dirname "$0")"

# Download dependencies if needed
./download-dependencies.sh

# Build the application
echo "Building application in $BUILD_TYPE mode..."
./gradlew buildWithCMake -PbuildType=$BUILD_TYPE

# Check if build was successful
if [ $? -ne 0 ]; then
  echo "Build failed!"
  exit 1
fi

# Run the application if requested
if [ "$RUN_APP" = true ]; then
  echo "Running application..."
  ./gradlew run -PbuildType=$BUILD_TYPE
fi
