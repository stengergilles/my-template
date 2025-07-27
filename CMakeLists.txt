cmake_minimum_required(VERSION 3.10)
project(imgui_hello_world)
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

option(BUILD_TESTS "Build tests" OFF)

# For tests
# Automatically detect if there are any test files
file(GLOB_RECURSE TEST_FILES
    "tests/test_*.cpp"
    "tests/integration_*.cpp"
)

if(BUILD_TESTS AND NOT TEST_FILES)
    message(FATAL_ERROR "BUILD_TESTS is ON, but no test files found in 'tests/' directory. Aborting configuration.")
elseif(NOT TEST_FILES)
    message(STATUS "No test files found in 'tests/' directory. Disabling BUILD_TESTS.")
    set(BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
endif()

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

option(USE_SYSTEM_OPENSSL "Use system OpenSSL libraries" OFF)

if(USE_SYSTEM_OPENSSL)
  message(STATUS "Using system OpenSSL libraries")
  find_package(OpenSSL REQUIRED)
  include_directories(${OPENSSL_INCLUDE_DIR})
else()
  # Use bundled OpenSSL
  message(STATUS "Using bundled OpenSSL")
  
  # Set OpenSSL paths
  set(OPENSSL_ROOT_DIR "${PROJECT_ROOT}/external/openssl/native")
  set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
  set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
  set(OPENSSL_SSL_LIBRARY "${OPENSSL_LIBRARIES}/libssl.a")
  set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_LIBRARIES}/libcrypto.a")
  
  # Check if OpenSSL is already built
  if(NOT EXISTS "${OPENSSL_INCLUDE_DIR}/openssl/opensslconf.h")
    message(STATUS "Building OpenSSL from source")
    execute_process(
        COMMAND bash -c "
        cd ${PROJECT_ROOT}/external/openssl-src
        ./Configure no-tests --prefix=${OPENSSL_ROOT_DIR}
        make -j\$(nproc)
        make install
        "
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE OPENSSL_BUILD_RESULT
    )
    if(NOT OPENSSL_BUILD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to build OpenSSL!")
    endif()
  else()
    message(STATUS "Using pre-built OpenSSL from ${OPENSSL_ROOT_DIR}")
  endif()
  
  # Include OpenSSL headers
  include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# Find GLFW
if(NOT ANDROID)
  # First try to find system GLFW
  find_package(glfw3 QUIET)
  
  if(NOT glfw3_FOUND)
    # If not found, use our own GLFW
    message(STATUS "System GLFW not found, using bundled GLFW")
    
    # Check if GLFW is already built
    if(EXISTS "${PROJECT_ROOT}/external/glfw/build/src/libglfw3.a")
      message(STATUS "Using pre-built GLFW")
      set(GLFW_LIBRARIES "${PROJECT_ROOT}/external/glfw/build/src/libglfw3.a")
      set(GLFW_INCLUDE_DIRS "${PROJECT_ROOT}/external/glfw/include")
      
      # Create imported target for GLFW
      if(NOT TARGET glfw)
        add_library(glfw STATIC IMPORTED GLOBAL)
        set_target_properties(glfw PROPERTIES
          IMPORTED_LOCATION "${GLFW_LIBRARIES}"
          INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIRS}"
        )
      endif()
    else()
      # If not built, add as subdirectory
      set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
      set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
      set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
      set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
      add_subdirectory(${PROJECT_ROOT}/external/glfw)
      set(GLFW_INCLUDE_DIRS "${PROJECT_ROOT}/external/glfw/include")
    endif()
  else()
    message(STATUS "Using system GLFW")
  endif()
  
  include_directories(${GLFW_INCLUDE_DIRS})
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app
    ${CMAKE_CURRENT_SOURCE_DIR}/include/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/external/IconFontCppHeaders
    ${PROJECT_ROOT}/external/json/include
    ${PROJECT_ROOT}/external/curl/include
    ${GTEST_INCLUDE_DIR}
)

set(SOURCES
    src/application.cpp
    src/platform/platform_base.cpp
    src/main.cpp
    src/http_client_curl.cpp
    src/write_cacert.cpp
    src/tempfile.cpp
    src/log_widget.cpp
    src/state_manager.cpp
    src/layout/Layout.cpp
    src/settings_manager.cpp
    src/platform/font_manager.cpp
    src/worker.cpp
    src/platform/logger.cpp
)

# Define platform-specific logger sources
if(ANDROID)
    set(PLATFORM_LOGGER_SOURCES src/platform/android/platform_android_logger.cpp)
else()
    set(PLATFORM_LOGGER_SOURCES src/platform/platform_logger.cpp)
endif()

