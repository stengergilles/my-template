cmake_minimum_required(VERSION 3.10)
project(imgui_hello_world)
# In your top-level CMakeLists.txt
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(CMAKE_FIND_PACKAGE_DEBUG_MODE TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Gtest

# Adjust these paths if your installation is elsewhere
set(GTEST_ROOT "$ENV{HOME}/.local")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIBRARY "${GTEST_ROOT}/lib/libgtest.a")
set(GTEST_MAIN_LIBRARY "${GTEST_ROOT}/lib/libgtest_main.a")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${PROJECT_ROOT}/external/json/include
    ${PROJECT_ROOT}/external/curl/include
    ${GTEST_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/application.cpp
    src/platform/platform_base.cpp
    src/main.cpp
    src/coingecko_fetcher.cpp
    src/polygon_fetcher.cpp
    src/http_client_curl.cpp
)

# ImGui source files
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)

# OpenSSL

set(OPENSSL_ROOT_DIR "${PROJECT_ROOT}/external/openssl" CACHE PATH "OpenSSL root" FORCE)
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include" CACHE PATH "OpenSSL include" FORCE)
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/libcrypto.a" CACHE FILEPATH "OpenSSL crypto lib" FORCE)
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/libssl.a" CACHE FILEPATH "OpenSSL ssl lib" FORCE)
set(CMAKE_PREFIX_PATH "${OPENSSL_ROOT_DIR}")
set(OPENSSL_USE_STATIC_LIBS TRUE)

# Only create alias targets if they don't exist already
if(NOT TARGET OpenSSL::SSL)
    add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
    set_target_properties(OpenSSL::SSL PROPERTIES
        IMPORTED_LOCATION "${OPENSSL_SSL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
    )
endif()
if(NOT TARGET OpenSSL::Crypto)
    add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
    set_target_properties(OpenSSL::Crypto PROPERTIES
        IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
    )
endif()
# nlohmann/json (header-only) 
add_subdirectory(external/json)  
# libcurl  
option(BUILD_CURL_EXE OFF) 
option(BUILD_SHARED_LIBS OFF)  
set(CURL_USE_OPENSSL ON CACHE BOOL "Use Openssl")
set(CURL_DISABLE_LIBIDN2 ON CACHE BOOL "Disable idn2")
set(CURL_DISABLE_IDN ON CACHE BOOL "Disable idn")
set(CURL_USE_LIBIDN2 OFF CACHE BOOL "Disable idn2")
set(CURL_DISABLE_LIBPSL ON CACHE BOOL "Disable PSL support" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "Use PSL support" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)       # Don't build curl command-line tool
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)        # Don't build tests
set(BUILD_TESTING_DEFAULT OFF CACHE BOOL "" FORCE) # Some curl versions use this
set(CURL_DISABLE_TESTS ON CACHE BOOL "" FORCE)    # For some curl CMake configs
add_subdirectory(external/curl) 
get_target_property(CURL_INCLUDE_DIRS libcurl INTERFACE_INCLUDE_DIRECTORIES)

# Add this near your other add_executable or add_test calls

add_executable(integration_coingecko_fetcher
	${PROJECT_ROOT}/tests/fetchers/integration_coingecko_fetcher.cpp
    	${PROJECT_ROOT}/src/coingecko_fetcher.cpp
	${PROJECT_ROOT}/src/http_client_curl.cpp
	${PROJECT_ROOT}/src/tempfile.cpp
	${PROJECT_ROOT}/src/write_cacert.cpp
)

add_executable(integration_polygon_fetcher
	${PROJECT_ROOT}/tests/fetchers/integration_polygon_fetcher.cpp
    	${PROJECT_ROOT}/src/polygon_fetcher.cpp
	${PROJECT_ROOT}/src/http_client_curl.cpp
	${PROJECT_ROOT}/src/tempfile.cpp
	${PROJECT_ROOT}/src/write_cacert.cpp
)

target_include_directories(integration_coingecko_fetcher PRIVATE 
	src
)
target_include_directories(integration_polygon_fetcher PRIVATE 
	src
)
# If you depend on curl, openssl, etc:
target_link_libraries(integration_coingecko_fetcher PRIVATE 
	CURL::libcurl
	"${GTEST_ROOT}/lib/libgtest.a"
        "${GTEST_ROOT}/lib/libgtest_main.a"
         pthread
)

