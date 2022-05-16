//This routine helps debug a single channel
void test_IRs(void)
{
  delay(50); //Wait for any extraneous chars
  if(Serial.available()) Serial.read(); //Flush all incoming
  
  while (true)
  {
    Serial.print("Beams: ");

    for (int channel_number = 0 ; channel_number < NUMBER_OF_CHANNELS ; channel_number++) {

      Serial.print(channel_number, DEC);
      Serial.print(":");

      if (test_channel(channel_number) == 0)
        Serial.print("--");
      else
        Serial.print("00");

      Serial.print(" ");
    }
    Serial.println();

    delayDog(10);

    if(Serial.available()) break;
  }
}

void demoIRLEDs(void)
{
#define ON HIGH
#define OFF LOW

  Serial.println("IRs are demoing - break out your smart phone to see");

  //Turn on all LEDs
  digitalWrite(irEmitter0, ON);
  digitalWrite(irEmitter1, ON);
  digitalWrite(irEmitter2, ON);
  //digitalWrite(irEmitter3, ON);

  Serial.println("Press a key to exit");

  while (Serial.available()) Serial.read();

  while (Serial.available() == 0) delayDog(50);

  //Turn off all LEDs
  digitalWrite(irEmitter0, OFF);
  digitalWrite(irEmitter1, OFF);
  digitalWrite(irEmitter2, OFF);
  //digitalWrite(irEmitter3, OFF);
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
