//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This handles all of the intricacies of replaying the gates saved in the loop
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loopTriggers() {

  calculateLooperVariables();  //calculates the looper variables especially the position of the current loop

  sendIndexClocks();  //sends out clocks to loop an attached sequencer

  readLoopMemory();  //gets the current gates saved in the current loop step

  saveLoopVariables();  //saves all of the current looper variables
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//does the heavy lifting of figuring out where in the loop we are

void calculateLooperVariables() {

  unsigned long currentMicros = micros();  //calculates the current time once only for efficiency

  //if we aren't in the menu collect the analog inputs
  if (!menuMode) loopLength = lengthScaled;
  if (!menuMode) loopOffset = wrapAround(modeDialPosition - originalLoopOffset, 8);
  if (!menuMode) loopRatchetAmount = ratchetAmount;
  if (!menuMode) loopClockSpeed = unscaledParam;

  //if we get a rising edge at the clock input
  if (clockEdge) {
    loopDivide = (loopDivide + 1) % (loopRatchetAmount);  //increase the counter that handles dividing the input clock
    loopCycleCount += 1;                                  //increase the counter that tracks how many clocks have passed to maintain the cycle
  }

  //if the param knob is clockwise of noon we are multiplying the speed of the clock
  if (loopClockSpeed > 9) {

    //Here we multiply the clock with a modulo function.  The time since the last clock rising edge is wrapped by a division of the last
    //clock interval.  If the clock speed isn't changing this gives a pretty good multiplication of the clock.  We give a high output when
    //the result of this calculation is less than 5000 for a 5 ms trigger to hopefully allow really fast clock multiplications.

    loopClockMult = (((currentMicros - lastClockTime) % (lastClockInterval / (loopRatchetAmount))) < 5000);

  }

  //if the clock is close to noon the clock output mirrors the clock input.  We have a big dead-zone to make normal clock operation easy to find.
  else if ((loopClockSpeed == 9) || (loopClockSpeed == 8) || (loopClockSpeed == 7)) {
    loopClockMult = clockIn;
    tickIndex = clockTick;
  }

  //if the param knob is counter-clockwise of noon we divide the clock
  else {

    //if the "loop divide" counter is 0 it means that we have counted past a number of clocks detemined by the "ratchetamount" parameter and
    //wrapped back around to 0.  Then we use the second term to turn this into a trigger.

    loopClockMult = (loopDivide == 0) && ((currentMicros - lastClockTime) < 5000);
  }

  //if the loop length is at its minimum and the param knob is at exactly noon we just turn off the looper clock to allow easy loop scrubbing.
  if ((loopLength == 1) && (loopClockSpeed == 8)) {
    loopClockMult = 0;
    tickIndex = 0;
  }

  //if the looper clock has a rising edge
  if (loopClockMult && !loopClockMultOld) {
    loopCycleIndex = ((loopCycleIndex + 1) % loopLength);       //advance one step in the loop
    loopClockMultInterval = currentMicros - loopClockMultTime;  //record the interval between loop clocks
    loopClockMultTime = currentMicros;                          //record the time of the most recent loop clock
  }

  //If we are not in the param knob close to noon dead zone we multiply the most recent loop interval by 4 to detect the current
  //multiplied or divided clock tick.  This works quite well at fast and slow speed but requires the loop to count at least one
  //clock in order to update it's time.
  if ((loopClockSpeed != 7) && (loopClockSpeed != 8) && (loopClockSpeed != 9)) {
    tickIndex = (4 * (currentMicros - loopClockMultTime)) / (loopClockMultInterval);
  }

  if (loopLength == 1) loopIndex = loopOffset;         //if the loop length is at minimum the loop index is set to be the loop offset
  else loopIndex = (loopOffset + loopCycleIndex) % 8;  //otherwise we add the loop index count to the loop offset and wrap it between 0 and 7.

  //if we have changed the loop Index calculate a new random value
  if (loopIndex != loopIndexOld) {
    loopIndexChanged = 1;
    loopRandom = random(950);

    //In split mode we calculate the random value per channel
    if (splitMode) {
      loopRandom1 = random(950);
      loopRandom2 = random(950);
      loopRandom3 = random(950);
      loopRandom4 = random(950);
    }

    else loopRandom1 = loopRandom2 = loopRandom3 = loopRandom4 = loopRandom;  //otherwise we copy the main random value to each channel

  }

  else loopIndexChanged = 0;  //Makes this easy to tell when the index has changed so we don't have to run a ton of calculations every loop.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function sends out clock skips in order to manipulate an external sequencer to reflect the behavior of the looper.

void sendIndexClocks() {

  if (loopIndex != loopIndexOld) {  //if the loop index has changed

    if (loopIndex > loopIndexOld) {  //if the index has increased step forward by the change in looper index
      skipSteps(loopIndex - loopIndexOld - !analogClock);
    }

    else {
      skipSteps(8 - (loopIndexOld - loopIndex) - !analogClock);  //if the index has decreased step backward by the change in looper index
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//reads the looper gates from the trigger history array

void readLoopMemory() {

  //loop index needs to be offset with the history index which keeps track of which array was written to most recently.
  //tick index is unimplemented but will be used once the looper resolution has been increased.

  trig1In = trigHistory[0][(loopIndex + originalHistoryIndex) % 8][tickIndex];
  trig2In = trigHistory[1][(loopIndex + originalHistoryIndex) % 8][tickIndex];
  trig3In = trigHistory[2][(loopIndex + originalHistoryIndex) % 8][tickIndex];
  trig4In = trigHistory[3][(loopIndex + originalHistoryIndex) % 8][tickIndex];
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//saves all relevant looper variables for edge detection and other tests running in the next loop.

void saveLoopVariables() {

  loopClockMultOld = loopClockMult;
  loopOffsetOld = loopOffset;
  loopIndexOld = loopIndex;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
