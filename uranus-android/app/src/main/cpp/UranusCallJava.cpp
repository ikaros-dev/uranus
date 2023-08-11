#include "UranusCallJava.h"
#include "AndroidLog.h"

UranusCallJava::UranusCallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj) {

    this->javaVM = javaVM;
    this->jniEnv = env;
    this->jObj = *obj;
    this->jObj = env->NewGlobalRef(jObj);

    jclass  jlz = jniEnv->GetObjectClass(jObj);
    if(!jlz)
    {
        if(LOG_DEBUG)
        {
            LOGE("get jclass wrong");
        }
        return;
    }

    jMid_prepared = env->GetMethodID(jlz, "onCallPrepared", "()V");
//    ------------新加-----------------
    jMid_timeInfo = env->GetMethodID(jlz, "onCallTimeInfo", "(II)V");

    jMid_load = env->GetMethodID(jlz, "onCallLoad", "(Z)V");
    jMid_renderYuv = env->GetMethodID(jlz, "onCallRenderYUV", "(II[B[B[B)V");

}

UranusCallJava::~UranusCallJava() {

}

void UranusCallJava::onCallPrepared(int type) {

    if(type == MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(jObj, jMid_prepared);
    }
    else if(type == CHILD_THREAD)
    {
        JNIEnv *jniEnv;
        if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
        {
            if(LOG_DEBUG)
            {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(jObj, jMid_prepared);
        javaVM->DetachCurrentThread();
    }


}

void UranusCallJava::onCallTimeInfo(int type, int curr, int total) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jObj, jMid_timeInfo, curr, total);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallTimeInfo wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jObj, jMid_timeInfo, curr, total);
        javaVM->DetachCurrentThread();
    }

}

void UranusCallJava::onCallLoad(int type, bool load) {

    if(type == MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(jObj, jMid_load, load);
    }
    else if(type == CHILD_THREAD)
    {
        JNIEnv *jniEnv;
        if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
        {
            if(LOG_DEBUG)
            {
                LOGE("call onCallLoad worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(jObj, jMid_load, load);
        javaVM->DetachCurrentThread();
    }


}

void UranusCallJava::onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {

    JNIEnv *jniEnv;
    if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
    {
        if(LOG_DEBUG)
        {
            LOGE("call onCallComplete worng");
        }
        return;
    }

    jbyteArray y = jniEnv->NewByteArray(width * height);
    jniEnv->SetByteArrayRegion(y, 0, width * height, reinterpret_cast<const jbyte *>(fy));

    jbyteArray u = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(u, 0, width * height / 4, reinterpret_cast<const jbyte *>(fu));

    jbyteArray v = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(v, 0, width * height / 4, reinterpret_cast<const jbyte *>(fv));

    jniEnv->CallVoidMethod(jObj, jMid_renderYuv, width, height, y, u, v);

    jniEnv->DeleteLocalRef(y);
    jniEnv->DeleteLocalRef(u);
    jniEnv->DeleteLocalRef(v);
    javaVM->DetachCurrentThread();

}
