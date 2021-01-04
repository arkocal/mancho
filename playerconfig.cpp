#include "playerconfig.h"

PlayerConfig::PlayerConfig()
{
    loopStartTime = 0;
    loopEndTime = 10;
    playheadTime = 0;
    playFlag = false;
    hasFile = false;
}

float PlayerConfig::endTime()
{
    return float(audioFile.getNumSamplesPerChannel()) / float(audioFile.getSampleRate());
}

void PlayerConfig::setTime(float time)
{
    playheadTime = time;
    shouldSeek = true;
}
