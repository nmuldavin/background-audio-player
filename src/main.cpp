#include <HomeSpan.h>
#include <HardwareSerial.h>
#include <BackgroundAudioPlayer.h>

const char *NAME = "Background Audio: Under Stairs";
const int DEFAULT_VOLUME = 5;
const int NUM_TRACKS = 5;

HardwareSerial dfPlayerSerial(1);

void setup()
{
  Serial.begin(19200);

  initializeAccessory(&dfPlayerSerial, NAME, DEFAULT_VOLUME, NUM_TRACKS);
}

void loop()
{
  homeSpan.poll();
}