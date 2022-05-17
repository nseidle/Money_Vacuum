/*
  A kid's museum donation money vacuum
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 24th, 2014
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  The Money Vacuum is a donation collection exhibit at the WOW! kid's museum in Lafayette, CO.
  The idea is that a kid or adult inserts a dollar bill into the end of a clear tube. This
  breaks the IR beam that causes the system to activate. This causes the dust collection
  system to kick on pull the dollar through a series of tubes. Near the end is a hopper
  that catches the bills before getting to the vacuum motor.

  The RedBoard monitors two IR gates for the entry and capture of the dollar. It also monitors
  the overall time the vacuum is on, turning it off after a preset amount of time (usually
  5-10 seconds is all it takes for a dollar to get through the system).

  The system will also ignore gates if they activate too often, too close together, or for
  too long of a time. For example, if the entrance gate is broken for more than 2 seconds the
  user is either holding the bill or just shoving their hand in there.

  3/1/2014: After lots of testing it was determined that the IR LED was so powerful and the RX so
  sensitive that I needed a 10k resistor inline with the TX LED in order to break the beam with my hand.

  5/5/2014: Added an override switch but don't have enough free pins. Had to take two pins
  from the hopper IR gate.

  6/16/2014: Covered in dust. Sound deadening box installed in the rafters. Single hopper
  channel install. Override switch installed.

  6/23/2014: Exhibit is up and operational.

  8/18/2014: Exhibit has been performing swimmingly. Increasing audio

  4/18/2022: Inlet wires broke. Running strong otherwise. Update to VS1053 sdFat library.

  ToDo: Add WDT

  How the override is wired:
  Orange of CAT cable connects to the orange paddle on the switch. The orange paddle
  connects (shorts) to the 5V paddle when switch is set to the OFF system position. This
  is read by the Arduino: if the pin is high (connected to 5V), system should be off. If pin is low the
  system should be on.

  Orange/White of the CAT cable connects to the red paddle on the switch. The red paddle
  is energized with 5V.

  Blue of the CAT cable connects to the brown paddle on the switch. The brown paddle
  connects to negative of the LED. If this pin is set LOW the LED will be on. If this
  pin is set high the LED will be off.


  MP3 decoding
  Amplifier
  2-4 IR gates
  2 relays

  13 - SCK/Status
  12 - MISO
  11 - MOSI
  10 - Entry IRX Gate

  9 - SD-CS
  8 - MP3-RST
  7 - MP3-DCS
  6 - MP3-CS
  5 - Hopper IR
  4 - Entry IR
  3 - Entry IR
  2 - MP3-DREQ
  1/0 - Serial / Imp

  A0 - Relay
  A1 - Relay
  A2 - Entry IRX Gate
  A3 - Hopper IRX Gate
  A4 - Hopper IRX Gate / Override Switch, yellow on the cntr, orange on CAT cable
  A5 - Hopper IR / Override LED, red on cntr, blue within CAT cable
*/

#define ENABLE_DEVELOPER //Uncomment this line to ignore override switch

//Include various libraries
//#include <avr/wdt.h> //We need watch dog for this program
#include <SdFat.h>         // SDFat Library
SdFat sd; // Create object to handle SD functions

//New library
#include <vs1053_SdFat.h> // Click here to get the library: http://librarymanager/All#VS1053_sdfat
vs1053 MP3player; // Create Mp3 library object

//Old library
//#include <SPI.h>           // SPI library
//#include "src/SFEMP3Shield/SFEMP3Shield.h"
//SFEMP3Shield MP3player; // Create Mp3 library object

//Hardware connections
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int irDetector0 = 10;
int irDetector1 = A2;
int irDetector2 = A3;
//int irDetector3 = A4;

int irEmitter0 = 3;
int irEmitter1 = 4;
int irEmitter2 = 5;
//int irEmitter3 = A5;

int overRideSwitch = A4;
int overRideLED = A5;

const byte blower = A0;
const byte strobe = A1;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//Global variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int Hits[10]; //This array holds the number of times a channel is activated in a row
//We use this to disable a channel if it gets too many sequential 'hits'

int MAXHITS = 10; //This is the number of hits before a channel is ignored.
int MINHITS = 5; //This is the number of hits required before a channel is considered broken (for reals).

int MAX_BLOW_TIME = 7000; //This is the max ms system will run. 7 seconds (7,000) is good.
int MAX_STROBE_TIME = 5000; //This is the max ms strobe will spin. 5 seconds (5,000) is good.

int TRANSMISSIONS = 50; //This is the number times we cycle a beam. 40 to 50 is good.
//Increasing this number slows down the system but helps prevent false positive breaks

