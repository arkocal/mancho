#ifndef CUSTOMSTREAM_H
#define CUSTOMSTREAM_H

#include <vector>
#include <string>

#include <SFML/Audio.hpp>
#include <rubberband/RubberBandStretcher.h>
#include "AudioFile.h"

#include "playerconfig.h"


class CustomStream : public sf::SoundStream
{
public:
    void setConfig(PlayerConfig *config);
    CustomStream();
private:
    virtual bool onGetData(Chunk& data);
    virtual void onSeek(sf::Time timeOffset);
    PlayerConfig *config;
    RubberBand::RubberBandStretcher *rubberBandStretcher;
    std::vector<sf::Int16> samples;
    std::size_t currentSample;
};

#endif // CUSTOMSTREAM_H
