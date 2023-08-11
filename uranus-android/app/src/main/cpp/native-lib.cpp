#include <jni.h>
#include <string>
#include "android/log.h"
#include "UranusFFmpeg.h"
#include "UranusPlayerStatus.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include <libavformat/avformat.h>
};


_JavaVM *javaVM = NULL;
UranusCallJava *callJava = NULL;
UranusFFmpeg *fFmpeg = NULL;
UranusPlayerStatus *playstatus = NULL;

bool nexit = true;
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {

        return result;
    }
    return JNI_VERSION_1_4;

}

extern "C" JNIEXPORT jstring JNICALL
Java_run_ikaros_uranus_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_run_ikaros_uranus_FfmpegInfoActivity_getFfmpegVersion(JNIEnv *env, jclass clazz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    __android_log_print(ANDROID_LOG_INFO,"Uranus","GetFFmpegVersion\\n%s", strBuffer);
    return env->NewStringUTF(strBuffer);
}

extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegatePrepare(JNIEnv *env, jobject instance,
                                                           jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);

    if(fFmpeg == NULL)
    {
        if(callJava == NULL)
        {
            callJava = new UranusCallJava(javaVM, env, &instance);
        }
        playstatus = new UranusPlayerStatus();
        fFmpeg = new UranusFFmpeg(playstatus, callJava, source);
        fFmpeg->prepare();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateStart(JNIEnv *env, jobject thiz) {
    if(fFmpeg != NULL)
    {
        fFmpeg->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegatePause(JNIEnv *env, jobject thiz) {
    if(fFmpeg != NULL)
    {
        fFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateResume(JNIEnv *env, jobject thiz) {
    if(fFmpeg != NULL)
    {
        fFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateStop(JNIEnv *env, jobject thiz) {

    if(!nexit)
    {
        return;
    }
// 正在退出 只调用一次
    nexit = false;
    if(fFmpeg != NULL)
    {
        fFmpeg->release();
        delete (fFmpeg);
        if(callJava != NULL)
        {
            delete(callJava);
            callJava = NULL;
        }
        if(playstatus != NULL)
        {
            delete(playstatus);
            playstatus = NULL;
        }
    }
    nexit = true;
}
extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateSeek(JNIEnv *env, jobject thiz, jint seconds) {
    if(fFmpeg != NULL)
    {
        fFmpeg->seek(seconds);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateVolume(JNIEnv *env, jobject thiz, jint percent) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setVolume(percent);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateSpeed(JNIEnv *env, jobject thiz, jfloat speed) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setSpeed(speed);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegatePitch(JNIEnv *env, jobject thiz, jfloat pitch) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setPitch(pitch);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_run_ikaros_uranus_player_UranusPlayer_delegateMute(JNIEnv *env, jobject thiz, jint mute) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setMute(mute);
    }
}