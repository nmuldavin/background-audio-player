#ifndef BACKGROUND_AUDIO_PLAYER_H
#define BACKGROUND_AUDIO_PLAYER_H

#include <HardwareSerial.h>
#include <DFRobot_DF1201S.h>
#include <HomeSpan.h>

// Light control for audio player (On/Off and Volume)
struct AudioPlayerLight : Service::LightBulb
{
    SpanCharacteristic *on;
    SpanCharacteristic *volume;
    DFRobot_DF1201S *player;

    AudioPlayerLight(DFRobot_DF1201S *playerRef, int defaultVolume);
    boolean update();
};

// Fan control for track selection (Rotation Speed)
struct AudioPlayerFan : Service::Fan
{
    SpanCharacteristic *active;
    SpanCharacteristic *rotationSpeed;
    DFRobot_DF1201S *player;
    int numTracks;

    AudioPlayerFan(DFRobot_DF1201S *playerRef, int numTracks, int defaultTrack);
    boolean update();
    int getTrackNumber();
    char *getTrackFile();
    void logNewState();
};

// Combined accessory manager
struct BackgroundAudioPlayer
{
    int dfPlayerBaudRate = 115200;
    int8_t rxPin = 16;
    int8_t txPin = 17;
    DFRobot_DF1201S player;

    BackgroundAudioPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume, int numTracks);
    void initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume);
};

void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume, int numTracks);

char *trackToFile(int track);

#endif