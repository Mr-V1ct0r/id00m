//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function runs once at startup and then the top level of code transitions to the "Loop" function in the next page
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setup() {

  assignPins();  //Set input and output status of pins.

  displayFirmwareVersion(2);  //This displays on the mode ring which firmware is loaded on your IDUM.  Increase the number for each update.

  if (printEnable) Serial.begin(9600);  //only setup the serial communication if printing is enabled.  Causes the MCU to work way slower.

  initializeEEPROM();  //check and read saved variables from before the last power down.

  initializeIntervals();  //Sets up some variables used in the "Boundary Utils" page of functions.

  initializeHardwareInterrupts();

  delay(1000);  //Delay the start of the program so that we can display the firmware number for a little bit longer.

  dTimer.every(1, displayTimer);  //Start up the display counter, runs every '1' microsecond.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Sets the input or output status of all of the Arduino Pins.  Pin numbers are refered to by names assigned in the "variables" page.

void assignPins() {

  pinMode(loopLED, OUTPUT);
  pinMode(loopButton, INPUT);

  pinMode(mode1LED, OUTPUT);
  pinMode(mode2LED, OUTPUT);
  pinMode(mode3LED, OUTPUT);

  pinMode(clockPin, INPUT);
  pinMode(trig1Pin, INPUT);
  pinMode(trig2Pin, INPUT);
  pinMode(trig3Pin, INPUT);
  pinMode(trig4Pin, INPUT);

  pinMode(clockOut, OUTPUT);
  pinMode(trig1Out, OUTPUT);
  pinMode(trig2Out, OUTPUT);
  pinMode(trig3Out, OUTPUT);
  pinMode(trig4Out, OUTPUT);

  pinMode(loopGate, INPUT);
  pinMode(cycleSw, INPUT);
  pinMode(modeBtn, INPUT_PULLUP);

  pinMode(loopLED, OUTPUT);

  pinMode(modePin, INPUT);
  pinMode(probPin, INPUT);
  pinMode(lengPin, INPUT);
  pinMode(paramPin, INPUT);
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Set the display multiplexer bits to display the current firmware number.
//LEDs are not in ascending order so the bits are kind of strange for each LED.

void displayFirmwareVersion(unsigned int firmwareNumber) {

  switch (firmwareNumber) {

    case 0:
      digitalWriteFast(mode1LED, HIGH);
      digitalWriteFast(mode2LED, LOW);
      digitalWriteFast(mode3LED, LOW);
      break;
    case 1:
      digitalWriteFast(mode1LED, LOW);
      digitalWriteFast(mode2LED, HIGH);
      digitalWriteFast(mode3LED, LOW);
      break;
    case 2:
      digitalWriteFast(mode1LED, LOW);
      digitalWriteFast(mode2LED, LOW);
      digitalWriteFast(mode3LED, HIGH);
      break;
    case 3:
      digitalWriteFast(mode1LED, LOW);
      digitalWriteFast(mode2LED, HIGH);
      digitalWriteFast(mode3LED, HIGH);
      break;
    case 4:
      digitalWriteFast(mode1LED, HIGH);
      digitalWriteFast(mode2LED, HIGH);
      digitalWriteFast(mode3LED, HIGH);
      break;
    case 5:
      digitalWriteFast(mode1LED, HIGH);
      digitalWriteFast(mode2LED, LOW);
      digitalWriteFast(mode3LED, HIGH);
      break;
    case 6:
      digitalWriteFast(mode1LED, HIGH);
      digitalWriteFast(mode2LED, HIGH);
      digitalWriteFast(mode3LED, LOW);
      break;
    case 7:
      digitalWriteFast(mode1LED, LOW);
      digitalWriteFast(mode2LED, LOW);
      digitalWriteFast(mode3LED, LOW);
      break;
    default: break;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Reads the options, active modes, and any other variables that have been saved from before the most recent power down.

void initializeEEPROM() {

  byte modeByte = 0;    //This is used to save all of the active modes into one byte instead of as separate variables.
  byte optionByte = 0;  //This is used to save all of the active options into one byte instead of as separate variables.

  char eepromInitialized = EEPROM.read(0);  //We save a letter into the first memory location on the EEPROM to show that it has been initialiazed.

  if (eepromInitialized == 'Q') {  //Make sure that the right letter is saved (this might change between firmwares).
    optionByte = EEPROM.read(1);
    modeByte = EEPROM.read(2);
    paramResolution = EEPROM.read(3);
    lengthResolution = EEPROM.read(4);

    setModeArray(modeByte);  //Take each bit of the mode byte and spit it into separate locations in the mode Array.
    setOptions(optionByte);  //Take each bit of the option byte and split it into separate variables.

    if (printEnable) Serial.println("test passed");  //Lets us know that the EEPROM had been saved to before in the appropriate format.
  }

  else {                                                                    //If our test of the EEPROM having the right letter saved fails we write some default values in.
    EEPROM.write(0, 'Q');                                                   //verifies that the eeprom has been initialized, if not we write everything for the first time.
    EEPROM.write(1, 0);                                                     //saves all '0's so all options are off
    EEPROM.write(2, 255);                                                   //saves all '1's so all modes are active
    EEPROM.write(3, 0);                                                     //sets the parameter resolution to 'odd'
    EEPROM.write(4, 0);                                                     //sets the length resolution to 'odd'
    if (printEnable) Serial.println("Test failed.  EEPROM Reinitialized");  //Lets us know that the EEPROM had to be reset.
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void initializeHardwareInterrupts() {

  attachInterrupt(trig1Pin, trig1InterruptHandler, RISING);
  attachInterrupt(trig2Pin, trig2InterruptHandler, RISING);
  attachInterrupt(trig3Pin, trig3InterruptHandler, RISING);
  attachInterrupt(trig4Pin, trig4InterruptHandler, RISING);
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
