#ifndef URANUS_ANDROID_URANUSCALLJAVA_H
#define URANUS_ANDROID_URANUSCALLJAVA_H

#include "jni.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class UranusCallJava {
public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jObj;

    jmethodID jMid_prepared;
    jmethodID jMid_timeInfo;
    jmethodID jMid_load;
    jmethodID jMid_renderYuv;

    UranusCallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~UranusCallJava();


    void onCallPrepared(int type);
    void onCallTimeInfo(int type, int curr, int total);
    void onCallLoad(int type, bool load);
    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);
};


#endif //URANUS_ANDROID_URANUSCALLJAVA_H
