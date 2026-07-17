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

    //CUSTOM (Path A): monotonic, musical delay = a clean fraction of THIS channel's own input gap, so it self-scales with the pattern
    //and always fits inside the gap (never eats a trigger).  On each trigger edge we sample a step 1-8 from PARAM, laid out as a
    //straight ramp (CCW short -> CW long) using the ASCENDING half of the active resolution table - so odd/even/pow2 pick the subdivision set.
    if (trig1Edge) {
      unsigned int idx = (unscaledParam1 / 2) + 8;  //map the knob monotonically into the ascending half of the resolution table
      if (idx > 15) idx = 15;
      if (paramResolution == 0) sampledRatchetAmount1 = odd[idx];
      else if (paramResolution == 1) sampledRatchetAmount1 = even[idx];
      else sampledRatchetAmount1 = powerOf2[idx];
    }

    //delay = gap * step/8, UNIFORM across channels.  Different inputs + independent split-mode rolls already make the four outputs
    //unique, so no artificial per-channel spread is needed - same philosophy as ratchet/mult mode.  Feed different inputs to spread.
    unsigned long delay1 = ((unsigned long)lastTrig1Interval * sampledRatchetAmount1) / 8;
    trig1State = ((currentMicros - lastTrig1Time) > delay1) && ((currentMicros - lastTrig1Time) < (delay1 + 10000));
  }

  if ((modifyLength2 > 0) && (mode2 == 5)) {

    if (trig2Edge) {
      unsigned int idx = (unscaledParam2 / 2) + 8;
      if (idx > 15) idx = 15;
      if (paramResolution == 0) sampledRatchetAmount2 = odd[idx];
      else if (paramResolution == 1) sampledRatchetAmount2 = even[idx];
      else sampledRatchetAmount2 = powerOf2[idx];
    }

    unsigned long delay2 = ((unsigned long)lastTrig2Interval * sampledRatchetAmount2) / 8;
    trig2State = ((currentMicros - lastTrig2Time) > delay2) && ((currentMicros - lastTrig2Time) < (delay2 + 10000));
  }

  if ((modifyLength3 > 0) && (mode3 == 5)) {

    if (trig3Edge) {
      unsigned int idx = (unscaledParam3 / 2) + 8;
      if (idx > 15) idx = 15;
      if (paramResolution == 0) sampledRatchetAmount3 = odd[idx];
      else if (paramResolution == 1) sampledRatchetAmount3 = even[idx];
      else sampledRatchetAmount3 = powerOf2[idx];
    }

    unsigned long delay3 = ((unsigned long)lastTrig3Interval * sampledRatchetAmount3) / 8;
    trig3State = ((currentMicros - lastTrig3Time) > delay3) && ((currentMicros - lastTrig3Time) < (delay3 + 10000));
  }

  if ((modifyLength4 > 0) && (mode4 == 5)) {

    if (trig4Edge) {
      unsigned int idx = (unscaledParam4 / 2) + 8;
      if (idx > 15) idx = 15;
      if (paramResolution == 0) sampledRatchetAmount4 = odd[idx];
      else if (paramResolution == 1) sampledRatchetAmount4 = even[idx];
      else sampledRatchetAmount4 = powerOf2[idx];
    }

    unsigned long delay4 = ((unsigned long)lastTrig4Interval * sampledRatchetAmount4) / 8;
    trig4State = ((currentMicros - lastTrig4Time) > delay4) && ((currentMicros - lastTrig4Time) < (delay4 + 10000));
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
