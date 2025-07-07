#pragma once

#include <cmath>
#include <android/log.h>
#include <android/configuration.h>

// Forward declaration
struct AConfiguration;

// Structure to hold system insets
struct SystemInsets {
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
    bool isLandscape = false;
};

// Singleton class to manage UI scaling consistently across the application
class ScalingManager {
public:
    // Get the singleton instance
    static ScalingManager& getInstance() {
        static ScalingManager instance;
        return instance;
    }

    // Set the Android configuration
    void setConfiguration(AConfiguration* config) {
        m_config = config;
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "Android configuration set");
    }

    // Set system insets (from navigation bar, status bar, etc.)
    void setSystemInsets(int top, int bottom, int left, int right, bool isLandscape) {
        m_insets.top = top;
        m_insets.bottom = bottom;
        m_insets.left = left;
        m_insets.right = right;
        m_insets.isLandscape = isLandscape;
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                           "System insets set: top=%d, bottom=%d, left=%d, right=%d, isLandscape=%d",
                           top, bottom, left, right, isLandscape);
    }

    // Get system insets
    const SystemInsets& getSystemInsets() const {
        return m_insets;
    }

    // Calculate and get the appropriate scale factor based on device density
    float getScaleFactor(int screenWidth, int screenHeight) {
        // Get the device density if available
        float xdpi = 160.0f; // Default density
        
        if (m_config) {
            // Try to get the screen density if available
            #ifdef AConfiguration_getScreenDensity
            int density = AConfiguration_getScreenDensity(m_config);
            if (density > 0) {
                xdpi = (float)density;
                __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                                  "Got density from AConfiguration: %d", density);
            } else {
                __android_log_print(ANDROID_LOG_WARN, "ScalingManager", 
                                  "AConfiguration_getScreenDensity returned invalid density: %d", density);
            }
            #else
            __android_log_print(ANDROID_LOG_WARN, "ScalingManager", 
                              "AConfiguration_getScreenDensity not available");
            #endif
        } else {
            __android_log_print(ANDROID_LOG_WARN, "ScalingManager", 
                              "No Android configuration available for density calculation");
        }
        
        // Calculate scale based on device density (normalized to 160dpi)
        float densityScale = xdpi / 160.0f;
        
        // Apply the adjustment factor to fine-tune the scale
        float finalScale = densityScale * m_scaleAdjustment;
        
        // Ensure minimum scale
        if (finalScale < 1.0f) {
            finalScale = 1.0f;
        }
        
        // Log the scale calculation
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                           "Calculated scale: %f (density: %f, adjustment: %f) for screen dimensions: %dx%d", 
                           finalScale, densityScale, m_scaleAdjustment, screenWidth, screenHeight);
        
        // For debugging, force a minimum scale if the calculated scale is too small
        if (finalScale < 1.2f) {
            finalScale = 1.2f;
            __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                               "Forcing minimum scale to 1.2 for visibility");
        }
        
        return finalScale;
    }
    
    // Apply scaling to ImGui
    void applyScaling(float scale) {
        if (std::abs(scale - m_lastAppliedScale) > 0.01f || m_forceNextApplication) {
            __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                              "Applying scale: %f (previous: %f, forced: %s)", 
                              scale, m_lastAppliedScale, m_forceNextApplication ? "yes" : "no");
            
            // Store the scale
            m_lastAppliedScale = scale;
            m_forceNextApplication = false;
            
            // The actual application will be done by the caller
        }
    }
    
    // Force the next scaling application
    void forceNextApplication() {
        m_forceNextApplication = true;
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "Forcing next scale application");
    }
    
    // Get the last applied scale
    float getLastAppliedScale() const {
        return m_lastAppliedScale;
    }
    
    // Reset the scaling state
    void reset() {
        m_lastAppliedScale = 0.0f;
        m_forceNextApplication = true;
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "Scaling state reset");
    }
    
    // Set the scale adjustment factor (to fine-tune the scaling)
    void setScaleAdjustment(float adjustment) {
        if (adjustment > 0.1f && adjustment < 2.0f) {
            m_scaleAdjustment = adjustment;
            __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "Scale adjustment set to: %f", adjustment);
            // Force reapplication of scaling with the new adjustment
            forceNextApplication();
        }
    }
    
    // Get the current scale adjustment factor
    float getScaleAdjustment() const {
        return m_scaleAdjustment;
    }

private:
    // Private constructor for singleton
    ScalingManager() : m_lastAppliedScale(0.0f), m_forceNextApplication(true), m_scaleAdjustment(1.0f), m_config(nullptr) {
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "ScalingManager initialized with adjustment: %f", m_scaleAdjustment);
    }
    
    // Prevent copying
    ScalingManager(const ScalingManager&) = delete;
    ScalingManager& operator=(const ScalingManager&) = delete;
    
    float m_lastAppliedScale;
    bool m_forceNextApplication;
    float m_scaleAdjustment; // Adjustment factor to fine-tune scaling
    AConfiguration* m_config; // Pointer to the Android configuration
    SystemInsets m_insets;    // System insets (navigation bar, status bar, etc.)
};
