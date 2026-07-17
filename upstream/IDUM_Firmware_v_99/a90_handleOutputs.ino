//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Sets the output gates to their calculated states
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function also went through a fair number of changes.  We now run the modification code multiple times in order to enable split mode
//behavior.  We considered having each modification code be a function that was called with the trigger channel provided as an input variable
//and the trigger results returned as a bool but I decided that the code would be way too different from previous versions.  This would make it
//not only so that people who had spent some time with the code (hello anyone who did that) would have to relearn everything but also make
//a lot more work for me.  Code for each individual modes now has tests to determine which channels are currently included in the mode and then
//sets the relevant output variables only for those channels.

void handleOutputs() {

  calculateTriggerStates();  //determine which variables set the output states

  outputGates();  //send messages to the hardware to turn the outputs high or low
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void calculateTriggerStates() {

  //if there are no modifications in progress, set the clock and trigger outputs to ther corresponding gate inputs

  if (modifyLength == 0) {  //if no modification is active on this channel
    clockState = clockIn;   //pass the inputs straight to the outputs
    clockChoke = 1;         //and turn off all chokes
  }
  if (modifyLength1 == 0) {
    trig1State = trig1In;
    trig1Choke = 1;
  }
  if (modifyLength2 == 0) {
    trig2State = trig2In;
    trig2Choke = 1;
  }
  if (modifyLength3 == 0) {
    trig3State = trig3In;
    trig3Choke = 1;
  }
  if (modifyLength4 == 0) {
    trig4State = trig4In;
    trig4Choke = 1;
  }

  if (modificationActive) {  //if a modification is active on any channel we need to calculate the output gates.

    if (clockEdge && !cycleActive) clockChoke = 1;  //clock outputs are disabled when a slow cycle is active

    //We need to check 8 times for an active modification
    for (int i = 0; i < 8; i++) {

      //If any of the channels modes are set to the one of the 8 channels being checked
      if ((mode == i) || (mode1 == i) || (mode2 == i) || (mode3 == i) || (mode4 == i)) {
        switch (i) {  //if a modification is being in progress then call the appropriate modification function based on the current mode.

          case 0:  //HOLD MODE
            hold();
            break;

          case 1:  //BURST MODE
            burst();
            break;

          case 2:  //RATCHET MODE
            ratchet();
            break;

          case 3:  //BOUNCING BALL MODE
            ball();
            break;

          case 4:  //ROTATE MODE
            rotate();
            break;

          case 5:  //DELAY MODE
            trigDelay();
            break;

          case 6:  //BREAK MODE
            breakBeat();
            break;

          case 7:  //CLOCK SKIP MODE
            skip();
            break;

          default:  //call a break just in case
            break;
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//set the outputs based on the variables returning from the modification functions

void outputGates() {

  if (loopEnable) clockState = 0;  //if the looper is active the clock outputs are written to directly from the loop function

  if (!cycleActive) {  //if we aren't in the middle of a slow cycle maintain loop
    if (clockState) {  //write to the clock digital output based on the calculated "clockstate" variable
      if (clockChoke)
        digitalWrite(clockOut, LOW);  //if clock choke is on another function is controlling the clock state directly
    } else {
      if (clockChoke)
        digitalWrite(clockOut, HIGH);  //so we don't write to the clock pin here.
    }
  }

  if (trig1State && trig1Choke) {     //for all other outputs turning the clock choke off means that the output should stay off
    digitalWriteFast(trig1Out, LOW);  //write to each trigger output based on the calculated 'trigXState' variables
  } else {
    digitalWriteFast(trig1Out, HIGH);  //again outputs are inverted in hardware so we write the opposite of the state variable
  }

  if (trig2State && trig2Choke) {
    digitalWriteFast(trig2Out, LOW);
  } else {
    digitalWriteFast(trig2Out, HIGH);
  }

  if (trig3State && trig3Choke) {
    digitalWriteFast(trig3Out, LOW);
  } else {
    digitalWriteFast(trig3Out, HIGH);
  }

  if (trig4State && trig4Choke) {
    digitalWriteFast(trig4Out, LOW);
  } else {
    digitalWriteFast(trig4Out, HIGH);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
