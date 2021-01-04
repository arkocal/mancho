#ifndef PLAYERCONFIG_H
#define PLAYERCONFIG_H

#include <mutex>
#include "AudioFile.h"

class PlayerConfig
{
public:
    float loopStartTime;
    float loopEndTime;
    float playbackSpeed;
    float playheadTime;
    bool playFlag;
    AudioFile<float> audioFile;
    PlayerConfig();
    float endTime();
    std::mutex mtx;
    void setTime(float seconds);
    bool shouldSeek;
    bool hasFile;
};

#endif // PLAYERCONFIG_H
