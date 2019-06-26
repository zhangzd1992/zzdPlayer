#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
extern "C" {
    #include <libavutil/avutil.h>
}



ANativeWindow *window = 0;


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_zhangzd_cusplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}




extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_cusplayer_LivePlayer_native_1start(JNIEnv *env, jobject instance) {

    // TODO

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
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_zhangzd_cusplayer_LivePlayer_native_1prepare(JNIEnv *env, jobject instance,
                                                              jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);

    // TODO

    env->ReleaseStringUTFChars(dataSource_, dataSource);
}