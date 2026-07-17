//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// LAVENDER LED mode that plays a preset rhythm which is modified by each trigger input
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F14/F6): compute the break advance tick ONCE per loop, driven by the clock input, whether or not a modification is active.  This
//is what keeps breakFreeIndex (the CUT sculpt-gate phase) continuously clock-locked instead of only advancing while a break modification
//runs.  breakSpeed sets the rate: 1x uses the stock times2 edge; 2/3/4x subdivide the measured clock interval into 2*breakSpeed sub-slots
//and edge-detect the sub-slot.  On the first clock edge after power-on we snap breakFreeIndex to step 0 so the pattern downbeat aligns to
//clock-in; thereafter it advances on every tick.  Called from the main loop (a20) right after edgeDetect, before handleOutputs/breakBeat.
void updateBreakClock() {

  unsigned long currentMicros = micros();

  breakTickEdge = 0;

  if (breakSpeed <= 1) {
    breakTickEdge = times2ClockEdge;                                //1x: stock 2-per-clock edge
    if (times2ClockEdge) breakTickTime = times2ClockTime;
  } else {
    unsigned int breakSubdiv = 2 * breakSpeed;                      //4, 6 or 8 sub-slots per clock
    if (clockEdge) breakSubIndexOld = 65535;                        //new clock -> force sub-slot 0 (the downbeat) to register as a tick
    if (lastClockInterval > 0) {
      unsigned long subInterval = lastClockInterval / breakSubdiv;
      if (subInterval > 0) {
        unsigned int subIndex = (currentMicros - lastClockTime) / subInterval;
        if (subIndex > (breakSubdiv - 1)) subIndex = breakSubdiv - 1;  //clamp while waiting on a late clock so we don't over-advance
        if (subIndex != breakSubIndexOld) {                            //crossed into a new sub-slot -> a tick
          breakTickEdge = 1;
          breakTickTime = currentMicros;
          breakSubIndexOld = subIndex;
        }
      }
    }
  }

  //phase lock: snap the sculpt phase to step 0 on the first clock after power-on AND whenever the clock resumes after a stop (a gap longer
  //than BREAK_RESYNC_MULT normal intervals) - so stopping and restarting your sequencer re-aligns the pattern downbeat to your bar 1
  //without a power cycle.  Otherwise advance on each tick.  breakStableInterval tracks the running clock period; the oversized restart gap
  //is deliberately NOT folded into it, so it stays a clean reference.  Only breakFreeIndex is clock-locked here - the per-modification
  //breakIndexN advance stays in breakBeat() (those reset on input).
  bool breakPhaseReset = false;
  if (clockEdge) {
    if (!breakClockStarted) {
      breakPhaseReset = true;
      breakClockStarted = 1;
    } else if (breakStableInterval > 0 && lastClockInterval > (breakStableInterval * BREAK_RESYNC_MULT)) {
      breakPhaseReset = true;                          //clock resumed after a long gap -> re-lock phase
    } else {
      breakStableInterval = lastClockInterval;         //steady clock -> track the tempo (skipped on a restart so the gap can't poison it)
    }
  }

  if (breakPhaseReset) breakFreeIndex = 0;
  else if (breakTickEdge) breakFreeIndex = (breakFreeIndex + 1) % 16;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void breakBeat() {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  unsigned int pattern1 = unscaledParam1;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern2 = unscaledParam2;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern3 = unscaledParam3;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern4 = unscaledParam4;  //a little more readable than using the param knob directly, also easier to change later

  //CUSTOM (F13): point at the active bank of 16 patterns.  breakBank is the secondary-menu selection (0 = drums, 1 = polyrhythm/clave);
  //everything below reads 'bank' exactly as it used to read breakBeat1, so bank 0 is bit-for-bit the stock behaviour.
  const int (*bank)[4][16] = (breakBank == 0) ? breakBeat1 : breakBeat2;

  if ((modifyLength > 0) && (mode == 6)) clockState = ((currentMicros - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //CUSTOM (F14/F6): the advance tick (breakTickEdge/breakTickTime) and the clock-locked sculpt phase (breakFreeIndex) are now computed
  //once per loop in updateBreakClock() - which runs whether or not a modification is active - so the sculpt gate stays locked to clock-in
  //between modifications.  Here we just consume the shared edge to advance THIS modification's per-channel pattern indices; unlike
  //breakFreeIndex, these reset to step 0 on their channel's input trigger (the reset-anchored mask behavior break is built around).
  if (breakTickEdge) {
    breakIndex1 = (breakIndex1 + 1) % 16;
    breakIndex2 = (breakIndex2 + 1) % 16;
    breakIndex3 = (breakIndex3 + 1) % 16;
    breakIndex4 = (breakIndex4 + 1) % 16;
  }

  //Whenever we receive a trigger rising edge on any channel we reset the pattern count for each channel
  if (trig1Edge) breakIndex1 = 0;
  if (trig2Edge) breakIndex2 = 0;
  if (trig3Edge) breakIndex3 = 0;
  if (trig4Edge) breakIndex4 = 0;

  //Read the preset rhythm determined by the selected pattern and the current breakbeat index count for each channel
  //Then we convert it either to a ratchet or a trigger

  //If the stored value is one or less we output a trigger if the stored value is '1' or nothing if its '0'
  if ((modifyLength1 > 0) && (mode1 == 6)) {
    breakStepGate1 = (bank[pattern1][0][breakFreeIndex] != 0);  //CUSTOM (F6): step-length gate for MERGE CUT sculpt (free-running phase, held across the step)
    if (bank[pattern1][0][breakIndex1] < 2) trig1State = bank[pattern1][0][breakIndex1] && ((currentMicros - breakTickTime) < 5000);

    //If the stored value is 2 or greater we play a ratchet with a multiplication factor that is set by the stored value
    else trig1State = (((currentMicros - lastClockTime) % (lastClockInterval / bank[pattern1][0][breakIndex1])) < 5000);
  }

  //we now calculate this function per channel
  if ((modifyLength2 > 0) && (mode2 == 6)) {
    breakStepGate2 = (bank[pattern2][1][breakFreeIndex] != 0);  //CUSTOM (F6): step-length gate for MERGE CUT sculpt (free-running phase)
    if (bank[pattern2][1][breakIndex2] < 2) trig2State = bank[pattern2][1][breakIndex2] && ((currentMicros - breakTickTime) < 5000);
    else trig2State = (((currentMicros - lastClockTime) % (lastClockInterval / bank[pattern2][1][breakIndex2])) < 5000);
  }

  if ((modifyLength3 > 0) && (mode3 == 6)) {
    breakStepGate3 = (bank[pattern3][2][breakFreeIndex] != 0);  //CUSTOM (F6): step-length gate for MERGE CUT sculpt (free-running phase)
    if (bank[pattern3][2][breakIndex3] < 2) trig3State = bank[pattern3][2][breakIndex3] && ((currentMicros - breakTickTime) < 5000);
    else trig3State = (((currentMicros - lastClockTime) % (lastClockInterval / bank[pattern3][2][breakIndex3])) < 5000);
  }

  if ((modifyLength4 > 0) && (mode4 == 6)) {
    breakStepGate4 = (bank[pattern4][3][breakFreeIndex] != 0);  //CUSTOM (F6): step-length gate for MERGE CUT sculpt (free-running phase)
    if (bank[pattern4][3][breakIndex4] < 2) trig4State = bank[pattern4][3][breakIndex4] && ((currentMicros - breakTickTime) < 5000);
    else trig4State = (((currentMicros - lastClockTime) % (lastClockInterval / bank[pattern4][3][breakIndex4])) < 5000);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
