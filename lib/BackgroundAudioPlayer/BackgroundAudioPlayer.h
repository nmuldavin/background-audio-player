#ifndef BACKGROUND_AUDIO_PLAYER_H
#define BACKGROUND_AUDIO_PLAYER_H

#include <HardwareSerial.h>
#include <DFRobot_DF1201S.h>
#include <HomeSpan.h>

struct BackgroundAudioPlayer : Service::LightBulb
{
    int dfPlayerBaudRate = 115200;
    int8_t rxPin = 16;
    int8_t txPin = 17;
    int8_t numTracks = 1;
    DFRobot_DF1201S player;
    SpanCharacteristic *on;
    SpanCharacteristic *volume;
    SpanCharacteristic *track;

    BackgroundAudioPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume, int numTracks);
    void initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume);
    void logNewState();
    int getNextTrackNumber();
    char *getNextFile();

    boolean update();
};

void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume, int numTracks);

char *trackToFile(int track);

#endif