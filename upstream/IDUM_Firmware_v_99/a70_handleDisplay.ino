//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Drives the display LEDs.  Used to be called once per loop but is now called from a timer interrupt.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void handleDisplay() {

  loopButtonDisplay();  //handles the looper button which has a couple of under the hood options

  calculateDisplayCounters();  //counts every time we run this program for different brightness levels

  calculateActiveLED();  //set the active LED

  writeToModeMUX();  //send out the hardware messages to select an LED on the ring
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//sets the loop button status, can be over-ridden for faster debugging feedback without the need to use the serial port

void loopButtonDisplay() {

  if (!menuMode) {  //if we aren't in the menu just display the looper status

    //debug allows us to use the loop LED to indicate something going on within the program without slowing things down with serial communication
    if (IDUMLEDebugEnable) {
      if (IDUMLEDebug) {
        digitalWriteFast(loopLED, HIGH);
      } else {
        digitalWriteFast(loopLED, LOW);
      }
    } else {
      if (loopEnable) {
        digitalWriteFast(loopLED, HIGH);
      } else {
        digitalWriteFast(loopLED, LOW);
      }
    }
  }

  else {  //if we are in the menu display the looper behavior input option status
    if (loopInputBehavior) {
      digitalWriteFast(loopLED, HIGH);
    }

    else {
      digitalWriteFast(loopLED, LOW);
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//decrement the LED counter.  We use a multiplexer instead of a shift register to drive the mode ring so in order to change the brightness of
//a certain LED we just stay on that position for longer than the others.

void calculateDisplayCounters() {

  int i = 0;

  if (LEDCounter == 0) {  //if our counter has run out

    modeCounter = (modeCounter + 1) % 8;       //advance to the next LED in the ring
    if (!menuMode && !loopEnable) {            //if we aren't in the menu or the looper
      while (activeModes[modeCounter] == 0) {  //skip inactive modes
        modeCounter = (modeCounter + 1) % 8;
        i += 1;
        if (i > 16) break;  //just a small double check to avoid an infinite loop
      }
    }
    LEDCounterReset = 1;  //set the reset flag high for the next function
  }

  else {  //if the counter is not zero we just decrement the counter and make sure that the reset flag is low
    LEDCounter -= 1;
    LEDCounterReset = 0;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Since we only have one LED on at a time we use this counter to allow for more than one brightness level.
//Modes that are off are skipped
//Modes that are on but not selected are only counted once
//The mode that is currently selected is held high for 16 counts of calling this function

void calculateActiveLED() {

  unsigned int highestOption = 0;
  unsigned int menuModeCount = 2;
  unsigned int skipOption = 1;

  //if the looper is active and the menu is off we just display the current loop index
  //we now capture the originalLoopOffset variable to make the looper always start at step 1, the display counter also needs to be offset.
  if (loopEnable && !menuMode) modeCounter = (loopIndex + originalLoopOffset) % 8;

  //if we are doing some kind of modification we need to see if we are in split mode
  else if (modificationActive) {

    //if a modification is active and we aren't in split mode we only display the currently selected mode
    if (!splitMode) modeCounter = mode;

    //if we are in split mode things get a bit more complicated
    else {
      if (LEDCounterReset) {  //if the LED counter has run out

        //Then we have a new counter that we use to jump between all active modes
        splitModeDisplayCounter = (splitModeDisplayCounter + 1) % 5;  //there can only be 5 modes active (one per channel) maximum

        //We run the following code until we find the next modification to display and then we break the loop manually.
        while (1) {

          //If we are checking for channel 0 (aka the clock channel)
          if (splitModeDisplayCounter == 0) {
            if (modifyLength > 0) {  //and the modification for channel 0 is active
              modeCounter = mode;    //then we set the current display address to the mode for channel 0
              break;                 //and manually break the loop
            }

            else splitModeDisplayCounter += 1;  //otherwise we check the next channel's modification status
          }

          //If we are checking for channel 1 (aka the TR1 channel)
          if (splitModeDisplayCounter == 1) {
            if (modifyLength1 > 0) {  //and the modification for channel 1 is active
              modeCounter = mode1;    //then we set the current display address to the mode for channel 1
              break;                  //and manually break the loop

            }

            else splitModeDisplayCounter += 1;  //otherwise we check the next channel's modification status
          }

          if (splitModeDisplayCounter == 2) {
            if (modifyLength2 > 0) {              //and the modification for channel 2 is active
              modeCounter = mode2;                //then we set the current display address to the mode for channel 2
              break;                              //and manually break the loop
            } else splitModeDisplayCounter += 1;  //otherwise we check the next channel's modification status
          }

          if (splitModeDisplayCounter == 3) {
            if (modifyLength3 > 0) {              //and the modification for channel 3 is active
              modeCounter = mode3;                //then we set the current display address to the mode for channel 3
              break;                              //and manually break the loop
            } else splitModeDisplayCounter += 1;  //otherwise we check the next channel's modification status
          }

          if (splitModeDisplayCounter == 4) {
            if (modifyLength4 > 0) {             //and the modification for channel 4 is active
              modeCounter = mode4;               //then we set the current display address to the mode for channel 4
              break;                             //and manually break the loop
            } else splitModeDisplayCounter = 0;  //otherwise we loop back around to channel 0's tests
          }

          LEDCounter = 1;  //And we set the LED counter to 1 because all active LEDs should have the same brightness.
        }
      }
    }
  }

  //if no modifications are active we have to display multiple brightnesses.  This makes it so that we only change the active LED
  //when the LEDCounterReset flag is set high
  else if (LEDCounterReset) {

    if (!menuMode) {  //if we aren't in the Menu we are just displaying the currently active and selected modes

      //if the active LED is the same as the currently selected mode we set a longer LED counter.  This makes the currently selected
      //mode's LED last longer and therefor appear brighter than the other LEDs that just indicate the other modes that have not
      //been disabled with the "remove mode" button.
      if (modeCounter == mode) {

        //displayed mode helps avoid visual hiccups when the selected mode decreases
        //it does this by going through an LED cycle where all LEDs are the same brightness
        if (displayedMode >= modeCounter) LEDCounter = 10;  //if we are on the selected mode set the counter to a longer number
        else LEDCounter = 1;                                //if the selected mode has just decreased we set the LEDcounter to 1 for a quick sweep of the LEDs
        displayedMode = modeCounter;                        //record the most recent selected mode LED to avoid visual hiccups
      } else LEDCounter = 1;                                //if the mode is active but not selected set the counter to 1 so its LED is only on for a moment
    }

    else {  //if we are in the menu display the current options

      //skip option lets us turn off specific LEDs instead of briefly illuminating them
      //we basically run this loop until we find an active option and then set this flag low

      while (skipOption) {

        switch (modeCounter) {  //select the next active LED based on which LED we currently are displaying

          case 0:
            if (analogClock) {             //if an option is active
              skipOption = 0;              //exit the 'while' loop
              LEDCounter = menuModeCount;  //set the LED counter to a predetermined number
              break;                       //restart the loop
            } else {
              modeCounter += 1;  //otherwise increase the counter for the selected LED
              skipOption = 1;    //keep the 'while' loop going
              break;             //restart the loop
            }


          case 1:
            if (paramResolution == 2) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 2:
            if (paramResolution == 1) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 3:
            if (paramResolution == 0) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 4:
            if (lengthResolution == 0) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 5:
            if (lengthResolution == 1) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 6:
            if (lengthResolution == 2) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter += 1;
              skipOption = 1;
              break;
            }


          case 7:
            if (splitMode) {
              skipOption = 0;
              LEDCounter = menuModeCount;
              break;
            } else {
              modeCounter = 0;
              skipOption = 1;
              break;
            }
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//set the multiplexer digital outputs based on the active LED.  LEDs are out of order from the multiplexer addresses so they need to be set
//manually and in a kind of weird order.  It was easier to lay out the board with the LEDs out of order.

void writeToModeMUX() {

  if (modeCounter != modeCounterOld) {  //only send the write commands when the active LED has changed
    //display values for cd4051 driving mode display
    switch (modeCounter) {

      case 0:
        digitalWriteFast(mode1LED, HIGH);
        digitalWriteFast(mode2LED, LOW);
        digitalWriteFast(mode3LED, LOW);
        break;
      case 1:
        digitalWriteFast(mode1LED, LOW);
        digitalWriteFast(mode2LED, HIGH);
        digitalWriteFast(mode3LED, LOW);
        break;
      case 2:
        digitalWriteFast(mode1LED, LOW);
        digitalWriteFast(mode2LED, LOW);
        digitalWriteFast(mode3LED, HIGH);
        break;
      case 3:
        digitalWriteFast(mode1LED, LOW);
        digitalWriteFast(mode2LED, HIGH);
        digitalWriteFast(mode3LED, HIGH);
        break;
      case 4:
        digitalWriteFast(mode1LED, HIGH);
        digitalWriteFast(mode2LED, HIGH);
        digitalWriteFast(mode3LED, HIGH);
        break;
      case 5:
        digitalWriteFast(mode1LED, HIGH);
        digitalWriteFast(mode2LED, LOW);
        digitalWriteFast(mode3LED, HIGH);
        break;
      case 6:
        digitalWriteFast(mode1LED, HIGH);
        digitalWriteFast(mode2LED, HIGH);
        digitalWriteFast(mode3LED, LOW);
        break;
      case 7:
        digitalWriteFast(mode1LED, LOW);
        digitalWriteFast(mode2LED, LOW);
        digitalWriteFast(mode3LED, LOW);
        break;
      default: break;
    }
  }

  modeCounterOld = modeCounter;  //remember the most recently active LED between function calls
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
