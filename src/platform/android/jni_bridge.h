#ifndef _JNI_BRIDGE
#define _JNI_BRIDGE

#include <jni.h>

extern JavaVM* g_JavaVM;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_my_app_ImGuiJNI_setPackageName(JNIEnv* env, jclass clazz, jstring packageName);
JNIEXPORT void JNICALL Java_com_my_app_ImGuiJNI_nativeLogInfo(JNIEnv* env, jclass clazz, jstring tag, jstring message);
JNIEXPORT void JNICALL Java_com_my_app_ImGuiJNI_nativeLogError(JNIEnv* env, jclass clazz, jstring tag, jstring message);

#ifdef __cplusplus
}
#endif

#endif
