//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Drives the display LEDs.  Used to be called once per loop but is now called from a timer interrupt.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void handleDisplay() {

  if (f9EditPage) {   //CUSTOM (F9): the editor (page 3) owns the whole display - ring dot + loop-LED channel count - so handle it first and bail
    drawF9Editor();
    return;
  }

  loopButtonDisplay();  //handles the looper button which has a couple of under the hood options

  if (secondaryMenu) {   //CUSTOM: the secondary (page 2) menu has its own blink-count display; handle it separately and bail out
    drawSecondaryMenu();
    return;
  }

  calculateDisplayCounters();  //counts every time we run this program for different brightness levels

  calculateActiveLED();  //set the active LED

  writeToModeMUX();  //send out the hardware messages to select an LED on the ring
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM: is a secondary-menu LED lit right now?  LED 1 (pos 0) is the linear on/off indicator - steady.  LEDs 2-5 (pos 1-4) are the
//four channels, but only ONE is ever lit at a time: the channel sitting at the current chase rank.  Ring slot 6 (F13) is a STEADY dot
//when a non-default break bank is selected.  Everything lit here holds a constant simultaneous count (linear steady + exactly one chase
//dot + steady bank dot), so the count never churns and no dot pulses - the same rule the F1 chase relies on.
bool secondaryLEDOn(unsigned int pos) {
  if (pos == 0) return linearDrumming;                 //LED 1: linear on/off, steady at its dial slot (position 0)
  if (pos == 5) return mergeState > 0;                 //CUSTOM F6: ring slot 5 solid = MERGE active (loop LED spells which state)
  if (pos == 6) return breakBank > 0;                  //CUSTOM F13: ring slot 6 (break's own LED) solid = non-default break bank
  if (pos == 7) return breakSpeed > 1;                 //CUSTOM F14: ring slot 7 (scatter's LED) solid = break speed >1x (loop LED spells N)
  if (pos >= 1 && pos <= 4) return pos == priorityOrder[secondaryBlinkPhase];  //LEDs 2-5: the channel at the current chase rank
  return false;                                        //no other slots used on page 2
}

//CUSTOM: draw the secondary (page-2) menu.  Priority is a CHASE - a single dot steps the channels in rank order (top rank held longer to
//anchor the cycle), advancing on a slow time base.  Linear on/off is a steady dot on LED 1.  Both live on the ring: we fast-multiplex the
//lit LEDs with a SHORT dwell so the refresh is flicker-free, and since the chase keeps the lit-count constant every dot reads as steady.

void drawSecondaryMenu() {

  const unsigned long TOP_DWELL  = 700;   //ms the top-rank channel is held (long hold = the cycle anchor)
  const unsigned long STEP_DWELL = 450;   //ms each of ranks 2-4 is held

  if (millis() - secondaryBlinkTime > ((secondaryBlinkPhase == 0) ? TOP_DWELL : STEP_DWELL)) {
    secondaryBlinkPhase = (secondaryBlinkPhase + 1) % 4;   //advance the chase to the next rank (0 = top .. 3 = bottom)
    secondaryBlinkTime = millis();
  }

  if (LEDCounter > 0) {   //still dwelling on the current LED in the fast POV multiplex
    LEDCounter -= 1;
    return;
  }

  unsigned int guard = 0;                              //advance to the next lit LED (linear + the single chase dot + bank/speed dots); skip the dark ones
  do {
    modeCounter = (modeCounter + 1) % 8;               //cycle secondary-menu positions 0-7 (0 = linear, 1-4 = channels, 6 = break bank, 7 = break speed)
    guard += 1;
  } while (!secondaryLEDOn(modeCounter) && (guard < 9));

  LEDCounter = secondaryLEDOn(modeCounter) ? 3 : 0;    //short dwell -> fast refresh so both dots read steady; 0 keeps spinning if none lit
  writeToModeMUX();
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F9): draw the editor overlay (page 3).  A single steady ring dot shows the current channel's stored value (mux address = the value
//slot 0-7, same ring the mode selector uses), and the loop LED blinks the channel number 1-4 via the shared blink-count helper.  Exactly one
//ring LED is ever lit and it's held (writeToModeMUX only re-addresses on a change), so it reads as a solid full-brightness dot with no pulse.

void drawF9Editor() {
  modeCounter = f9Value[f9EditChannel];   //ring position 0-7 = the current channel's value slot
  writeToModeMUX();
  drawLoopBlinkCount(f9EditChannel + 1);  //loop LED spells which channel (1-4) is being edited
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
    if (secondaryMenu) {  //CUSTOM: page 2 repurposes the loop LED for the blink-count of whichever slot the dial points at; linear lives on ring LED 1
      if (modeDialPosition == 7 && breakSpeed > 1) drawLoopBlinkCount(breakSpeed);   //F14: spell the break speed while pointing at slot 7
      else if (modeDialPosition == 5 && mergeState > 0) drawLoopBlinkCount(mergeState);  //F6: spell the MERGE state while pointing at slot 5
      else digitalWriteFast(loopLED, LOW);
    }

    else if (modeDialPosition == 7) {  //CUSTOM: at the split slot the loop LED reports FREEZE, not loop-input behaviour
      //freeze on -> blink (2.5 Hz); off -> dark.  A blink (vs the SOLID loopInputBehavior indicator on other slots)
      //makes it unmistakable that this LED means something different here.  freezeParam is only ever set with split on.
      if (freezeParam) { digitalWriteFast(loopLED, (millis() % 400 < 200) ? HIGH : LOW); }  //braces required: digitalWriteFast is an if/else macro
      else { digitalWriteFast(loopLED, LOW); }
    }

    else if (loopInputBehavior) {
      digitalWriteFast(loopLED, HIGH);
    }

    else {
      digitalWriteFast(loopLED, LOW);
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F14/F6): spell a small count on the loop LED as a blink group - N blinks then a gap, repeating.  The loop LED is a discrete GPIO
//(not on the mux ring), so blinking it can't disturb the ring's steady lit-count - that's the whole reason page-2 counts live here instead
//of on a ring LED.  Shared by the break-speed dot (slot 7) and the MERGE dot (slot 5); they never collide because each only draws while the
//dial points at its own slot.  Phase is derived straight from millis() so no state is needed: 150ms on, 150ms off, then a 700ms gap.
void drawLoopBlinkCount(unsigned int blinks) {
  const unsigned long ON_MS = 150, OFF_MS = 150, GAP_MS = 700;
  unsigned long cycle = blinks * (ON_MS + OFF_MS) + GAP_MS;
  unsigned long t = millis() % cycle;

  bool on = false;
  if (t < (blinks * (ON_MS + OFF_MS))) on = (t % (ON_MS + OFF_MS)) < ON_MS;  //inside the blink group; the tail GAP stays dark

  digitalWriteFast(loopLED, on ? HIGH : LOW);
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