# Add the platform logger library
add_library(platform_logger_lib STATIC ${PLATFORM_LOGGER_SOURCES})
target_include_directories(platform_logger_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/platform)

set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)

set(APP_MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_main.cpp")
if(EXISTS ${APP_MAIN_FILE})
    list(APPEND SOURCES ${APP_MAIN_FILE})
    add_definitions(-DUSE_EXTERNAL_RENDER_IMGUI)
endif()

# Auto-collect all *.cpp files from src/app
file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.cpp")
if(EXISTS ${APP_MAIN_FILE})
    list(REMOVE_ITEM APP_SOURCES ${APP_MAIN_FILE})
endif()
if(APP_SOURCES)
    add_library(appcode STATIC ${APP_SOURCES})
    target_include_directories(appcode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/app)
endif()

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
    if(NOT USE_SYSTEM_OPENSSL)
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
    endif()

    set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL with libcurl" FORCE)
    set(CURL_ENABLE_OPENSSL_AUTO_FETCH OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LIBPSL ON CACHE BOOL "Disable PSL support" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "Use PSL support" FORCE)

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
        if(USE_SYSTEM_OPENSSL)
            # If using system OpenSSL, we should have already found it
            message(STATUS "Using system OpenSSL libraries")
        else()
            message(FATAL_ERROR "OpenSSL libraries not found in ${OPENSSL_ROOT_DIR}. Build OpenSSL before configuring this project.")
        endif()
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
  # Create a library with all application sources, excluding main.cpp
  set(LIB_SOURCES ${SOURCES})
  list(REMOVE_ITEM LIB_SOURCES "src/main.cpp")
  
  add_library(app STATIC
      ${LIB_SOURCES}
      ${IMGUI_SOURCES}
  )

  target_link_libraries(app PUBLIC
      CURL::libcurl
      OpenSSL::SSL
      OpenSSL::Crypto
      nlohmann_json::nlohmann_json
      platform_logger_lib
  )
  if(TARGET appcode)
      target_link_libraries(app PUBLIC appcode)
  endif()

  # Automatically discover and add tests
  file(GLOB_RECURSE TEST_SOURCES 
      "tests/test_*.cpp"
      "tests/integration_*.cpp"
  )

  foreach(TEST_SOURCE ${TEST_SOURCES})
      # Extract the filename without extension to use as the test name
      string(REGEX REPLACE ".*/([^/]+)\\.cpp$" "\\1" TEST_NAME ${TEST_SOURCE})
      add_executable(${TEST_NAME} ${TEST_SOURCE})
      target_link_libraries(${TEST_NAME} PRIVATE
          app
          "${GTEST_ROOT}/lib/libgtest.a"
          "${GTEST_ROOT}/lib/libgtest_main.a"
          pthread
      )
      add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

      # Add dependency on cacert header for fetcher integration tests
      if (TEST_SOURCE MATCHES "integration_.*_fetcher\.cpp")
          add_dependencies(${TEST_NAME} gen_cacert_header)
      endif()
  endforeach()
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/app
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
    target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

    set(FINAL_LIBS ${PLATFORM_LIBS})
    if(TARGET appcode)
        list(APPEND FINAL_LIBS appcode)
    endif()
    list(APPEND FINAL_LIBS platform_logger_lib)

    target_link_libraries(${PROJECT_NAME} PRIVATE ${FINAL_LIBS})
    add_dependencies(${PROJECT_NAME} gen_cacert_header)

    # Custom target to copy Font Awesome font to Android assets
    add_custom_target(copy_fontawesome_font ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_ROOT}/external/fontawesome/fontawesome-free-6.5.1-web/webfonts/fa-solid-900.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main/assets/fa-solid-900.ttf
        COMMENT "Copying Font Awesome font to Android assets"
    )
    add_dependencies(${PROJECT_NAME} copy_fontawesome_font)

    # Custom target to copy DroidSans.ttf to Android assets from external/fonts
    add_custom_target(copy_droidsans_font ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_ROOT}/external/fonts/DroidSans.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main/assets/DroidSans.ttf
        COMMENT "Copying DroidSans.ttf from external/fonts to Android assets"
    )
    add_dependencies(${PROJECT_NAME} copy_droidsans_font)

    

    
    
endif()



if(NOT ANDROID)
    

    

    set(PLATFORM_SOURCES
        src/platform/platform_glfw.cpp
        src/platform/platform_base.cpp
        src/platform/linux/asset_manager.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/platform/linux
    )

    add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${PLATFORM_SOURCES})

    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        CURL::libcurl
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        platform_logger_lib
        GL
        X11
    )

    if(TARGET appcode)
        target_link_libraries(${PROJECT_NAME} PRIVATE appcode)
    endif()

    add_dependencies(${PROJECT_NAME} gen_cacert_header)

    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
