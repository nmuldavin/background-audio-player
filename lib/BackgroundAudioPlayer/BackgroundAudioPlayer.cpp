#include "HomeSpan.h"
#include "BackgroundAudioPlayer.h"

// scale factor mapping 'color temperature' to track number
const uint8_t TRACK_SCALE_FACTOR = 5;
const uint8_t MAX_TRACKS = 20;

BackgroundAudioPlayer::BackgroundAudioPlayer(
    HardwareSerial *dfPlayerSerial,
    int defaultVolume,
    int numTracks)
    : Service::LightBulb(), player()
{
    on = new Characteristic::On(false);

    volume = new Characteristic::Brightness(defaultVolume);
    volume->setRange(1, 30, 1);

    track = new Characteristic::ColorTemperature(TRACK_SCALE_FACTOR);
    track->setRange(
        TRACK_SCALE_FACTOR,
        MAX_TRACKS * TRACK_SCALE_FACTOR,
        TRACK_SCALE_FACTOR);

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
    player.setLED(false);
    player.setVol(defaultVolume);
    LOG0("Volume:");
    LOG0(player.getVol());
    LOG0("\n");
    player.setPlayMode(player.SINGLECYCLE);
}

boolean BackgroundAudioPlayer::update()
{
    if (this->on->getNewVal() != this->on->getVal())
    {
        if (this->on->getNewVal())
        {
            this->player.start();
        }
        else
        {
            this->player.pause();
        }
    }

    if (this->volume->getNewVal() != this->volume->getVal())
    {
        player.setVol(this->volume->getNewVal());
    }

    if (this->track->getNewVal() != this->track->getVal())
    {
        this->player.playSpecFile(this->getNextFile());
    }

    this->logNewState();

    return (true);
}

void BackgroundAudioPlayer::logNewState()
{
    if (this->on->getNewVal())
    {
        LOG0("Playing file ");
    }
    else
    {
        LOG0("Paused file ");
    }

    LOG0(this->getNextTrackNumber());
    LOG0(".\n");

    LOG0("Volume: ");
    LOG0(this->volume->getNewVal());
    LOG0(".\n");
}

int BackgroundAudioPlayer::getNextTrackNumber()
{
    int trackValue = this->track->getNewVal();
    return (trackValue + TRACK_SCALE_FACTOR / 2) / TRACK_SCALE_FACTOR;
}

char *BackgroundAudioPlayer::getNextFile()
{
    return trackToFile(this->getNextTrackNumber());
}

char *trackToFile(int track)
{
    static char buffer[9];
    sprintf(buffer, "%04d.mp3", track);

    return buffer;
}

// Must use this initializer so that homespan initializes with an accessory before
// invoking service constructor
void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume, int numTracks)
{
    homeSpan.begin(Category::Other, name);

    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();

    new BackgroundAudioPlayer(dfPlayerSerial, 5, numTracks);
}