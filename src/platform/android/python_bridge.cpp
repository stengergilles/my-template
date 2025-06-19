#include <jni.h>
#include <vector>

extern JavaVM* g_JavaVM;

struct PythonInitResult {
    jobject instance;            // global ref to PyObject (class instance)
    std::vector<jobject> object_args;  // global refs to any PyObject arguments
};

// Helper to check if an object is a PyObject
bool is_pyobject(JNIEnv* env, jobject obj) {
    if (!obj) return false;
    jclass pyObjectClass = env->FindClass("com/chaquo/python/PyObject");
    return env->IsInstanceOf(obj, pyObjectClass);
}

PythonInitResult python_generic_initialize(
    jobject pythonObject,
    const char* class_name,
    jobjectArray args // all arguments, primitives boxed and PyObjects as needed
) {
    PythonInitResult result;
    JNIEnv* env = nullptr;
    if (g_JavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        g_JavaVM->AttachCurrentThread(&env, nullptr);
    }

    jclass initializerClass = env->FindClass("com/example/imguihelloworld/PythonObjectInitializer");
    jmethodID initMethod = env->GetStaticMethodID(
        initializerClass,
        "initialize",
        "(Ljava/lang/String;Lcom/chaquo/python/Python;[Ljava/lang/Object;)Lcom/example/imguihelloworld/PythonObjectInitializer$InitResult;"
    );

    jstring jClassName = env->NewStringUTF(class_name);

    jobject initResult = env->CallStaticObjectMethod(initializerClass, initMethod, jClassName, pythonObject, args);

    // Extract instance
    jclass initResultClass = env->GetObjectClass(initResult);
    jfieldID instanceField = env->GetFieldID(initResultClass, "instance", "Lcom/chaquo/python/PyObject;");
    jobject instance = env->GetObjectField(initResult, instanceField);
    result.instance = env->NewGlobalRef(instance);

    // Extract objectArgs list
    jfieldID objectArgsField = env->GetFieldID(initResultClass, "objectArgs", "Ljava/util/List;");
    jobject objectArgsList = env->GetObjectField(initResult, objectArgsField);
    jclass listClass = env->FindClass("java/util/List");
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    jint size = env->CallIntMethod(objectArgsList, sizeMethod);
    for (jint i = 0; i < size; ++i) {
        jobject obj = env->CallObjectMethod(objectArgsList, getMethod, i);
        result.object_args.push_back(env->NewGlobalRef(obj));
        env->DeleteLocalRef(obj);
    }

    // Clean up
    env->DeleteLocalRef(jClassName);
    env->DeleteLocalRef(initializerClass);
    env->DeleteLocalRef(initResultClass);
    env->DeleteLocalRef(initResult);
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(objectArgsList);

    return result;
}
