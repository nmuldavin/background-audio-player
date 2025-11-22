/*!
 *@file play.ino
 *@brief Music Playing Example Program
 *@details  Experimental phenomenon: control MP3 play music, obtain song information
 *@copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [fengli](li.feng@dfrobot.com)
 *@version  V1.1
 *@date  2021-10-15
 *@url https://github.com/DFRobot/DFRobot_DF1201S
 */

#include <DFRobot_DF1201S.h>
#include <SoftwareSerial.h>

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 17; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 16; // Connects to module's TX
SoftwareSerial DF1201SSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobot_DF1201S player;

void setup()
{
  Serial.begin(115200);
  DF1201SSerial.begin(115200);

  int startupCount = 1;

  while (!player.begin(DF1201SSerial))
  {

    if (startupCount <= 10)
    {
      Serial.print("Startup attempt ");
      Serial.print(startupCount);
      Serial.println(" failed. Retrying.");

      startupCount++;
    }
    delay(1000);
  }

  Serial.print("Startup attempt ");
  Serial.print(startupCount);
  Serial.println(" succeeded. Configuring settings.");

  player.setPrompt(false);
  player.setVol(5);
  Serial.print("VOL:");
  /*Get volume*/
  Serial.println(player.getVol());
  /*Repeat current track indefinitely"*/
  player.setPlayMode(player.SINGLECYCLE);
}

void loop()
{
  Serial.println("Playing");
  player.start();

  while (1)
  {
  }
}