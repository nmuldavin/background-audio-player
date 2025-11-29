#ifndef BACKGROUND_AUDIO_PLAYER_H
#define BACKGROUND_AUDIO_PLAYER_H

#include <HardwareSerial.h>
#include <DFRobot_DF1201S.h>
#include <HomeSpan.h>

struct BackgroundAudioPlayer : Service::Switch
{
    int dfPlayerBaudRate = 115200;
    int8_t rxPin = 16;
    int8_t txPin = 17;
    DFRobot_DF1201S player;
    SpanCharacteristic *power;

    BackgroundAudioPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume);

    void initializeDFPlayer(HardwareSerial *dfPlayerSerial, int defaultVolume);

    boolean update();
};

void initializeAccessory(HardwareSerial *dfPlayerSerial, const char *name, int defaultVolume);

#endif