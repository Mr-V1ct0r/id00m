//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// RED LED mode that ratchets each trigger out with a speed determined by the clock input
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F9): scale a time PERIOD by this channel's multiplier - the shared per-channel primitive.  A multiply shortens the period (num=1,
//den>1 -> faster events), a divide lengthens it (num>1, den=1 -> slower); x1 (f9Value 3) returns it unchanged, so any mode stays bit-for-bit
//stock until F9 is dialled.  Both burst's clock interval and ball's bounce loop-count divide are periods, so they scale identically.  ch is 1-4.
unsigned long f9ScalePeriod(unsigned long period, unsigned int ch) {
  unsigned int v = f9Value[ch - 1];
  return period * f9Num[v] / f9Den[v];
}

//CUSTOM (F9): burst's per-channel view of the master clock interval - a thin wrapper over the shared period scaler.
unsigned long chanClockInterval(unsigned int ch) {
  return f9ScalePeriod(lastClockInterval, ch);
}

//CUSTOM (F9): proportional pulse width.  Stock burst emitted a fixed 10ms gate; once a subdivision shrinks the sub-trigger interval below
//~10ms those fixed pulses fuse into one sustained gate ("smear" - the F14 ceiling).  Scale the pulse to ~half the sub-interval instead so a
//gap always survives, clamped to [F9_MIN_PULSE, 10ms].  At musical tempos the interval is wide and this returns the stock 10ms, so it only
//changes behaviour in the range that used to smear.  Reusable by break/ball if they adopt the per-channel tick later.
unsigned long f9PulseWidth(unsigned long subInterval) {
  unsigned long pw = subInterval / 2;
  if (pw > 10000) pw = 10000;
  if (pw < F9_MIN_PULSE) pw = F9_MIN_PULSE;
  return pw;
}

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

    //CW side (right of center): multiply the clock -> add fast ratchets.  Stock burst behavior, unchanged.
    if (unscaledParam1 > 7) {
      scaledClockInterval1 = chanClockInterval(1) / ratchetAmount1;                    //CUSTOM (F9): divide THIS channel's scaled clock, not the raw master clock
      if (scaledClockInterval1 > 0)                                                    //F9's multiply can shrink the sub-interval to 0 on a fast clock - guard the modulo
        trig1State = ((currentMicros - lastTrig1Time) % scaledClockInterval1) < f9PulseWidth(scaledClockInterval1);  //CUSTOM (F9): proportional pulse (was fixed 10ms)
      else trig1State = 0;
    }

    //CCW side (left of center): CUSTOM thinning - probabilistically DROP incoming triggers instead of the stock slow clock-divide echo.
    //dropChance is 0 at center and rises toward the far left (full CCW = fully muted).  Rolled once per incoming trigger (on the rising
    //edge) so a dropped trigger stays dropped for its whole gate.  Computed signed and clamped so it can't underflow at the center boundary.
    else {
      int dropChance1 = (512 - (int)modifyParam1) * 2;
      if (dropChance1 < 0) dropChance1 = 0;
      if (trig1Edge) burstDrop1 = (random(1000) < dropChance1);
      trig1State = trig1In && !burstDrop1;
    }
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength2 > 0) && (mode2 == 1)) {
    if (unscaledParam2 > 7) {  //CW: add fast ratchets (stock, now F9-scaled)
      scaledClockInterval2 = chanClockInterval(2) / ratchetAmount2;
      if (scaledClockInterval2 > 0)
        trig2State = ((currentMicros - lastTrig2Time) % scaledClockInterval2) < f9PulseWidth(scaledClockInterval2);
      else trig2State = 0;
    }

    else {  //CCW: CUSTOM thinning - drop incoming triggers with a chance that rises toward full CCW
      int dropChance2 = (512 - (int)modifyParam2) * 2;
      if (dropChance2 < 0) dropChance2 = 0;
      if (trig2Edge) burstDrop2 = (random(1000) < dropChance2);
      trig2State = trig2In && !burstDrop2;
    }
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength3 > 0) && (mode3 == 1)) {

    if (unscaledParam3 > 7) {  //CW: add fast ratchets (stock, now F9-scaled)
      scaledClockInterval3 = chanClockInterval(3) / ratchetAmount3;
      if (scaledClockInterval3 > 0)
        trig3State = ((currentMicros - lastTrig3Time) % scaledClockInterval3) < f9PulseWidth(scaledClockInterval3);
      else trig3State = 0;
    }

    else {  //CCW: CUSTOM thinning - drop incoming triggers with a chance that rises toward full CCW
      int dropChance3 = (512 - (int)modifyParam3) * 2;
      if (dropChance3 < 0) dropChance3 = 0;
      if (trig3Edge) burstDrop3 = (random(1000) < dropChance3);
      trig3State = trig3In && !burstDrop3;
    }
  }

  //run the modification code on each trigger channel if we the modify length for that channel is more than 0
  //and the mode for that channel is set to burst
  if ((modifyLength4 > 0) && (mode4 == 1)) {

    if (unscaledParam4 > 7) {  //CW: add fast ratchets (stock, now F9-scaled)
      scaledClockInterval4 = chanClockInterval(4) / ratchetAmount4;
      if (scaledClockInterval4 > 0)
        trig4State = ((currentMicros - lastTrig4Time) % scaledClockInterval4) < f9PulseWidth(scaledClockInterval4);
      else trig4State = 0;
    }

    else {  //CCW: CUSTOM thinning - drop incoming triggers with a chance that rises toward full CCW
      int dropChance4 = (512 - (int)modifyParam4) * 2;
      if (dropChance4 < 0) dropChance4 = 0;
      if (trig4Edge) burstDrop4 = (random(1000) < dropChance4);
      trig4State = trig4In && !burstDrop4;
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
