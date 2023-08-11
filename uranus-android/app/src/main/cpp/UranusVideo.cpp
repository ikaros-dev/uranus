#include "UranusVideo.h"
#include "AndroidLog.h"

UranusVideo::UranusVideo(UranusPlayerStatus *playstatus, UranusCallJava *wlCallJava) {
    this->playstatus = playstatus;
    this->wlCallJava = wlCallJava;
    queue = new UranusQueue(playstatus);
    pthread_mutex_init(&codecMutex, NULL);
}

UranusVideo::~UranusVideo() {

}

void *playVideo(void *data) {
//    C函数 1   C++函数2
    UranusVideo *video = static_cast<UranusVideo *>(data);
//    死循环轮训
    while (video->playstatus != NULL && !video->playstatus->exit) {
//         解码 seek   puase   队列没有数据
        if (video->playstatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (video->playstatus->pause) {
            av_usleep(1000 * 100);
            continue;
        }
        if (video->queue->getQueueSize() == 0) {
//            网络不佳  请慢慢等待  回调应用层
            if (!video->playstatus->load) {
                video->playstatus->load = true;
                video->wlCallJava->onCallLoad(CHILD_THREAD, true);
                av_usleep(1000 * 100);
                continue;
            }

        }

        AVPacket *avPacket = av_packet_alloc();
        if (video->queue->getAvPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
//        视频解码 比较耗时  多线程环境
        pthread_mutex_lock(&video->codecMutex);
//解码操作
        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
//            括号就失败了
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            pthread_mutex_unlock(&video->codecMutex);
            continue;
        }
        AVFrame *avFrame = av_frame_alloc();

        if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0) {
//          括号就失败了
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            pthread_mutex_unlock(&video->codecMutex);
            continue;
        }
//        此时解码成功了  如果 之前是yuv420  ----》   opengl
        if (avFrame->format == AV_PIX_FMT_YUV420P) {
//            压缩1  原始数据2
//            avFrame->data[0];//y
//            avFrame->data[1];//u
//            avFrame->data[2];//v
//            直接转换   yuv420     ---> yuv420
//其他格式 --yuv420
//休眠33ms  不可取33 * 1000
//计算  音频 视频
//            av_usleep(33 * 1000);


            double diff = video->getFrameDiffTime(avFrame);
//            通过diff 计算休眠时间
            av_usleep(video->getDelayTime(diff) * 1000000);
            video->wlCallJava->onCallRenderYUV(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    avFrame->data[0],
                    avFrame->data[1],
                    avFrame->data[2]);
            LOGI("当前视频是YUV420P格式");
        } else {
            LOGI("当前视频不是YUV420P格式");
            AVFrame *pFrameYUV420P = av_frame_alloc();
            int num = av_image_get_buffer_size(
                    AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);
            uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
            av_image_fill_arrays(
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize,
                    buffer,
                    AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);
            SwsContext *sws_ctx = sws_getContext(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    video->avCodecContext->pix_fmt,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC, NULL, NULL, NULL);

            if (!sws_ctx) {
                av_frame_free(&pFrameYUV420P);
                av_free(pFrameYUV420P);
                av_free(buffer);
                pthread_mutex_unlock(&video->codecMutex);
                continue;
            }
            sws_scale(
                    sws_ctx,
                    reinterpret_cast<const uint8_t *const *>(avFrame->data),
                    avFrame->linesize,
                    0,
                    avFrame->height,
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize);


            double diff = video->getFrameDiffTime(avFrame);
//            通过diff 计算休眠时间
            av_usleep(video->getDelayTime(diff) * 1000000);

            //渲染
            video->wlCallJava->onCallRenderYUV(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    pFrameYUV420P->data[0],
                    pFrameYUV420P->data[1],
                    pFrameYUV420P->data[2]);

            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            av_free(buffer);
            sws_freeContext(sws_ctx);
        }
        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
        pthread_mutex_unlock(&video->codecMutex);
    }
    pthread_exit(&video->thread_play);
}

void UranusVideo::play() {
    // 子线程播放   解码
    pthread_create(&thread_play, NULL, playVideo, this);
}

double UranusVideo::getDelayTime(double diff) {
    return 0;
}

double UranusVideo::getFrameDiffTime(AVFrame *avFrame) {
//    先获取视频时间戳  处理之后
//    double pts = av_frame_get_best_effort_timestamp(avFrame);
    double pts = avFrame->pts;
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
//     1.001*40ms
//    pts=pts * time_base.num / time_base.den;
    pts *= av_q2d(time_base);

    if (pts > 0) {
        clock = pts;
    }

    double diff = audio->clock - clock;
    return diff;
}
