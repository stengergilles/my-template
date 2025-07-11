#include <jni.h>
#include <jni.h>
#include <android/input.h>
#include <string>
#include "imgui.h"
#include "../../../include/logger.h"
#include "../../../include/scaling_manager.h"

extern bool g_initialized;

// Global JavaVM reference - make it accessible to other files
JavaVM* g_JavaVM = nullptr;
jclass g_MainActivityClass = nullptr;
jmethodID g_ShowKeyboardMethod = nullptr;
jmethodID g_HideKeyboardMethod = nullptr;

// Global variable to store the package name
std::string g_PackageName;

// Called when the library is loaded
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_JavaVM = vm;
    
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    
    // Find and cache the MainActivity class and keyboard methods
    jclass mainActivityClass = env->FindClass("com/my/app/MainActivity");
    if (mainActivityClass == nullptr) {
        LOG_ERROR("Failed to find MainActivity class");
        env->ExceptionClear(); // Clear any pending exception
        return JNI_VERSION_1_6; // Continue loading even if class not found
    }
    
    // Create a global reference to the class
    g_MainActivityClass = (jclass)env->NewGlobalRef(mainActivityClass);
    
    // Get the showKeyboard method ID
    g_ShowKeyboardMethod = env->GetStaticMethodID(g_MainActivityClass, "showKeyboard", "()V");
    if (g_ShowKeyboardMethod == nullptr) {
        LOG_ERROR("Failed to find showKeyboard method");
        env->ExceptionClear(); // Clear any pending exception
        // Not a fatal error, we'll try to find it later
    }
    
    // Get the hideKeyboard method ID
    g_HideKeyboardMethod = env->GetStaticMethodID(g_MainActivityClass, "hideKeyboard", "()V");
    if (g_HideKeyboardMethod == nullptr) {
        LOG_ERROR("Failed to find hideKeyboard method");
        env->ExceptionClear(); // Clear any pending exception
        // Not a fatal error, we'll try to find it later
    }
    
    
    
    LOG_INFO("JNI_OnLoad completed successfully");
    return JNI_VERSION_1_6;
}

