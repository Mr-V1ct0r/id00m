//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// RED LED mode that ratchets each trigger out with a speed determined by the clock input
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void burst() {

  unsigned long currentMicros = micros();  //only sample the system time once

  //microsecond timing of last clock interval multiplied by param knob position
  unsigned long scaledClockInterval1 = 0;
  unsigned long scaledClockInterval2 = 0;
  unsigned long scaledClockInterval3 = 0;
  unsigned long scaledClockInterval4 = 0;

  // set the clock to make a trigger once at the start of the modification
  if ((modifyLength > 0) && (mode == 1)) clockState = ((currentMicros - firstStepTime) < 10000);

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength1 > 0) && (mode1 == 1)) {

    //We are doing this function a bit differently to make the parameter resolution code a bit simpler

    //If we are on the right side of the param knob we are multiplying the clock speed
    if (unscaledParam1 > 7) {
      scaledClockInterval1 = lastClockInterval / ratchetAmount1;  //to multiply the clock speed we divide the most recent clock interval
    }

    //if we are on the left side of the param knob we are dividing the clock speed
    else {
      scaledClockInterval1 = lastClockInterval * ratchetAmount1;  //to divide the clock speed we multiple the most recent clock interval
    }

    //This is the main ratcheting/ multiplication code that is used across lots of different functions across IDUMs code
    //The first section calculates the time since the last rising edge time at each input
    //Then we use a "modulo" function to wrap that elapsed time around by a multiple of the time between the last two clock rising edges
    //This makes it so that we can split each clock cycle into an even number of subdivisions and detect the time since we passed one of those
    //subdivisions.  Then we set the trigger out high if we are within a certain period of time from one of those subdivisions.

    trig1State = ((currentMicros - lastTrig1Time) % scaledClockInterval1) < 10000;  //bursted clocks are only 10 milliseconds long.  Should be
    //changed at some point to allow a more dynamic trigger length depending on the clock time and burst ratchet speed.
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength2 > 0) && (mode2 == 1)) {
    if (unscaledParam2 > 7) {
      scaledClockInterval2 = lastClockInterval / ratchetAmount2;
    }

    else {
      scaledClockInterval2 = lastClockInterval * ratchetAmount2;
    }

    trig2State = ((currentMicros - lastTrig2Time) % scaledClockInterval2) < 10000;  //(scaledClockInterval2 / 2);
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength3 > 0) && (mode3 == 1)) {

    if (unscaledParam3 > 7) {
      scaledClockInterval3 = lastClockInterval / ratchetAmount3;
    }

    else {
      scaledClockInterval3 = lastClockInterval * ratchetAmount3;
    }

    trig3State = ((currentMicros - lastTrig3Time) % scaledClockInterval3) < 10000;
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength4 > 0) && (mode4 == 1)) {

    if (unscaledParam4 > 7) {
      scaledClockInterval4 = lastClockInterval / ratchetAmount4;
    }

    else {
      scaledClockInterval4 = lastClockInterval * ratchetAmount4;
    }

    trig4State = ((currentMicros - lastTrig4Time) % scaledClockInterval4) < 10000;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
