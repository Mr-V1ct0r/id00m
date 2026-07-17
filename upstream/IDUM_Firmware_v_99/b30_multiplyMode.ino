//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// ORANGE LED mode that ratchets each trigger out with a speed determined by each trigger input seperately
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void ratchet() {

  unsigned long currentMicros = micros();  //only sample the system time once

  unsigned long scaledClockInterval1 = 0;
  unsigned long scaledClockInterval2 = 0;
  unsigned long scaledClockInterval3 = 0;
  unsigned long scaledClockInterval4 = 0;

  if ((modifyLength > 0) && (mode == 2)) clockState = ((currentMicros - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //check if each channel is performing the current modification
  if ((modifyLength1 > 0) && (mode1 == 2)) {
    if (unscaledParam1 > 7)
      //same burst code as in the last function except now it references the interval between the two most recent gate rising edges
      //at each gate input.  Now we either multiply or divide the trig intervals instead of the clock intervals
      scaledClockInterval1 = lastTrig1Interval / ratchetAmount1;

    else
      //if we are on the left side of the param knob we divide the clock
      scaledClockInterval1 = lastTrig1Interval * ratchetAmount1;

    trig1State = (currentMicros - lastTrig1Time) % (scaledClockInterval1) < 10000;
  }

  //check if each channel is performing the current modification (I don't feel like copying all of the comments per channel)
  if ((modifyLength2 > 0) && (mode2 == 2)) {
    if (unscaledParam2 > 7)
      scaledClockInterval2 = lastTrig2Interval / ratchetAmount2;

    else
      scaledClockInterval2 = lastTrig2Interval * ratchetAmount2;

    trig2State = (currentMicros - lastTrig2Time) % (scaledClockInterval2) < 10000;
  }

  //check if each channel is performing the current modification
  if ((modifyLength3 > 0) && (mode3 == 2)) {
    if (unscaledParam3 > 7)
      scaledClockInterval3 = lastTrig3Interval / ratchetAmount3;

    else
      scaledClockInterval3 = lastTrig3Interval * ratchetAmount3;

    trig3State = (currentMicros - lastTrig3Time) % (scaledClockInterval3) < 10000;
  }

  //check if each channel is performing the current modification
  if ((modifyLength4 > 0) && (mode4 == 2)) {
    if (unscaledParam4 > 7)
      scaledClockInterval4 = lastTrig4Interval / ratchetAmount4;

    else
      scaledClockInterval4 = lastTrig4Interval * ratchetAmount4;

    trig4State = (currentMicros - lastTrig4Time) % (scaledClockInterval4) < 10000;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