// JNI method implementations for ImGuiKeyboardHelper
extern "C" {

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiKeyboardHelper_nativeOnKeyDown(JNIEnv *env, jobject thiz, jint key_code, jint meta_state) {
    LOG_INFO("Key down: %s, meta: %s", std::to_string(key_code).c_str(), std::to_string(meta_state).c_str());
    
    // Map Android key codes to ImGui key codes
    int imguiKey = -1;
    if (key_code >= AKEYCODE_A && key_code <= AKEYCODE_Z) {
        imguiKey = ImGuiKey_A + (key_code - AKEYCODE_A);
    } else if (key_code >= AKEYCODE_0 && key_code <= AKEYCODE_9) {
        imguiKey = ImGuiKey_0 + (key_code - AKEYCODE_0);
    }
    else {
        // Map other keys
        switch (key_code) {
            case AKEYCODE_SPACE: imguiKey = ImGuiKey_Space; break;
            case AKEYCODE_ENTER: imguiKey = ImGuiKey_Enter; break;
            case AKEYCODE_ESCAPE: imguiKey = ImGuiKey_Escape; break;
            case AKEYCODE_TAB: imguiKey = ImGuiKey_Tab; break;
            case AKEYCODE_DEL: imguiKey = ImGuiKey_Backspace; break;
            case AKEYCODE_FORWARD_DEL: imguiKey = ImGuiKey_Delete; break;
            case AKEYCODE_DPAD_LEFT: imguiKey = ImGuiKey_LeftArrow; break;
            case AKEYCODE_DPAD_RIGHT: imguiKey = ImGuiKey_RightArrow; break;
            case AKEYCODE_DPAD_UP: imguiKey = ImGuiKey_UpArrow; break;
            case AKEYCODE_DPAD_DOWN: imguiKey = ImGuiKey_DownArrow; break;
            case AKEYCODE_PAGE_UP: imguiKey = ImGuiKey_PageUp; break;
            case AKEYCODE_PAGE_DOWN: imguiKey = ImGuiKey_PageDown; break;
            case AKEYCODE_HOME: imguiKey = ImGuiKey_Home; break;
            case AKEYCODE_MOVE_END: imguiKey = ImGuiKey_End; break;
        }
    }
    
    if (imguiKey != -1) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent((ImGuiKey)imguiKey, true);
        
        // Handle modifiers
        io.AddKeyEvent(ImGuiKey_ModShift, (meta_state & AMETA_SHIFT_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModCtrl, (meta_state & AMETA_CTRL_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModAlt, (meta_state & AMETA_ALT_ON) != 0);
    }
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiKeyboardHelper_nativeOnKeyUp(JNIEnv *env, jobject thiz, jint key_code, jint meta_state) {
    LOG_INFO("Key up: %s, meta: %s", std::to_string(key_code).c_str(), std::to_string(meta_state).c_str());
    
    // Map Android key codes to ImGui key codes (same mapping as in key down)
    int imguiKey = -1;
    if (key_code >= AKEYCODE_A && key_code <= AKEYCODE_Z) {
        imguiKey = ImGuiKey_A + (key_code - AKEYCODE_A);
    } else if (key_code >= AKEYCODE_0 && key_code <= AKEYCODE_9) {
        imguiKey = ImGuiKey_0 + (key_code - AKEYCODE_0);
    }
    else {
        // Map other keys
        switch (key_code) {
            case AKEYCODE_SPACE: imguiKey = ImGuiKey_Space; break;
            case AKEYCODE_ENTER: imguiKey = ImGuiKey_Enter; break;
            case AKEYCODE_ESCAPE: imguiKey = ImGuiKey_Escape; break;
            case AKEYCODE_TAB: imguiKey = ImGuiKey_Tab; break;
            case AKEYCODE_DEL: imguiKey = ImGuiKey_Backspace; break;
            case AKEYCODE_FORWARD_DEL: imguiKey = ImGuiKey_Delete; break;
            case AKEYCODE_DPAD_LEFT: imguiKey = ImGuiKey_LeftArrow; break;
            case AKEYCODE_DPAD_RIGHT: imguiKey = ImGuiKey_RightArrow; break;
            case AKEYCODE_DPAD_UP: imguiKey = ImGuiKey_UpArrow; break;
            case AKEYCODE_DPAD_DOWN: imguiKey = ImGuiKey_DownArrow; break;
            case AKEYCODE_PAGE_UP: imguiKey = ImGuiKey_PageUp; break;
            case AKEYCODE_PAGE_DOWN: imguiKey = ImGuiKey_PageDown; break;
            case AKEYCODE_HOME: imguiKey = ImGuiKey_Home; break;
            case AKEYCODE_MOVE_END: imguiKey = ImGuiKey_End; break;
        }
    }
    
    if (imguiKey != -1) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent((ImGuiKey)imguiKey, false);
        
        // Update modifiers
        io.AddKeyEvent(ImGuiKey_ModShift, (meta_state & AMETA_SHIFT_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModCtrl, (meta_state & AMETA_CTRL_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModAlt, (meta_state & AMETA_ALT_ON) != 0);
    }
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiKeyboardHelper_nativeOnKeyMultiple(JNIEnv *env, jobject thiz, jint key_code, jint count, jobject event) {
    // Handle repeated key events
    LOG_INFO("Key multiple: %s, count: %s", std::to_string(key_code).c_str(), std::to_string(count).c_str());
    
    // For text input, we can extract the characters
    if (key_code == AKEYCODE_UNKNOWN) {
        // Get the KeyEvent class
        jclass keyEventClass = env->GetObjectClass(event);
        
        // Get the getCharacters method
        jmethodID getCharactersMethod = env->GetMethodID(keyEventClass, "getCharacters", "()Ljava/lang/String;");
        if (getCharactersMethod == nullptr) {
            LOG_ERROR("Could not find getCharacters method");
            return;
        }
        
        // Call getCharacters
        jstring jChars = (jstring)env->CallObjectMethod(event, getCharactersMethod);
        if (jChars == nullptr) {
            LOG_ERROR("getCharacters returned null");
            return;
        }
        
        // Convert to C string
        const char* chars = env->GetStringUTFChars(jChars, nullptr);
        if (chars != nullptr) {
            // Add the characters to ImGui
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharactersUTF8(chars);
            
            // Release the string
            env->ReleaseStringUTFChars(jChars, chars);
        }
    }
}

// JNI method implementations for ImGuiJNI
JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_onKeyEvent(JNIEnv *env, jclass clazz, jint key_code, jint action, jint meta_state) {
    LOG_INFO("Key event: code=%s, action=%s, meta=%s", std::to_string(key_code).c_str(), std::to_string(action).c_str(), std::to_string(meta_state).c_str());
    
    // Map Android key codes to ImGui key codes
    int imguiKey = -1;
    if (key_code >= AKEYCODE_A && key_code <= AKEYCODE_Z) {
        imguiKey = ImGuiKey_A + (key_code - AKEYCODE_A);
    } else if (key_code >= AKEYCODE_0 && key_code <= AKEYCODE_9) {
        imguiKey = ImGuiKey_0 + (key_code - AKEYCODE_0);
    }
    else {
        // Map other keys
        switch (key_code) {
            case AKEYCODE_SPACE: imguiKey = ImGuiKey_Space; break;
            case AKEYCODE_ENTER: imguiKey = ImGuiKey_Enter; break;
            case AKEYCODE_ESCAPE: imguiKey = ImGuiKey_Escape; break;
            case AKEYCODE_TAB: imguiKey = ImGuiKey_Tab; break;
            case AKEYCODE_DEL: imguiKey = ImGuiKey_Backspace; break;
            case AKEYCODE_FORWARD_DEL: imguiKey = ImGuiKey_Delete; break;
            case AKEYCODE_DPAD_LEFT: imguiKey = ImGuiKey_LeftArrow; break;
            case AKEYCODE_DPAD_RIGHT: imguiKey = ImGuiKey_RightArrow; break;
            case AKEYCODE_DPAD_UP: imguiKey = ImGuiKey_UpArrow; break;
            case AKEYCODE_DPAD_DOWN: imguiKey = ImGuiKey_DownArrow; break;
            case AKEYCODE_PAGE_UP: imguiKey = ImGuiKey_PageUp; break;
            case AKEYCODE_PAGE_DOWN: imguiKey = ImGuiKey_PageDown; break;
            case AKEYCODE_HOME: imguiKey = ImGuiKey_Home; break;
            case AKEYCODE_MOVE_END: imguiKey = ImGuiKey_End; break;
        }
    }
    
    if (imguiKey != -1) {
        ImGuiIO& io = ImGui::GetIO();
        
        // Handle key down/up
        if (action == 0) { // ACTION_DOWN
            io.AddKeyEvent((ImGuiKey)imguiKey, true);
        } else if (action == 1) { // ACTION_UP
            io.AddKeyEvent((ImGuiKey)imguiKey, false);
        }
        
        // Handle modifiers
        io.AddKeyEvent(ImGuiKey_ModShift, (meta_state & AMETA_SHIFT_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModCtrl, (meta_state & AMETA_CTRL_ON) != 0);
        io.AddKeyEvent(ImGuiKey_ModAlt, (meta_state & AMETA_ALT_ON) != 0);
    }
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_onTextInput(JNIEnv *env, jclass clazz, jstring text) {
    const char* utf8Text = env->GetStringUTFChars(text, nullptr);
    if (utf8Text != nullptr) {
        LOG_INFO("Received text input: %s", std::string(utf8Text).c_str());
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharactersUTF8(utf8Text);
        env->ReleaseStringUTFChars(text, utf8Text);
    }
}





// Function to update system insets from Java
JNIEXPORT jboolean JNICALL
Java_com_my_app_ImGuiJNI_wantsTextInput(JNIEnv *env, jclass clazz) {
    // Check if ImGui is initialized before accessing its context
    if (!g_initialized) {
        return JNI_FALSE;
    }
    return ImGui::GetIO().WantTextInput;
}

// Function to update system insets from Java
JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_updateSystemInsets(JNIEnv *env, jclass clazz, jint top, jint bottom, jint left, jint right, jboolean unused) {
    LOG_INFO("Updating system insets: top=%s, bottom=%s, left=%s, right=%s", std::to_string(top).c_str(), std::to_string(bottom).c_str(), std::to_string(left).c_str(), std::to_string(right).c_str());
    // Update the insets in the ScalingManager (always portrait mode)
    ScalingManager::getInstance().setSystemInsets(top, bottom, left, right, false);
}

// Function to set screen density from Java
JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_setScreenDensity(JNIEnv *env, jclass clazz, jfloat density) {
    LOG_INFO("Setting screen density: %s", std::to_string(density).c_str());
    // Update the density in the ScalingManager
    ScalingManager::getInstance().setScreenDensity(density);
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_setPackageName(JNIEnv* env, jclass clazz, jstring packageName) {
    const char* packageNameCStr = env->GetStringUTFChars(packageName, nullptr);
    if (packageNameCStr != nullptr) {
        g_PackageName = packageNameCStr;
        LOG_INFO("Package name set to: %s", g_PackageName.c_str());
        env->ReleaseStringUTFChars(packageName, packageNameCStr);
    }
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_nativeLogInfo(JNIEnv* env, jclass clazz, jstring tag, jstring message) {
    const char* tagCStr = env->GetStringUTFChars(tag, nullptr);
    const char* messageCStr = env->GetStringUTFChars(message, nullptr);
    if (tagCStr != nullptr && messageCStr != nullptr) {
        LOG_INFO(tagCStr, messageCStr);
        env->ReleaseStringUTFChars(tag, tagCStr);
        env->ReleaseStringUTFChars(message, messageCStr);
    }
}

JNIEXPORT void JNICALL
Java_com_my_app_ImGuiJNI_nativeLogError(JNIEnv* env, jclass clazz, jstring tag, jstring message) {
    const char* tagCStr = env->GetStringUTFChars(tag, nullptr);
    const char* messageCStr = env->GetStringUTFChars(message, nullptr);
    if (tagCStr != nullptr && messageCStr != nullptr) {
        LOG_ERROR(tagCStr, messageCStr);
        env->ReleaseStringUTFChars(tag, tagCStr);
        env->ReleaseStringUTFChars(message, messageCStr);
    }
}

} // extern "C"