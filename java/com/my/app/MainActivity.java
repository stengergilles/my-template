package com.my.app;

import android.os.Bundle;
import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;
import android.view.View;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.os.Build;
import android.graphics.Rect;
import java.util.List;
import java.util.ArrayList;

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
        
        Log.d(TAG, "MainActivity created");
        
        // Make the app fullscreen
        setupFullscreen();
        
        // Start checking for insets
        startInsetsCheck();
        
        startKeyboardVisibilityCheck();
    }
    
    /**
     * Set up fullscreen mode
     */
    private void setupFullscreen() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            // For Android 11+
            getWindow().setDecorFitsSystemWindows(false);
            
            View decorView = getWindow().getDecorView();
            decorView.setOnApplyWindowInsetsListener((v, insets) -> {
                updateInsets(insets);
                return insets;
            });
            
            if (decorView.getWindowInsetsController() != null) {
                decorView.getWindowInsetsController().hide(
                        WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                decorView.getWindowInsetsController().setSystemBarsBehavior(
                        WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            // For older Android versions
            View decorView = getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            decorView.setSystemUiVisibility(uiOptions);
        }
    }
    
    /**
     * Update insets based on WindowInsets
     */
    private void updateInsets(WindowInsets insets) {
        int top = 0;
        int bottom = 0;
        int left = 0;
        int right = 0;
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            // For Android 11+
            top = insets.getInsets(WindowInsets.Type.statusBars()).top;
            bottom = insets.getInsets(WindowInsets.Type.navigationBars()).bottom;
            left = insets.getInsets(WindowInsets.Type.navigationBars()).left;
            right = insets.getInsets(WindowInsets.Type.navigationBars()).right;
            
            // Add additional padding to ensure we're below the status bar
            top += 10; // Add a small buffer
        } else {
            // For older Android versions
            Rect rect = new Rect();
            getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);
            top = rect.top;
            
            // Add additional padding
            top += 10;
            
            // Try to estimate navigation bar height
            int resourceId = getResources().getIdentifier("navigation_bar_height", "dimen", "android");
            if (resourceId > 0) {
                bottom = getResources().getDimensionPixelSize(resourceId);
            }
        }
        
        Log.d(TAG, "System insets: top=" + top + ", bottom=" + bottom + ", left=" + left + ", right=" + right);
        
        // Send insets to native code
        ImGuiJNI.updateSystemInsets(top, bottom, left, right);
    }
    
    /**
     * Start a periodic check for insets
     */
    private void startInsetsCheck() {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                try {
                    // Get current insets
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                        WindowInsets insets = getWindow().getDecorView().getRootWindowInsets();
                        if (insets != null) {
                            updateInsets(insets);
                        }
                    } else {
                        Rect rect = new Rect();
                        getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);
                        
                        int top = rect.top;
                        int bottom = 0;
                        int left = 0;
                        int right = 0;
                        
                        // Try to estimate navigation bar height
                        int resourceId = getResources().getIdentifier("navigation_bar_height", "dimen", "android");
                        if (resourceId > 0) {
                            bottom = getResources().getDimensionPixelSize(resourceId);
                        }
                        
                        ImGuiJNI.updateSystemInsets(top, bottom, left, right);
                    }
                    
                    // Continue checking
                    mHandler.postDelayed(this, 1000); // Check every second
                } catch (Exception e) {
                    Log.e(TAG, "Error in insets check: " + e.getMessage(), e);
                }
            }
        }, 500); // Start after 500ms
    }
    
    /**
     * Static method to get system insets - called from native code via JNI
     */
    public static int[] getSystemInsets() {
        int[] insets = new int[4]; // top, bottom, left, right
        
        if (instance != null) {
            try {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                    WindowInsets windowInsets = instance.getWindow().getDecorView().getRootWindowInsets();
                    if (windowInsets != null) {
                        insets[0] = windowInsets.getInsets(WindowInsets.Type.statusBars()).top;
                        insets[1] = windowInsets.getInsets(WindowInsets.Type.navigationBars()).bottom;
                        insets[2] = windowInsets.getInsets(WindowInsets.Type.navigationBars()).left;
                        insets[3] = windowInsets.getInsets(WindowInsets.Type.navigationBars()).right;
                        
                        // Add additional padding to ensure we're below the status bar
                        insets[0] += 10; // Add a small buffer
                    }
                } else {
                    Rect rect = new Rect();
                    instance.getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);
                    insets[0] = rect.top;
                    
                    // Add additional padding
                    insets[0] += 10;
                    
                    // Try to estimate navigation bar height
                    int resourceId = instance.getResources().getIdentifier("navigation_bar_height", "dimen", "android");
                    if (resourceId > 0) {
                        insets[1] = instance.getResources().getDimensionPixelSize(resourceId);
                    }
                }
                
                Log.d(TAG, "getSystemInsets: top=" + insets[0] + ", bottom=" + insets[1] + 
                      ", left=" + insets[2] + ", right=" + insets[3]);
            } catch (Exception e) {
                Log.e(TAG, "Error getting system insets: " + e.getMessage(), e);
            }
        } else {
            Log.e(TAG, "Cannot get system insets - instance is null");
        }
        
        return insets;
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        
        // Store instance for JNI access (in case it was lost)
        instance = this;
        
        // Re-apply fullscreen
        setupFullscreen();
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
                Log.d(TAG, "Sending text input to ImGui: " + characters);
                ImGuiJNI.onTextInput(characters);
                return true;
            }
        }
        
        // For normal keys, pass to the native code
        if (action == KeyEvent.ACTION_DOWN || action == KeyEvent.ACTION_UP) {
            Log.d(TAG, "Sending key event to ImGui: " + keyCode + ", action: " + action);
            ImGuiJNI.onKeyEvent(keyCode, action, event.getMetaState());
            
            // For character keys on ACTION_DOWN, also send the character
            if (action == KeyEvent.ACTION_DOWN) {
                int unicodeChar = event.getUnicodeChar();
                if (unicodeChar != 0) {
                    String charStr = String.valueOf((char)unicodeChar);
                    Log.d(TAG, "Sending unicode character to ImGui: " + charStr);
                    ImGuiJNI.onTextInput(charStr);
                }
            }
            
            return true;
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
                    Log.d(TAG, "ImGui WantTextInput: " + wantsTextInput + ", Keyboard visible: " + mKeyboardVisible);
                    
                    // Show or hide keyboard based on ImGui's WantTextInput flag
                    if (wantsTextInput && !mKeyboardVisible) {
                        showKeyboard();
                    } else if (!wantsTextInput && mKeyboardVisible) {
                        hideKeyboard();
                    }
                    
                    // Continue checking
                    mHandler.postDelayed(this, 500); // Check every 500ms
                } catch (Exception e) {
                    Log.e(TAG, "Error in keyboard visibility check: " + e.getMessage(), e);
                }
            }
        }, 1000); // Start after 1 second
    }
    
    /**
     * Static method to show the keyboard - called from native code via JNI
     */
    public static void showKeyboard() {
        Log.d(TAG, "Static showKeyboard called");
        if (instance != null) {
            // Run on UI thread to avoid crashes
            instance.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Log.d(TAG, "Showing keyboard on UI thread");
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
                            
                            Log.d(TAG, "Keyboard show methods attempted");
                        } else {
                            Log.e(TAG, "InputMethodManager is null");
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Error showing keyboard: " + e.getMessage(), e);
                    }
                }
            });
        } else {
            Log.e(TAG, "Cannot show keyboard - instance is null");
        }
    }
    
    /**
     * Static method to hide the keyboard - called from native code via JNI
     */
    public static void hideKeyboard() {
        Log.d(TAG, "Static hideKeyboard called");
        if (instance != null) {
            // Run on UI thread to avoid crashes
            instance.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Log.d(TAG, "Hiding keyboard on UI thread");
                        InputMethodManager imm = (InputMethodManager) instance.getSystemService(Context.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            View view = instance.getWindow().getDecorView().getRootView();
                            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
                            
                            // Update visibility flag
                            instance.mKeyboardVisible = false;
                            
                            Log.d(TAG, "Keyboard hide attempted");
                        } else {
                            Log.e(TAG, "InputMethodManager is null");
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Error hiding keyboard: " + e.getMessage(), e);
                    }
                }
            });
        } else {
            Log.e(TAG, "Cannot hide keyboard - instance is null");
        }
    }
}
