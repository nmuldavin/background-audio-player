#include "HomeSpan.h"
#include "BackgroundAudioPlayer.h"

const int VOLUME_STEPSIZE = 5;

// AudioPlayerLight implementation
AudioPlayerLight::AudioPlayerLight(DFRobot_DF1201S *playerRef, int defaultVolume)
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
        }
        else
        {
            player->pause();
        }
    }

    if (volume->getNewVal() != volume->getVal())
    {
        player->setVol(volume->getNewVal() / VOLUME_STEPSIZE);
    }

    return true;
}

// AudioPlayerFan implementation
AudioPlayerFan::AudioPlayerFan(DFRobot_DF1201S *playerRef, int numTracks, int defaultTrack)
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
    if (rotationSpeed->getNewVal() != rotationSpeed->getVal())
    {
        // Use playSpecFile with the actual file path
        player->playSpecFile(String(trackToFile(getTrackNumber())));
        logNewState();
    }

    return true;
}

int AudioPlayerFan::getTrackNumber()
{
    int speed = rotationSpeed->getNewVal();
    int stepSize = 100 / numTracks;

    // Convert rotation speed percentage to track number (1-based)
    int trackNum = (speed / stepSize) + 1;

    // Clamp to valid range
    if (trackNum < 1)
        trackNum = 1;
    if (trackNum > numTracks)
        trackNum = numTracks;

    return trackNum;
}

char *AudioPlayerFan::getTrackFile()
{
    return trackToFile(getTrackNumber());
}

void AudioPlayerFan::logNewState()
{
    LOG0("Changing to track ");
    LOG0(getTrackNumber());
    LOG0(" (rotation speed: ");
    LOG0(rotationSpeed->getNewVal());
    LOG0("%).\n");
    LOG0("File name: ");
    LOG0(player->getFileName());
    LOG0("\n");
    LOG0("File number: ");
    LOG0(player->getCurFileNumber());
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
    player.switchFunction(player.MUSIC);

    // CRITICAL: Wait for mode switch to complete
    delay(2000);

    player.setVol(defaultVolume);
    LOG0("Volume:");
    LOG0(player.getVol());
    LOG0("\n");

    // Use ALLCYCLE instead of SINGLECYCLE to allow track changes
    player.setPlayMode(player.ALLCYCLE);
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