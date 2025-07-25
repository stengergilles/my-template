package com.my.app;

import android.os.Bundle;
import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;
import android.view.View;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import com.my.app.AppLogger;
import java.util.List;
import java.util.ArrayList;

import androidx.core.view.WindowCompat;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.graphics.Insets;

/**
 * Main activity for the ImGui Hello World application.
 * This extends our custom ImGuiKeyboardHelper which provides better keyboard support.
 */
public class MainActivity extends ImGuiKeyboardHelper {
    
    private static final String TAG = "MainActivity";

    
    // Static instance for access from native code via JNI
    public static MainActivity instance;
    
    // Handler for delayed tasks
    private Handler mHandler = new Handler(Looper.getMainLooper());
    
    // Flag to track keyboard visibility
    private boolean mKeyboardVisible = false;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Store instance for JNI access
        instance = this;
        
        AppLogger.info(TAG, "MainActivity created");
        
        // Set up edge-to-edge display
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);

        // Listen for window insets changes
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(android.R.id.content), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            Insets displayCutout = insets.getInsets(WindowInsetsCompat.Type.displayCutout());

            int left = Math.max(systemBars.left, displayCutout.left);
            int top = Math.max(systemBars.top, displayCutout.top);
            int right = Math.max(systemBars.right, displayCutout.right);
            int bottom = Math.max(systemBars.bottom, displayCutout.bottom);

            // Determine if in landscape mode
            boolean isLandscape = v.getWidth() > v.getHeight();

            // Pass insets and landscape status to native code
            ImGuiJNI.updateSystemInsets(top, bottom, left, right, isLandscape);
            AppLogger.info(TAG, "System Insets: Left=" + left + ", Top=" + top + ", Right=" + right + ", Bottom=" + bottom);


            // Get screen density and pass it to native code
            float density = getResources().getDisplayMetrics().density;
            ImGuiJNI.setScreenDensity(density);
            AppLogger.info(TAG, "Screen Density: " + density);

            return insets;
        });

        startKeyboardVisibilityCheck();
        
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        
        // Store instance for JNI access (in case it was lost)
        instance = this;
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        
        // Clear static instance if this instance is being destroyed
        if (instance == this) {
            instance = null;
        }
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // Handle back button
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // Let the native code handle it first using ImGuiJNI
            ImGuiJNI.onKeyEvent(keyCode, KeyEvent.ACTION_UP, event.getMetaState());
            
            // If we're still here, handle it normally
            return super.onKeyUp(keyCode, event);
        }
        
        return super.onKeyUp(keyCode, event);
    }
    
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // Pass all key events to ImGui
        int action = event.getAction();
        int keyCode = event.getKeyCode();
        
        // Handle special keys
        if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || 
            keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_MUTE) {
            return super.dispatchKeyEvent(event);
        }
        
        // For text input, handle it specially
        if (action == KeyEvent.ACTION_MULTIPLE && keyCode == KeyEvent.KEYCODE_UNKNOWN) {
            String characters = event.getCharacters();
            if (characters != null && !characters.isEmpty()) {
                AppLogger.info(TAG, "Sending text input to ImGui: " + characters);
                ImGuiJNI.onTextInput(characters);
                return true;
            }
        }
        
        // For normal keys, pass to the native code
        if (action == KeyEvent.ACTION_DOWN || action == KeyEvent.ACTION_UP) {
            AppLogger.info(TAG, "Sending key event to ImGui: " + keyCode + ", action: " + action);
            ImGuiJNI.onKeyEvent(keyCode, action, event.getMetaState());
            
            
        }
        
        return super.dispatchKeyEvent(event);
    }
    
    /**
     * Start a periodic check for keyboard visibility based on ImGui's WantTextInput flag
     */
    private void startKeyboardVisibilityCheck() {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                try {
                    // Check if ImGui wants text input
                    boolean wantsTextInput = ImGuiJNI.wantsTextInput();
                    AppLogger.info(TAG, "ImGui WantTextInput: " + wantsTextInput + ", Keyboard visible: " + mKeyboardVisible);
                    
                    // Show or hide keyboard based on ImGui's WantTextInput flag
                    if (wantsTextInput && !mKeyboardVisible) {
                        showKeyboard();
                    } else if (!wantsTextInput && mKeyboardVisible) {
                        hideKeyboard();
                    }
                    
                    // Continue checking
                    mHandler.postDelayed(this, 500); // Check every 500ms
                } catch (Exception e) {
                    AppLogger.error(TAG, "Error in keyboard visibility check: " + e.getMessage());
                }
            }
        }, 1000); // Start after 1 second
    }
    
    /**
     * Static method to show the keyboard - called from native code via JNI
     */
    public static void showKeyboard() {
        AppLogger.info(TAG, "Static showKeyboard called");
        if (instance != null) {
            // Run on UI thread to avoid crashes
            instance.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        AppLogger.info(TAG, "Showing keyboard on UI thread");
                        InputMethodManager imm = (InputMethodManager) instance.getSystemService(Context.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            View view = instance.getWindow().getDecorView().getRootView();
                            
                            // Try multiple approaches to ensure keyboard shows
                            view.requestFocus();
                            
                            // Method 1: Direct show
                            imm.showSoftInput(view, InputMethodManager.SHOW_FORCED);
                            
                            // Method 2: Toggle
                            imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, 0);
                            
                            // Update visibility flag
                            instance.mKeyboardVisible = true;
                            
                            AppLogger.info(TAG, "Keyboard show methods attempted");
                        } else {
                            AppLogger.error(TAG, "InputMethodManager is null");
                        }
                    } catch (Exception e) {
                        AppLogger.error(TAG, "Error showing keyboard: " + e.getMessage());
                    }
                }
            });
        } else {
            AppLogger.error(TAG, "Cannot show keyboard - instance is null");
        }
    }
    
    /**
     * Static method to hide the keyboard - called from native code via JNI
     */
    public static void hideKeyboard() {
        AppLogger.info(TAG, "Static hideKeyboard called");
        if (instance != null) {
            // Run on UI thread to avoid crashes
            instance.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        AppLogger.info(TAG, "Hiding keyboard on UI thread");
                        InputMethodManager imm = (InputMethodManager) instance.getSystemService(Context.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            View view = instance.getWindow().getDecorView().getRootView();
                            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
                            
                            // Update visibility flag
                            instance.mKeyboardVisible = false;
                            
                            AppLogger.info(TAG, "Keyboard hide attempted");
                        } else {
                            AppLogger.error(TAG, "InputMethodManager is null");
                        }
                    } catch (Exception e) {
                        AppLogger.error(TAG, "Error hiding keyboard: " + e.getMessage());
                    }
                }
            });
        } else {
            AppLogger.error(TAG, "Cannot hide keyboard - instance is null");
        }
    }
}
