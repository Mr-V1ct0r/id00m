//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function reads and scales all of the inputs.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void readInputs() {

  readGateInputs();  //grab the gate status of all gate inputs.

  readAnalogInputs();  //grab the value of all analog CV inputs.

  scaleCVInputs();  //scale the analog CV inputs.

  handleFreezeParam();  //record the parameters only once per modification if we are using the freezeParam secret option
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function reads all of the gate inputs.  Note that the value of all inputs are logically inverted because there is an inverting buffer
//in the hardware.  We also use the "digitalReadFast" function in the "fastdigitalio" library to speed up the code wherever we can afford it.

void readGateInputs() {

  clockIn = !digitalReadFast(clockPin);
  trig1In = !digitalReadFast(trig1Pin);
  trig2In = !digitalReadFast(trig2Pin);
  trig3In = !digitalReadFast(trig3Pin);
  trig4In = !digitalReadFast(trig4Pin);

  cycleIn = !digitalReadFast(cycleSw);
  loopGateIn = !digitalReadFast(loopGate);
  loopButtonIn = !digitalReadFast(loopButton);

  modeBtnIn = !digitalReadFast(modeBtn);
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function reads all of the analog inputs from the analog to digital converters.  The value is subtracted from 1024 because again the value
//is inverted in the hardware.

void readAnalogInputs() {

  modeRead = 1024 - analogRead(modePin);
  probRead = 1024 - analogRead(probPin);
  lengRead = 1024 - analogRead(lengPin);
  paramRead = 1024 - analogRead(paramPin);

  //Copy to another variable for legibility.  We used to do more here with the freezeParam function but now we have a separate function for that.
  modifyParam = paramRead;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function does quite a lot to smooth out the annoying curve of the analog inputs.  There are a lot of discrete variable on IDUM (i.e. they
//only have 8 possible values and can't do values in between) that need "hysteresis" added to prevent them from bouncing between values when the
//voltage that controls them is close to a boundary between two values.  This also handles the scaling using the "resolution" menu options.

void scaleCVInputs() {

  //scale length slider to stupid analog curve.  Adds hysteresis to stop jittering
  if (lengRead <= (50 + ((lengthPosition == 0) * 10) + ((lengthPosition == 1) * -2))) lengthPosition = 0;
  else if ((lengRead > (50 + ((lengthPosition == 0) * 10) + ((lengthPosition == 1) * -2))) && (lengRead <= (120 + ((lengthPosition == 1) * 10) + ((lengthPosition == 2) * -10)))) lengthPosition = 1;
  else if ((lengRead > (120 + ((lengthPosition == 1) * 10) + ((lengthPosition == 2) * -10))) && (lengRead <= (300 + ((lengthPosition == 2) * 10) + ((lengthPosition == 3) * -10)))) lengthPosition = 2;
  else if ((lengRead > (300 + ((lengthPosition == 2) * 10) + ((lengthPosition == 3) * -10))) && (lengRead <= (520 + ((lengthPosition == 3) * 10) + ((lengthPosition == 4) * -10)))) lengthPosition = 3;
  else if ((lengRead > (520 + ((lengthPosition == 3) * 10) + ((lengthPosition == 4) * -10))) && (lengRead <= (680 + ((lengthPosition == 4) * 10) + ((lengthPosition == 5) * -10)))) lengthPosition = 4;
  else if ((lengRead > (680 + ((lengthPosition == 4) * 10) + ((lengthPosition == 5) * -10))) && (lengRead <= (880 + ((lengthPosition == 5) * 10) + ((lengthPosition == 6) * -10)))) lengthPosition = 5;
  else if ((lengRead > (880 + ((lengthPosition == 5) * 10) + ((lengthPosition == 6) * -10))) && (lengRead <= (1000 + ((lengthPosition == 6) * 10) + ((lengthPosition == 7) * -10)))) lengthPosition = 6;
  else if (lengRead > (1000 + ((lengthPosition == 6) * 10) + ((lengthPosition == 7) * -10))) lengthPosition = 7;

  //scale length using length resolution
  switch (lengthResolution) {  //use the length resolution option to determine the actual loop length
    case 0:
      lengthScaled = lengthPosition + 1;
      break;

    case 1:
      lengthScaled = even[lengthPosition + 8];  //these arrays have been updated to be mirrored around 1 in the center
      break;

    case 2:
      lengthScaled = powerOf2[lengthPosition + 8];
  }

  //scale paramRead for modes that have integer params.  Adds hysteresis to stop jittering
  if (modifyParam <= (64 + ((unscaledParam == 0) * 20) + ((unscaledParam == 1) * -20))) unscaledParam = 0;
  else if ((modifyParam > (64 + ((unscaledParam == 0) * 20) + ((unscaledParam == 1) * -20))) && (modifyParam <= (128 + ((unscaledParam == 1) * 20) + ((unscaledParam == 2) * -20)))) unscaledParam = 1;
  else if ((modifyParam > (128 + ((unscaledParam == 1) * 20) + ((unscaledParam == 2) * -20))) && (modifyParam <= (192 + ((unscaledParam == 2) * 20) + ((unscaledParam == 3) * -20)))) unscaledParam = 2;
  else if ((modifyParam > (192 + ((unscaledParam == 2) * 20) + ((unscaledParam == 3) * -20))) && (modifyParam <= (256 + ((unscaledParam == 3) * 20) + ((unscaledParam == 4) * -20)))) unscaledParam = 3;
  else if ((modifyParam > (256 + ((unscaledParam == 3) * 20) + ((unscaledParam == 4) * -20))) && (modifyParam <= (320 + ((unscaledParam == 4) * 20) + ((unscaledParam == 5) * -20)))) unscaledParam = 4;
  else if ((modifyParam > (320 + ((unscaledParam == 4) * 20) + ((unscaledParam == 5) * -20))) && (modifyParam <= (384 + ((unscaledParam == 5) * 20) + ((unscaledParam == 6) * -20)))) unscaledParam = 5;
  else if ((modifyParam > (384 + ((unscaledParam == 5) * 20) + ((unscaledParam == 6) * -20))) && (modifyParam <= (448 + ((unscaledParam == 6) * 20) + ((unscaledParam == 7) * -20)))) unscaledParam = 6;
  else if ((modifyParam > (448 + ((unscaledParam == 6) * 20) + ((unscaledParam == 7) * -20))) && (modifyParam <= (512 + ((unscaledParam == 7) * 20) + ((unscaledParam == 8) * -20)))) unscaledParam = 7;
  else if ((modifyParam > (512 + ((unscaledParam == 7) * 20) + ((unscaledParam == 8) * -20))) && (modifyParam <= (576 + ((unscaledParam == 8) * 20) + ((unscaledParam == 9) * -20)))) unscaledParam = 8;
  else if ((modifyParam > (576 + ((unscaledParam == 8) * 20) + ((unscaledParam == 9) * -20))) && (modifyParam <= (640 + ((unscaledParam == 9) * 20) + ((unscaledParam == 10) * -20)))) unscaledParam = 9;
  else if ((modifyParam > (640 + ((unscaledParam == 9) * 20) + ((unscaledParam == 10) * -20))) && (modifyParam <= (704 + ((unscaledParam == 10) * 20) + ((unscaledParam == 11) * -20)))) unscaledParam = 10;
  else if ((modifyParam > (704 + ((unscaledParam == 10) * 20) + ((unscaledParam == 11) * -20))) && (modifyParam <= (768 + ((unscaledParam == 11) * 20) + ((unscaledParam == 12) * -20)))) unscaledParam = 11;
  else if ((modifyParam > (768 + ((unscaledParam == 11) * 20) + ((unscaledParam == 12) * -20))) && (modifyParam <= (832 + ((unscaledParam == 12) * 20) + ((unscaledParam == 13) * -20)))) unscaledParam = 12;
  else if ((modifyParam > (832 + ((unscaledParam == 12) * 20) + ((unscaledParam == 13) * -20))) && (modifyParam <= (896 + ((unscaledParam == 13) * 20) + ((unscaledParam == 14) * -20)))) unscaledParam = 13;
  else if ((modifyParam > (896 + ((unscaledParam == 13) * 20) + ((unscaledParam == 14) * -20))) && (modifyParam <= (960 + ((unscaledParam == 14) * 20) + ((unscaledParam == 15) * -20)))) unscaledParam = 14;
  else if (modifyParam > (960 + ((unscaledParam == 14) * 20) + ((unscaledParam == 15) * -20))) unscaledParam = 15;

  //scale param knob using param resolution (for modes that ratchet)
  switch (paramResolution) {

    //We added an "odd" array to make each mode treat the variables the same, helped clean up some awkward code.
    case 0:
      ratchetAmount = odd[unscaledParam];
      break;
    case 1:
      ratchetAmount = even[unscaledParam];
      break;
    case 2:
      ratchetAmount = powerOf2[unscaledParam];
      break;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function copies the scaled param variables into each channel's modification variables depending on it's modification status.
//If freeze param is off it just copies the current param variables into each channel's individual variables all the time.

void handleFreezeParam() {


  //If the freezeParam flag is off we just continuously copy the main parameter variables into the channel specific variables
  if (!freezeParam && !loopEnable) {
    modifyParam1 = modifyParam2 = modifyParam3 = modifyParam4 = modifyParam;
    ratchetAmount1 = ratchetAmount2 = ratchetAmount3 = ratchetAmount4 = ratchetAmount;
    unscaledParam1 = unscaledParam2 = unscaledParam3 = unscaledParam4 = unscaledParam;
  }

  //otherwise we only copy the main parameter readings into the individual parameter variables when there is no modification active.
  else {
    if (modifyLength1 == 0) {
      modifyParam1 = modifyParam;
      ratchetAmount1 = ratchetAmount;
      unscaledParam1 = unscaledParam;
    }
    if (modifyLength2 == 0) {
      modifyParam2 = modifyParam;
      ratchetAmount2 = ratchetAmount;
      unscaledParam2 = unscaledParam;
    }
    if (modifyLength3 == 0) {
      modifyParam3 = modifyParam;
      ratchetAmount3 = ratchetAmount;
      unscaledParam3 = unscaledParam;
    }
    if (modifyLength4 == 0) {
      modifyParam4 = modifyParam;
      ratchetAmount4 = ratchetAmount;
      unscaledParam4 = unscaledParam;
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
