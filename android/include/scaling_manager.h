#pragma once

#include <cmath>
#include <android/log.h>

// Singleton class to manage UI scaling consistently across the application
class ScalingManager {
public:
    // Get the singleton instance
    static ScalingManager& getInstance() {
        static ScalingManager instance;
        return instance;
    }

    // Calculate and get the appropriate scale factor based on screen dimensions
    float getScaleFactor(int screenWidth, int screenHeight) {
        // Always use the larger dimension to determine scale
        int largerDimension = (screenWidth > screenHeight) ? screenWidth : screenHeight;
        
        // Calculate scale based on screen size
        float scale;
        if (largerDimension > 1080) {
            scale = 2.0f; // High-res screens
        } else if (largerDimension > 720) {
            scale = 1.5f; // Medium-res screens
        } else {
            scale = 1.5f; // Always use at least 1.5 for Android
        }
        
        // Log the scale calculation
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", 
                           "Calculated scale: %f for screen dimensions: %dx%d", 
                           scale, screenWidth, screenHeight);
        
        return scale;
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

private:
    // Private constructor for singleton
    ScalingManager() : m_lastAppliedScale(0.0f), m_forceNextApplication(true) {
        __android_log_print(ANDROID_LOG_INFO, "ScalingManager", "ScalingManager initialized");
    }
    
    // Prevent copying
    ScalingManager(const ScalingManager&) = delete;
    ScalingManager& operator=(const ScalingManager&) = delete;
    
    float m_lastAppliedScale;
    bool m_forceNextApplication;
};
