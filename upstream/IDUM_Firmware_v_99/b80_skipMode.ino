//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// PURPLE LED mode that manipulates just the clock output
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void skip() {

  unsigned long ratchetMicros = 0;

  //if the clock channel is on skip mode we run the skip, clock mode is the only one that should skip the clock
  if ((mode == 7) && (modifyLength > 0) && !loopEnable) {  //we will eventually allow skip mode in the looper

    if (clockEdge) setClockSkipMode();  //this makes it so that the mode only changes on clock edges, helps avoid glitches with maintain cycle

    if (!clockRatchet && !clockSkip) {                                //if we aren't ratcheting or skipping the clock then we just pass through the input
      clockState = clockIn;                                           //clock output is the same as the input
      clockChoke = 1;                                                 //normal clock output is enabled
      if (clockState && !clockStateOld) clockRatchetCycleCount += 1;  //keep track of how many steps we passed for the maintain cycle function
      clockStateOld = clockState;                                     //keep track of current clock state for testing on next loop
    }

    if (clockRatchet) {  //if we are in clock ratchet mode
      clockChoke = 1;    //we enable the clock

      //here we run the same code we do in the normal ratchet modes but on the clock output
      clockRatchetState = ((micros() - lastClockTime) % (lastClockInterval / ((unsigned long)skipRatchetAmount))) < (lastClockInterval / ((unsigned long)skipRatchetAmount * 2));

      if (clockRatchetState && !clockRatchetStateOld) {  //if the clock output just turned on add to the cycle count for the maintain cycle function
        clockRatchetCycleCount += 1;
      }

      clockState = clockRatchetState;            //set the clock output to the ratchet function output
      clockRatchetStateOld = clockRatchetState;  //keep track of current clock state for testing on next loop
    }

    if (clockSkip) {                                    //if we are in the skip mode.
      if (clockEdge || forceSkip) {                     //and we received a rising clock at the input
        forceSkip = 0;                                  //forceSkip fixes a bug that prevented skips on the first step of a modification
        skipSteps(skipRatchetAmount - (!analogClock));  //skip a certain number of steps based on the param knob position
        clockSkipCycleCount += (skipRatchetAmount);     //keep track of how many steps we have skipped
        clockChoke = 0;                                 //normal clock output is disabled
        clockState = 0;                                 //and also turn off the actual clock state
        clockRatchetStateOld = 0;                       //keep the clock ratchet state old for more accurate edge detection
      }
    }
  }

  forceSkip = 0;  //if we made it through the entire function once and didn't set this flag low already set it low just in case

  //triggers all just pass through to the input if they are also assigned to this mode
  if ((modifyLength1 > 0) && (mode1 == 7)) trig1State = trig1In;
  if ((modifyLength2 > 0) && (mode2 == 7)) trig2State = trig2In;
  if ((modifyLength3 > 0) && (mode3 == 7)) trig3State = trig3In;
  if ((modifyLength4 > 0) && (mode4 == 7)) trig4State = trig4In;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setClockSkipMode() {

  skipRatchetAmount = ratchetAmount;

  if ((unscaledParam == 7) || (unscaledParam == 8)) {  //if param knob is close to noon we pass the input clock
    clockRatchet = clockSkip = 0;
  }
  if (unscaledParam > 8) {  //if the param knob is right of noon we ratchet the clock
    clockRatchet = 1;
    clockSkip = 0;
  }
  if (unscaledParam < 7) {  //if the param knob is left of noon we skip a few steps every clock
    clockRatchet = 0;
    clockSkip = 1;
  }

  if ((!clockRatchet) && (clockRatchetStateOld)) {  //If we just switched modes and the ratchet clock is still high force it low
    digitalWrite(clockOut, HIGH);
    digitalWrite(clockOut, HIGH);
    digitalWrite(clockOut, HIGH);
    clockRatchetStateOld = 0;
  }
}
