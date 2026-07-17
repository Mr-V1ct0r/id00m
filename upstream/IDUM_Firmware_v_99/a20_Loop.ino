//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This is the main top level function that runs in the IDUM code.  Everything else is called from here.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loop() {

  //The timer interrupts need to be "ticked" by these functions.  I don't think that this library implements an actual hardware interrupt.
  cTimer.tick();  //used for the "slow clock" interval timers.
  dTimer.tick();  //used for the display interval timers.

  readInputs();  //reads the gate and ADC inputs

  edgeDetect();  //sets the edge detection variables

  chooseMode();  //choose the current modification mode

  handleLooper();  //Either play saved loop gate data or record incoming gate data to the looper.

  modifyTest();  //Check to see if a modification is being done, if not randomly decide if one will be done.

  handleMenu();  //handle the menu that is called up by holding the "mode" button

  handleOutputs();  //Send out the final calculated values of the clock and gates.

  ////////////////////////////////////////////////////////////debugging

  //Debugging stuff, when 'printEnable' is set high in the first page this will print anything you put here whenever you get a clock.
  //Get rid of the term '&& clockEdge' if you want it to print every loop but be aware that your code will run super slow.

  if (printEnable && clockEdge) {
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
