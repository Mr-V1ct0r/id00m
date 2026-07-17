//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// LAVENDER LED mode that plays a preset rhythm which is modified by each trigger input
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void breakBeat() {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  unsigned int pattern1 = unscaledParam1;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern2 = unscaledParam2;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern3 = unscaledParam3;  //a little more readable than using the param knob directly, also easier to change later
  unsigned int pattern4 = unscaledParam4;  //a little more readable than using the param knob directly, also easier to change later

  if ((modifyLength > 0) && (mode == 6)) clockState = ((currentMicros - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //whenever the double time clock has a rising edge we advance the index of each preset rhythm.  Now this is calculated globally for the module.
  if (times2ClockEdge) {
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
    if (breakBeat1[pattern1][0][breakIndex1] < 2) trig1State = breakBeat1[pattern1][0][breakIndex1] && ((currentMicros - times2ClockTime) < 5000);

    //If the stored value is 2 or greater we play a ratchet with a multiplication factor that is set by the stored value
    else trig1State = (((currentMicros - lastClockTime) % (lastClockInterval / breakBeat1[pattern1][0][breakIndex1])) < 5000);
  }

  //we now calculate this function per channel
  if ((modifyLength2 > 0) && (mode2 == 6)) {
    if (breakBeat1[pattern2][1][breakIndex2] < 2) trig2State = breakBeat1[pattern2][1][breakIndex2] && ((currentMicros - times2ClockTime) < 5000);
    else trig2State = (((currentMicros - lastClockTime) % (lastClockInterval / breakBeat1[pattern2][1][breakIndex2])) < 5000);
  }

  if ((modifyLength3 > 0) && (mode3 == 6)) {
    if (breakBeat1[pattern3][2][breakIndex3] < 2) trig3State = breakBeat1[pattern3][2][breakIndex3] && ((currentMicros - times2ClockTime) < 5000);
    else trig3State = (((currentMicros - lastClockTime) % (lastClockInterval / breakBeat1[pattern3][2][breakIndex3])) < 5000);
  }

  if ((modifyLength4 > 0) && (mode4 == 6)) {
    if (breakBeat1[pattern4][3][breakIndex4] < 2) trig4State = breakBeat1[pattern4][3][breakIndex4] && ((currentMicros - times2ClockTime) < 5000);
    else trig4State = (((currentMicros - lastClockTime) % (lastClockInterval / breakBeat1[pattern4][3][breakIndex4])) < 5000);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
