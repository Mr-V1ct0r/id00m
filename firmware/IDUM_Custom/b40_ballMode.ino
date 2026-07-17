//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// YELLOW LED mode that triggers a bouncing ball effect after each incoming trigger
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void ball() {

  if ((modifyLength > 0) && (mode == 3)) clockState = ((micros() - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //check if each channel is performing the current modification
  if ((modifyLength1 > 0) && (mode1 == 3)) {

    //If we are on the right side of the knob we are making an expanding bouncing ball
    if (unscaledParam1 > 8) {
      if (trig1Edge || forceBounce1) {                  //if the trigger rising edge is received reset the bouncing ball settings
        bounce1Time = 0;                                //bounceXTime keeps track of how many code loops its been since we sent out a bouncing ball trigger
        trig1Divide = f9ScalePeriod(10 + (512 - (modifyParam1 / 2)), 1);  //CUSTOM (F9): scale the bounce period per-channel (trigXDivide is a loop-count period)
        forceBounce1 = 0;
      }

      //bouncing ball is one of the few modes that just counts once per loop instead of referencing everything to a microsecond timing function.
      //The triggers don't need to be precise so microsecond timing isn't necessary.

      //we output a gate during the first 10 loops of each bouncing ball time
      //we also turn off the effect after it gets longer than 1000 loops (CUSTOM F9: cutoff scaled too, so a divide-slowed ball isn't muted)
      trig1State = (bounce1Time < 10) && (trig1Divide < f9ScalePeriod(1000, 1));

      if (bounce1Time > trig1Divide) {                                             //if we have passed the time specified by the trigXDivide parameter
        bounce1Time = 0;                                                           //reset the bounceXTime variable
        trig1Divide += f9ScalePeriod(1 + (((1025 - modifyParam1) * originalModifyLength1) / 8), 1);  //CUSTOM (F9): scale the growth by the same factor -> same shape, scaled speed
      }

    }

    //if we are on the left side of the knob we are making a contracting bouncing ball.
    else {
      if (trig1Edge || forceBounce1) {  //if the trigger rising edge is received reset the bouncing ball settings
        bounce1Time = 0;
        trig1Divide = f9ScalePeriod(100 + ((modifyParam1 / 2)), 1);  //CUSTOM (F9): scale the contracting start period (the *=ratio decay then scales with it)
        forceBounce1 = 0;
      }

      //we output a gate during the first 10 loops of each bouncing ball time
      trig1State = (bounce1Time < 10) && (trig1Divide > 10);  //we also turn off the effect after it gets shorter than 10 loops

      if (bounce1Time > trig1Divide) {                                  //if we have passed the time specified by the trigXDivide parameter
        bounce1Time = 0;                                                //reset the bounceXTime variable
        trig1Divide *= .5 + (.4 * ((float)originalModifyLength1 / 8));  //and decrease the trigXDivide variable by a ratio set with the param knob
      }
    }

    //increment the bounceXTime counters by one every time we call this function from the main loop
    bounce1Time += 1;
  }

  //check if each channel is performing the current modification
  if ((modifyLength2 > 0) && (mode2 == 3)) {

    //If we are on the right side of the knob we are making an expanding bouncing ball
    if (unscaledParam2 > 8) {
      if (trig2Edge || forceBounce2) {                  //if the trigger rising edge is received reset the bouncing ball settings
        bounce2Time = 0;                                //bounceXTime keeps track of how many code loops its been since we sent out a bouncing ball trigger
        trig2Divide = f9ScalePeriod(10 + (512 - (modifyParam2 / 2)), 2);  //CUSTOM (F9): scale the bounce period per-channel (trigXDivide is a loop-count period)
        forceBounce2 = 0;
      }

      //bouncing ball is one of the few modes that just counts once per loop instead of referencing everything to a microsecond timing function.
      //The triggers don't need to be precise so microsecond timing isn't necessary.

      //we output a gate during the first 10 loops of each bouncing ball time
      //we also turn off the effect after it gets longer than 1000 loops (CUSTOM F9: cutoff scaled too, so a divide-slowed ball isn't muted)
      trig2State = (bounce2Time < 10) && (trig2Divide < f9ScalePeriod(1000, 2));

      if (bounce2Time > trig2Divide) {                                             //if we have passed the time specified by the trigXDivide parameter
        bounce2Time = 0;                                                           //reset the bounceXTime variable
        trig2Divide += f9ScalePeriod(1 + (((1025 - modifyParam2) * originalModifyLength2) / 8), 2);  //CUSTOM (F9): scale the growth by the same factor -> same shape, scaled speed
      }

    }

    //if we are on the left side of the knob we are making a contracting bouncing ball.
    else {
      if (trig2Edge || forceBounce2) {  //if the trigger rising edge is received reset the bouncing ball settings
        bounce2Time = 0;
        trig2Divide = f9ScalePeriod(100 + ((modifyParam2 / 2)), 2);  //CUSTOM (F9): scale the contracting start period (the *=ratio decay then scales with it)
        forceBounce2 = 0;
      }

      //we output a gate during the first 10 loops of each bouncing ball time
      trig2State = (bounce2Time < 10) && (trig2Divide > 10);  //we also turn off the effect after it gets shorter than 10 loops

      if (bounce2Time > trig2Divide) {                                  //if we have passed the time specified by the trigXDivide parameter
        bounce2Time = 0;                                                //reset the bounceXTime variable
        trig2Divide *= .5 + (.4 * ((float)originalModifyLength2 / 8));  //and decrease the trigXDivide variable by a ratio set with the param knob
      }
    }

    //increment the bounceXTime counters by one every time we call this function from the main loop
    bounce2Time += 1;
  }

  //check if each channel is performing the current modification
  if ((modifyLength3 > 0) && (mode3 == 3)) {

    //If we are on the right side of the knob we are making an expanding bouncing ball
    if (unscaledParam3 > 8) {
      if (trig3Edge || forceBounce3) {                  //if the trigger rising edge is received reset the bouncing ball settings
        bounce3Time = 0;                                //bounceXTime keeps track of how many code loops its been since we sent out a bouncing ball trigger
        trig3Divide = f9ScalePeriod(10 + (512 - (modifyParam3 / 2)), 3);  //CUSTOM (F9): scale the bounce period per-channel (trigXDivide is a loop-count period)
        forceBounce3 = 0;
      }

      //bouncing ball is one of the few modes that just counts once per loop instead of referencing everything to a microsecond timing function.
      //The triggers don't need to be precise so microsecond timing isn't necessary.

      //we output a gate during the first 10 loops of each bouncing ball time
      //we also turn off the effect after it gets longer than 1000 loops (CUSTOM F9: cutoff scaled too, so a divide-slowed ball isn't muted)
      trig3State = (bounce3Time < 10) && (trig3Divide < f9ScalePeriod(1000, 3));

      if (bounce3Time > trig3Divide) {                                             //if we have passed the time specified by the trigXDivide parameter
        bounce3Time = 0;                                                           //reset the bounceXTime variable
        trig3Divide += f9ScalePeriod(1 + (((1025 - modifyParam3) * originalModifyLength3) / 8), 3);  //CUSTOM (F9): scale the growth by the same factor -> same shape, scaled speed
      }

    }

    //if we are on the left side of the knob we are making a contracting bouncing ball.
    else {
      if (trig3Edge || forceBounce3) {  //if the trigger rising edge is received reset the bouncing ball settings
        bounce3Time = 0;
        trig3Divide = f9ScalePeriod(100 + ((modifyParam3 / 2)), 3);  //CUSTOM (F9): scale the contracting start period (the *=ratio decay then scales with it)
        forceBounce3 = 0;
      }

      //we output a gate during the first 10 loops of each bouncing ball time
      trig3State = (bounce3Time < 10) && (trig3Divide > 10);  //we also turn off the effect after it gets shorter than 10 loops

      if (bounce3Time > trig3Divide) {                                  //if we have passed the time specified by the trigXDivide parameter
        bounce3Time = 0;                                                //reset the bounceXTime variable
        trig3Divide *= .5 + (.4 * ((float)originalModifyLength3 / 8));  //and decrease the trigXDivide variable by a ratio set with the param knob
      }
    }

    //increment the bounceXTime counters by one every time we call this function from the main loop
    bounce3Time += 1;
  }

  //check if each channel is performing the current modification
  if ((modifyLength4 > 0) && (mode4 == 3)) {

    //If we are on the right side of the knob we are making an expanding bouncing ball
    if (unscaledParam4 > 8) {
      if (trig4Edge || forceBounce4) {                  //if the trigger rising edge is received reset the bouncing ball settings
        bounce4Time = 0;                                //bounceXTime keeps track of how many code loops its been since we sent out a bouncing ball trigger
        trig4Divide = f9ScalePeriod(10 + (512 - (modifyParam4 / 2)), 4);  //CUSTOM (F9): scale the bounce period per-channel (trigXDivide is a loop-count period)
        forceBounce4 = 0;
      }

      //bouncing ball is one of the few modes that just counts once per loop instead of referencing everything to a microsecond timing function.
      //The triggers don't need to be precise so microsecond timing isn't necessary.

      //we output a gate during the first 10 loops of each bouncing ball time
      //we also turn off the effect after it gets longer than 1000 loops (CUSTOM F9: cutoff scaled too, so a divide-slowed ball isn't muted)
      trig4State = (bounce4Time < 10) && (trig4Divide < f9ScalePeriod(1000, 4));

      if (bounce4Time > trig4Divide) {                                             //if we have passed the time specified by the trigXDivide parameter
        bounce4Time = 0;                                                           //reset the bounceXTime variable
        trig4Divide += f9ScalePeriod(1 + (((1025 - modifyParam4) * originalModifyLength4) / 8), 4);  //CUSTOM (F9): scale the growth by the same factor -> same shape, scaled speed
      }

    }

    //if we are on the left side of the knob we are making a contracting bouncing ball.
    else {
      if (trig4Edge || forceBounce4) {  //if the trigger rising edge is received reset the bouncing ball settings
        bounce4Time = 0;
        trig4Divide = f9ScalePeriod(100 + ((modifyParam4 / 2)), 4);  //CUSTOM (F9): scale the contracting start period (the *=ratio decay then scales with it)
        forceBounce4 = 0;
      }

      //we output a gate during the first 10 loops of each bouncing ball time
      trig4State = (bounce4Time < 10) && (trig4Divide > 10);  //we also turn off the effect after it gets shorter than 10 loops

      if (bounce4Time > trig4Divide) {                                  //if we have passed the time specified by the trigXDivide parameter
        bounce4Time = 0;                                                //reset the bounceXTime variable
        trig4Divide *= .5 + (.4 * ((float)originalModifyLength4 / 8));  //and decrease the trigXDivide variable by a ratio set with the param knob
      }
    }

    //increment the bounceXTime counters by one every time we call this function from the main loop
    bounce4Time += 1;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
