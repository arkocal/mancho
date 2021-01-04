#include <QDebug>
#include <SFML/Audio.hpp>

#include "mainwindow.h"
#include "playerconfig.h"
#include "stretchstream.h"

void audioLoop()
{
    PlayerConfig* config = MainWindow::playerConfig;
    qDebug() << "Audio loop started";
    while (true)
    {
        // Wait until play
        while (!config->playFlag)
        {
            sf::sleep(sf::milliseconds(50));
        }
        // Initialize stream
        StretchStream myStream;
        int firstsample = config->loopStartTime * config->audioFile.getSampleRate();
        myStream.init(&(config->audioFile.samples[0][firstsample]),
                config->audioFile.getSampleRate(),
                config->audioFile.getNumSamplesPerChannel(),
                config->playbackSpeed);
        myStream.play();
        // Play until paused
        while (config->playFlag)
        {
            config->mtx.lock();
            if (config->shouldSeek)
            {
                myStream.setPlayingOffset(sf::seconds(config->playheadTime - config->loopStartTime)/config->playbackSpeed);
                config->shouldSeek = false;
            }
            float currTime = config->loopStartTime +
                    myStream.getPlayingOffset().asSeconds() * config->playbackSpeed;
            // Loop if necessary
            if (currTime > config->loopEndTime)
            {
                myStream.setPlayingOffset(sf::seconds(0));
                currTime = config->loopStartTime;
            }
            config->playheadTime = currTime;
            config->mtx.unlock();
            sf::sleep(sf::milliseconds(10));
        }
        myStream.stop();
    }
}