int NUMBER_OF_CHANNELS = 4;

long thisSecond = 0; //For blinking the status LED
boolean announceHits = false;

long blowerStart = 0; //Keeps track of when the blower was started
long strobeStart = 0;

int previousTrack1 = 1; //Used to prevent the playing of sounds twice in a row
int previousTrack2 = 2;

boolean playerStopped = false; //These are booleans used to control the main loop
boolean blowerBeamBroken = false;
boolean strobeBeamBroken = false;

int reportNumber = 0;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//These are the messages that are sent to the Imp
//They can be anything really but we chose a start/stop byte with a number
//for easier parsing
#define MONEY_ONLINE  "0,Money_Vacuum_Online"
#define STROBE_OFF  "1,Strobe_off"
#define PLAYER_OFF  "2,Player_off"

#define BLOWER_ON_NEWBILL "3,Blow-on:newbill"
#define BLOWER_ON_MAXTIME "4,Blow-on:maxtime"
#define BLOWER_OFF_BILLEXIT "5,Blow-off:bill_exit"
#define BLOWER_OFF_MAXTIME "6,Blow-off:maxtime"

#define OVERRIDE_ON "7,Override_on"
#define OVERRIDE_OFF "8,Override_off"
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void setup() {
  //wdt_reset(); //Pet the dog
  //wdt_disable(); //We don't want the watchdog during init

  pinMode(A0, INPUT); //Just for a second so we can seed the random generator
  randomSeed(analogRead(0)); //For picking random audio tracks

  pinMode(overRideSwitch, INPUT);
  pinMode(overRideLED, OUTPUT);

  pinMode(irEmitter0, OUTPUT);
  pinMode(irEmitter1, OUTPUT);
  pinMode(irEmitter2, OUTPUT);

  pinMode(irDetector0, INPUT);
  pinMode(irDetector1, INPUT);
  pinMode(irDetector2, INPUT);

  digitalWrite(irDetector0, HIGH);
  digitalWrite(irDetector1, HIGH);
  digitalWrite(irDetector2, HIGH);

  pinMode(blower, OUTPUT);
  pinMode(strobe, OUTPUT);
  digitalWrite(blower, LOW);
  digitalWrite(strobe, LOW);

  //Prefill the hits array - this was needed during testing
  for (int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++)
    Hits[channel] = MAXHITS;

  Serial.begin(115200);
  Serial.println("Money Vacuum");

  printMenu();

  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield

  playRandomTrack();

  digitalWrite(overRideLED, LOW); //Turn LED on

  //Let the world know we are online
  reportStatus(MONEY_ONLINE); //Report this interaction to the imp
  //wdt_enable(WDTO_1S); //Unleash the beast
}

