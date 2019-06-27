//
// Created by Administrator on 2019/6/4.
//

#include "JavaCallHelper.h"
#include "macro.h"


JavaCallHelper::JavaCallHelper(JavaVM *_javaVM, JNIEnv *_env, jobject &_jobj) : javaVM(_javaVM),
                                                                                env(_env){
    //为了延长obj的生命周期，所以实例化全局对象
    jobj = env->NewGlobalRef(_jobj);
    //获取对象的字节码
    jclass jclazz = env->GetObjectClass(jobj);
    jmid_error =  env->GetMethodID(jclazz,"onError","(I)V");
    jmid_prepare = env->GetMethodID(jclazz,"onPrepare","()V");
    jmid_progress = env->GetMethodID(jclazz, "onProgress", "(I)V");
}

JavaCallHelper::~JavaCallHelper() {

}

void JavaCallHelper::onError(int thread, int code) {
    if(thread == THREAD_CHILD) {
        JNIEnv* p_env;
        //将当前线程绑定到虚拟机，然后执行Java方法，最后将当前线程与虚拟机解绑
       if(javaVM->AttachCurrentThread(&p_env,0) != JNI_OK)   {
           return;
       }
        p_env->CallVoidMethod(jobj,jmid_error,code);
        javaVM->DetachCurrentThread();

    }else {
        //当前线程是主线程，直接调用反射调用Java方法
        env->CallVoidMethod(jobj,jmid_error,code);
    }

}

void JavaCallHelper::onParpare(int thread) {
    if(thread == THREAD_CHILD) {
        JNIEnv *p_env;
        if(javaVM->AttachCurrentThread(&p_env,0) != JNI_OK) {
            return;
        }
        p_env->CallVoidMethod(jobj,jmid_prepare);
        javaVM->DetachCurrentThread();
    }else {
        env->CallVoidMethod(jobj,jmid_prepare);
    }

}

void JavaCallHelper::onProgress(int thread, int progress) {
    if (thread == THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_progress, progress);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_progress, progress);
    }
}
