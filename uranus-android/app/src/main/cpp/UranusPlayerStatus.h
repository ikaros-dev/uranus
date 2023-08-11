#ifndef URANUS_ANDROID_URANUSPLAYERSTATUS_H
#define URANUS_ANDROID_URANUSPLAYERSTATUS_H

class UranusPlayerStatus {
public:
    bool exit;
    bool seek = false;
    bool pause = false;
    bool load = true;

    UranusPlayerStatus();
};

#endif //URANUS_ANDROID_URANUSPLAYERSTATUS_H
