//Plays a random track
//Then turns off audio to reduce hissing
void playRandomTrack()
{
  //Used for sound effects
  char track_name[13];
  byte trackNumber = previousTrack1;

  while (trackNumber == previousTrack1 || trackNumber == previousTrack2) //Don't play the same track as the last donation
  {
    //wdt_reset(); //Pet the dog
    trackNumber = random(1, 6); //(inclusive min, exclusive max)
  }

  sprintf(track_name, "TRACK%03d.WAV", trackNumber); //Splice the track number into file name

  //Serial.print("Playing: ");
  //Serial.println(track_name);

  if (MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

  //wdt_reset(); //Pet the dog
  MP3player.playMP3(track_name);
  //wdt_reset(); //Pet the dog

  //Update the previous variables
  previousTrack2 = previousTrack1;
  previousTrack1 = trackNumber;

  playerStopped = false; //Boolean for main loop to turn off MP3 IC
}

//Plays a random track
//Then turns off audio to reduce hissing
void playSilence()
{
  if (MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

  MP3player.playMP3("SILENCE.MP3");

  Serial.println("Playing silence");
}

// initSD() initializes the SD card and checks for an error.
void initSD()
{
  //Initialize the SdCard.
  if (!sd.begin(9, SPI_HALF_SPEED))
  {
    Serial.println("SD begin error");
    sd.initErrorHalt();
  }
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
  if (result != 0) // check result, see readme for error codes.
  {
    if (result != 6) //6 is firmware patch, not real issue
      Serial.println("MP3 begin error");
  }
  else
    Serial.println("MP3 online");

  //This was the original volume. Too low for a busy day. It is hard to say
  //what level the amp is set to but we can tweak this up in software.
  //MP3player.setVolume(5, 5); // MP3 Player volume 0=max, 255=lowest (off)

  //New volume level 8/18/2014
  //MP3player.setVolume(0, 0); // MP3 Player volume 0=max, 255=lowest (off)

  //New volume level 1/12/2015 - I don't believe this does anything. Adjusting knob on amp
  //MP3player.setVolume(250, 250); // MP3 Player volume 0=max, 255=lowest (off)

  //Causes new MP3 library to not play tracks
  //MP3player.setMonoMode(1); // Mono setting: 0=off, 1 = on, 3=max
  
}
