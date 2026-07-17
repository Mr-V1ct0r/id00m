//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// ORANGE LED mode (div/mult).  CW of center: ratchets each trigger out faster, speed set by each trigger input's own interval.
// CCW of center: CUSTOM true trigger divider - passes 1 of every N incoming triggers and drops the rest (N = 1 at center .. 8 at
// full CCW).  The stock CCW behavior stretched the trigger interval, which never actually dropped a hit on a steady stream, so it
// was inaudible on a groove; the divider replaces it with real, audible division.
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

    //CW side (right of center): multiply the trigger rate -> fast ratchets.  Stock behavior, unchanged.  References the interval
    //between the two most recent rising edges at THIS gate input, so each channel ratchets against its own rhythm.
    if (unscaledParam1 > 7) {
      scaledClockInterval1 = lastTrig1Interval / ratchetAmount1;
      //B6 fix: scaledClockInterval is 0 until this channel has measured a trigger interval (two rising edges); guard the modulo so
      //an unpatched or just-triggered channel doesn't divide by zero.  No interval to ratchet against yet -> stay silent.
      if (scaledClockInterval1 > 0) trig1State = (currentMicros - lastTrig1Time) % scaledClockInterval1 < 10000;
      else trig1State = 0;
    }

    //CCW side (left of center): CUSTOM true trigger divider - pass 1 of every ratchetAmount incoming triggers, drop the rest
    //(ratchetAmount = 1 at center -> pass all, up to 8 at full CCW -> keep every 8th).  The stock code here multiplied the interval,
    //which could never drop a hit on a steady stream (each input resets the modulo phase onto a fire), so it was inaudible on a
    //groove.  Decision is made once per incoming trigger (rising edge) and held for the whole gate.
    else {
      if (trig1Edge) {
        ratchetDrop1 = (ratchetDivCount1 != 0);                                          //keep only when the counter is at slot 0
        ratchetDivCount1 = (ratchetDivCount1 + 1) % (ratchetAmount1 > 0 ? ratchetAmount1 : 1);
      }
      trig1State = trig1In && !ratchetDrop1;
    }
  }

  //check if each channel is performing the current modification (see channel 1 above for the full commentary)
  if ((modifyLength2 > 0) && (mode2 == 2)) {
    if (unscaledParam2 > 7) {  //CW: multiply -> fast ratchets (stock)
      scaledClockInterval2 = lastTrig2Interval / ratchetAmount2;
      if (scaledClockInterval2 > 0) trig2State = (currentMicros - lastTrig2Time) % scaledClockInterval2 < 10000;
      else trig2State = 0;
    }

    else {  //CCW: CUSTOM true trigger divider - pass 1 of every ratchetAmount incoming triggers, drop the rest
      if (trig2Edge) {
        ratchetDrop2 = (ratchetDivCount2 != 0);
        ratchetDivCount2 = (ratchetDivCount2 + 1) % (ratchetAmount2 > 0 ? ratchetAmount2 : 1);
      }
      trig2State = trig2In && !ratchetDrop2;
    }
  }

  //check if each channel is performing the current modification
  if ((modifyLength3 > 0) && (mode3 == 2)) {
    if (unscaledParam3 > 7) {  //CW: multiply -> fast ratchets (stock)
      scaledClockInterval3 = lastTrig3Interval / ratchetAmount3;
      if (scaledClockInterval3 > 0) trig3State = (currentMicros - lastTrig3Time) % scaledClockInterval3 < 10000;
      else trig3State = 0;
    }

    else {  //CCW: CUSTOM true trigger divider - pass 1 of every ratchetAmount incoming triggers, drop the rest
      if (trig3Edge) {
        ratchetDrop3 = (ratchetDivCount3 != 0);
        ratchetDivCount3 = (ratchetDivCount3 + 1) % (ratchetAmount3 > 0 ? ratchetAmount3 : 1);
      }
      trig3State = trig3In && !ratchetDrop3;
    }
  }

  //check if each channel is performing the current modification
  if ((modifyLength4 > 0) && (mode4 == 2)) {
    if (unscaledParam4 > 7) {  //CW: multiply -> fast ratchets (stock)
      scaledClockInterval4 = lastTrig4Interval / ratchetAmount4;
      if (scaledClockInterval4 > 0) trig4State = (currentMicros - lastTrig4Time) % scaledClockInterval4 < 10000;
      else trig4State = 0;
    }

    else {  //CCW: CUSTOM true trigger divider - pass 1 of every ratchetAmount incoming triggers, drop the rest
      if (trig4Edge) {
        ratchetDrop4 = (ratchetDivCount4 != 0);
        ratchetDivCount4 = (ratchetDivCount4 + 1) % (ratchetAmount4 > 0 ? ratchetAmount4 : 1);
      }
      trig4State = trig4In && !ratchetDrop4;
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
