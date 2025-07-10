package com.my.app;

import android.util.Log;
import com.my.app.AppLogger;

/**
 * JNI interface for ImGui
 */
public class ImGuiJNI {
    private static final String TAG = "ImGuiJNI";
    
    // Load the native library
    static {
        try {
            System.loadLibrary("imgui_hello_world");
            AppLogger.info(TAG, "Native library imgui_hello_world loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            AppLogger.error(TAG, "Failed to load native library imgui_hello_world: " + e.getMessage());
        }
    }
    
    // Native methods
    public static native void onKeyEvent(int keyCode, int action, int metaState);
    public static native void onTextInput(String text);
    public static native boolean wantsTextInput();
    public static native void updateSystemInsets(int top, int bottom, int left, int right, boolean unused);
    public static native void setScreenDensity(float density);
    public static native void setPackageName(String packageName);
    public static native void nativeLogInfo(String tag, String message);
    public static native void nativeLogError(String tag, String message);
    
    // Helper method to log and forward text input
    public static void sendTextInput(String text) {
        AppLogger.info(TAG, "Sending text input: " + text);
        onTextInput(text);
    }
}
