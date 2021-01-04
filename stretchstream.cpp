#include <iostream>
#include <thread>

#include "stretchstream.h"

//TODO dynamically find the right size
#define CHUNKSIZE 10000


void StretchStream::init(float* dataSource, int samplingRate, int nrSamples, float speed)
{
  // TODO assert speed is reasonable?
  buffer = std::vector<sf::Int16>(CHUNKSIZE*3);
  this->samplingRate = samplingRate;
  rbProcessedUntil = 0;
  this->dataSource = dataSource;
  this->nrSamples = nrSamples;
  rbProcessedUntil = 0;
  currentSample = 0;
  this->speed = speed;
  initialize(1, samplingRate);
  // TODO stereo support
  rbs = new RubberBand::RubberBandStretcher(
                        samplingRate, // sampling Rate
					    1, // nr Channels
					    RubberBand::RubberBandStretcher::Option::OptionProcessRealTime,
					    1/speed,
					    1.0);
}

bool StretchStream::onGetData(Chunk& data)
{
  const float* chunkPointer = dataSource + currentSample;
  const float* const* sourcePointer = &chunkPointer;
  rbs->process(sourcePointer, CHUNKSIZE, false);
  currentSample += CHUNKSIZE;
  int available = rbs->available();
  std::vector<float> rbsOut = std::vector<float>(available);
  float* rbsOutputPtr = &(rbsOut[0]);
  float* const* rbsOutputPointer = &rbsOutputPtr;
  rbs->retrieve(rbsOutputPointer, available);
  const int AMPLITUDE = 30000;
  for (unsigned i=0; i<available; i++)
  {
      buffer[i] = rbsOut[i] * AMPLITUDE;
  }
  data.samples = &(buffer[0]);
  data.sampleCount = available;
  return true;
}


void StretchStream::onSeek(sf::Time time)
{   
  currentSample = (time.asSeconds()*samplingRate);
}
