#include <HomeSpan.h>
#include <HardwareSerial.h>
#include <BackgroundAudioPlayer.h>

HardwareSerial dfPlayerSerial(1);

void setup()
{
  Serial.begin(19200);

  homeSpan.begin(Category::Switches, "Background audio: under stairs");

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();

  new BackgroundAudioPlayer(&dfPlayerSerial, 5);
}

void loop()
{
  homeSpan.poll();
}