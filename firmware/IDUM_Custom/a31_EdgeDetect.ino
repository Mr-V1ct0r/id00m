//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function takes the digital inputs and tracks rising edges, debounces buttons etc.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void edgeDetect() {

  detectEdges();  //calculate rising edge variables

  slowCycleHelper();  //adds to the count whenever we receive a clock during a slow cycle

  grabLooperVariables();  //calculates looper status and sets up looper variables when we turn it on or off

  debounceModeButton();  //handles holding the mode button to enable the menu

  calculateGateVariables();  //calculate some other information based on the gate inputs used in various modes.

  saveGateStates();  //saves the current state of all gates for edge detection on the next loop

  multiplyClocks();  //comes up with a few clock multiples based on the incoming clock

  handleChoke();  //calculates the choke variables which disable each output
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This tells us when there is a rising edge.  We also detect falling edges which are only used for the Hold mode function but could also be useful
//to you.

void detectEdges() {

  //reset all edge variables
  clockEdge = clockFallingEdge = trig1Edge = trig2Edge = trig3Edge = trig4Edge = trig1FallingEdge = trig2FallingEdge = trig3FallingEdge = trig4FallingEdge = modeBtnEdge = 0;

  //if each gate is currently on and was off in the last loop then that gate has a rising edge and we set the relevant flag high
  if (clockIn && !clockOld) clockEdge = 1;
  if (trig1In && !trig1Old) trig1Edge = 1;
  if (trig2In && !trig2Old) trig2Edge = 1;
  if (trig3In && !trig3Old) trig3Edge = 1;
  if (trig4In && !trig4Old) trig4Edge = 1;

  //if each gate is currently off and was on in the last loop then that gate has a falling edge and we set the relevant flag high
  if (clockOld && !clockIn) clockFallingEdge = 1;
  if (!trig1In && trig1Old) trig1FallingEdge = 1;
  if (!trig2In && trig2Old) trig2FallingEdge = 1;
  if (!trig3In && trig3Old) trig3FallingEdge = 1;
  if (!trig4In && trig4Old) trig4FallingEdge = 1;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//If we have a slow cycle maintenance clock burst happening and a clock comes in at the input we add a clock to the cycle count variable.

void slowCycleHelper() {

  if (cycleActive && clockEdge && !loopEnable && !((modifyLength > 0) && modeOwnsClock(mode))) cycleCount += 2;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Turns on or off the looper based on the loop button and loop gate inputs.  Then we set some variables to help the looper function.

void grabLooperVariables() {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  if (!loopButtonIn) loopButtonActive = 0;                               //turns off these variables for debouncing
  if (!loopGateIn) loopGateActive = 0;                                   //we actually do a little bit of debouncing to the gate input to clean up messy transitions between 'on' and 'off' gates.
  if ((loopButtonIn && !loopButtonOld)) loopButtonTime = currentMicros;  //if the loop button just got a rising edge we save the current system time
  if ((loopGateIn && !loopGateInOld)) loopGateTime = currentMicros;      //if the loop gate just got a rising edge we save the current system time

  //if the loop button stays on for more than 500 micro seconds we set the loop button variable on
  if (loopButtonIn && ((currentMicros - loopButtonTime) > 500)) loopButtonActive = 1;

  //if the loop gate stays on for more than 5 micro seconds we set the loop button variable on
  if (loopGateIn && ((currentMicros - loopGateTime) > 5)) loopGateActive = 1;

  //Sets the rising edge variables based on the debounced loop button and gate variables
  if (!loopButtonActiveOld && loopButtonActive) loopButtonActiveEdge = 1;
  else loopButtonActiveEdge = 0;
  if (!loopGateActiveOld && loopGateActive) loopGateActiveEdge = 1;
  else loopGateActiveEdge = 0;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function helps to detect when the mode button has been held long enough to enable the menu

void debounceModeButton() {

  unsigned long currentMicros = micros();

  byte optionByte = 0;

  //resets the mode button rising edge variable
  modeBtnEdge = 0;

  //Resets some mode button variables associated with the falling edge
  if (modeBtnFallingEdge) {
    modeBtnFallingEdge = 0;
    modeBtnReset = 0;
    menuModeReset = 0;
  }

  //if we have a mode button rising edge on the raw input we save the current time for debouncing purposes
  if (modeBtnIn && !modeBtnInOld) {
    modeBtnDebounceTime = currentMicros;
  }

  //If the amount of time since the saved debounce time is over 1ms and the button is still on set some variables high.
  if (modeBtnIn && ((currentMicros - modeBtnDebounceTime) > 1000) && !modeBtnReset && !loopEnable) {
    modeBtnEdge = 1;   //indicates that we have a rising edge on the mode button
    modeBtnReset = 1;  //helps to debounce the falling edge of the mode button which is used to turn on and off modes
  }

  //If the amount of time since the saved debounce time is over 1 second and the button is still on we either enter or exit menu mode.
  if (modeBtnIn && ((currentMicros - modeBtnDebounceTime) > 1000000) && !menuModeReset) {
    menuMode = !menuMode;  //changes if menu mode is active or inactive
    menuModeReset = 1;     //makes sure that the first falling edge after entering the menu doesn't change any options
    secondaryMenu = 0;     //always enter (and leave) on the regular page 1
    f9EditPage = 0;        //CUSTOM (F9): the editor overlay is never carried across a menu enter/exit - always start on page 1

    if (!menuMode) {  //if we just exited the menu then we save the options to the eeprom for power off.
      saveOptions();
    }
  }

  //CUSTOM: if we keep holding past 2.5 seconds while in the (regular) menu, promote to the SECONDARY page.  Gated on !secondaryMenu
  //so it fires exactly once; the blinking LEDs 2-5 in the display tell the user they've reached it.
  if (modeBtnIn && ((currentMicros - modeBtnDebounceTime) > 2500000) && menuMode && !secondaryMenu) {
    secondaryMenu = 1;
    secondaryBlinkPhase = 0;  //CUSTOM: start the priority chase on the top-ranked channel
  }

  if (!modeBtnIn && modeBtnInOld) {  //sets the falling edge for the mode button.  Plays nicer when this is at the end of the function.
    modeBtnFallingEdge = 1;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function saves a bunch of relevant variables for each gate input and does a tiny bit of debouncing.

void calculateGateVariables() {

  unsigned long currentMicros = micros();  //only calls the system time once for efficiency.

  //clock input
  if (clockEdge && (currentMicros - lastClockTime > 100)) {  //minimum time between clocks is 100 microseconds
    lastClockInterval = currentMicros - lastClockTime;       //saves the time interval between the two most recent clocks
    lastClockTime = currentMicros;                           //saves the current time of the most recent rising edge
  }

  if (!loopEnable) {  //ignores the gate inputs if we are currently playing a loop.

    //trig 1
    if (trig1Edge && (currentMicros - lastTrig1Time > 100)) {  //minimum time between triggers is 100 microseconds
      lastTrig1Interval = currentMicros - lastTrig1Time;       //saves the time interval between the two most recent triggers
      lastTrig1Time = currentMicros;                           //saves the current time of the most recent rising edge
    }

    //we also keep track of the falling edge time
    if (trig1FallingEdge) lastTrig1FallingTime = currentMicros;

    //trig 2
    if (trig2Edge && (currentMicros - lastTrig2Time > 100)) {  //minimum time between triggers is 100 microseconds
      lastTrig2Interval = currentMicros - lastTrig2Time;       //saves the time interval between the two most recent triggers
      lastTrig2Time = currentMicros;                           //saves the current time of the most recent rising edge
    }

    //we also keep track of the falling edge time
    if (trig2FallingEdge) lastTrig2FallingTime = currentMicros;

    //trig 3
    if (trig3Edge && (currentMicros - lastTrig3Time > 100)) {  //minimum time between triggers is 100 microseconds
      lastTrig3Interval = currentMicros - lastTrig3Time;       //saves the time interval between the two most recent triggers
      lastTrig3Time = currentMicros;                           //saves the current time of the most recent rising edge
    }

    //we also keep track of the falling edge time
    if (trig3FallingEdge) lastTrig3FallingTime = currentMicros;

    //trig 4
    if (trig4Edge && (currentMicros - lastTrig4Time > 100)) {  //minimum time between triggers is 100 microseconds
      lastTrig4Interval = currentMicros - lastTrig4Time;       //saves the time interval between the two most recent triggers
      lastTrig4Time = currentMicros;                           //saves the current time of the most recent rising edge
    }

    //we also keep track of the falling edge time
    if (trig4FallingEdge) lastTrig4FallingTime = currentMicros;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//save gate states for next loop.  Comparing these with the new values lets us tell when there is a rising or falling edge.

void saveGateStates() {

  clockOld = clockIn;
  trig1Old = trig1In;
  trig2Old = trig2In;
  trig3Old = trig3In;
  trig4Old = trig4In;
  loopGateInOld = loopGateIn;
  loopButtonOld = loopButtonIn;
  modeBtnInOld = modeBtnIn;
  loopButtonActiveOld = loopButtonActive;
  loopGateActiveOld = loopGateActive;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//A somewhat imperfect way of multiplying the clock.  Ideally this should be done with a timer interrupt but this worked well enough to
//capture ratchets.  This clock multiplication is only used in the loop recording and break mode.

void multiplyClocks() {

  unsigned long currentMicros = micros();  //calculating the current system time once per function is more efficient

  unsigned int clockMultWindow = 1000;  //sets the global sensitivity for crossing a tick threshold.

  //Reset the rising edge variables every loop, these are set high if we hit a rising edge.
  times2ClockEdge = times4ClockEdge = 0;

  //A good example of the test we run, if the time since the most recent clock time is less than the clock window time then we are
  //in the first "tick" of the clock interval.
  bool clockMultiplyTest1 = (currentMicros - lastClockTime) < clockMultWindow;

  //If the time since the most recent clock time plus one quarter of the most recent clock interval is less than the clock window time
  //then we are in the second tick of the clock interval.
  bool clockMultiplyTest2 = ((currentMicros - lastClockTime) > (lastClockInterval / 4))
                            && ((currentMicros - lastClockTime) < ((lastClockInterval / 4) + clockMultWindow));

  //If the time since the most recent clock time plus one hald of the most recent clock interval is less than the clock window time
  //then we are in the third tick of the clock interval.
  bool clockMultiplyTest3 = ((currentMicros - lastClockTime) > (lastClockInterval / 2))
                            && ((currentMicros - lastClockTime) < ((lastClockInterval / 2) + clockMultWindow));

  //If the time since the most recent clock time plus three quarters of the most recent clock interval is less than the clock window time
  //then we are in the fourth tick of the clock interval.
  bool clockMultiplyTest4 = ((currentMicros - lastClockTime) > ((3 * lastClockInterval) / 4))
                            && ((currentMicros - lastClockTime) < (((3 * lastClockInterval) / 4) + clockMultWindow));

  //The first and third clock ticks of the clock interval go twice as fast as the clock.  Tracking this variable lets us detect rising
  //clock edges at twice the speed of the clock.
  times2Clock = clockMultiplyTest1 || clockMultiplyTest3;

  //If we check all four tick tests we are going four times as fast as the clock.  Tracking this variable lets us detect rising
  //clock edges at four times the speed of the clock.
  times4Clock = clockMultiplyTest1 || clockMultiplyTest2 || clockMultiplyTest3 || clockMultiplyTest4;

  //Edge detection of this variable similarly to how we track rising edges for all other gate inputs.
  if (times2Clock && !times2ClockOld) times2ClockEdge = 1;
  if (times4Clock && !times4ClockOld) times4ClockEdge = 1;

  //Save the rising edge time.
  if (times2ClockEdge) times2ClockTime = currentMicros;
  if (times4ClockEdge) times4ClockTime = currentMicros;

  //Save the current values to detect rising edges.
  times2ClockOld = times2Clock;
  times4ClockOld = times4Clock;

  //And manually set the clockTick variable whenever we pass a test.  Important to note that we can only get one test passed at a time.
  if (clockMultiplyTest1) clockTick = 0;
  if (clockMultiplyTest2) clockTick = 1;
  if (clockMultiplyTest3) clockTick = 2;
  if (clockMultiplyTest4) clockTick = 3;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function sets the choke variables high whenever there is a rising edge on that channel.  The chokes are set low when a modification is
//started.

void handleChoke() {

  //if choke option is enabled, set choke flag high on trigger edge (flag is set low when modification starts).
  if (!chokeDisable) {
    if (trig1Edge) trig1Choke = 1;
    if (trig2Edge) trig2Choke = 1;
    if (trig3Edge) trig3Choke = 1;
    if (trig4Edge) trig4Choke = 1;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