target_link_libraries(integration_polygon_fetcher PRIVATE 
	CURL::libcurl
	"${GTEST_ROOT}/lib/libgtest.a"
        "${GTEST_ROOT}/lib/libgtest_main.a"
         pthread
)
# Optionally, add as a test (CMake's ctest)
add_test(NAME integration_coingecko_fetcher COMMAND integration_coingecko_fetcher)
add_test(NAME integration_polygon_fetcher COMMAND integration_polygon_fetcher)

set(CACERT_PEM_URL "https://curl.se/ca/cacert.pem")
set(CACERT_PEM "${CMAKE_BINARY_DIR}/cacert.pem")
set(CACERT_HEADER "${CMAKE_SOURCE_DIR}/src/cacert_pem_data.hpp")

# Download cacert.pem
add_custom_command(
    OUTPUT ${CACERT_PEM}
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading cacert.pem..."
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CACERT_PEM}
    COMMAND curl -L -o ${CACERT_PEM} ${CACERT_PEM_URL}
    COMMENT "Downloading latest cacert.pem"
)

# Convert to header and fix variable names
add_custom_command(
    OUTPUT ${CACERT_HEADER}
    DEPENDS ${CACERT_PEM}
    COMMAND xxd -i ${CACERT_PEM} > ${CACERT_HEADER}
    COMMAND sed -i -e 's/unsigned char .*/unsigned char cacert_pem[] = {/' ${CACERT_HEADER}
    COMMAND sed -i -e '"s/unsigned int .*/unsigned int cacert_pem_len = sizeof(cacert_pem);/"' ${CACERT_HEADER}
    COMMENT "Generating and fixing cacert_pem_data.hpp"
)

# Add as a custom target so it runs before build
add_custom_target(gen_cacert_header ALL DEPENDS ${CACERT_HEADER})

# Make sure your sources include the generated header
add_dependencies(integration_coingecko_fetcher gen_cacert_header)
add_dependencies(integration_polygon_fetcher gen_cacert_header)

# Platform-specific source files and libraries
if(ANDROID)
    # Android-specific setup
    # Don't specify a C++ standard library explicitly
    # Let the NDK use its default (libc++)

    
    set(PLATFORM_SOURCES
        src/platform/platform_android.cpp
        src/platform/android/android_native_app_glue.c
        src/platform/android/jni_bridge.cpp
        src/platform/android/android_main.cpp
        src/platform/android/imgui_impl_android.cpp
        src/platform/android/keyboard_helper.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    
    # Include Android-specific directories
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/android
        ${ANDROID_NDK}/sources/android/native_app_glue
	${CURL_INCLUDE_DIRS}
	${PROJECT_ROOT}/external/json/include
    )
    
    # Android-specific libraries
    set(PLATFORM_LIBS
        android
        EGL
        GLESv3
        log
        nlohmann_json::nlohmann_json
	CURL::libcurl
    )
    
    # Set to build a shared library for Android
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
elseif(EMSCRIPTEN)
    # WebAssembly-specific setup
    set(PLATFORM_SOURCES
        src/platform/platform_wasm.cpp
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    
    # Emscripten-specific settings
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/wasm/shell.html")
    
    # No additional libraries needed for Emscripten
    set(PLATFORM_LIBS)
    
    # Create executable for WebAssembly
    add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
else()
    # Desktop platforms (Windows, Linux, macOS)
    set(PLATFORM_SOURCES
        src/platform/platform_glfw.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    
    # Find required packages
    if(POLICY CMP0072)
       cmake_policy(SET CMP0072 NEW)
    endif()
    find_package(OpenGL REQUIRED)
    
    # Platform-specific libraries
    set(PLATFORM_LIBS
        OpenGL::GL
        glfw
    )
    
    # macOS-specific settings
    if(APPLE)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
        find_library(CORE_VIDEO_LIBRARY CoreVideo)
        list(APPEND PLATFORM_LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY} ${CORE_VIDEO_LIBRARY})
    endif()
    
    # Windows-specific settings
    if(WIN32)
        # Add Windows-specific libraries if needed
    endif()
    
    # Create executable for desktop platforms
    add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})

# Installation
if(NOT ANDROID)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
