#include <DFRobot_DF1201S.h>
#include <HomeSpan.h>
#include <HardwareSerial.h>
#include <BackgroundAudioPlayer.h>

const byte RXD2 = 16; // Connects to module's RX
const byte TXD2 = 17; // Connects to module's TX

HardwareSerial dfPlayerSerial(1); // Use UART channel 1
DFRobot_DF1201S player;           // player object

void initializeDFPlayer()
{
  dfPlayerSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

  int startupCount = 1;

  while (!player.begin(dfPlayerSerial))
  {
    Serial.print("DFPlayer connection attempt ");
    Serial.print(startupCount);
    Serial.println(" failed. Retrying.");

    startupCount++;
    delay(1000);
  }

  LOG0("DFPlayer connection attempt ");
  LOG0(startupCount);
  LOG0(" succeeded. Configuring settings.");

  /* Configure */
  player.setPrompt(false);
  player.setVol(5);
  LOG0("Volume:");
  LOG0(player.getVol());
  /* Repeat current track indefinitely" */
  player.setPlayMode(player.SINGLECYCLE);
}

void setup()
{
  Serial.begin(19200);
  initializeDFPlayer();

  // IMPORTANT: The Name you choose below MUST BE UNIQUE across all your HomeSpan devices!

  homeSpan.begin(Category::Switches, "Background audio: under stairs");

  // Next, we construct a simple HAP Accessory Database with a single Accessory containing 3 Services,
  // each with their own required Characteristics.

  new SpanAccessory(); // Begin by creating a new Accessory using SpanAccessory(), no arguments needed

  new Service::AccessoryInformation(); // HAP requires every Accessory to implement an AccessoryInformation Service

  // The only required Characteristic for the Accessory Information Service is the special Identify Characteristic.  It takes no arguments:

  new Characteristic::Identify(); // Create the required Identify Characteristic

  new BackgroundAudioPlayer(&player);
}

void loop()
{
  homeSpan.poll();
}