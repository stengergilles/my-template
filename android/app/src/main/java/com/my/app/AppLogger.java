package com.my.app;

public class AppLogger {

    public static void info(String tag, String message) {
        ImGuiJNI.nativeLogInfo(tag, message);
    }

    public static void error(String tag, String message) {
        ImGuiJNI.nativeLogError(tag, message);
    }
}