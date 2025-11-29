#include "BackgroundAudioPlayer.h"

BackgroundAudioPlayer::BackgroundAudioPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume)
    : Service::Switch(), player()
{
    power = new Characteristic::On(false);

    this->initializeDFPlayer(dfPlayerSerial, defaultVolume);
}

void BackgroundAudioPlayer::initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume)
{
    dfPlayerSerial->begin(this->dfPlayerBaudRate, SERIAL_8N1, this->rxPin, this->txPin);

    int startupCount = 1;

    while (!this->player.begin(*dfPlayerSerial))
    {
        LOG0("DFPlayer connection attempt ");
        LOG0(startupCount);
        LOG0(" failed. Retrying.\n");

        startupCount++;
        delay(1000);
    }

    LOG0("DFPlayer connection attempt ");
    LOG0(startupCount);
    LOG0(" succeeded. Configuring settings.\n");

    player.setPrompt(false);
    player.setVol(defaultVolume);
    LOG0("Volume:");
    LOG0(player.getVol());
    LOG0("\n");
    player.setPlayMode(player.SINGLECYCLE);
}

boolean BackgroundAudioPlayer::update()
{
    if (this->power->getNewVal())
    {
        this->player.start();
    }
    else
    {
        this->player.pause();
    }

    return (true);
}