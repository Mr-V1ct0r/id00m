//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function handles all of the intricacies of sending clock bursts to catch us up to the original cycle position
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//True when the given mode index drives the CLOCK output (and therefore needs the special skip-mode branch of the cycle engine).
//Slot 7 used to be CLOCK SKIP, but it now hosts a custom trigger mode (SCATTER, see b82), so NO mode manipulates the clock
//output any more - this returns false for everything.  The cycle engine keys off this predicate: if you ever put a clock-driving mode
//back into a slot, return true for that index here and the follow/catch-up logic will treat it correctly.
bool modeOwnsClock(unsigned int m) {
  return false;
}

//Called from a bunch of different functions to keep track of the current sequence position and send out a burst of clocks at the end of a
//modification to catch us up to wherever we are supposed to be in the original sequence.

void maintainCycle() {

  unsigned int cycleNumSteps = 0;
  unsigned int stepsLeft = originalModifyLength - modifyLength + (modifyLength > 0);  //this variable helps with exiting skip mode early

  //calculates the relative sequencer position when we have left a skip mode
  unsigned int skipModeStepsPassed = (clockSkipCycleCount + (clockRatchetCycleCount)) % 8;

  if (modeOwnsClock(mode)) {

    if (skipModeStepsPassed != stepsLeft) {                     //if we haven't magically skipped to where we are already supposed to be
      if (skipModeStepsPassed > stepsLeft) {                    //if we passed the step we are supposed to be on
        cycleNumSteps = 8 - (skipModeStepsPassed - stepsLeft);  //we have to skip backwards
      } else {
        cycleNumSteps = (stepsLeft - skipModeStepsPassed);  //otherwise we skip forwards the difference
      }
    }

    //this section executes if we happen to have skipped the same number of steps that we were supposed to pass
    else {
      if (clockRatchetStateOld) {      //if the 'clock ratchet' was high when we exited a clock skip
        digitalWrite(clockOut, HIGH);  //make absolutely sure the clock output goes low
        digitalWrite(clockOut, HIGH);
        digitalWrite(clockOut, HIGH);
        clockChoke = 0;  //and activate the clock choke
      } else {
        cycleNumSteps = 0;  //otherwise just ignore the skip function
      }
    }
  }

  else {  //if we are in any other modes besides skip mode

    if (originalModifyLength == 0) {  //if our original length is 0 we skip the modification (this is just for error checking)
      cycleNumSteps = 0;
    }

    else if (modifyLength == 0) {  //if we went through the entire modification
      //we send out as many clocks as the length of the modification minus 1
      //this is because we have already sent a clock at the start of each modification
      cycleNumSteps = originalModifyLength - 1;
    } else {  //if we left a modification early
      //we skip the number of steps left in our modification taking into account the number of steps already passed
      cycleNumSteps = originalModifyLength - modifyLength;
    }
  }

  //if we are on a different step than we are supposed to be on
  if (cycleNumSteps > 0) {
    skipSteps(cycleNumSteps);  //skip steps until we have caught up.
  }

  skipModeCarryOverCount = 0;  //Placeholder variable right now
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//A version of the maintain cycle function made specifically to handle the looper's clock manipulation

void loopCycle() {

  unsigned int cycleNumSteps = 0;

  if (loopCycleCount != loopIndex) {         //if the number of steps that we have passed during the loop is different from the current loop step.
    if ((loopCycleCount % 8) > loopIndex) {  //if we are behind where we are supposed to be in the loop
      //we skip forwards by the difference between the step we are supposed to be on and the current loop step.
      cycleNumSteps = (loopCycleCount % 8) - loopIndex;
    } else {  //if we are in front of where we are supposed to be in the loop
      //we skip backwards by the difference between the step we are supposed to be in and the current loop step.
      cycleNumSteps = 8 - (loopIndex - (loopCycleCount % 8));
    }

    if (clockIn) {         //if the clock input is still high
      cycleNumSteps -= 1;  //we decrease the number of steps to skip by 1
    }

    //If we are in the slow clock speed decrease the cycle length by 1
    if (!analogClock && (cycleNumSteps > 0)) cycleNumSteps -= 1;

    if (cycleNumSteps > 0) {     //if we are on a different step than we are supposed to be on
      skipSteps(cycleNumSteps);  //skip steps until we have caught up.
    }
  }

  loopCycleCount = 0;  //reset the counter that keeps track of how many clocks have passed during a loop.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function handles all of the clock skipping that occurs throughout various parts of IDUM.
//It has two modes for how to do this, which mode is in use is determined by the "analog clock" variable
//Setting this high makes the clock bursts as fast as possible
//Setting this low makes the clock bursts slow down to work with slower sequencers.

void skipSteps(unsigned int numSkips) {

  if (analogClock) {               //if we are in the faster clock speed
    digitalWrite(clockOut, HIGH);  //make absolutely sure that the clock output is low
    digitalWrite(clockOut, HIGH);
    digitalWrite(clockOut, HIGH);

    //then we alternate between a high an low clock as fast as possible for the number of steps we have to skip
    for (unsigned int i = 0; i < numSkips; i++) {
      digitalWrite(clockOut, LOW);
      digitalWrite(clockOut, HIGH);
    }

    digitalWrite(clockOut, HIGH);  //make absolutely sure that the clock output is low
    digitalWrite(clockOut, HIGH);
    digitalWrite(clockOut, HIGH);

    clockChoke = 1;
  }

  else {                                               //if we are in the slower clock speed
    if (cycleActive) {                                 //and there is currently a slow speed skip active
      cycleCount = cycleCount + ((numSkips + 1) * 2);  //we add the number of steps to skip to the current skip    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<this could use some improvement
    } else {                                           //if we are starting a new clock skip
      clockChoke = 0;                                  //we disable the normal clock operation
      cycleCount = (numSkips + 1) * 2;                 //we set the counter to the number of steps that we want to skip
      cycleActive = 1;                                 //set the cycle flag high to make sure we know that a cycle is already active
      cTimer.every(slowClockSpeed, slowClockTimer);    //then we activate the slow speed cycle timer interupt.
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
