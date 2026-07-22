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

  applyMerge();  //CUSTOM (F6): combine the mode output with the raw input per the MERGE state (ADD/CUT), before linear de-conflict

  applyLinearDrumming();  //CUSTOM: if linear drumming is on, keep only the highest-priority trigger firing at this instant

  outputGates();  //send messages to the hardware to turn the outputs high or low
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F6 - MERGE): decide how a channel's mode output should combine with its raw input, given the current mergeState.  Returns
//1 = ADD (input OR mode), 2 = CUT (input AND NOT mode), 0 = REPLACE (leave the mode output alone - the stock behavior).  Only the four
//"movement/scramble/pattern" modes are ever touched; the trigger-anchored modes (hold/burst/ratchet/ball) already contain the input as
//their anchor, so ADD/CUT would be degenerate there and they always stay REPLACE.  See the state table in a00 (mergeState).
unsigned int mergeOpForMode(unsigned int m) {
  if (mergeState == 0) return 0;                                   //OFF -> everything REPLACE
  if (m == 5 || m == 7) return (mergeState == 1 || mergeState == 3) ? 1 : 0;  //delay/scatter: ADD in ADD+MIX, else REPLACE
  if (m == 4 || m == 6) return (mergeState == 1) ? 1 : 2;          //rotate/break: ADD in ADD state, CUT in CUT+MIX
  return 0;                                                        //all other modes: always REPLACE
}

//Apply the MERGE combine to each trigger channel.  ADD keeps the input and layers the mode output on top (echo/ghost/merge/layer).  CUT
//keeps the input but gates it by the mode, with two flavors by intent: ROTATE cuts "minus" (input AND NOT the rotated-in channel = hocket/
//ducking), while BREAK "sculpts" (input passes only where the pattern has a hit-step = the incoming beat played into the pattern's rhythm).
//Both CUT flavors latch their pass/drop verdict at the input's RISING EDGE - break from breakStepGateN (cutBreakPassN), rotate from "is the
//rotated-in voice silent" (cutRotatePassN) - then output (trigNIn && verdict) so the incoming gate keeps its FULL WIDTH (IDUM is a gate
//processor, we must not re-pulse it) while a mid-gate flicker of the mode output can no longer chop one hit into a flam.  Between hits trigNIn
//is low so the held verdict is harmless.  op == 2 only ever reaches rotate (mode 4) or break (mode 6) - see mergeOpForMode.  Only runs on a
//channel actually mid-modification (modifyLengthN > 0).  Scoped to live playing for now (not loop playback) - looper interaction is later.
void applyMerge() {
  if (mergeState == 0 || loopEnable) return;

  if (modifyLength1 > 0) {
    unsigned int op = mergeOpForMode(mode1);
    if (op == 1) trig1State = trig1State || trig1In;
    else if (op == 2) {
      if (mode1 == 6) { if (trig1Edge) cutBreakPass1 = breakStepGate1; trig1State = trig1In && cutBreakPass1; }
      else { if (trig1Edge) cutRotatePass1 = !trig1State; trig1State = trig1In && cutRotatePass1; }  //rotate (mode 4)
    }
  }
  if (modifyLength2 > 0) {
    unsigned int op = mergeOpForMode(mode2);
    if (op == 1) trig2State = trig2State || trig2In;
    else if (op == 2) {
      if (mode2 == 6) { if (trig2Edge) cutBreakPass2 = breakStepGate2; trig2State = trig2In && cutBreakPass2; }
      else { if (trig2Edge) cutRotatePass2 = !trig2State; trig2State = trig2In && cutRotatePass2; }  //rotate (mode 4)
    }
  }
  if (modifyLength3 > 0) {
    unsigned int op = mergeOpForMode(mode3);
    if (op == 1) trig3State = trig3State || trig3In;
    else if (op == 2) {
      if (mode3 == 6) { if (trig3Edge) cutBreakPass3 = breakStepGate3; trig3State = trig3In && cutBreakPass3; }
      else { if (trig3Edge) cutRotatePass3 = !trig3State; trig3State = trig3In && cutRotatePass3; }  //rotate (mode 4)
    }
  }
  if (modifyLength4 > 0) {
    unsigned int op = mergeOpForMode(mode4);
    if (op == 1) trig4State = trig4State || trig4In;
    else if (op == 2) {
      if (mode4 == 6) { if (trig4Edge) cutBreakPass4 = breakStepGate4; trig4State = trig4In && cutBreakPass4; }
      else { if (trig4Edge) cutRotatePass4 = !trig4State; trig4State = trig4In && cutRotatePass4; }  //rotate (mode 4)
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Returns the EFFECTIVE output of a trigger channel right now (state AND choke - i.e. what would actually reach the jack).
bool channelIsFiring(unsigned int ch) {
  switch (ch) {
    case 1: return trig1State && trig1Choke;
    case 2: return trig2State && trig2Choke;
    case 3: return trig3State && trig3Choke;
    case 4: return trig4State && trig4Choke;
  }
  return false;
}

//Force a trigger channel's output off for this instant.
void muteChannel(unsigned int ch) {
  switch (ch) {
    case 1: trig1State = 0; break;
    case 2: trig2State = 0; break;
    case 3: trig3State = 0; break;
    case 4: trig4State = 0; break;
  }
}

//LINEAR DRUMMING (custom, toggled in the secondary menu): walk the priority order from top to bottom and keep only the first channel
//that is actually firing this instant; mute any lower-priority channel that is also firing.  Per-instant, so modes still overlap and
//interleave - only simultaneous OUTPUT pulses are de-conflicted.  When off, this is a no-op.

void applyLinearDrumming() {
  if (!linearDrumming) return;

  bool taken = false;
  for (unsigned int i = 0; i < 4; i++) {  //priorityOrder[0] is highest priority
    unsigned int ch = priorityOrder[i];
    if (channelIsFiring(ch)) {
      if (taken) muteChannel(ch);  //a higher-priority channel already claimed this instant
      else taken = true;           //this is the highest-priority firing channel - it wins
    }
  }
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

          case 7:  //SCATTER MODE (custom - purple slot 7; replaces contamination, which is kept dead-but-present in b81 for restore)
            scatter();
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
