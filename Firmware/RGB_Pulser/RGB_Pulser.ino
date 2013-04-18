/*
 11-12-2012
 Spark Fun Electronics
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Tri-Color Breakout is a board with a very high power red, green, and blue LED, controlled via transistors.
 We use the PWM outputs of an Arduino to mix these bright lights. I recommend you cover up the RGB LEDs with
 some sort of light diffuser. Using a white sheet of paper will save you from a migraine. Or if you really
 want to get crazy: http://www.google.com/#hl=en&q=white+plastic+acrylic+globe+lamp
  
 To get this code to work, attached an Tri-Color breakout to an Arduino Uno using the following pins:
 Pin 9 on Uno to RED on Tri-color Breakout
 Pin 10 on Uno to GRN on Tri-color Breakout
 Pin 11 on Uno to BLU on Tri-color Breakout
 VIN to PWR
 GND to GND
 
 Note: This code will output debug statements at 9600bps.
 
 This is closely related to the RGB circuit of the SparkFun Inventor's Kit circuit.
*/

//LED leads connected to PWM pins
const int RED_LED_PIN = 9;
const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;

//Length of time we spend showing each color
const int DISPLAYTIME = 500; // In milliseconds

//How quickly do we want to get to the next color?
const int STEPS = 200; //Amount of grace we take from getting from old to new. 200 works well.
const int STEPVALUE = 5; //How quickly we takes those steps. 1-10 works well.

long newColor;
long oldColor;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop()
{
  oldColor = newColor; //Remember the old color so that we can slowly mix from old to new

  newColor = random(0xFFFFFF); //Pick a random number between 0 and 16 million
  //8 bits are for red, 8 for green, 8 for blue

  //Push more values to off, 25% chance
  if(random(100) < 25) newColor = newColor & 0xFFFF00; //Turn blue off
  if(random(100) < 25) newColor = newColor & 0xFF00FF; //Turn green off
  if(random(100) < 25) newColor = newColor & 0x00FFFF; //Turn red off

  //Now that we have a new color, let's go to it!
  setColor(oldColor, newColor); //Move slowly from old color to new color

  Serial.print("Old:");
  Serial.print(oldColor, HEX);
  Serial.print(" New:");
  Serial.println(newColor, HEX);

  delay(DISPLAYTIME); //Wait an amount of time between color changes
}

//This function slowly changes the outputs from the previous color to the new color
//The two inputs are 32-bits. 8 bits are red, 8 bits green, 8 bits blue. 8 bits are unused.
//This allows us to drift between colors without the jarring color changes
void setColor(unsigned long oldClr, unsigned long newClr)
{
  /* 
   0x specifies the value is a hexadecimal number
   RR is a byte specifying the red intensity
   GG is a byte specifying the green intensity
   BB is a byte specifying the blue intensity
   */

  // Extract the intensity values from the hex triplet
  float oldRed = (oldClr >> 16) & 0xFF;
  float oldGreen = (oldClr >> 8) & 0xFF;
  float oldBlue = oldClr & 0xFF;

  byte newRed = (newClr >> 16) & 0xFF;
  byte newGreen = (newClr >> 8) & 0xFF;
  byte newBlue = newClr & 0xFF;

  //Find the difference between the old values and the new values
  int redDiff = abs(oldRed - newRed);
  int greenDiff = abs(oldGreen - newGreen);
  int blueDiff = abs(oldBlue - newBlue);

  //Figure out the steps we need to take to get to the new values
  float redStep = (float)redDiff / (STEPS/STEPVALUE); //50 / (STEPS/5) = 0;
  float greenStep = (float)greenDiff / (STEPS/STEPVALUE);
  float blueStep = (float)blueDiff / (STEPS/STEPVALUE);
  
  //Take care of a fringe case
  if (redStep == 0) redStep = 1;
  if (greenStep == 0) greenStep = 1;
  if (blueStep == 0) blueStep = 1;

  //Move Intensity1 towards Intensity2
  for(int fadeSteps = 0 ; fadeSteps <= STEPS ; fadeSteps += STEPVALUE)
  {
    //if less, then add
    //if more, then subtract
    //if equal, do nothing

    if(oldRed == newRed) ; //Do nothing
    else if(oldRed < newRed) oldRed += redStep;
    else if(oldRed > newRed)
    {
      if(oldRed > redStep) oldRed -= redStep;
      else if(oldRed > 0) oldRed--;
    }

    if(oldGreen == newGreen) ; //Do nothing
    else if(oldGreen < newGreen) oldGreen += greenStep;
    else if(oldGreen > newGreen)
    {
      if(oldGreen > greenStep) oldGreen -= greenStep;
      else if(oldGreen > 0) oldGreen--;
    }

    if(oldBlue == newBlue) ; //Do nothing
    else if(oldBlue < newBlue) oldBlue += blueStep;
    else if(oldBlue > newBlue)
    {
      if(oldBlue > blueStep) oldBlue -= blueStep;
      else if(oldBlue > 0) oldBlue--;
    }

    //Some interesting case corrections
    if(oldRed > 255) oldRed = 255;
    if(oldBlue > 255) oldBlue = 255;
    if(oldGreen > 255) oldGreen = 255;
    if(oldRed < 0) oldRed = 0;
    if(oldBlue < 0) oldBlue = 0;
    if(oldGreen < 0) oldGreen = 0;

    analogWrite(RED_LED_PIN, oldRed);
    analogWrite(GREEN_LED_PIN, oldGreen);
    analogWrite(BLUE_LED_PIN, oldBlue); 

    /*Serial.print("redDiff:");
     Serial.print(blueDiff, DEC);
     Serial.print(" redStep:");
     Serial.print(blueStep, 2);
     Serial.print(" oldRed:");
     Serial.print(oldBlue, 2);
     Serial.print(" newRed:");
     Serial.println(newBlue, DEC);*/

    delay(45); //45 miliseconds between steps
  }
}
