#include "keyboard_helper.h"
#include <jni.h>
#include "imgui.h"  // Added ImGui header
#include "../../../include/logger.h" // Include logger.h

#define LOG_TAG "keyboard_helper"

// Forward declaration of the JavaVM reference from jni_bridge.cpp
extern JavaVM* g_JavaVM;

// These need to be declared as extern variables from jni_bridge.cpp
extern jclass g_MainActivityClass;
extern jmethodID g_ShowKeyboardMethod;
extern jmethodID g_HideKeyboardMethod;

// Implementation of the showKeyboard function
extern "C" void showKeyboard() {
    
    showKeyboardSafely();
}

// Implementation of the hideKeyboard function
extern "C" void hideKeyboard() {
    
    hideKeyboardSafely();
}

// Safe implementation of showKeyboard that checks for null pointers
extern "C" bool showKeyboardSafely() {
    
    
    if (!g_JavaVM) {
        LOG_ERROR("JavaVM is null, cannot show keyboard");
        return false;
    }
    
    JNIEnv* env = nullptr;
    bool attached = false;
    
    // Get the JNIEnv safely
    jint result = g_JavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        
        if (g_JavaVM->AttachCurrentThread(&env, nullptr) != 0) {
            LOG_ERROR("Failed to attach thread to JavaVM");
            return false;
        }
        attached = true;
    } else if (result != JNI_OK || env == nullptr) {
        LOG_ERROR("Failed to get JNIEnv");
        return false;
    }
    
    // Find the MainActivity class if not already cached
    jclass mainActivityClass = g_MainActivityClass;
    if (mainActivityClass == nullptr) {
        
        mainActivityClass = env->FindClass("com/my/app/MainActivity");
        if (mainActivityClass == nullptr) {
            LOG_ERROR("Failed to find MainActivity class");
            env->ExceptionClear(); // Clear any pending exception
            if (attached) g_JavaVM->DetachCurrentThread();
            return false;
        }
        g_MainActivityClass = (jclass)env->NewGlobalRef(mainActivityClass);
    }
    
    // Find the showKeyboard method if not already cached
    jmethodID showKeyboardMethod = g_ShowKeyboardMethod;
    if (showKeyboardMethod == nullptr) {
        
        showKeyboardMethod = env->GetStaticMethodID(g_MainActivityClass, "showKeyboard", "()V");
        if (showKeyboardMethod == nullptr) {
            LOG_ERROR("Failed to find showKeyboard method");
            env->ExceptionClear(); // Clear any pending exception
            if (attached) g_JavaVM->DetachCurrentThread();
            return false;
        }
        g_ShowKeyboardMethod = showKeyboardMethod;
    }
    
    // Call the static method
    
    env->CallStaticVoidMethod(mainActivityClass, showKeyboardMethod);
    
    // Check for exceptions
    if (env->ExceptionCheck()) {
        LOG_ERROR("Exception occurred while calling showKeyboard");
        env->ExceptionDescribe();
        env->ExceptionClear();
        if (attached) g_JavaVM->DetachCurrentThread();
        return false;
    }
    
    // Detach the thread if we attached it
    if (attached) {
        
        g_JavaVM->DetachCurrentThread();
    }
    
    
    return true;
}

// Safe implementation of hideKeyboard that checks for null pointers
extern "C" bool hideKeyboardSafely() {
    
    
    if (!g_JavaVM) {
        LOG_ERROR("JavaVM is null, cannot hide keyboard");
        return false;
    }
    
    JNIEnv* env = nullptr;
    bool attached = false;
    
    // Get the JNIEnv safely
    jint result = g_JavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        
        if (g_JavaVM->AttachCurrentThread(&env, nullptr) != 0) {
            LOG_ERROR("Failed to attach thread to JavaVM");
            return false;
        }
        attached = true;
    } else if (result != JNI_OK || env == nullptr) {
        LOG_ERROR("Failed to get JNIEnv");
        return false;
    }
    
    // Find the MainActivity class if not already cached
    jclass mainActivityClass = g_MainActivityClass;
    if (mainActivityClass == nullptr) {
        
        mainActivityClass = env->FindClass("com/my/app/MainActivity");
        if (mainActivityClass == nullptr) {
            LOG_ERROR("Failed to find MainActivity class");
            env->ExceptionClear(); // Clear any pending exception
            if (attached) g_JavaVM->DetachCurrentThread();
            return false;
        }
        g_MainActivityClass = (jclass)env->NewGlobalRef(mainActivityClass);
    }
    
    // Find the hideKeyboard method if not already cached
    jmethodID hideKeyboardMethod = g_HideKeyboardMethod;
    if (hideKeyboardMethod == nullptr) {
        
        hideKeyboardMethod = env->GetStaticMethodID(mainActivityClass, "hideKeyboard", "()V");
        if (hideKeyboardMethod == nullptr) {
            LOG_ERROR("Failed to find hideKeyboard method");
            env->ExceptionClear(); // Clear any pending exception
            if (attached) g_JavaVM->DetachCurrentThread();
            return false;
        }
        g_HideKeyboardMethod = hideKeyboardMethod;
    }
    
    // Call the static method
    
    env->CallStaticVoidMethod(mainActivityClass, hideKeyboardMethod);
    
    // Check for exceptions
    if (env->ExceptionCheck()) {
        LOG_ERROR("Exception occurred while calling hideKeyboard");
        env->ExceptionDescribe();
        env->ExceptionClear();
        if (attached) g_JavaVM->DetachCurrentThread();
        return false;
    }
    
    // Detach the thread if we attached it
    if (attached) {
        
        g_JavaVM->DetachCurrentThread();
    }
    
    
    return true;
}


