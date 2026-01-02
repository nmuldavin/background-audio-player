#include "HomeSpan.h"
#include "BackgroundAudioPlayer.h"

const int VOLUME_STEPSIZE = 4;

// AudioPlayerLight implementation
AudioPlayerLight::AudioPlayerLight(DFRobotDFPlayerMini *playerRef, int defaultVolume)
    : Service::LightBulb()
{
    new Characteristic::ConfiguredName("Volume Control");
    on = new Characteristic::On(false);
    volume = (new Characteristic::Brightness(defaultVolume * VOLUME_STEPSIZE))->setRange(0, 100, VOLUME_STEPSIZE);
    player = playerRef;
}

boolean AudioPlayerLight::update()
{
    if (on->getNewVal() != on->getVal())
    {
        if (on->getNewVal())
        {
            player->start();
            LOG0("Playing\n");
        }
        else
        {
            player->pause();
            LOG0("Pausing\n");
        }
    }

    if (volume->getNewVal() != volume->getVal())
    {
        int newVol = volume->getNewVal() / VOLUME_STEPSIZE;
        player->volume(newVol);
        LOG0("Volume: ");
        LOG0(newVol);
        LOG0("\n");
    }

    return true;
}

// AudioPlayerFan implementation
AudioPlayerFan::AudioPlayerFan(DFRobotDFPlayerMini *playerRef, int numTracks, int defaultTrack)
    : Service::Fan()
{
    new Characteristic::ConfiguredName("Track Selection");
    active = new Characteristic::Active(1);

    // Map tracks 1-numTracks to rotation speed percentage
    // Each track gets an equal slice of the 0-100% range
    int stepSize = 100 / numTracks;
    int defaultSpeed = (defaultTrack - 1) * stepSize + stepSize / 2;

    rotationSpeed = (new Characteristic::RotationSpeed(defaultSpeed))->setRange(0, 100, stepSize);

    player = playerRef;
    this->numTracks = numTracks;
}

boolean AudioPlayerFan::update()
{
    if (getTrackNumber(rotationSpeed->getNewVal(), numTracks) != getTrackNumber(rotationSpeed->getVal(), numTracks))
    {
        int trackNum = getTrackNumber(rotationSpeed->getNewVal(), numTracks);

        if (trackNum == 0)
        {
            player->pause();
            LOG0("Pausing\n");
        }
        else
        {
            // Use loop to play and loop the specified track
            player->loop(trackNum);

            logNewState();
        }
    }

    return true;
}

int getTrackNumber(int rotationSpeed, int numTracks)
{
    int stepSize = 100 / numTracks;

    // Convert rotation speed percentage to track number (1-based)
    return rotationSpeed / stepSize;
}

char *AudioPlayerFan::getTrackFile()
{
    return trackToFile(getTrackNumber(rotationSpeed->getNewVal(), numTracks));
}

void AudioPlayerFan::logNewState()
{
    LOG0("Changing to track ");
    LOG0(getTrackNumber(rotationSpeed->getNewVal(), numTracks));
    LOG0(" (rotation speed: ");
    LOG0(rotationSpeed->getNewVal());
    LOG0("%).\n");
    LOG0("Current track: ");
    LOG0(player->readCurrentFileNumber());
    LOG0("\n");
}

// BackgroundAudioPlayer implementation
BackgroundAudioPlayer::BackgroundAudioPlayer(
    HardwareSerial *dfPlayerSerial,
    int defaultVolume,
    int numTracks)
    : player()
{
    initializeDFPlayer(dfPlayerSerial, defaultVolume);
}

void BackgroundAudioPlayer::initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume)
{
    dfPlayerSerial->begin(this->dfPlayerBaudRate, SERIAL_8N1, this->rxPin, this->txPin);

    int startupCount = 1;

    // Note: begin() parameters are (stream, isACK, doReset)
    while (!this->player.begin(*dfPlayerSerial, true, true))
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

    // Set communication timeout
    player.setTimeOut(500);
    player.enableDAC();

    // Set initial volume
    player.volume(defaultVolume);
    LOG0("Volume: ");
    LOG0(defaultVolume);
    LOG0("\n");

    // Note: No need to set play mode globally - loop() will handle it per track
}

char *trackToFile(int track)
{
    static char buffer[14];
    sprintf(buffer, "/mp3/%04d.mp3", track);

    return buffer;
}

// Must use this initializer so that homespan initializes with an accessory before
// invoking service constructor
void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume, int numTracks)
{
    homeSpan.begin(Category::Fans, name);

    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();

    // Create the background audio player and initialize DFPlayer
    BackgroundAudioPlayer *audioPlayer = new BackgroundAudioPlayer(dfPlayerSerial, defaultVolume, numTracks);

    // Add light service for play/pause and volume control
    new AudioPlayerLight(&(audioPlayer->player), defaultVolume);

    // Add fan service for track selection
    new AudioPlayerFan(&(audioPlayer->player), numTracks, 1);
}