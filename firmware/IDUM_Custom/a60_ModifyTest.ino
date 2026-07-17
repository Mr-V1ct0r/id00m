//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Runs the test that decides if we enter a modification
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//A lot of changes have been made to this function since the implementation of split mode.  In most cases we just run different versions of the
//same test per channel but this also requires having separate variables for each channel.  Most of the time when we are not in split mode we
//just copy the individual variables from the main variables.  This generally makes things easier to run the same code for split mode and normal
//mode but also makes things easier to save per-channel for the looper.  This also makes it so that you can actually go into the menu and turn on
//split mode during a loop and then that will actually change the behavior during a loop.

void modifyTest() {

  if (!loopEnable) {  //We only run this function when a loop isn't playing.  Otherwise we use the stored loop mode.

    errorChecking();  //some housekeeping to stop glitches

    calculateProbability();  //run the tests that decide if a modification is starting

    decideModificationStatus();  //set some variables if we are starting, ending or carrying over straight into another modification

    activateModification();  //set a bunch of variables when we do activate a modification

  }

  else loopModificationHandler();  //we now have a function that more accurately determines modification status when we are in a loop.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//For some reason I would sometimes get modes that were out of bounds.  This function makes extra sure that this doesn't happen.

void errorChecking() {

  if (modifyLength > 8) modifyLength = modifyLength % 8;
  if (modifyLength < 0) modifyLength = 0;

  //check all mode variables when we are in split mode
  if (splitMode) {
    if (modifyLength1 > 8) modifyLength1 = modifyLength1 % 8;
    if (modifyLength1 < 0) modifyLength1 = 0;

    if (modifyLength2 > 8) modifyLength2 = modifyLength2 % 8;
    if (modifyLength2 < 0) modifyLength2 = 0;

    if (modifyLength3 > 8) modifyLength3 = modifyLength3 % 8;
    if (modifyLength3 < 0) modifyLength3 = 0;

    if (modifyLength4 > 8) modifyLength4 = modifyLength4 % 8;
    if (modifyLength4 < 0) modifyLength4 = 0;
  }

  startModification = startModification1 = startModification2 = startModification3 = startModification4 = 0;
  endModification = endModification1 = endModification2 = endModification3 = endModification4 = 0;  //reset some flags
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Runs the random processes that determine if a modification will happen.

void calculateProbability() {

  int probUpperThreshold = 1000;  //global variable to alter the dead zone where a modification will always happen.

  if (clockEdge) {  //at each clock edge

    //probability modifier makes it so that when the length is long it is less likely this test is less likely to go into another modifier right away.
    //Makes it so that the prob setting reflects the percentage of time that modifiers happen more accurately if lengths are long.

    if (probabilityModifier > 0) probabilityModifier -= 1;  //reduce the probability modifier at each clock edge

    //In split mode each channel has it's own probability modifier.
    if (splitMode) {
      if (probabilityModifier1 > 0) probabilityModifier1 -= 1;  //reduce the probability modifier at each clock edge
      if (probabilityModifier2 > 0) probabilityModifier2 -= 1;  //reduce the probability modifier at each clock edge
      if (probabilityModifier3 > 0) probabilityModifier3 -= 1;  //reduce the probability modifier at each clock edge
      if (probabilityModifier4 > 0) probabilityModifier4 -= 1;  //reduce the probability modifier at each clock edge
    }

    //set the random value based on our random number generator and the probability modifier.
    //this value maxes out at the upper threshold set by probUpperThreshold so if the probability slider is above that
    //value we always get a modification.

    randomValue = min(random(probUpperThreshold) + (probabilityModifier * 90), probUpperThreshold);

    //Run the same test per channel in split mode.
    if (splitMode) {
      randomValue1 = min(random(probUpperThreshold) + (probabilityModifier1 * 90), probUpperThreshold);
      randomValue2 = min(random(probUpperThreshold) + (probabilityModifier2 * 90), probUpperThreshold);
      randomValue3 = min(random(probUpperThreshold) + (probabilityModifier3 * 90), probUpperThreshold);
      randomValue4 = min(random(probUpperThreshold) + (probabilityModifier4 * 90), probUpperThreshold);
    }

    //If we are not in split mode we usually just copy all relevant variables to the mode specific variables.  This makes saving things
    //into the looper individually per channel much easier.

    else
      randomValue1 = randomValue2 = randomValue3 = randomValue4 = randomValue;

    //If we just received a clock and the modification length is 1 then we are about to end a modification
    if (modifyLength == 1) {
      endModification = 1;                             //set the end modification flag high, mostly used to determine a carryover.
      probabilityModifier = originalModifyLength - 1;  //set the probability modifier based on the most recent modification length.
    }

    //if we are in split mode we set these variables individually per channel
    if (splitMode) {
      if (modifyLength1 == 1) {
        endModification1 = 1;                              //set the end modification flag high, mostly used to determine a carryover.
        probabilityModifier1 = originalModifyLength1 - 1;  //set the probability modifier based on the most recent modification length.
      }

      if (modifyLength2 == 1) {
        endModification2 = 1;                              //set the end modification flag high, mostly used to determine a carryover.
        probabilityModifier2 = originalModifyLength2 - 1;  //set the probability modifier based on the most recent modification length.
      }

      if (modifyLength3 == 1) {
        endModification3 = 1;                              //set the end modification flag high, mostly used to determine a carryover.
        probabilityModifier3 = originalModifyLength3 - 1;  //set the probability modifier based on the most recent modification length.
      }

      if (modifyLength4 == 1) {
        endModification4 = 1;                              //set the end modification flag high, mostly used to determine a carryover.
        probabilityModifier4 = originalModifyLength4 - 1;  //set the probability modifier based on the most recent modification length.
      }
    }

    //if we are already in the middle of a modification then decrease the modify counter at each clock edge.  Loops handle this differently.
    if (!loopEnable && (modifyLength > 0)) modifyLength -= 1;
    if (!loopEnable && (modifyLength1 > 0)) modifyLength1 -= 1;
    if (!loopEnable && (modifyLength2 > 0)) modifyLength2 -= 1;
    if (!loopEnable && (modifyLength3 > 0)) modifyLength3 -= 1;
    if (!loopEnable && (modifyLength4 > 0)) modifyLength4 -= 1;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//determine whether we are starting or ending a modification and whether we are doing a carryover straight into another modification.

void decideModificationStatus() {

  int probLowerThreshold = 15;  //global variable to alter the dead zone where a modification will never happen.

  //pressing the mode button quickly will turn off any active modifications

  if ((modeBtnFallingEdge) && modificationActive) {  //changed for split mode
    if ((modifyLength > 0) && cycleIn) {             //if the clock channel is being modified
      maintainCycle();                               //maintain the cycle if we leave early.
    }
    probabilityModifier = originalModifyLength - 1;  //set probability modifier
    modifyLength = 0;                                //end modification.

    //in split mode we need to turn off all active modifications separately.
    if (splitMode) {
      probabilityModifier1 = originalModifyLength1 - 1;  //set probability modifier
      modifyLength1 = 0;                                 //end modification.

      probabilityModifier2 = originalModifyLength2 - 1;  //set probability modifier
      modifyLength2 = 0;                                 //end modification.

      probabilityModifier3 = originalModifyLength3 - 1;  //set probability modifier
      modifyLength3 = 0;                                 //end modification.

      probabilityModifier4 = originalModifyLength4 - 1;  //set probability modifier
      modifyLength4 = 0;                                 //end modification.
    }
  }

  //runs the test to see if we are getting a modification on this step.
  //We need a clock edge, no active modification, our random value to be lower than the probability slider, the probability slider to be
  //above 15probLowerThresholdnd for IDUM to be out of menu mode.  This gives probability sliders some dead zones at the top and bottom of travel.

  if ((clockEdge && (modifyLength == 0) && (randomValue < probRead) && (probRead > probLowerThreshold) && !menuMode)) {
    startModification = 1;  //if we pass the test set the startModification flag high
  }

  //In split mode we run this test per channel.  This is the active part that handles when modifications happen in split mode.
  if (splitMode) {
    if ((clockEdge && (modifyLength1 == 0) && (randomValue1 < probRead) && (probRead > probLowerThreshold) && !menuMode)) {
      startModification1 = 1;  //if we pass the test set the startModification flag high for this channel
    }

    if ((clockEdge && (modifyLength2 == 0) && (randomValue2 < probRead) && (probRead > probLowerThreshold) && !menuMode)) {
      startModification2 = 1;  //if we pass the test set the startModification flag high for this channel
    }

    if ((clockEdge && (modifyLength3 == 0) && (randomValue3 < probRead) && (probRead > probLowerThreshold) && !menuMode)) {
      startModification3 = 1;  //if we pass the test set the startModification flag high for this channel
    }

    if ((clockEdge && (modifyLength4 == 0) && (randomValue4 < probRead) && (probRead > probLowerThreshold) && !menuMode)) {
      startModification4 = 1;  //if we pass the test set the startModification flag high for this channel
    }
  }

  //carryover indicates if we are going straight into another modification at the end of a previous modification

  if (startModification && endModification) {  //if both the start and end flags are on at the same time
    carryOver = 1;                             //we set the carryover flag high
    //B10: the chooseMode() that used to be here was a no-op - selectCurrentMode() only writes 'mode' while modificationActive == 0,
    //which a back-to-back carryover never is.  The real refresh happens after maintainCycle below (see B10 fix).
  }

  else carryOver = 0;  //otherwise we set the carryover flag low

  //In split mode we set each flag per channel
  if (splitMode) {
    if (startModification1 && endModification1) {
      carryOver1 = 1;
      mode1 = chooseMode();  //we need to run the choose mode function again because this doesn't run during an active modification.
    }

    else carryOver1 = 0;

    if (startModification2 && endModification2) {
      carryOver2 = 1;
      mode2 = chooseMode();  //we need to run the choose mode function again because this doesn't run during an active modification.
    }

    else carryOver2 = 0;

    if (startModification3 && endModification3) {
      carryOver3 = 1;
      mode3 = chooseMode();  //we need to run the choose mode function again because this doesn't run during an active modification.
    }

    else carryOver3 = 0;

    if (startModification4 && endModification4) {
      carryOver4 = 1;
      mode4 = chooseMode();  //we need to run the choose mode function again because this doesn't run during an active modification.
    }

    else carryOver4 = 0;
  }

  //'maintainCycle' maintains the cycle position of external sequencers hooked up to IDUMs clock output.
  //We only need to run it if the most recent modification was longer than 1 step or if we were in skip clock mode and also only if the
  //cycle switch is on.

  //we are only doing this on the main mode for split mode because the main modification is the only one where we modify the clock
  if (endModification && ((originalModifyLength > 1) || modeOwnsClock(mode)) && !loopEnable && cycleIn) {
    maintainCycle();
  }

  //B10 fix: refresh 'mode' for a non-split carryover's NEW modification.  modePosition already holds the live dial position (the main
  //loop runs chooseMode() before modifyTest), so we assign it directly.  Done AFTER maintainCycle above so that call still sees the
  //ENDING modification's mode, and scoped to !splitMode because the split trigger channels already capture chooseMode() into modeN.
  if (carryOver && !splitMode) mode = modePosition;

  //If we are running a modification on any channel then we set a flag high to make it easy to test.  Before we just checked that the modifyLength
  //parameter was above 0 for the main modification.  Now it needs to be done for all channels and this flag makes it easier to detect.
  if ((modifyLength > 0) || (modifyLength1 > 0) || (modifyLength2 > 0) || (modifyLength3 > 0) || (modifyLength4 > 0)) modificationActive = 1;
  else modificationActive = 0;

  //Does a similar thing to figure out when a carryover is happening on any channel.
  if ((carryOver > 0) || (carryOver1 > 0) || (carryOver2 > 0) || (carryOver3 > 0) || (carryOver4 > 0)) carryOverActive = 1;
  else carryOverActive = 0;

  if (carryOverActive) LEDCounter = 10;  //prevents a visual glitch during carryovers
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//sets a bunch of variables whenever we start a new modification.

void activateModification() {

  unsigned long currentMicros = micros();  //calculates the current time once only for efficiency

  //even in split mode we test for the "main" modifications to decide if we are modifying the clock.  I will probably make it so that the "main"
  //modification controls both the clock and channel 1 together or at least make that an advanced option.  Because of this the cycle maintenance
  //variables are only touched when we are activating the "main" modification function.

  if (startModification) {  //if we have decided to start a modification

    //Check for the freeze param flag
    if (freezeParam) {
      modifyParam = paramRead;  //if Freeze Param is set high we still need to sample this once

      //and then we copy the modify parameters into each channel's individual variables.
      //Any modifications that activate at the same time will have the same modification variables.
      modifyParam1 = modifyParam2 = modifyParam3 = modifyParam4 = modifyParam;
      unscaledParam1 = unscaledParam2 = unscaledParam3 = unscaledParam4 = unscaledParam;
      ratchetAmount1 = ratchetAmount2 = ratchetAmount3 = ratchetAmount4 = ratchetAmount;
    }

    //sample the length input and then record what the original length is for later
    modifyLength = lengthScaled;          //this variable decreases once per clock until it is 0, then the modification ends.
    originalModifyLength = modifyLength;  //this keeps track of the overall modification length for maintain cycle and some other scaling

    //keeps track of the beginning time of the modification for sending out the first clock
    //we only need to keep track of this on the main modification channel
    if (clockEdge) firstStepTime = currentMicros;
    else
      firstStepTime = lastClockTime;  //this will be useful once we allow modifications to start between clocks

    //reset some other mode specific variables

    if (modeOwnsClock(mode)) {
      setClockSkipMode();          //skip mode only really effects the clock channel so we only have to check the main mode
      if (clockIn) forceSkip = 1;  //fixes a bug where skip does not happen on the first step of a modification

      //reset the clock skip variables which help with cycle maintenance
      clockSkipCycleCount = 0;
      clockRatchetCycleCount = 0;
      clockRatchetStateOld = 0;
    }

    //if we aren't going straight from one modification to another or if the carryOverDisable option is set high on the first page

    if (!splitMode) {                                                              //if we are not in split mode we set all of these variables at once, otherwise we set them individually
      if (!(carryOver && !carryOverDisable)) {                                     //if carryOver is disabled we do this no matter what
        breakIndex1 = breakIndex2 = breakIndex3 = breakIndex4 = 0;                 //reset all of the break beat counters
        trig1Divide = trig2Divide = trig3Divide = trig4Divide = clockDivide = -1;  //B4 fix: was 'trig4Divide' twice with trig3Divide missing, so ch3's divide counter (e.g. ball mode) leaked from the previous modification
        bounce1Time = bounce2Time = bounce3Time = bounce4Time = 0;                 //reset the bouncing ball counters
        ratchetDivCount1 = ratchetDivCount2 = ratchetDivCount3 = ratchetDivCount4 = 0;  //CUSTOM: restart the ratchet-CCW divider so the first hit of each modification passes
        scatterCount1 = scatterCount2 = scatterCount3 = scatterCount4 = 0;              //CUSTOM: clear any stale scatter schedule so channels pass on-spot until their first edge (see b82)

        //chokes make it so that each output only turns on after a trigger is received during each modification
        //normally we ignore these during a carryOver.  This can also be disabled with a special option on the first page.
        if (!chokeDisable) {
          if (!trig1In) trig1Choke = 0;
          if (!trig2In) trig2Choke = 0;
          if (!trig3In) trig3Choke = 0;
          if (!trig4In) trig4Choke = 0;
        }
      }

      ///////////////////////////////////////////////////////////////////////////////////////
      //CALCULATE MODE SPECIFIC VARIABLES AT MODIFICATION START TO HELP WITH BUG PREVENTION//
      ///////////////////////////////////////////////////////////////////////////////////////

      //doing a new random generation for each modification start helps get rid of some weird cases where triggers are a little early
      if (mode == 0) {
        hold1Choke = (random(450) < modifyParam);
        hold2Choke = (random(450) < modifyParam);
        hold3Choke = (random(450) < modifyParam);
        hold4Choke = (random(450) < modifyParam);
      }

      // if we are in ball mode double check the inputs, helps deal with triggers that happen slightly before the clock
      forceBounce1 = forceBounce2 = forceBounce3 = forceBounce4 = 0;
      if (mode == 3) {
        if (trig1In) forceBounce1 = 1;
        if (trig2In) forceBounce2 = 1;
        if (trig3In) forceBounce3 = 1;
        if (trig4In) forceBounce4 = 1;
      }

      //if we are not in split mode then we just copy the variables from the main modification into the individual channel variables
      mode1 = mode2 = mode3 = mode4 = mode;
      modifyLength1 = modifyLength2 = modifyLength3 = modifyLength4 = modifyLength;
      originalModifyLength1 = originalModifyLength2 = originalModifyLength3 = originalModifyLength4 = originalModifyLength;
    }
  }

  //if we are in split mode we calculate the modification variables for each channel independently
  if (splitMode) {

    if (startModification1) {  //if we are starting a new modification
      mode1 = chooseMode();    //we check the mode again just to be sure
      if (freezeParam) {
        modifyParam1 = paramRead;  //if Freeze Param is set high we still need to sample this once
        unscaledParam1 = unscaledParam;
        ratchetAmount1 = ratchetAmount;
      }
      //sample the length input and then record what the original length is for later
      modifyLength1 = lengthScaled;           //this variable decreases once per clock until it is 0, then the modification ends.
      originalModifyLength1 = modifyLength1;  //this keeps track of the overall modification length for maintain cycle and some other scaling

      //Check the individual mode specific variables for bug squashing
      if (mode1 == 0) hold1Choke = (random(450) < modifyParam);
      if ((mode1 == 3) && trig1In) forceBounce1 = 1;
      else forceBounce1 = 0;

      if (!(carryOver1 && !carryOverDisable)) {
        breakIndex1 = 0;       //reset all of the break beat counters
        trig1Divide = -1;      //reset the clock divide counters
        bounce1Time = 0;       //reset the bouncing ball counters
        ratchetDivCount1 = 0;  //CUSTOM: restart the ratchet-CCW divider (see b30)
        scatterCount1 = 0;     //CUSTOM: clear any stale scatter schedule (see b82)

        //chokes make it so that each output only turns on after a trigger is received during each modification
        //normally we ignore these during a carryOver.  This can also be disabled with a special option on the first page.
        if (!chokeDisable) {
          if (!trig1In) trig1Choke = 0;
        }
      }
    }

    if (startModification2) {
      mode2 = chooseMode();
      if (freezeParam) {
        modifyParam2 = paramRead;  //if Freeze Param is set high we still need to sample this once
        unscaledParam2 = unscaledParam;
        ratchetAmount2 = ratchetAmount;
      }
      //sample the length input and then record what the original length is for later
      modifyLength2 = lengthScaled;           //this variable decreases once per clock until it is 0, then the modification ends.
      originalModifyLength2 = modifyLength2;  //this keeps track of the overall modification length for maintain cycle and some other scaling

      if (mode2 == 0) hold2Choke = (random(450) < modifyParam);
      if ((mode2 == 3) && trig2In) forceBounce2 = 1;
      else forceBounce2 = 0;

      if (!(carryOver2 && !carryOverDisable)) {
        breakIndex2 = 0;       //reset all of the break beat counters
        trig2Divide = -1;      //reset the clock divide counters
        bounce2Time = 0;       //reset the bouncing ball counters
        ratchetDivCount2 = 0;  //CUSTOM: restart the ratchet-CCW divider (see b30)
        scatterCount2 = 0;     //CUSTOM: clear any stale scatter schedule (see b82)

        //chokes make it so that each output only turns on after a trigger is received during each modification
        //normally we ignore these during a carryOver.  This can also be disabled with a special option on the first page.
        if (!chokeDisable) {
          if (!trig2In) trig2Choke = 0;
        }
      }
    }

    if (startModification3) {
      mode3 = chooseMode();
      if (freezeParam) {
        modifyParam3 = paramRead;  //if Freeze Param is set high we still need to sample this once
        unscaledParam3 = unscaledParam;
        ratchetAmount3 = ratchetAmount;
      }
      //sample the length input and then record what the original length is for later
      modifyLength3 = lengthScaled;           //this variable decreases once per clock until it is 0, then the modification ends.
      originalModifyLength3 = modifyLength3;  //this keeps track of the overall modification length for maintain cycle and some other scaling

      if (mode3 == 0) hold3Choke = (random(450) < modifyParam);
      if ((mode3 == 3) && trig3In) forceBounce3 = 1;
      else forceBounce3 = 0;

      if (!(carryOver3 && !carryOverDisable)) {
        breakIndex3 = 0;       //reset all of the break beat counters
        trig3Divide = -1;      //reset the clock divide counters
        bounce3Time = 0;       //reset the bouncing ball counters
        ratchetDivCount3 = 0;  //CUSTOM: restart the ratchet-CCW divider (see b30)
        scatterCount3 = 0;     //CUSTOM: clear any stale scatter schedule (see b82)

        //chokes make it so that each output only turns on after a trigger is received during each modification
        //normally we ignore these during a carryOver.  This can also be disabled with a special option on the first page.
        if (!chokeDisable) {
          if (!trig3In) trig3Choke = 0;
        }
      }
    }

    if (startModification4) {
      mode4 = chooseMode();
      if (freezeParam) {
        modifyParam4 = paramRead;  //if Freeze Param is set high we still need to sample this once
        unscaledParam4 = unscaledParam;
        ratchetAmount4 = ratchetAmount;
      }
      //sample the length input and then record what the original length is for later
      modifyLength4 = lengthScaled;           //this variable decreases once per clock until it is 0, then the modification ends.
      originalModifyLength4 = modifyLength4;  //this keeps track of the overall modification length for maintain cycle and some other scaling

      if (mode4 == 0) hold4Choke = (random(450) < modifyParam);
      if ((mode4 == 3) && trig4In) forceBounce4 = 1;
      else forceBounce4 = 0;

      if (!(carryOver4 && !carryOverDisable)) {
        breakIndex4 = 0;       //reset all of the break beat counters
        trig4Divide = -1;      //reset the clock divide counters
        bounce4Time = 0;       //reset the bouncing ball counters
        ratchetDivCount4 = 0;  //CUSTOM: restart the ratchet-CCW divider (see b30)
        scatterCount4 = 0;     //CUSTOM: clear any stale scatter schedule (see b82)

        //chokes make it so that each output only turns on after a trigger is received during each modification
        //normally we ignore these during a carryOver.  This can also be disabled with a special option on the first page.
        if (!chokeDisable) {
          if (!trig4In) trig4Choke = 0;
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//The previous looper modification calculator was a bit basic and would trigger sometimes when it didn't need to.  We now save a lot more data
//about currently running modifications into the looper making it reflect the recorded behavior much more accurately.  This made adding a loop
//modification specific function make a lot more sense.

void loopModificationHandler() {

  unsigned int modifyLengthTest = 0;  //temporary parameter to load variables from the paramHistory array

  int probLowerThreshold = 15;  //global variable to alter the dead zone where a modification will never happen.

  //B3 fix: recorded gates and recorded modifications are BOTH written at 'historyIndex' (see a42_RecordTriggers), so they must be
  //read back at the same absolute buffer address.  readLoopMemory (a41) already translates the relative loop position to an absolute
  //address with (loopIndex + originalHistoryIndex) % 8, but the modification reads below used a bare 'loopIndex', leaving the recorded
  //modifications rotated away from the gates they belong to by whatever the buffer phase was when the loop engaged.  Use the same
  //absolute index here so a step's modification lines up with that step's gates.
  unsigned int loopReadIndex = (loopIndex + originalHistoryIndex) % 8;

  //paramHistory [ trigg channel ] [ step ] [ param-to-save ( mode/ modifyParam/ modifyLength/ originalModifyLength ) ]

  //if we have changed the loop index for any reason we run a test to see if we should run modifications on any channels
  if (loopIndexChanged) {

    if (!splitMode) {  //un-split mode is faster to calculate so we do it first

      //we grab the recorded 'modifyLength' param to only run modifications on steps that actually had a modification recorded
      modifyLengthTest = paramHistory[0][loopReadIndex][2];

      //run a different version of the modifyTest using the stored looper and loop randomness parameters
      if ((modifyLengthTest > 0) && (loopRandom < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        //If we pass the modification test we load all saved variables into the modification variables.
        mode = paramHistory[0][loopReadIndex][0];
        modifyParam = paramHistory[0][loopReadIndex][1];
        modifyLength = modifyLengthTest;
        originalModifyLength = paramHistory[0][loopReadIndex][3];

        //We need to calculate some of these variables again per modification, we simply replace the global variables as they have
        //already been used in any relevant functions and from here on out in the code will only be used for modifications.
        //This is much more elegant in the modification code than having a separate set of looper specific variables and tests.

        //Super based scaling of the modifyParam variable.  We don't need to add the hysteresis since this is only saved once per clock.
        unscaledParam = modifyParam / 64;
        if (unscaledParam > 15) unscaledParam = 15;  //B8: keep the index inside the 16-entry table (modifyParam/64 tops out at 15, but clamp defensively)

        //B8 fix: index the SAME mirrored tables the live path uses (a30), so a looped ratchet replays exactly as recorded.  The old
        //(unscaledParam % 8)+1 / [% 8] folded the knob into a sawtooth (and half-table for even/pwr2), flipping half the PARAM travel vs live.
        switch (paramResolution) {
          case 0:
            ratchetAmount = odd[unscaledParam];
            break;
          case 1:
            ratchetAmount = even[unscaledParam];
            break;
          case 2:
            ratchetAmount = powerOf2[unscaledParam];
            break;
        }
      }

      else modifyLength = 0;  //turns off the modification for all channels if the test comes back negative

      //copy all of the above parameters into all of the other mode variables since we are not in split mode
      mode1 = mode2 = mode3 = mode4 = mode;
      modifyParam1 = modifyParam2 = modifyParam3 = modifyParam4 = modifyParam;
      modifyLength1 = modifyLength2 = modifyLength3 = modifyLength4 = modifyLength;
      originalModifyLength1 = originalModifyLength2 = originalModifyLength3 = originalModifyLength4 = originalModifyLength;
      unscaledParam1 = unscaledParam2 = unscaledParam3 = unscaledParam4 = unscaledParam;
      ratchetAmount1 = ratchetAmount2 = ratchetAmount3 = ratchetAmount4 = ratchetAmount;

    }

    //If we are in split mode we run the modification test for each trigger channel separately.  Currently the un-numbered mode parameters select the clock
    //in split mode which is ignored in the looper.  Eventually we will add support for skip mode in the looper where this will matter more.

    //B2 fix: the guards below test modifyLengthTest, which is unsigned, so the original '>= 0' was ALWAYS true - meaning split-mode
    //loop playback re-fired a modification on every step even where none was recorded (contradicting the "only run modifications on
    //steps that actually had a modification recorded" comment).  Changed to '> 0' to match the non-split path above.

    else {

      //this makes it so that we only run modifications on steps that actually had a modification recorded
      modifyLengthTest = paramHistory[0][loopReadIndex][2];

      if ((modifyLengthTest > 0) && (loopRandom < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        mode = paramHistory[0][loopReadIndex][0];
        modifyParam = paramHistory[0][loopReadIndex][1];
        modifyLength = modifyLengthTest;
        originalModifyLength = paramHistory[0][loopReadIndex][3];

        //some mode specific variables
        unscaledParam = modifyParam / 64;
        if (unscaledParam > 15) unscaledParam = 15;  //B8: keep the index inside the 16-entry table
        //B8 fix: use the SAME mirrored tables as the live path (a30) so looped ratchets replay as recorded (was a sawtooth via % 8).
        switch (paramResolution) {
          case 0:
            ratchetAmount = odd[unscaledParam];
            break;
          case 1:
            ratchetAmount = even[unscaledParam];
            break;
          case 2:
            ratchetAmount = powerOf2[unscaledParam];
            break;
        }
      }

      else modifyLength = 0;  //turns off modification just for this channel

      modifyLengthTest = paramHistory[1][loopReadIndex][2];  //this makes it so that we only run modifications on steps that actually had a modification recorded

      if ((modifyLengthTest > 0) && (loopRandom1 < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        mode1 = paramHistory[1][loopReadIndex][0];
        modifyParam1 = paramHistory[1][loopReadIndex][1];
        modifyLength1 = modifyLengthTest;
        originalModifyLength1 = paramHistory[1][loopReadIndex][3];

        //some mode specific variables
        unscaledParam1 = modifyParam1 / 64;
        if (unscaledParam1 > 15) unscaledParam1 = 15;  //B8: keep the index inside the 16-entry table
        //B8 fix: use the SAME mirrored tables as the live path (a30) so looped ratchets replay as recorded (was a sawtooth via % 8).
        switch (paramResolution) {
          case 0:
            ratchetAmount1 = odd[unscaledParam1];
            break;
          case 1:
            ratchetAmount1 = even[unscaledParam1];
            break;
          case 2:
            ratchetAmount1 = powerOf2[unscaledParam1];
            break;
        }
      }

      else modifyLength1 = 0;  //turns off modification just for this channel

      modifyLengthTest = paramHistory[2][loopReadIndex][2];  //this makes it so that we only run modifications on steps that actually had a modification recorded

      if ((modifyLengthTest > 0) && (loopRandom2 < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        mode2 = paramHistory[2][loopReadIndex][0];
        modifyParam2 = paramHistory[2][loopReadIndex][1];
        modifyLength2 = modifyLengthTest;
        originalModifyLength2 = paramHistory[2][loopReadIndex][3];

        //some mode specific variables
        unscaledParam2 = modifyParam2 / 64;
        if (unscaledParam2 > 15) unscaledParam2 = 15;  //B8: keep the index inside the 16-entry table
        //B8 fix: use the SAME mirrored tables as the live path (a30) so looped ratchets replay as recorded (was a sawtooth via % 8).
        switch (paramResolution) {
          case 0:
            ratchetAmount2 = odd[unscaledParam2];
            break;
          case 1:
            ratchetAmount2 = even[unscaledParam2];
            break;
          case 2:
            ratchetAmount2 = powerOf2[unscaledParam2];
            break;
        }
      }

      else modifyLength2 = 0;  //turns off modification just for this channel

      modifyLengthTest = paramHistory[3][loopReadIndex][2];  //this makes it so that we only run modifications on steps that actually had a modification recorded

      if ((modifyLengthTest > 0) && (loopRandom3 < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        mode3 = paramHistory[3][loopReadIndex][0];
        modifyParam3 = paramHistory[3][loopReadIndex][1];
        modifyLength3 = modifyLengthTest;
        originalModifyLength3 = paramHistory[3][loopReadIndex][3];

        //some mode specific variables
        unscaledParam3 = modifyParam3 / 64;
        if (unscaledParam3 > 15) unscaledParam3 = 15;  //B8: keep the index inside the 16-entry table
        //B8 fix: use the SAME mirrored tables as the live path (a30) so looped ratchets replay as recorded (was a sawtooth via % 8).
        switch (paramResolution) {
          case 0:
            ratchetAmount3 = odd[unscaledParam3];
            break;
          case 1:
            ratchetAmount3 = even[unscaledParam3];
            break;
          case 2:
            ratchetAmount3 = powerOf2[unscaledParam3];
            break;
        }
      }

      else modifyLength3 = 0;  //turns off modification just for this channel

      modifyLengthTest = paramHistory[4][loopReadIndex][2];  //this makes it so that we only run modifications on steps that actually had a modification recorded

      if ((modifyLengthTest > 0) && (loopRandom4 < probRead) && (probRead > probLowerThreshold) && !menuMode) {

        mode4 = paramHistory[4][loopReadIndex][0];
        modifyParam4 = paramHistory[4][loopReadIndex][1];
        modifyLength4 = modifyLengthTest;
        originalModifyLength4 = paramHistory[4][loopReadIndex][3];

        //some mode specific variables
        unscaledParam4 = modifyParam4 / 64;
        if (unscaledParam4 > 15) unscaledParam4 = 15;  //B8: keep the index inside the 16-entry table
        //B8 fix: use the SAME mirrored tables as the live path (a30) so looped ratchets replay as recorded (was a sawtooth via % 8).
        switch (paramResolution) {
          case 0:
            ratchetAmount4 = odd[unscaledParam4];
            break;
          case 1:
            ratchetAmount4 = even[unscaledParam4];
            break;
          case 2:
            ratchetAmount4 = powerOf2[unscaledParam4];
            break;
        }
      }

      else modifyLength4 = 0;  //turns off modification just for this channel
    }
  }

  //We need to run these tests again after the looper modification calculations
  if ((modifyLength > 0) || (modifyLength1 > 0) || (modifyLength2 > 0) || (modifyLength3 > 0) || (modifyLength4 > 0)) modificationActive = 1;
  else modificationActive = 0;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
