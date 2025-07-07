#pragma once

#include <cmath>
#include <iostream>

// Structure to hold system insets
struct SystemInsets {
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
    bool isLandscape = false;
};

// Forward declaration for Android compatibility
struct AConfiguration;

// Singleton class to manage UI scaling consistently across the application
class ScalingManager {
public:
    // Get the singleton instance
    static ScalingManager& getInstance() {
        static ScalingManager instance;
        return instance;
    }

    // Set the Android configuration (no-op on Linux)
    void setConfiguration(AConfiguration* config) {
        // No-op on Linux
        std::cout << "ScalingManager: Android configuration set (no-op on Linux)" << std::endl;
    }

    // Set system insets (from navigation bar, status bar, etc.)
    void setSystemInsets(int top, int bottom, int left, int right, bool isLandscape) {
        m_insets.top = top;
        m_insets.bottom = bottom;
        m_insets.left = left;
        m_insets.right = right;
        m_insets.isLandscape = isLandscape;
        std::cout << "ScalingManager: System insets set: top=" << top 
                  << ", bottom=" << bottom 
                  << ", left=" << left 
                  << ", right=" << right 
                  << ", isLandscape=" << isLandscape << std::endl;
    }

    // Get system insets
    const SystemInsets& getSystemInsets() const {
        return m_insets;
    }

    // Calculate and get the appropriate scale factor based on device density
    float getScaleFactor(int screenWidth, int screenHeight) {
        // On Linux, we use a simpler approach based on screen dimensions
        float baseScale = 1.0f;
        
        // Adjust scale based on screen resolution
        if (screenWidth > 1920 || screenHeight > 1080) {
            baseScale = 1.5f;
        } else if (screenWidth > 1280 || screenHeight > 720) {
            baseScale = 1.2f;
        }
        
        // Apply the adjustment factor to fine-tune the scale
        float finalScale = baseScale * m_scaleAdjustment;
        
        // Ensure minimum scale
        if (finalScale < 1.0f) {
            finalScale = 1.0f;
        }
        
        // Log the scale calculation
        std::cout << "ScalingManager: Calculated scale: " << finalScale 
                  << " (base: " << baseScale 
                  << ", adjustment: " << m_scaleAdjustment 
                  << ") for screen dimensions: " << screenWidth << "x" << screenHeight << std::endl;
        
        return finalScale;
    }
    
    // Apply scaling to ImGui
    void applyScaling(float scale) {
        if (std::abs(scale - m_lastAppliedScale) > 0.01f || m_forceNextApplication) {
            std::cout << "ScalingManager: Applying scale: " << scale 
                      << " (previous: " << m_lastAppliedScale 
                      << ", forced: " << (m_forceNextApplication ? "yes" : "no") << ")" << std::endl;
            
            // Store the scale
            m_lastAppliedScale = scale;
            m_forceNextApplication = false;
            
            // The actual application will be done by the caller
        }
    }
    
    // Force the next scaling application
    void forceNextApplication() {
        m_forceNextApplication = true;
        std::cout << "ScalingManager: Forcing next scale application" << std::endl;
    }
    
    // Get the last applied scale
    float getLastAppliedScale() const {
        return m_lastAppliedScale;
    }
    
    // Reset the scaling state
    void reset() {
        m_lastAppliedScale = 0.0f;
        m_forceNextApplication = true;
        std::cout << "ScalingManager: Scaling state reset" << std::endl;
    }
    
    // Set the scale adjustment factor (to fine-tune the scaling)
    void setScaleAdjustment(float adjustment) {
        if (adjustment > 0.1f && adjustment < 2.0f) {
            m_scaleAdjustment = adjustment;
            std::cout << "ScalingManager: Scale adjustment set to: " << adjustment << std::endl;
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
    ScalingManager() : m_lastAppliedScale(0.0f), m_forceNextApplication(true), m_scaleAdjustment(1.0f) {
        std::cout << "ScalingManager: ScalingManager initialized with adjustment: " << m_scaleAdjustment << std::endl;
    }
    
    // Prevent copying
    ScalingManager(const ScalingManager&) = delete;
    ScalingManager& operator=(const ScalingManager&) = delete;
    
    float m_lastAppliedScale;
    bool m_forceNextApplication;
    float m_scaleAdjustment; // Adjustment factor to fine-tune scaling
    SystemInsets m_insets;    // System insets (navigation bar, status bar, etc.)
};
