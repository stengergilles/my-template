cmake_minimum_required(VERSION 3.10)
project(imgui_hello_world)
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

# For tests
option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Gtest

if (BUILD_TESTS)
  set(GTEST_ROOT "$ENV{HOME}/.local")
  set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
  set(GTEST_LIBRARY "${GTEST_ROOT}/lib/libgtest.a")
  set(GTEST_MAIN_LIBRARY "${GTEST_ROOT}/lib/libgtest_main.a")
  enable_testing()
else()
  set(GTEST_INCLUDE_DIR "")
endif()

# Prebuild openssl for everything except Android

message(STATUS "CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}")
message(STATUS "BUILD_TESTS ${BUILD_TESTS}")

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android" AND BUILD_TESTS OR EXISTS "/data/data/com.termux/files/usr")
  if(NOT EXISTS "${PROJECT_ROOT}/external/openssl/native/include/openssl/opensslconf.h")
    execute_process(
        COMMAND bash -c "
	    cd ${PROJECT_ROOT}/external/openssl-src
	    ./Configure no-tests --prefix=${PROJECT_ROOT}/external/openssl/native
	    make -j\$(nproc)
	    make install
        "
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE OPENSSL_BUILD_RESULT
    )
    if(NOT OPENSSL_BUILD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to build OpenSSL!")
    endif()
  endif()
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${PROJECT_ROOT}/external/json/include
    ${PROJECT_ROOT}/external/curl/include
    ${GTEST_INCLUDE_DIR}
)

set(SOURCES
    src/application.cpp
    src/platform/platform_base.cpp
    src/main.cpp
    src/coingecko_fetcher.cpp
    src/polygon_fetcher.cpp
    src/http_client_curl.cpp
    src/write_cacert.cpp
    src/tempfile.cpp
)

set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)

add_subdirectory(external/json)

# --- OpenSSL & curl reproducible build block ---

if(ANDROID)
    # Set ABI variable if not set by environment
    if(NOT DEFINED ANDROID_ABI)
        if(DEFINED CMAKE_ANDROID_ARCH_ABI)
            set(ANDROID_ABI ${CMAKE_ANDROID_ARCH_ABI})
        else()
            set(ANDROID_ABI "arm64-v8a")
        endif()
    endif()

    set(OPENSSL_ROOT_DIR "${PROJECT_ROOT}/external/openssl/android/${ANDROID_ABI}")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")

    # Define imported OpenSSL targets for use by curl and your code
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

    set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL with libcurl" FORCE)
    set(CURL_ENABLE_OPENSSL_AUTO_FETCH OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LIBPSL ON CACHE BOOL "Disable PSL support" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "Use PSL support" FORCE)

    add_subdirectory(external/curl)
else()
    # For native platforms, always use project-controlled OpenSSL build
    set(OPENSSL_ROOT_DIR "${PROJECT_ROOT}/external/openssl/native")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")

    if(EXISTS "${OPENSSL_SSL_LIBRARY}" AND EXISTS "${OPENSSL_CRYPTO_LIBRARY}")
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
    else()
        message(FATAL_ERROR "OpenSSL libraries not found in ${OPENSSL_ROOT_DIR}. Build OpenSSL before configuring this project.")
    endif()

    set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL with libcurl" FORCE)
    set(CURL_ENABLE_OPENSSL_AUTO_FETCH OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LIBPSL ON CACHE BOOL "Disable PSL support" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "Use PSL support" FORCE)

    add_subdirectory(external/curl)
endif()

get_target_property(CURL_INCLUDE_DIRS libcurl INTERFACE_INCLUDE_DIRECTORIES)

# --- End OpenSSL & curl reproducible build block ---

# Test executables

