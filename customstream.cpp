#include <string>

#include <rubberband/RubberBandStretcher.h>
#include <SFML/Audio.hpp>

#include "customstream.h"
#include "playerconfig.h"

// NOTE TO SELF
// You were following this
// https://www.sfml-dev.org/tutorials/2.5/audio-streams.php

// TODO ADD DESTRUCTOR!

using namespace RubberBand;

CustomStream::CustomStream()
{
}
void CustomStream::setConfig(PlayerConfig *config)
{
    this->config = config;
    int nrChannels = 1;
    // As of now, we are ignoring the second channel
    /*if (config->audioFile.isStereo())
    {
        nrChannels = 2;
    }*/
    this->samples.resize(8000);
    // TODO stop and delete old one
    rubberBandStretcher = new RubberBandStretcher(config->audioFile.getSampleRate(), 1,
                                                  1);
    rubberBandStretcher->setTimeRatio(2.0);
    initialize(nrChannels, config->audioFile.getSampleRate());
}

bool CustomStream::onGetData(Chunk& data)
{
    // TODO run rubber band on a different thread as preprocessor only
    // TODO make a program with rubberband only to figure this out
    // and do not feed output directly
    std::cout << "onGetData" << std::endl;
    // number of samples to stream every time the function is called;
    // in a more robust implementation, it should be a fixed
    // amount of time rather than an arbitrary number of samples
    const int samplesToFeedRB = 16000;
    int samplesToStream = 16000;
    float samplesToFeed[samplesToFeedRB];
    // TODO memcopy
    for (int i=0; i<samplesToFeedRB; i++)
    {
        samplesToFeed[i] = config->audioFile.samples[0][currentSample+i];
    }
    const float* mp = samplesToFeed;
    const float* const* p = &mp;
    /*std::cout << "About to lock" << std::endl;
    config->mtx.lock();
    std::cout << "Acquired lock" << std::endl;*/
    rubberBandStretcher->process(p, samplesToFeedRB, false);
    int available = rubberBandStretcher->available();
    samples = std::vector<sf::Int16>(available);
    std::cout << "Available" << available << std::endl;
    const unsigned AMPLITUDE = 30000;
    //config->audioFile.printSummary();
    samplesToStream = available;
    float buffer[samplesToStream];
    float* fp = &buffer[0];
    float* const* cp = &fp;
    rubberBandStretcher->retrieve(cp, samplesToStream);

    for (unsigned i = 0; i < samplesToStream; i++) {
        samples[i] = buffer[i] * AMPLITUDE;
      //samples[i] = config->audioFile.samples[0][currentSample+i] * AMPLITUDE;
      //std::cout << "Samples[i]" << samples[i] << "@" << currentSample+i << std::endl;
    }
    data.samples = &samples[0];
    data.sampleCount = samplesToStream;
    currentSample += samplesToStream;
    return true;
}

void CustomStream::onSeek(sf::Time timeOffset)
{
    currentSample = timeOffset.asSeconds()*config->audioFile.getSampleRate();
}
