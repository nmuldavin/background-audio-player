#ifndef BACKGROUND_AUDIO_PLAYER_H
#define BACKGROUND_AUDIO_PLAYER_H

#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <HomeSpan.h>

// Light control for audio player (On/Off and Volume)
struct AudioPlayerLight : Service::LightBulb
{
    SpanCharacteristic *on;
    SpanCharacteristic *volume;
    DFRobotDFPlayerMini *player;

    AudioPlayerLight(DFRobotDFPlayerMini *playerRef, int defaultVolume);
    boolean update();
};

// Fan control for track selection (Rotation Speed)
struct AudioPlayerFan : Service::Fan
{
    SpanCharacteristic *active;
    SpanCharacteristic *rotationSpeed;
    DFRobotDFPlayerMini *player;
    int numTracks;

    AudioPlayerFan(DFRobotDFPlayerMini *playerRef, int numTracks, int defaultTrack);
    boolean update();
    char *getTrackFile();
    void logNewState();
};

// Combined accessory manager
struct BackgroundAudioPlayer
{
    int dfPlayerBaudRate = 9600;
    int8_t rxPin = 16;
    int8_t txPin = 17;
    DFRobotDFPlayerMini player;

    BackgroundAudioPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume, int numTracks);
    void initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume);
};

void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume, int numTracks);

int getTrackNumber(int rotationSpeed, int numTracks);

char *trackToFile(int track);

#endif