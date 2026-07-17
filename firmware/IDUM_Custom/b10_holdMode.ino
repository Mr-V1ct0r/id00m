//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Pink LED mode that either skips or holds incoming triggers/ gates
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void hold() {

  unsigned long currentMicros = micros();  //only sample the system time once

  // set the clock to make a trigger once at the start of the modification
  if ((modifyLength > 0) && (mode == 0)) clockState = ((currentMicros - firstStepTime) < 10000);

  //If you saw this code before the split mode was added you will notice that things look quite different.  The fact that each channel can play
  //a different mode with hypothetically different parameter values (depending on freezeParam which was intended to work with split mode) means
  //that not only do we need to run a test before every single channel's modification code but also we have to split the code up into four
  //identical chunks for almost every modification.  This still was deemed as being preferable to having a generic function that calls an
  //individual channel as a generic input/ output function because a few modifications treat different channels differently.  Also I thought that
  //this would be faster to adapt from the old code and it gives me a lot of control over every detail of the function even if it means writing
  //the same thing x4 every time I make a change.

  //if our first trigger is in a modification then we run the hold code for that channel.
  if ((modifyLength1 > 0) && (mode1 == 0)) {

    //for this mode we sample the param input for each trigger whenever a trigger is received.  This prevents weird jitters whenever param changes
    //regions of the knob.  We put this inside the function since it is the only one that works this way and it's easier with the looper.
    if (trig1Edge) {
      sampledModifyParam1 = modifyParam1;
      sampledUnscaledParam1 = unscaledParam1;
    }

    if (sampledModifyParam1 < 20) trig1State = 0;  //when param is all the way down we silence the output

    else if (sampledUnscaledParam1 < 8) {
      //On every trigger edge we calculate whether or not to turn off each input
      //probability of skipping a gate is set by param knob
      if (trig1Edge) hold1Choke = (random(450) < sampledModifyParam1);
      trig1State = trig1In && hold1Choke;
    }

    else if (sampledUnscaledParam1 == 8) trig1State = trig1In;

    //this is the only mode that uses the falling time parameter to extend the length of a gate.if
    //if none of the other tests are passed then we are on the right hand side of the param knob.
    else {
      trig1State = trig1In || ((currentMicros - lastTrig1FallingTime) < (lastClockInterval * (((originalModifyLength1 + 1) * (sampledModifyParam1 - 512)) / 512)));
    }
  }

  //channel 2 test

  if ((modifyLength2 > 0) && (mode2 == 0)) {

    if (trig2Edge) {
      sampledModifyParam2 = modifyParam2;
      sampledUnscaledParam2 = unscaledParam2;
    }

    if (sampledModifyParam2 < 20) trig2State = 0;  //when param is all the way down we silence the output

    else if (sampledUnscaledParam2 < 8) {
      //On every trigger edge we calculate whether or not to turn off each input
      //probability of skipping a gate is set by param knob
      if (trig2Edge) hold2Choke = (random(450) < sampledModifyParam2);
      trig2State = trig2In && hold2Choke;
    }

    else if (sampledUnscaledParam2 == 8) trig2State = trig2In;

    //this is the only mode that uses the falling time parameter.
    else {
      trig2State = trig2In || ((currentMicros - lastTrig2FallingTime) < (lastClockInterval * (((originalModifyLength2 + 1) * (sampledModifyParam2 - 512)) / 512)));
    }
  }

  //channel 3 test

  if ((modifyLength3 > 0) && (mode3 == 0)) {

    if (trig3Edge) {
      sampledModifyParam3 = modifyParam3;
      sampledUnscaledParam3 = unscaledParam3;
    }

    if (sampledModifyParam3 < 20) trig3State = 0;  //when param is all the way down we silence the output

    else if (sampledUnscaledParam3 < 8) {
      //On every trigger edge we calculate whether or not to turn off each input
      //probability of skipping a gate is set by param knob
      if (trig3Edge) hold3Choke = (random(450) < sampledModifyParam3);
      trig3State = trig3In && hold3Choke;
    }

    else if (sampledUnscaledParam3 == 8) trig3State = trig3In;

    //this is the only mode that uses the falling time parameter.
    else {
      trig3State = trig3In || ((currentMicros - lastTrig3FallingTime) < (lastClockInterval * (((originalModifyLength3 + 1) * (sampledModifyParam3 - 512)) / 512)));
    }
  }

  //channel 4 test

  if ((modifyLength4 > 0) && (mode4 == 0)) {

    if (trig4Edge) {
      sampledModifyParam4 = modifyParam4;
      sampledUnscaledParam4 = unscaledParam4;
    }

    if (sampledModifyParam4 < 20) trig4State = 0;  //when param is all the way down we silence the output

    else if (sampledUnscaledParam4 < 8) {
      //On every trigger edge we calculate whether or not to turn off each input
      //probability of skipping a gate is set by param knob
      if (trig4Edge) hold4Choke = (random(450) < sampledModifyParam4);
      trig4State = trig4In && hold4Choke;
    }

    else if (sampledUnscaledParam4 == 8) trig4State = trig4In;

    //this is the only mode that uses the falling time parameter.
    else {
      trig4State = trig4In || ((currentMicros - lastTrig4FallingTime) < (lastClockInterval * (((originalModifyLength4 + 1) * (sampledModifyParam4 - 512)) / 512)));
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
