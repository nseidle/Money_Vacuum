void mainMenu()
{
  byte incoming = Serial.read();

  if (incoming == 't')
  {
    Serial.println("Test IR");
    //This is a test routine that displays what the IRs are detecting
    //Do not call in production - it is an infinite loop
    test_IRs();
  }
  else if (incoming == 'l')
  {
    Serial.println("Turn on IR LEDs");

    //Turn on all the IR LEDs so that we can see what's going on from a smart phone
    demoIRLEDs();
  }
  else if (incoming == 'd')
  {
    Serial.println("Toggle I/O - press key to end");

    while(Serial.available()) Serial.read();
    
    while(1)
    {
      //Toggle pins so we can see them on DMM
      
      digitalWrite(irEmitter0, HIGH);
      digitalWrite(irEmitter1, LOW);
      digitalWrite(irDetector0, HIGH);
      digitalWrite(irDetector1, LOW);
      delay(3000);

      digitalWrite(irEmitter0, LOW);
      digitalWrite(irEmitter1, HIGH);
      digitalWrite(irDetector0, LOW);
      digitalWrite(irDetector1, HIGH);
      delay(3000);

      if(Serial.available()) break;
    }
    demoIRLEDs();
  }
  else if (incoming == '\r' || incoming == '\n')
  {
    //Do nothing
  }
  else
  {
    Serial.print("Unknown: ");
    Serial.write(incoming);
    Serial.println();
    printMenu();
  }

}

void printMenu()
{
  Serial.println("t) Test IR LEDs");
  Serial.println("l) Light up IR LEDs");
  Serial.println("d) Toggle I/O");
}
