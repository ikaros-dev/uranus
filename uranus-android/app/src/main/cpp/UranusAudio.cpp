#include "UranusAudio.h"
#include "AndroidLog.h"

UranusAudio::UranusAudio(UranusPlayerStatus *playstatus, int sample_rate,
                         UranusCallJava *callJava) {
    this->playstatus = playstatus;
    this->sample_rate = sample_rate;
    queue = new UranusQueue(playstatus);
    buffer = (uint8_t *) av_malloc(sample_rate * 2 * 2);
    this->callJava = callJava;
    sampleBuffer= static_cast<SAMPLETYPE *>(malloc(sample_rate * 2 * 2));
    soundTouch = new SoundTouch();
    soundTouch->setSampleRate(sample_rate);
    soundTouch->setChannels(2);
//    speed  1.1   1.5  2.1
    soundTouch->setTempo(speed);
    soundTouch->setPitch(pitch);
}

UranusAudio::~UranusAudio() {

}

void *decodePlay(void *data)
{
    UranusAudio *wlAudio = (UranusAudio *) data;

    wlAudio->initOpenSLES();

    pthread_exit(&wlAudio->thread_play);
}

void UranusAudio::play() {
    pthread_create(&thread_play, NULL, decodePlay, this);
}

int UranusAudio::resampleAudio(void **pcmbuf) {

    while(playstatus != NULL && !playstatus->exit)
    {
        avPacket = av_packet_alloc();
        if(queue->getAvPacket(avPacket) != 0)
        {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if(ret != 0)
        {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if(ret == 0)
        {

            if(avFrame->channels && avFrame->channel_layout == 0)
            {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            }
            else if(avFrame->channels == 0 && avFrame->channel_layout > 0)
            {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swr_ctx;

            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL, NULL
            );
            if(!swr_ctx || swr_init(swr_ctx) <0)
            {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swr_ctx);
                continue;
            }

            nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples);
            LOGI("nb------>%d",nb);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
//计算时间 pts 序号1     时间戳 2
//            avFrame->pts *  time_base.num / (double) time_base.den;
            now_time= avFrame->pts * av_q2d(time_base);
            if(now_time < clock)
            {
                now_time = clock;
            }
            clock = now_time;
//
            *pcmbuf = buffer;
            if(LOG_DEBUG)
            {
                LOGI("data_size is %d", data_size);
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            break;
        } else{
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }
    return data_size;
}


//播放   喇叭需要取出数据   ---》播放
void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void * context)
{
    LOGI("-------->播放 ");
    UranusAudio *wlAudio = (UranusAudio *) context;
    if(wlAudio != NULL)
    {

//        int buffersize = wlAudio->resampleAudio();
        int buffersize = wlAudio->getSoundTouchData();
//        buffersize  25       sountouch       opensles   100
        if(buffersize > 0)
        {
            wlAudio->clock+=buffersize/ ((double)(wlAudio->sample_rate * 2 * 2));
//            100*1000字节
            if(wlAudio->clock - wlAudio->last_tiem >= 0.1){
                wlAudio->last_tiem = wlAudio->clock;
//            pcmBufferCallBack 回调次数比较频繁    回调java
                wlAudio->callJava->onCallTimeInfo(CHILD_THREAD,wlAudio->clock,wlAudio->duration);

            }
            (* wlAudio-> pcmBufferQueue)->Enqueue( wlAudio->pcmBufferQueue, (char *) wlAudio-> sampleBuffer, buffersize*2*2);
        }
    }
}

void UranusAudio::initOpenSLES() {


    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //第二步，创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void)result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void)result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};


    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};

    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            static_cast<SLuint32>(getCurrentSampleRateForOpensles(sample_rate)),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};


    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE,SL_IID_VOLUME,SL_IID_MUTESOLO};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 2, ids, req);
    //初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

//    得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);
//    获取声道操作接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmMutePlay);
//   拿控制  播放暂停恢复的句柄
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_VOLUME,&pcmVolumePlay);
//    注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
//    获取播放状态接口
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    pcmBufferCallBack(pcmBufferQueue, this);
}

int UranusAudio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate)
    {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate =  SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void UranusAudio::onCallTimeInfo(int type, int curr, int total) {
    // todo impl UranusAudio::onCallTimeInfo
}

void UranusAudio::pause() {
    if(pcmPlayerPlay != NULL)
    {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void UranusAudio::resume() {
    if(pcmPlayerPlay != NULL)
    {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void UranusAudio::setMute(int mute) {
    LOGI(" 声道  接口%p", pcmMutePlay);
    if(pcmMutePlay == NULL)
    {
        return;
    }
    this->mute = mute;
    if(mute == 0)//right   0
    {
//        1
        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, false);
        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, true);

    } else if(mute == 1)//left
    {
        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, true);
        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, false);
    } else if(mute == 2)//center
    {

        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, false);
        (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, false);
    }

}

void UranusAudio::setVolume(int percent) {

    if(pcmVolumePlay != NULL) {
        if (percent > 30) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -40);
        } else {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -100);
        }
    }
}

void UranusAudio::setSpeed(float speed) {
    this->speed = speed;
    if (soundTouch != NULL) {
        soundTouch->setTempo(speed);
    }
}

int UranusAudio::getSoundTouchData() {
//        重新整理波形  sountouch
//我们先取数据   pcm 的数据  就在  outbuffer
    while(playstatus != NULL && !playstatus->exit){
        out_buffer = NULL;
        if(finished){
            finished = false;
//            从网络流 文件 读取数据 out_buffer  字节数量  out_buffer   是一个旧波
            data_size = this->resampleAudio(reinterpret_cast<void **>(&out_buffer));
            if (data_size > 0) {
                for(int i = 0; i < data_size / 2 + 1; i++){
//short  2个字节  pcm数据   ====波形
                    sampleBuffer[i]=(out_buffer[i * 2] | ((out_buffer[i * 2 + 1]) << 8));
                }
//丢给sountouch   进行波的整理
                soundTouch->putSamples(sampleBuffer, nb);
//                接受一个新波 sampleBuffer
                num=soundTouch->receiveSamples(sampleBuffer, data_size / 4);
            }else{
                soundTouch->flush();
            }
        }

        if (num == 0) {
            finished = true;
            continue;
        } else{
            if(out_buffer == NULL){
                num=soundTouch->receiveSamples(sampleBuffer, data_size / 4);
                if(num == 0)
                {
                    finished = true;
                    continue;
                }
            }
            return num;
        }

    }
    return 0;
}

void UranusAudio::setPitch(float pitch) {

    this->pitch = pitch;
    if(soundTouch != NULL)
    {
        soundTouch->setPitch(pitch);
    }
}

void UranusAudio::release() {

    if(queue != NULL)
    {
        delete(queue);
        queue = NULL;
    }
    if(pcmPlayerObject != NULL)
    {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }

    if(outputMixObject != NULL)
    {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if(engineObject != NULL)
    {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if(buffer != NULL)
    {
        free(buffer);
        buffer = NULL;
    }

    if(avCodecContext != NULL)
    {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if(playstatus != NULL)
    {
        playstatus = NULL;
    }
    if(callJava != NULL)
    {
        callJava = NULL;
    }
}
