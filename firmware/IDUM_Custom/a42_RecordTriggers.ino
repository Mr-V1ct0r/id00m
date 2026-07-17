//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// When the looper is inactive all incoming gates are recorded for replay later
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void recordTriggers() {

  storeCurrentModificationVariables();  //Save all variables related to any currently engaged modifications

  storeCurrentTriggersOnClockEdge();  //save the loop gates and variables for later

  storeTrigInterruptsInLoop();  //Interrupt driven storage of gates whenever a rising edge is received

  detectRatchets();  //Extra input checking to make sure that ratchets are accurately captured.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void storeCurrentModificationVariables() {

  // modification parameters just need to be saved once per clock
  if (clockEdge) {

    //paramHistory [ trigg channel ] [ step ] [ param-to-save ( mode/ modifyParam/ modifyLength/ originalModifyLength ) ]

    //save the current modification variables.  Now is done per channel.
    paramHistory[0][historyIndex][0] = mode;
    paramHistory[0][historyIndex][1] = modifyParam;
    paramHistory[0][historyIndex][2] = modifyLength;
    paramHistory[0][historyIndex][3] = originalModifyLength;

    paramHistory[1][historyIndex][0] = mode1;
    paramHistory[1][historyIndex][1] = modifyParam;
    paramHistory[1][historyIndex][2] = modifyLength1;
    paramHistory[1][historyIndex][3] = originalModifyLength1;

    paramHistory[2][historyIndex][0] = mode2;
    paramHistory[2][historyIndex][1] = modifyParam;
    paramHistory[2][historyIndex][2] = modifyLength2;
    paramHistory[2][historyIndex][3] = originalModifyLength2;

    paramHistory[3][historyIndex][0] = mode3;
    paramHistory[3][historyIndex][1] = modifyParam;
    paramHistory[3][historyIndex][2] = modifyLength3;
    paramHistory[3][historyIndex][3] = originalModifyLength3;

    paramHistory[4][historyIndex][0] = mode4;
    paramHistory[4][historyIndex][1] = modifyParam;
    paramHistory[4][historyIndex][2] = modifyLength4;
    paramHistory[4][historyIndex][3] = originalModifyLength4;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//save incoming gates and modification variables

void storeCurrentTriggersOnClockEdge() {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  unsigned long loopTickLookAhead = 10;  //Global variable that sets the threshold for a tick changing to record current trigger status.

  //trigger data is saved every tick which currently runs 4 times per clock
  if (times4ClockEdge) {

    //reset all saved gates at the current time in the trigger history array
    trigHistory[0][historyIndex][clockTick] = trigHistory[1][historyIndex][clockTick] =
      trigHistory[2][historyIndex][clockTick] = trigHistory[3][historyIndex][clockTick] = 0;

    if (trig1In) trigHistory[0][historyIndex][clockTick] = 1;
    if (trig2In) trigHistory[1][historyIndex][clockTick] = 1;
    if (trig3In) trigHistory[2][historyIndex][clockTick] = 1;
    if (trig4In) trigHistory[3][historyIndex][clockTick] = 1;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Atomically read one of the 32-bit ISR timestamps.  On an 8-bit AVR a 4-byte read takes several instructions; briefly disabling
//interrupts guarantees a trigger ISR can't overwrite the value halfway through the read and hand us a torn number.  Safe here because
//this only ever runs in the main loop, where interrupts are already enabled.
unsigned long atomicReadULong(volatile unsigned long *v) {
  noInterrupts();
  unsigned long r = *v;
  interrupts();
  return r;
}

//A somewhat imperfect solution for more accurate trigger recording.  In the next firmware update the input triggers will drive everything
//using external interrupts to make all trigger input timings more accurate.
//NOTE: the flag is still cleared after the timestamp is consumed, so a trigger that fires between the snapshot and the clear is still
//dropped (a pre-existing logical race, separate from the tearing fixed here - proper handling needs a small capture queue).  Tracked
//tracked as B11; it is a faithful fix, just low priority.

void storeTrigInterruptsInLoop() {

  unsigned int currentTick = 0;

  //If the trigger flag is set high by the external interrupt based on trigger input 1.
  if (forceTrig1LoopRecord) {

    //calculate the tick that the trigger interrupt happened in based on the recorded time.
    currentTick = decideTickRecord(atomicReadULong(&forceTrig1LoopRecordTime));

    //If the returned tick value is within expected parameters store a trigger in the looper at the calculated tick value.
    if ((currentTick > 0) && (currentTick < 4)) trigHistory[0][historyIndex][currentTick] = 1;  //error checking

    //If the most recent trigger happened outside of the expected trigger interval past the most recent clock time don't store any triggers.
    if (currentTick < 4) forceTrig1LoopRecord = 0;
  }

  if (forceTrig2LoopRecord) {

    currentTick = decideTickRecord(atomicReadULong(&forceTrig2LoopRecordTime));
    if ((currentTick > 0) && (currentTick < 4)) trigHistory[1][historyIndex][currentTick] = 1;
    if (currentTick < 4) forceTrig2LoopRecord = 0;
  }

  if (forceTrig3LoopRecord) {

    currentTick = decideTickRecord(atomicReadULong(&forceTrig3LoopRecordTime));
    if ((currentTick > 0) && (currentTick < 4)) trigHistory[2][historyIndex][currentTick] = 1;
    if (currentTick < 4) forceTrig3LoopRecord = 0;
  }

  if (forceTrig4LoopRecord) {

    currentTick = decideTickRecord(atomicReadULong(&forceTrig4LoopRecordTime));
    if ((currentTick > 0) && (currentTick < 4)) trigHistory[3][historyIndex][currentTick] = 1;
    if (currentTick < 4) forceTrig4LoopRecord = 0;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function calculates the tick that the most recent external interrupt happened in depending on the provided system time.
//Designed to handle changing clocks to prevent storing values into the looper array at indices that are outside expected values.

int decideTickRecord(unsigned long risingEdgeTime) {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  unsigned long loopWindow = 100;  //global variable to set threshold for a new tick

  unsigned int tickResults = 0;  //calculated tick value to return

  //time of most recent second tick based on clock intervals, returns in microseconds
  unsigned long quarterTickTime = lastClockTime + (lastClockInterval / 4);

  //time of most recent third tick based on clock intervals, returns in microseconds
  unsigned long halfTickTime = lastClockTime + (lastClockInterval / 2);

  //time of most recent fourth tick based on clock intervals, returns in microseconds
  unsigned long threeQuarterTickTime = lastClockTime + ((lastClockInterval * 3) / 4);

  //time of next predicted clock, if triggers are received after this time we stop recording.
  unsigned long nextPredictedClock = lastClockTime + lastClockInterval;

  //If the input time happened after the most recent clock then we calculate the tick.
  if (risingEdgeTime >= lastClockTime) {

    //test to show that the rising edge happened close enough to the clock edge
    if (risingEdgeTime <= (quarterTickTime - loopWindow)) tickResults = 0;

    //test to show that the rising edge happened between the clock edge and the second tick time
    else if ((risingEdgeTime >= (quarterTickTime - loopWindow)) && (risingEdgeTime < (halfTickTime - loopWindow))) tickResults = 1;

    //test to show that the rising edge happened between the second and third tick time
    else if ((risingEdgeTime >= (halfTickTime - loopWindow)) && (risingEdgeTime < (threeQuarterTickTime - loopWindow))) tickResults = 2;

    //test to show that the rising edge happened between the third and fourth tick time
    else if ((risingEdgeTime >= (threeQuarterTickTime - loopWindow)) && (risingEdgeTime < (nextPredictedClock - loopWindow))) tickResults = 3;

    //test to show that the rising edge happened past the expected next clock time.
    else if ((risingEdgeTime > (nextPredictedClock - loopWindow))) tickResults = 4;

  }

  //if the most recent rising edge happened before the most recent clock but within the loopWindow error threshold we still return a tick 0
  else if (lastClockTime - risingEdgeTime < loopWindow)
    tickResults = 0;

  //If the most recent rising edge time happened far enough in the past that it was in a previous clock we don't store the value

  else tickResults = -1;  //return -1 when there is an error

  return tickResults;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This is a function that sort of "favors" ratchet timings in order to more accurately capture ratchets.  The downside is that it might cut
//off longer triggers by a tick or extend gates by a tick longer than expected.  When loop recording is called by a timer interrupt this sort
//of thing will hopefully no longer be necessary but I think that the trade for accurate ratchet detection to give up accurate gate length
//detection is worth it.

void detectRatchets() {

  //If the normal trigger rising edge flag happens at times that are favored by x2 ratchets we prioritize saving these variables.
  if (((clockTick == 0) || (clockTick == 2)) && trig1Edge) trigHistory[0][historyIndex][clockTick] = 1;
  if (((clockTick == 0) || (clockTick == 2)) && trig2Edge) trigHistory[1][historyIndex][clockTick] = 1;
  if (((clockTick == 0) || (clockTick == 2)) && trig3Edge) trigHistory[2][historyIndex][clockTick] = 1;
  if (((clockTick == 0) || (clockTick == 2)) && trig4Edge) trigHistory[3][historyIndex][clockTick] = 1;

  //If the normal trigger falling edge flag happens at times that are favored by x2 ratchets we prioritize saving these variables.
  if (((clockTick == 1) || (clockTick == 3)) && trig1FallingEdge) trigHistory[0][historyIndex][clockTick] = 0;
  if (((clockTick == 1) || (clockTick == 3)) && trig2FallingEdge) trigHistory[1][historyIndex][clockTick] = 0;
  if (((clockTick == 1) || (clockTick == 3)) && trig3FallingEdge) trigHistory[2][historyIndex][clockTick] = 0;
  if (((clockTick == 1) || (clockTick == 3)) && trig4FallingEdge) trigHistory[3][historyIndex][clockTick] = 0;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
