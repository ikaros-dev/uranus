#ifndef URANUS_ANDROID_URANUSQUEUE_H
#define URANUS_ANDROID_URANUSQUEUE_H

#include "queue"
#include "pthread.h"
#include "UranusPlayerStatus.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};

class UranusQueue {
public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    UranusPlayerStatus *playstatus = NULL;

    UranusQueue(UranusPlayerStatus *playstatus);
    ~UranusQueue();

    int putAvPacket(AVPacket *packet);
    int getAvPacket(AVPacket *packet);

    int getQueueSize();

    void clearAvPacket();
};

#endif //URANUS_ANDROID_URANUSQUEUE_H
