#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "CusPlayerFFmpeg.h"
#include "JavaCallHelper.h"

extern "C" {
    #include <libavutil/avutil.h>
}



ANativeWindow *window = 0;
CusPlayerFFmpeg *cusPlayerFFmpeg;
JavaVM * javaVM = NULL;
JavaCallHelper *javaCallHelper = NULL;

//系统自动调用该方法，可获取jvm实例
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void *unused) {
    javaVM = vm;
    return JNI_VERSION_1_4;
}






extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_cusplayer_LivePlayer_native_1start(JNIEnv *env, jobject instance) {
    //    正是进入播放状态
    if(cusPlayerFFmpeg) {
        cusPlayerFFmpeg->start();
    }


}

/**
 * 设置视频显示的surface对象
 */

extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_cusplayer_LivePlayer_native_1set_1surface(JNIEnv *env, jobject instance,jobject surface) {

    if(window){
        ANativeWindow_release(window);
        window = 0;
    }
    //通过surface获取nativeWindow 对象，用于视频渲染
    window = ANativeWindow_fromSurface(env,surface);
}



extern "C" JNIEXPORT void JNICALL Java_com_example_zhangzd_cusplayer_LivePlayer_native_1prepare(JNIEnv *env,
        jobject instance, //该对象即为LivePlayer对象
        jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    javaCallHelper = new JavaCallHelper(javaVM, env, instance);
    cusPlayerFFmpeg = new CusPlayerFFmpeg(dataSource, javaCallHelper);
    //初始化ffmpeg
    cusPlayerFFmpeg->prepare();


    env->ReleaseStringUTFChars(dataSource_, dataSource);
}