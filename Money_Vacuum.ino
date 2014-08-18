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
 
 ToDo: Add Watchdog to main code.
 
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

//Include various libraries
//#include <avr/wdt.h> //We need watch dog for this program
#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object

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

  Serial.begin(9600);
  //Serial.println("Money Vacuum"); 

  pinMode(A0, INPUT); //Just for a second so we can see the random generator
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

  //Turn on all the IR LEDs so that we can see what's going on from a smart phone
  //demoIRLEDs();

  //This is a test routine that displays what the IRs are detecting
  //Do not call in production - it is an infinite loop
  //test_IRs();

  //Prefill the hits array - this was needed during testing
  for(int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++)
    Hits[channel] = MAXHITS;

  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield

  playRandomTrack();

  digitalWrite(overRideLED, LOW); //Turn LED on

  //Let the world know we are online
  reportStatus(MONEY_ONLINE); //Report this interaction to the imp
  //wdt_enable(WDTO_1S); //Unleash the beast
}

void loop() {
  //wdt_reset(); //Pet the dog

  //Stop everything if the override switch is engaged
  if(isOverrideTurnedOn() == true)
  {
    digitalWrite(overRideLED, HIGH); //Turn off LED to indicate system is off
    
    digitalWrite(blower, LOW); //Turn off blower immediately
    digitalWrite(strobe, LOW); //Turn off strobe if it happens to be on
    MP3player.end(); //Turn off any MP3 playing
    playerStopped = true;
    
    while(isOverrideTurnedOn() == true)
    {
      delayDog(1000); //Delay with watchdog in mind
      reportStatus(OVERRIDE_ON); //Report this interaction to the imp
    }
    digitalWrite(overRideLED, LOW); //Turn LED on, System is back on
    reportStatus(OVERRIDE_OFF); //Report this interaction to the imp
  }

  if(playerStopped == true) checkBeams(); //Test the beams only when MP3 is not playing

  if(blowerBeamBroken == true)
  {
    blowerBeamBroken = false;

    blowerStart = millis(); //Record this time. If someone puts in two bills this will cause the system to run longer.

    //Check to see if we are already running
    if(digitalRead(blower) == LOW)
    {
      reportStatus(BLOWER_ON_NEWBILL); //Report this interaction to the imp
      digitalWrite(blower, HIGH); //Start blower
    }
  }

  if(strobeBeamBroken == true)
  {
    strobeBeamBroken = false;

    //Check to see if we are already spinning the strobe
    if(digitalRead(strobe) == LOW)
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

  if(digitalRead(blower) == HIGH) //Check to see if the blower is running
  {
    if(millis() - blowerStart > MAX_BLOW_TIME)
    {
      digitalWrite(blower, LOW); //Turn off blower

      reportStatus(BLOWER_OFF_MAXTIME); //Report this interaction to the imp

      //Check to see if we are already spinning the strobe
      if(digitalRead(strobe) == LOW)
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

  if(digitalRead(strobe) == HIGH) //Check to see if the strobe is blinking
  {
    if(millis() - strobeStart > MAX_STROBE_TIME)
    {
      digitalWrite(strobe, LOW); //Turn off strobe

      reportStatus(STROBE_OFF); //Report this interaction to the imp
    }
  }

  //Turn off MP3 chip when not playing
  if(playerStopped == false)
  {
    //Serial.println("Player is playing");
    if(MP3player.isPlaying() == false)
    {
      MP3player.end(); //MP3 player is buzzing. This disables it
      
      playerStopped = true;

      reportStatus(PLAYER_OFF); //Report this interaction to the imp
    }
  }
}

//Delays a given amount, petting the dog while we delay
void delayDog(int delay_amt)
{
  long startTime = millis();
  while(millis() - startTime < delay_amt) {
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
boolean isOverrideTurnedOn(void)
{
  boolean systemOn = false;
  
  pinMode(overRideSwitch, OUTPUT); //Let's control the pin for a second
  digitalWrite(overRideSwitch, LOW);
  
  if(digitalRead(overRideSwitch) == HIGH)
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
  
  return(systemOn);
}

//Checks the beams for breaking at sets variables if one is broken
void checkBeams()
{
  //wdt_reset(); //Pet the dog
  
  for(int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++) 
  {
    if(test_channel(channel) == 0) //Beam is broken
    {
      Hits[channel]++; //Record this hit
      if(Hits[channel] > MAXHITS) Hits[channel] = MAXHITS;
    }
    else
      Hits[channel] = 0; //Reset this channel

    delay(2); //We have to have some amount of time in between IR flooding/checking, 1 to 5ms works well
  } //End beam checking

  for(int channel = 0 ; channel < NUMBER_OF_CHANNELS ; channel++) 
  {
      /*Serial.print(channel, DEC);
      Serial.print(":");
      Serial.print(Hits[channel]);
      Serial.print(" ");
      
      if(channel == NUMER_OF_CHANNELS - 1) Serial.println();
      */
    
      //Figure out if a channel is truely broken or if we should ignore it
      if(Hits[channel] > MINHITS & Hits[channel] < MAXHITS)
      {
        if(channel == 0 || channel == 1) {
          //Serial.println("Blower Break!");
          blowerBeamBroken = true;
        }
        if(channel == 2 || channel == 3)
        {
          //Serial.println("Hopper Break!");
          strobeBeamBroken = true;
        }
      }
  }

  //wdt_reset(); //Pet the dog
}

//Plays a random track
//Then turns off audio to reduce hissing
void playRandomTrack()
{
  //Used for sound effects
  char track_name[13];
  byte trackNumber = previousTrack1;

  while(trackNumber == previousTrack1 || trackNumber == previousTrack2) //Don't play the same track as the last donation
  {
    //wdt_reset(); //Pet the dog
    trackNumber = random(1, 6); //(inclusive min, exclusive max)
  }

  sprintf(track_name, "TRACK%03d.WAV", trackNumber); //Splice the track number into file name

  Serial.print("Playing: ");
  Serial.println(track_name);

  if(MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

  //wdt_reset(); //Pet the dog
  //Not sure how long these functions take
  MP3player.begin();
  MP3player.playMP3(track_name);
  //wdt_reset(); //Pet the dog

  //Update the previous variables
  previousTrack2 = previousTrack1;
  previousTrack1 = trackNumber;
  
  playerStopped = false; //Boolean for main loop to turn off MP3 IC
}

//Tests a given IR channel to see if it is currently broken or clear
//Returns 1 if channel is clear
//Returns 0 if something is breaking the beam

//http://en.wikipedia.org/wiki/RC-5
//We need on for 8us, off for 20us
//We need 32 of these clicks, total of 889us
//Then a pause of 889us
//This will signify bit '1'

//For the illumitune we use 38kHz IR receivers, not 36kHz. Let's tweak to 38kHz.
//We need on for 8us, off for 20us
int test_channel(int channel_number) {

  //wdt_reset(); //Pet the dog

  for(int x = 0 ; x < TRANSMISSIONS ; x++) {

    switch(channel_number) {
      case(0):
      digitalWrite(irEmitter0, HIGH); 
      break;
      case(1):
      digitalWrite(irEmitter1, HIGH); 
      break;
      case(2):
      digitalWrite(irEmitter2, HIGH); 
      break;
      //case(3):
      //digitalWrite(irEmitter3, HIGH); 
      //break;
    }

    //Delay exactly 7.8us, delayMicrosecond() doesn't work for this short of a delay
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    switch(channel_number) {
      case(0): 
      digitalWrite(irEmitter0, LOW); 
      break;
      case(1): 
      digitalWrite(irEmitter1, LOW); 
      break;
      case(2): 
      digitalWrite(irEmitter2, LOW); 
      break;
      //case(3): 
      //digitalWrite(irEmitter3, LOW); 
      //break;
    }

    //Delay exactly 18.5us, delayMicrosecond() isn't accurate enough
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    __asm__("nop\n\t""nop\n\t""nop\n\t");
  }

  //wdt_reset(); //Pet the dog

  if(channel_number == 0) 
    if(digitalRead(irDetector0) == HIGH) return(0); //Beam is broken!
  if(channel_number == 1) 
    if(digitalRead(irDetector1) == HIGH) return(0); //Beam is broken!
  if(channel_number == 2) 
    if(digitalRead(irDetector2) == HIGH) return(0); //Beam is broken!
  //if(channel_number == 3) 
  //  if(digitalRead(irDetector3) == HIGH) return(0); //Beam is broken!

  return(1); //Beam is intact
}

//This routine helps debug a single channel
//It's an infinite loop. Don't call in production code

void test_IRs(void)
{
  while(1) 
  {
    Serial.print("Beams: ");

    for(int channel_number = 0 ; channel_number < NUMBER_OF_CHANNELS ; channel_number++) {

      Serial.print(channel_number, DEC);
      Serial.print(":");

      if(test_channel(channel_number) == 0) 
        Serial.print("--");
      else
        Serial.print("00");

      Serial.print(" ");
    }
    Serial.println();

    //wdt_reset(); //Pet the dog
    delay(10);
  }
}

void demoIRLEDs(void) {
  #define ON HIGH
  #define OFF LOW

  Serial.println("IRs are demoing - break out your smart phone to see");

  //Turn on all LEDs
  digitalWrite(irEmitter0, ON);
  digitalWrite(irEmitter1, ON);
  digitalWrite(irEmitter2, ON);
  //digitalWrite(irEmitter3, ON);

  delayDog(1000);

  //Turn off all LEDs
  digitalWrite(irEmitter0, OFF);
  digitalWrite(irEmitter1, OFF);
  digitalWrite(irEmitter2, OFF);
  //digitalWrite(irEmitter3, OFF);
}

// initSD() initializes the SD card and checks for an error.
void initSD()
{
  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
    sd.initErrorHalt();
  //if(!sd.chdir("/")) 
  //  sd.errorHalt("sd.chdir");
}

// initMP3Player() sets up all of the initialization for the
// MP3 Player Shield. It runs the begin() function, checks
// for errors, applies a patch if found, and sets the volume/
// stero mode.
void initMP3Player()
{
  uint8_t result = MP3player.begin(); // init the mp3 player shield
  if(result != 0) // check result, see readme for error codes.
  {
    // Error checking can go here!
  }

  MP3player.setVolume(5, 5); // MP3 Player volume 0=max, 255=lowest (off)
  MP3player.setMonoMode(1); // Mono setting: 0=off, 1 = on, 3=max
}