void loop()
{
  //wdt_reset(); //Pet the dog

  //Stop everything if the override switch is engaged
  if (isOverrideTurnedOn() == true)
  {
    digitalWrite(overRideLED, HIGH); //Turn off LED to indicate system is off

    digitalWrite(blower, LOW); //Turn off blower immediately
    digitalWrite(strobe, LOW); //Turn off strobe if it happens to be on
    //MP3player.end(); //Turn off any MP3 playing
    if (MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track
    playerStopped = true;

    while (isOverrideTurnedOn() == true)
    {
      delayDog(1000); //Delay with watchdog in mind
      reportStatus(OVERRIDE_ON); //Report this interaction to the imp
    }
    digitalWrite(overRideLED, LOW); //Turn LED on, System is back on
    reportStatus(OVERRIDE_OFF); //Report this interaction to the imp
  }

  if (playerStopped == true) checkBeams(); //Test the beams only when MP3 is not playing

  if (blowerBeamBroken == true)
  {
    blowerBeamBroken = false;

    blowerStart = millis(); //Record this time. If someone puts in two bills this will cause the system to run longer.

    //Check to see if we are already running
    if (digitalRead(blower) == LOW)
    {
      reportStatus(BLOWER_ON_NEWBILL); //Report this interaction to the imp
      digitalWrite(blower, HIGH); //Start blower
    }
  }

  if (strobeBeamBroken == true)
  {
    strobeBeamBroken = false;

    //Check to see if we are already spinning the strobe
    if (digitalRead(strobe) == LOW)
    {
      //Serial.println("Bill has finished! Blower off. Strobe on. Playing sound.");
      reportStatus(BLOWER_OFF_BILLEXIT); //Report this interaction to the imp

      digitalWrite(blower, LOW); //Stop blower
      digitalWrite(strobe, HIGH); //Start strobe
      strobeStart = millis(); //Record the time the strobe starts

      delayDog(1000); //Put some time between strobe, blower, and sound
      playRandomTrack(); //Play sound
    }
  }

  if (digitalRead(blower) == HIGH) //Check to see if the blower is running
  {
    if (millis() - blowerStart > MAX_BLOW_TIME)
    {
      digitalWrite(blower, LOW); //Turn off blower

      reportStatus(BLOWER_OFF_MAXTIME); //Report this interaction to the imp

      //Check to see if we are already spinning the strobe
      if (digitalRead(strobe) == LOW)
      {
        //Run the strobe for a little bit because we can
        //For some reason the exit gate isn't working
        digitalWrite(strobe, HIGH); //Start strobe
        strobeStart = millis(); //Record the time the strobe starts

        delayDog(1000); //Put some time between strobe, blower, and sound
        playRandomTrack(); //Play sound
      }
    }
  }

  if (digitalRead(strobe) == HIGH) //Check to see if the strobe is blinking
  {
    if (millis() - strobeStart > MAX_STROBE_TIME)
    {
      digitalWrite(strobe, LOW); //Turn off strobe

      reportStatus(STROBE_OFF); //Report this interaction to the imp
    }
  }

  //Turn off MP3 chip when not playing
  if (playerStopped == false)
  {
    //Serial.println("Player is playing");
    if (MP3player.isPlaying() == false)
    {
      //MP3player.end(); //MP3 player is buzzing. This disables it

      playerStopped = true;

      playSilence();

      reportStatus(PLAYER_OFF); //Report this interaction to the imp
    }
  }

  //If we reach the end of the silence MP3, start again
  if (playerStopped == true && MP3player.isPlaying() == false)
  {
    playSilence();
  }

  if (Serial.available()) mainMenu();
}

//Delays a given amount, petting the dog while we delay
void delayDog(int delay_amt)
{
  long startTime = millis();
  while (millis() - startTime < delay_amt) {
    delay(10);
    //wdt_reset(); //Pet the dog
  }
}

//Takes in a string and creates a parsable string out of it
void reportStatus(char* eventCode)
{
  Serial.print("$,");
  Serial.print(eventCode);
  Serial.print(",");
  Serial.print(reportNumber++);
  Serial.println(",#,"); //That final comma is important
}

//The over-ride switch is wired a bit funky. See comments at top.
//If switch is set to the position labeled 'OFF' it is really connecting
//5V to the pin. If switch is in 'ON' position, the overRide pin is left floating.
//Therefore we will do a high-z test to see if the pin is connected to 5V or left floating.
//Additionally, if we are bench testing, ignore pin state
boolean isOverrideTurnedOn(void)
{
#ifdef ENABLE_DEVELOPER
  return (false);
#endif

  boolean systemOn = false;

  pinMode(overRideSwitch, OUTPUT); //Let's control the pin for a second
  digitalWrite(overRideSwitch, LOW);

  if (digitalRead(overRideSwitch) == HIGH)
  {
    //The pin is *really* shorted to 5V
    systemOn = false; //System should be off
  }
  else
  {
    //The pin is not connected to anything so it is ok to run the system
    systemOn = true;
  }

  digitalWrite(overRideSwitch, HIGH);
  pinMode(overRideSwitch, INPUT); //Stop trying to control the pin

  return (systemOn);
}

//Checks the beams for breaking at sets variables if one is broken
void checkBeams()
{
  //wdt_reset(); //Pet the dog

  for (int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++)
  {
    if (test_channel(channel) == 0) //Beam is broken
    {
      Hits[channel]++; //Record this hit
      if (Hits[channel] > MAXHITS) Hits[channel] = MAXHITS;
    }
    else
      Hits[channel] = 0; //Reset this channel

    delay(2); //We have to have some amount of time in between IR flooding/checking, 1 to 5ms works well
  } //End beam checking

  for (int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++)
  {
    /*Serial.print(channel, DEC);
      Serial.print(":");
      Serial.print(Hits[channel]);
      Serial.print(" ");

      if(channel == NUMER_OF_CHANNELS - 1) Serial.println();
    */

    //Figure out if a channel is truely broken or if we should ignore it
    if (Hits[channel] > MINHITS & Hits[channel] < MAXHITS)
    {
      if (channel == 0 || channel == 1) {
        //Serial.println("Blower Break!");
        blowerBeamBroken = true;
      }
      if (channel == 2 || channel == 3)
      {
        //Serial.println("Hopper Break!");
        strobeBeamBroken = true;
      }
    }
  }

  //wdt_reset(); //Pet the dog
}
