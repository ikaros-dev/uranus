
#ifndef URANUS_ANDROIDLOG_H
#define URANUS_ANDROIDLOG_H

#include "android/log.h"

#define LOG_DEBUG true

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,"Uranus",FORMAT,##__VA_ARGS__);
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"Uranus",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"Uranus",FORMAT,##__VA_ARGS__);

#endif //URANUS_ANDROIDLOG_H

