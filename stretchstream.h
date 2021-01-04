#include <thread>

#include <SFML/Audio.hpp>
#include <rubberband/RubberBandStretcher.h>


class StretchStream: public sf::SoundStream
{
public:

  void init(float* dataSource, int samplingRate, int nrSamples, float speed);
  virtual bool onGetData(Chunk& data);
  virtual void onSeek(sf::Time time);
  float* dataSource;
  int nrSamples;
  RubberBand::RubberBandStretcher *rbs;
  int samplingRate;
private:
  std::vector<sf::Int16> buffer;
  float speed;
  unsigned int rbProcessedUntil;
  int currentSample;
};
