//
// Created by li-guohao on 2023/8/11.
//

#ifndef URANUS_ANDROID_URANUSVIDEO_H
#define URANUS_ANDROID_URANUSVIDEO_H

#include "UranusPlayerStatus.h"
#include "UranusCallJava.h"
#include "UranusQueue.h"
#include "UranusAudio.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/time.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

class UranusVideo {
public:
    UranusQueue *queue = NULL;
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    UranusPlayerStatus *playstatus = NULL;
    UranusCallJava *wlCallJava = NULL;
    pthread_mutex_t codecMutex;
    pthread_t thread_play;
    double clock = 0;
    // 实时计算出来   与音频的差值
    double delayTime = 0;
    // 默认休眠时间   40ms  0.04s    帧率 25帧
    double defaultDelayTime = 0.04;
    UranusAudio *audio = NULL;
    AVRational time_base;

    UranusVideo(UranusPlayerStatus *playstatus, UranusCallJava *wlCallJava);
    ~UranusVideo();
    void play();
    double getDelayTime(double diff);
    double getFrameDiffTime(AVFrame *avFrame);
};
#endif //URANUS_ANDROID_URANUSVIDEO_H
