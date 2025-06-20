#pragma once
#include <jni.h>
#include <string>
#include <vector>
#include <type_traits>

// Wraps a reference to a Java PyObject (represents a Python object)
struct PyJniObject {
    jobject ref = nullptr;
    explicit PyJniObject(jobject obj) : ref(obj) {}
};

// Holds result of Python object initialization
struct PythonInitResult {
    jobject instance;                  // Global ref to initialized PyObject
    std::vector<PyJniObject> py_args;  // Global refs to any PyObject args
};

// Helper: convert std::string to jstring
inline jstring to_jstring(JNIEnv* env, const std::string& val) {
    return env->NewStringUTF(val.c_str());
}

// Helper: box primitive types for JNI
template<typename T>
jobject box(JNIEnv* env, T val) {
    if constexpr (std::is_same_v<T, int>) {
        jclass cls = env->FindClass("java/lang/Integer");
        jmethodID ctor = env->GetMethodID(cls, "<init>", "(I)V");
        jobject obj = env->NewObject(cls, ctor, val);
        env->DeleteLocalRef(cls);
        return obj;
    } else if constexpr (std::is_same_v<T, double>) {
        jclass cls = env->FindClass("java/lang/Double");
        jmethodID ctor = env->GetMethodID(cls, "<init>", "(D)V");
        jobject obj = env->NewObject(cls, ctor, val);
        env->DeleteLocalRef(cls);
        return obj;
    } else if constexpr (std::is_same_v<T, float>) {
        jclass cls = env->FindClass("java/lang/Float");
        jmethodID ctor = env->GetMethodID(cls, "<init>", "(F)V");
        jobject obj = env->NewObject(cls, ctor, val);
        env->DeleteLocalRef(cls);
        return obj;
    } else if constexpr (std::is_same_v<T, bool>) {
        jclass cls = env->FindClass("java/lang/Boolean");
        jmethodID ctor = env->GetMethodID(cls, "<init>", "(Z)V");
        jobject obj = env->NewObject(cls, ctor, val);
        env->DeleteLocalRef(cls);
        return obj;
    } else {
        static_assert(sizeof(T) == 0, "Unsupported primitive type for boxing");
    }
}

// Helper: add arguments to jobjectArray
template<typename Arg>
void set_arg(JNIEnv* env, jobjectArray array, int idx, Arg&& val) {
    if constexpr (std::is_same_v<std::decay_t<Arg>, std::string>) {
        jstring s = to_jstring(env, val);
        env->SetObjectArrayElement(array, idx, s);
        env->DeleteLocalRef(s);
    } else if constexpr (std::is_same_v<std::decay_t<Arg>, const char*>) {
        jstring s = env->NewStringUTF(val);
        env->SetObjectArrayElement(array, idx, s);
        env->DeleteLocalRef(s);
    } else if constexpr (std::is_same_v<std::decay_t<Arg>, int> ||
                         std::is_same_v<std::decay_t<Arg>, float> ||
                         std::is_same_v<std::decay_t<Arg>, double> ||
                         std::is_same_v<std::decay_t<Arg>, bool>) {
        jobject obj = box(env, val);
        env->SetObjectArrayElement(array, idx, obj);
        env->DeleteLocalRef(obj);
    } else if constexpr (std::is_same_v<std::decay_t<Arg>, PyJniObject>) {
        env->SetObjectArrayElement(array, idx, val.ref);
    } else {
        static_assert(sizeof(Arg) == 0, "Unsupported argument type");
    }
}

// Helper: build jobjectArray from C++ standard types and PyJniObject
template<typename... Args>
jobjectArray build_jobject_array(JNIEnv* env, Args&&... args) {
    constexpr size_t N = sizeof...(Args);
    jclass objectClass = env->FindClass("java/lang/Object");
    jobjectArray arr = env->NewObjectArray(N, objectClass, nullptr);
    int idx = 0;
    (set_arg(env, arr, idx++, std::forward<Args>(args)), ...);
    env->DeleteLocalRef(objectClass);
    return arr;
}

// The main initializer: instantiate a Python class with any arguments
template<typename... Args>
PythonInitResult python_generic_initialize(
    JNIEnv* env,
    jobject pythonObject,             // com.chaquo.python.Python instance
    const std::string& class_name,    // Full Python class name
    Args&&... args                    // Arguments: standard types and PyJniObject
) {
    jobjectArray jargs = build_jobject_array(env, std::forward<Args>(args)...);

    jclass initializerClass = env->FindClass("com/example/imguihelloworld/PythonObjectInitializer");
    jmethodID initMethod = env->GetStaticMethodID(
        initializerClass,
        "initialize",
        "(Ljava/lang/String;Lcom/chaquo/python/Python;[Ljava/lang/Object;)Lcom/example/imguihelloworld/PythonObjectInitializer$InitResult;"
    );

    jstring jClassName = to_jstring(env, class_name);
    jobject initResult = env->CallStaticObjectMethod(initializerClass, initMethod, jClassName, pythonObject, jargs);

    jclass resultClass = env->GetObjectClass(initResult);
    jfieldID instanceField = env->GetFieldID(resultClass, "instance", "Lcom/chaquo/python/PyObject;");
    jfieldID objectArgsField = env->GetFieldID(resultClass, "objectArgs", "Ljava/util/List;");

    jobject instance = env->GetObjectField(initResult, instanceField);

    // Extract PyObject args (non-primitives)
    jobject objectArgsList = env->GetObjectField(initResult, objectArgsField);
    jclass listClass = env->FindClass("java/util/List");
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    jint size = env->CallIntMethod(objectArgsList, sizeMethod);

    std::vector<PyJniObject> py_objs;
    for (jint i = 0; i < size; ++i) {
        jobject obj = env->CallObjectMethod(objectArgsList, getMethod, i);
        py_objs.emplace_back(env->NewGlobalRef(obj));
        env->DeleteLocalRef(obj);
    }

    PythonInitResult result = { env->NewGlobalRef(instance), std::move(py_objs) };

    // Clean up local refs
    env->DeleteLocalRef(jClassName);
    env->DeleteLocalRef(initializerClass);
    env->DeleteLocalRef(resultClass);
    env->DeleteLocalRef(initResult);
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(objectArgsList);
    env->DeleteLocalRef(jargs);

    return result;
}
