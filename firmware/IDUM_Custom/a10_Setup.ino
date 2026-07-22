//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function runs once at startup and then the top level of code transitions to the "Loop" function in the next page
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setup() {

  assignPins();  //Set input and output status of pins.

  randomSeed(gatherEntropy());  //Give the RNG a non-default starting point immediately; the strong seed comes at the first clock (see loop).

  //FACTORY RESET: hold the REMOVE MODE button while powering on to wipe all saved settings back to defaults.  The button is active-low,
  //so a held button reads 0 -> !0 = true.  We re-check after a short delay so a power-on glitch can't wipe the settings by accident,
  //then write the defaults BEFORE initializeEEPROM loads them.  Impossible to trigger in normal use (you have to power up holding it).
  bool factoryResetHeld = !digitalReadFast(modeBtn);
  if (factoryResetHeld) { delay(20); factoryResetHeld = !digitalReadFast(modeBtn); }  //confirm it is a real, steady hold
  if (factoryResetHeld) writeEEPROMDefaults();

  displayFirmwareVersion(0);  //CUSTOM (00 Modular): initial boot LED (first ring LED); the two-LED "00" signature (first+last together) is shown during the hold below via displayCustomFirmwareID().

  if (printEnable) Serial.begin(9600);  //only setup the serial communication if printing is enabled.  Causes the MCU to work way slower.

  initializeEEPROM();  //check and read saved variables from before the last power down (loads the freshly-written defaults if we just reset).

  //Confirm a reset with a ring-sweep animation that runs until the button is released.  Waiting for release also consumes the hold so
  //it can't fire a spurious mode toggle when the main loop starts.
  if (factoryResetHeld) {
    unsigned int sweep = 0;
    while (!digitalReadFast(modeBtn)) {  //while REMOVE MODE is still held down
      displayFirmwareVersion(sweep);      //light the next LED around the ring
      digitalWriteFast(loopLED, HIGH);
      sweep = (sweep + 1) % 8;
      delay(40);
    }
    digitalWriteFast(loopLED, LOW);
    displayFirmwareVersion(0);            //restore the boot indicator (the two-LED "00" signature is shown during the hold below)
  }

  initializeIntervals();  //Sets up some variables used in the "Boundary Utils" page of functions.

  initializeHardwareInterrupts();

  displayCustomFirmwareID(1000);  //hold the 00 Modular two-LED signature (first + last ring LEDs together) for ~1s so it's visible at boot before the display timer takes over.

  flashVersion();  //CUSTOM (00 Modular): hold the release-index color dot after the 00 signature (see FW_RELEASE)

  dTimer.every(1, displayTimer);  //Start up the display counter, runs every '1' microsecond.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Collect a hard-to-predict number to seed the random generator.  IDUM has no truly floating pins (every ADC input sits on a knob or
//CV jack) so we can't rely on classic analog noise alone.  Instead we fold the noisy least-significant bit of many ADC reads together
//with the free-running microsecond timer.  Only called from setup(), where spending a couple of milliseconds is free.

unsigned long gatherEntropy() {

  unsigned long e = micros();
  const uint8_t entropyPins[4] = { modePin, probPin, lengPin, paramPin };

  for (uint8_t i = 0; i < 16; i++) {
    e = (e << 1) | (analogRead(entropyPins[i & 3]) & 1);  //shift in one noisy least-significant bit per read
    e ^= micros();                                        //and keep stirring in the timer between reads
  }

  return e;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//The single best source of entropy this hardware has is WHEN the first external clock arrives after power up - that timing is decided
//by the outside world and is effectively unpredictable.  No random() is ever drawn before the first clock edge, so re-seeding here
//means the very first probability roll already benefits from it.  Runs exactly once.  Called from loop() right after edgeDetect().

void seedRNGOnFirstClock() {

  if (!rngSeeded && clockEdge) {
    randomSeed(micros() ^ ((unsigned long)analogRead(paramPin) << 6) ^ analogRead(probPin));
    rngSeeded = 1;
  }
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

//CUSTOM (00 Modular): show the firmware signature as the FIRST and LAST ring LEDs lit TOGETHER - a two-symbol "00" that reads as
//this fork, distinct from any single-LED upstream/stock version (stock lights one LED, and Eli's natural next bump would just move
//that single LED up the ring).  The CD4051 ring mux only drives one LED at a time, so we alternate the two addresses fast enough
//that persistence of vision fuses them.  Blocks for durationMs - only called from setup(), before the display timer starts.

void displayCustomFirmwareID(unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    displayFirmwareVersion(0);  //first ring LED
    delayMicroseconds(1200);
    displayFirmwareVersion(7);  //last ring LED
    delayMicroseconds(1200);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (00 Modular): on-module version readout.  After the "00" signature, one ring LED is held for ~1s; its POSITION - and therefore its fixed
//color - is the RELEASE INDEX, a global counter that ticks up by one on every public release (patch, minor OR major - a minor bump is just the
//next dot).  The dot answers "which release is this"; CHANGELOG.md maps the index to the semver string (1 -> v1.0.1, 2 -> next, ...).  v1.0.0
//predates this indicator and shows no dot.  Positions 1-8 cover releases 1-8; past 8 we add a loop-LED second digit rather than move the dot.
//The ring can never go dark (3 address pins -> exactly one of 8 LEDs always lit, no blank), so a short loop-LED tick - the loop LED is an
//independent GPIO - separates the version dot from the 00 flash.  Blocks ~1.3s; only called from setup(), before the display timer starts.
#define FW_RELEASE 1   //firmware v1.0.1 - BUMP BY ONE each public release and add the mapping row in CHANGELOG.md

void flashVersion() {
  digitalWriteFast(loopLED, HIGH);  //separator tick so the version dot reads separately from the 00 signature
  delay(120);
  digitalWriteFast(loopLED, LOW);
  delay(220);
  unsigned int pos = (FW_RELEASE >= 1 && FW_RELEASE <= 8) ? (FW_RELEASE - 1) : 0;  //release index -> ring position (displayFirmwareVersion(0) = first LED)
  displayFirmwareVersion(pos);      //hold the release-index dot; its fixed color = the release
  delay(1000);
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

    setModeArray(modeByte);            //Take each bit of the mode byte and spit it into separate locations in the mode Array.
    setOptions(optionByte);            //Take each bit of the option byte and split it into separate variables.
    setPriorityArray(EEPROM.read(5));  //CUSTOM: load the linear-drumming priority order (validates + falls back to default if unset).
    setBreakSettings(EEPROM.read(6));  //CUSTOM F13: load the active break pattern bank (clamps to a valid bank on old/first-run EEPROM).
    setF9Settings(EEPROM.read(7), EEPROM.read(8));  //CUSTOM F9: load per-channel clock multipliers (falls back to all-x1 on old/first-run EEPROM).

    if (printEnable) Serial.println("test passed");  //Lets us know that the EEPROM had been saved to before in the appropriate format.
  }

  else {                                                                    //If our test of the EEPROM having the right letter saved fails we write some default values in.
    writeEEPROMDefaults();                                                  //write the factory defaults for the first time
    if (printEnable) Serial.println("Test failed.  EEPROM Reinitialized");  //Lets us know that the EEPROM had to be reset.
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Write the factory-default settings to the EEPROM: all 8 modes active, both resolutions = odd, all options off.  Shared by the
//first-run initialiser above and the power-on factory reset (hold REMOVE MODE while powering up).

void writeEEPROMDefaults() {
  EEPROM.write(0, 'Q');   //magic byte marking the EEPROM as initialised in this format
  EEPROM.write(1, 0);     //option byte: all options off (split off, slow-clock off, loop-input-behaviour off, linear drumming off)
  EEPROM.write(2, 255);   //mode byte: all 8 modes active
  EEPROM.write(3, 0);     //parameter resolution = odd
  EEPROM.write(4, 0);     //length resolution = odd
  EEPROM.write(5, 228);   //priority byte: default TR1 > TR2 > TR3 > TR4 (packed {1,2,3,4})
  EEPROM.write(6, 0);     //CUSTOM F13: break settings byte -> bank 0 (default drum kit), speed reserved bits clear
  EEPROM.write(7, 0x33);  //CUSTOM F9: per-channel clock multipliers ch1|ch2 = x1|x1 (value 3 in each nibble)
  EEPROM.write(8, 0x33);  //CUSTOM F9: per-channel clock multipliers ch3|ch4 = x1|x1
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void initializeHardwareInterrupts() {

  attachInterrupt(trig1Pin, trig1InterruptHandler, RISING);
  attachInterrupt(trig2Pin, trig2InterruptHandler, RISING);
  attachInterrupt(trig3Pin, trig3InterruptHandler, RISING);
  attachInterrupt(trig4Pin, trig4InterruptHandler, RISING);
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
