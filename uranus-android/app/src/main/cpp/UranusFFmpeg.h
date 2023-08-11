#ifndef URANUS_ANDROID_URANUSFFMPEG_H
#define URANUS_ANDROID_URANUSFFMPEG_H

#include "UranusCallJava.h"
#include "UranusPlayerStatus.h"
#include "UranusAudio.h"
#include "UranusVideo.h"
#include "pthread.h"


extern "C"
{
#include <libavutil/time.h>
#include "libavformat/avformat.h"
};

class UranusFFmpeg{
public:
    UranusCallJava *callJava = NULL;
    const char* url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    UranusAudio *audio = NULL;
    UranusVideo *video = NULL;
    UranusPlayerStatus *playstatus = NULL;

    int duration = 0;
    pthread_mutex_t seek_mutex;
    pthread_mutex_t init_mutex;
    bool exit = false;


    UranusFFmpeg(UranusPlayerStatus *playstatus, UranusCallJava *callJava, const char *url);
    ~UranusFFmpeg();

    void prepare();
    void decodeFFmpegThread();
    void start();
    void pause();
    void seek(int64_t secds);
    void resume();
    void setMute(int mute);
    void setVolume(int percent);

    void setSpeed(float speed);

    void setPitch(float pitch);
    void release();
    int getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext);
};

#endif //URANUS_ANDROID_URANUSFFMPEG_H
