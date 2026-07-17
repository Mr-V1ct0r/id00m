//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This either replays gates saved in the loop or records new gates into the loop.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void handleLooper() {

  determineLooperStatus();  //sets whether or not the looper is currently playing or recording

  handleHistoryIndex();

  if (loopEnable) {  //if looping is enabled call the "loop trigger" playback function

    loopTriggers();  //replays saved gates in the loop

    edgeDetectLoop();  //rerun the edge detection tests on the outputs of the looper

  }

  else {

    recordTriggers();  //if looping is not enabled we record incoming gates.
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Takes the loop button and gate inputs and decides whether the looper is on or off.

void determineLooperStatus() {

  //if the looper button is pressed and we aren't in the menu change looper status
  if ((loopButtonActiveEdge == 1) && !menuMode) {
    loopEnable = !loopEnable;
  }

  //The behavior of the loop gate input changes based on the "loopInputBehavior" option set in the menu
  if (loopInputBehavior) {          //If "loopInputBehavior" is high then the gate input momentarily changes the looper status.
    if (loopGateActiveEdge == 1) {  //both rising and falling edges invert the looper status.
      loopEnable = !loopEnable;
    }

    if (!loopGateIn && loopGateInOld) {  //both rising and falling edges invert the looper status.
      loopEnable = !loopEnable;
    }
  }

  else {
    if (loopGateActiveEdge == 1) {  //If "loopInputBehavior" is low then the gate input toggles the looper status.
      loopEnable = !loopEnable;
    }
  }

  if ((loopButtonActiveEdge || loopGateActiveEdge) && loopEnable) {  //run this code if the loop has just turned on

    //if the 'cycle' switch is high and a modification is happening we run the maintain cycle function.
    if (cycleIn && (modifyLength != 0)) {
      maintainCycle();
    }

    //We now capture these variables at loop startup.  This allows us to reliably go back 8 steps on each loop.
    originalHistoryIndex = historyIndex;

    //We use this to offset the overall loop offset position so that loops that start in any mode wills start with 0 offset.
    originalLoopOffset = modeDialPosition;

    //If the clock is currently on then we increment the history index by 1, makes this function more accurate.
    if (clockIn) originalHistoryIndex = wrapAround(originalHistoryIndex - 1, 8);

    //reset all of the relevant looper variables.
    loopDivide = 0;
    loopCycleCount = 0;
    loopCycleIndex = 0;
    loopOffsetOld = 0;
    loopClockMultOld = 0;
    loopIndexOld = 0;

    //save the loop info to the EEPROM
    saveLoopData();
  }

  //run this code if the looper just turned off
  if ((loopButtonActiveEdge || loopGateActiveEdge) && !loopEnable) {
    if (cycleIn) {
      loopCycle();  //if the cycle switch is on we run a specialized cycle function for the looper
    }

    //set modifications off in case the looper was running a modification
    modifyLength = modifyLength1 = modifyLength2 = modifyLength3 = modifyLength4 = 0;
    saveLoopData();  //save all relevant looper info to the EEPROM
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void handleHistoryIndex() {

  //change the history index which remembers what the most recent cell in the array was saved.  This makes it so that we don't constantly have
  //to shift older steps into further cells in the array.
  if (clockEdge) historyIndex = (historyIndex + 1) % 8;  //We now increment the loop index even if we are in loop playback.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//this is a special version of the edge detect function that is designed to work with the output of the looper
//mostly ignores the clock variables and button/ switch inputs

void edgeDetectLoop() {

  unsigned long currentMicros = micros();

  //reset edge detection variables
  trig1Edge = trig2Edge = trig3Edge = trig4Edge = trig1FallingEdge = trig2FallingEdge = trig3FallingEdge = trig4FallingEdge = 0;

  //if each gate is currently on and was off in the last loop then that gate has a rising edge and we set the relevant flag high
  if (trig1In && !loopTrig1Old) trig1Edge = 1;
  if (trig2In && !loopTrig2Old) trig2Edge = 1;
  if (trig3In && !loopTrig3Old) trig3Edge = 1;
  if (trig4In && !loopTrig4Old) trig4Edge = 1;

  //if each gate is currently off and was on in the last loop then that gate has a falling edge and we set the relevant flag high
  if (!trig1In && loopTrig1Old) trig1FallingEdge = 1;
  if (!trig2In && loopTrig2Old) trig2FallingEdge = 1;
  if (!trig3In && loopTrig3Old) trig3FallingEdge = 1;
  if (!trig4In && loopTrig4Old) trig4FallingEdge = 1;

  //record intervals between triggers and time of most recent trigger rising and falling edges.
  //We don't need to debounce the outputs of the looper.

  if (trig1Edge) {                                      //if we have a rising edge
    lastTrig1Interval = currentMicros - lastTrig1Time;  //save the interval between the most recent two triggers
    lastTrig1Time = currentMicros;                      //and the current time of the rising edge
  }

  if (trig1FallingEdge) lastTrig1FallingTime = currentMicros;  //also save the time of the falling edge

  if (trig2Edge) {
    lastTrig2Interval = currentMicros - lastTrig2Time;
    lastTrig2Time = currentMicros;
  }

  if (trig2FallingEdge) lastTrig2FallingTime = currentMicros;

  if (trig3Edge) {
    lastTrig3Interval = currentMicros - lastTrig3Time;
    lastTrig3Time = currentMicros;
  }

  if (trig3FallingEdge) lastTrig3FallingTime = currentMicros;

  if (trig4Edge) {
    lastTrig4Interval = currentMicros - lastTrig4Time;
    lastTrig4Time = currentMicros;
  }

  if (trig4FallingEdge) lastTrig4FallingTime = currentMicros;

  //save trigger states for next loop
  loopTrig1Old = trig1In;
  loopTrig2Old = trig2In;
  loopTrig3Old = trig3In;
  loopTrig4Old = trig4In;

  //if choke option is enabled, set choke flag high on trigger edge (flag is set low when modification starts).
  if (!chokeDisable) {
    if (trig1Edge) trig1Choke = 1;
    if (trig2Edge) trig2Choke = 1;
    if (trig3Edge) trig3Choke = 1;
    if (trig4Edge) trig4Choke = 1;
  }

  else trig1Choke = trig2Choke = trig3Choke = trig4Choke = 1;  //otherwise the triggers are always on
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
