#include "../../../include/scaling_manager.h"
#include <android/log.h>

// This function should be called from your Android JNI bridge
extern "C" {
    JNIEXPORT void JNICALL
    Java_com_my_app_MainActivity_adjustScaleFactor(JNIEnv *env, jclass clazz, jfloat factor) {
        __android_log_print(ANDROID_LOG_INFO, "ScalingFix", "Adjusting scale factor to: %f", factor);
        ScalingManager::getInstance().setScaleAdjustment(factor);
    }
}
