//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// BLUE LED mode that delays each output from its input by a variable amount
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void trigDelay() {

  unsigned long currentMicros = micros();  //only sample the system time once

  if ((modifyLength > 0) && (mode == 5)) clockState = ((currentMicros - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //This is one of the more complex functions for calculating the trigger state
  //If the time elapsed since the last trigger input for each channel is longer than a certain ratio of the interval between the most recent
  //triggers on that channel we make a 10 ms trigger.  Each channel has a different scaling which changes how much the param knob delays
  //each channel.  We also make the delay time based on the interval between triggers to keep the delay time always relevant to the
  //pattern that is coming in.  Using the ratchet amount gives us nice integer ratios that get use sort of close to actual beats.
  if ((modifyLength1 > 0) && (mode1 == 5)) {

    //we sample the param knob whenever a trigger edge is received to avoid glitches from changing the delay amount too quickly
    if (trig1Edge) sampledRatchetAmount1 = ratchetAmount1;

    //if we are on the right side of the param knob we delay the triggers on the right side of the panel more
    if (unscaledParam1 <= 7) {

      //If the time since the most recent trigger edge is greater than the delayed trigger interval and that value plues 10 milliseconds
      //then the output is high
      trig1State = ((currentMicros - lastTrig1Time) > (lastTrig1Interval * (sampledRatchetAmount1) / 8))
                   && ((currentMicros - lastTrig1Time) < ((lastTrig1Interval * (sampledRatchetAmount1) / 8) + 10000));
    }

    //if we are on the left side of the param knob we delay the triggers on the left side of the panel more
    else {
      trig1State = ((currentMicros - lastTrig1Time) > (lastTrig1Interval * (sampledRatchetAmount1) / 16))
                   && ((currentMicros - lastTrig1Time) < ((lastTrig1Interval * (sampledRatchetAmount1) / 16) + 10000));
    }
  }

  if ((modifyLength2 > 0) && (mode2 == 5)) {

    if (trig2Edge) sampledRatchetAmount2 = ratchetAmount2;

    if (unscaledParam2 <= 7) {
      trig2State = ((currentMicros - lastTrig2Time) > (lastTrig2Interval * (sampledRatchetAmount2) / 12))
                   && ((currentMicros - lastTrig2Time) < ((lastTrig2Interval * (sampledRatchetAmount2) / 12) + 10000));
    }

    else {
      trig2State = ((currentMicros - lastTrig2Time) > (lastTrig2Interval * (sampledRatchetAmount2) / 14))
                   && ((currentMicros - lastTrig2Time) < ((lastTrig2Interval * (sampledRatchetAmount2) / 14) + 10000));
    }
  }

  if ((modifyLength3 > 0) && (mode3 == 5)) {

    if (trig3Edge) sampledRatchetAmount3 = ratchetAmount3;

    if (unscaledParam3 <= 7) {
      trig3State = ((currentMicros - lastTrig3Time) > (lastTrig3Interval * (sampledRatchetAmount3) / 14))
                   && ((currentMicros - lastTrig3Time) < ((lastTrig3Interval * (sampledRatchetAmount3) / 14) + 10000));
    }

    else {
      trig3State = ((currentMicros - lastTrig3Time) > (lastTrig3Interval * (sampledRatchetAmount3) / 12))
                   && ((currentMicros - lastTrig3Time) < ((lastTrig3Interval * (sampledRatchetAmount3) / 12) + 10000));
    }
  }

  if ((modifyLength4 > 0) && (mode4 == 5)) {

    if (trig4Edge) sampledRatchetAmount4 = ratchetAmount4;

    if (unscaledParam4 <= 7) {
      trig4State = ((currentMicros - lastTrig4Time) > (lastTrig4Interval * (sampledRatchetAmount4) / 16))
                   && ((currentMicros - lastTrig4Time) < ((lastTrig4Interval * (sampledRatchetAmount4) / 16) + 10000));
    }

    else {
      trig4State = ((currentMicros - lastTrig4Time) > (lastTrig4Interval * (sampledRatchetAmount4) / 8))
                   && ((currentMicros - lastTrig4Time) < ((lastTrig4Interval * (sampledRatchetAmount4) / 8) + 10000));
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
