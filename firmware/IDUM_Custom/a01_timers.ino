//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This is a collection of all of the timer interupts used in the code.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This function handles the "slow clock" speed cycle maintain function.  It needs to run in the background because we are intentionally making the clock
//pulses run slowly and therefor can't just wait for the function to be over in order to run the rest of our code.  This function runs enough times to
//send out the right amount of clocks and then stops the timer function.  The speed of the slow clocks is set by the "slowClockSpeed" variable in the
//"super secret variables" page.

bool slowClockTimer(void *) {

  //Decrement the cycle count which keeps track of how many steps we need to catch up.
  cycleCount -= 1;

  //if Cycle count is even we turn the clock output on, if Cycle count is odd we turn the clock output off.
  cycleState = cycleCount % 2;

  if (cycleState) {
    digitalWrite(clockOut, LOW);  //output gates are inverted by a hardware buffer so setting any output low turns the gate on.
  } else {
    digitalWrite(clockOut, HIGH);  //output gates are inverted by a hardware buffer so setting any output high turns the gate off.
  }

  if (cycleCount > 0) return true;  //if the cycle counter is still above zero we keep the timer on by returning 'true'.
  else {
    cycleActive = 0;  //otherwise we turn off timer by setting the 'cycleActive' flag low and returning a 'false'
    if ((originalModifyLength == 1) && (modifyLength > 0))
      clockChoke = 1;     //if we are currently in a modification with length 1 we turn back on the clock
    else clockChoke = 0;  //otherwise turn the clock output off until we get a new rising edge

    return false;  //returning "false" turns off the timer interrupt
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This timer drives the display by calling a function defined in another page.
//We should probably just be splitting this function between the loop and this timer interrupt but I found that this is what made the display
//look best.  Might change when we are adding more timer interrupts.

bool displayTimer(void *) {

  handleDisplay();

  return true;  //we never turn this timer off unless you want to freeze your display.
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