if(BUILD_TESTS)
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
  add_executable(test_atr
      ${PROJECT_ROOT}/tests/indicators/test_atr.cpp
      ${PROJECT_ROOT}/src/atr_indicator.cpp
  )
  add_executable(test_bollinger
      ${PROJECT_ROOT}/tests/indicators/test_bollinger.cpp
      ${PROJECT_ROOT}/src/bollinger_indicator.cpp
  )
  add_executable(test_breakout
      ${PROJECT_ROOT}/tests/indicators/test_breakout.cpp
      ${PROJECT_ROOT}/src/breakout_indicator.cpp
  )
  add_executable(test_ma
      ${PROJECT_ROOT}/tests/indicators/test_ma.cpp
      ${PROJECT_ROOT}/src/ma_indicator.cpp
  )
  add_executable(test_macd
      ${PROJECT_ROOT}/tests/indicators/test_macd.cpp
      ${PROJECT_ROOT}/src/macd_indicator.cpp
  )
  add_executable(test_rsi
      ${PROJECT_ROOT}/tests/indicators/test_rsi.cpp
      ${PROJECT_ROOT}/src/rsi_indicator.cpp
  )
  add_executable(test_volume_spike
      ${PROJECT_ROOT}/tests/indicators/test_volume_spike.cpp
      ${PROJECT_ROOT}/src/volume_spike_indicator.cpp
  )
  
  target_link_libraries(integration_coingecko_fetcher PRIVATE
      CURL::libcurl
      OpenSSL::SSL
      OpenSSL::Crypto
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(integration_polygon_fetcher PRIVATE
      CURL::libcurl
      OpenSSL::SSL
      OpenSSL::Crypto
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_atr PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_bollinger PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  
  target_link_libraries(test_breakout PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_ma PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_macd PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_rsi PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )
  target_link_libraries(test_volume_spike PRIVATE
      "${GTEST_ROOT}/lib/libgtest.a"
      "${GTEST_ROOT}/lib/libgtest_main.a"
      pthread
  )

  add_test(NAME integration_coingecko_fetcher COMMAND integration_coingecko_fetcher)
  add_test(NAME integration_polygon_fetcher COMMAND integration_polygon_fetcher)
  add_test(NAME test_atr COMMAND test_atr)
  add_test(NAME test_bollinger COMMAND test_bollinger)
  add_test(NAME test_breakout COMMAND test_breakout)
  add_test(NAME test_ma COMMAND test_ma)
  add_test(NAME test_macd COMMAND test_macd)
  add_test(NAME test_rsi COMMAND test_rsi)
  add_test(NAME test_volume_spike COMMAND test_volume_spike)
endif()
set(CACERT_PEM_URL "https://curl.se/ca/cacert.pem")
set(CACERT_PEM "${CMAKE_BINARY_DIR}/cacert.pem")
set(CACERT_HEADER "${CMAKE_SOURCE_DIR}/include/cacert_pem_data.hpp")

add_custom_command(
    OUTPUT ${CACERT_PEM}
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading cacert.pem..."
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CACERT_PEM}
    COMMAND curl -L -o ${CACERT_PEM} ${CACERT_PEM_URL}
    COMMENT "Downloading latest cacert.pem"
)

add_custom_command(
    OUTPUT ${CACERT_HEADER}
    DEPENDS ${CACERT_PEM}
    COMMAND xxd -i ${CACERT_PEM} > ${CACERT_HEADER}.tmp && mv ${CACERT_HEADER}.tmp ${CACERT_HEADER}
    COMMAND sed -i -e 's|unsigned char .*|unsigned char cacert_pem[] = {|g' "${CACERT_HEADER}"
    COMMAND sed -i -e 's|unsigned int[[:space:]]*.*|unsigned int cacert_pem_len = sizeof\(cacert_pem\)\;|' "${CACERT_HEADER}"
    COMMENT "Generating and fixing cacert_pem_data.hpp"
)

add_custom_target(gen_cacert_header ALL DEPENDS ${CACERT_HEADER})

if(BUILD_TESTS)
add_dependencies(integration_coingecko_fetcher gen_cacert_header)
add_dependencies(integration_polygon_fetcher gen_cacert_header)
endif()

if(ANDROID AND NOT BUILD_TESTS)
    set(PLATFORM_SOURCES
        src/platform/platform_android.cpp
        src/platform/android/android_native_app_glue.c
        src/platform/android/jni_bridge.cpp
        src/platform/android/android_main.cpp
        src/platform/android/imgui_impl_android.cpp
        src/platform/android/keyboard_helper.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/android
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${CURL_INCLUDE_DIRS}
        ${PROJECT_ROOT}/external/json/include
    )
    set(PLATFORM_LIBS
        android
        EGL
        GLESv3
        log
        nlohmann_json::nlohmann_json
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
    )
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
elseif(EMSCRIPTEN AND NOT BUILD_TESTS)
    set(PLATFORM_SOURCES
        src/platform/platform_wasm.cpp
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/wasm/shell.html")
    set(PLATFORM_LIBS)
    add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
else()
    if(NOT BUILD_TESTS)
      set(PLATFORM_SOURCES
          src/platform/platform_glfw.cpp
          external/imgui/backends/imgui_impl_glfw.cpp
          external/imgui/backends/imgui_impl_opengl3.cpp
      )
      if(POLICY CMP0072)
        cmake_policy(SET CMP0072 NEW)
      endif()
      find_package(OpenGL REQUIRED)
      set(PLATFORM_LIBS
        OpenGL::GL
        glfw
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
      )
      if(APPLE AND NOT BUILD_TESTS)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
        find_library(CORE_VIDEO_LIBRARY CoreVideo)
        list(APPEND PLATFORM_LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY} ${CORE_VIDEO_LIBRARY})
      endif()
      if(WIN32 AND NOT BUILD_TESTS)
        # Add Windows-specific libraries if needed
      endif()
      add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})
   endif()
endif()

if (NOT BUILD_TESTS)
   target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})
   add_dependencies(${PROJECT_NAME} gen_cacert_header)
endif()

if(NOT ANDROID)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
