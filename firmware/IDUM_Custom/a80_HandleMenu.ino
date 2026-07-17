//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Call this function when we are in the menu to handle button presses and change the options accordingly
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void handleMenu() {

  changeMenuOptions();
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Handy function to handle menu options

void changeMenuOptions() {

  int menuPosition = modeDialPosition;  //the mode dial selects which option we want to change

  if (menuMode) {  //if the menu is active

    //CUSTOM (F9): a CYCLE-switch flip while on page 2 toggles the F9 editor overlay (page 2 <-> page 3).  Gated on secondaryMenu so the
    //switch does nothing menu-wise on page 1 - no accidental jump into the editor.  On entry we reset the cursor to channel 1 and anchor
    //the knob pickup at the current dial detent so landing there doesn't immediately overwrite channel 1's stored value.
    if (secondaryMenu && (cycleIn != cycleInOld)) {
      f9EditPage = !f9EditPage;
      if (f9EditPage) { f9EditChannel = 0; f9KnobLatched = 0; f9KnobPosOld = modeDialPosition; }
    }

    if (f9EditPage) {
      handleF9Editor();  //CUSTOM (F9): the editor owns the controls - MODE-button option toggling is suppressed while it's open
    }

    else if (modeBtnFallingEdge && !menuModeReset) {  //and the mode button just fell a second time after we enabled the menu

      if (!secondaryMenu) {      //PAGE 1: the original options
        switch (menuPosition) {  //change the option being pointed to by the mode dial
          case 0:
            analogClock = !analogClock;
            break;
          case 1:
            paramResolution = 2;
            break;

          case 2:
            paramResolution = 1;
            break;

          case 3:
            paramResolution = 0;
            break;

          case 4:
            lengthResolution = 0;
            break;

          case 5:
            lengthResolution = 1;
            break;

          case 6:
            lengthResolution = 2;
            break;

          case 7:
            //CUSTOM: 3-state cycle on the split slot - OFF -> split -> split+freeze -> OFF.  freeze (the old
            //"super secret" freezeParam: PARAM is snapshotted at each modification's start instead of live, so
            //in split each channel can hold its own PARAM) is never on without split.  The loop LED blinks at
            //this slot to show the freeze sub-state (solid slot-7 ring = split active, either state).
            if (!splitMode) { splitMode = 1; freezeParam = 0; }   //click 1: split on
            else if (!freezeParam) freezeParam = 1;               //click 2: add freeze
            else { splitMode = 0; freezeParam = 0; }              //click 3: off
            break;
        }
      }

      else {                                                              //PAGE 2 (secondary): linear drumming + priority
        if (menuPosition == 0) linearDrumming = !linearDrumming;          //LED 1 toggles linear drumming
        else if (menuPosition >= 1 && menuPosition <= 4) moveToFront(menuPosition);  //LEDs 2-5: move that channel to top priority
        else if (menuPosition == 5) mergeState = (mergeState + 1) % MERGE_STATE_COUNT;  //CUSTOM F6: ring slot 5 cycles MERGE OFF->ADD->CUT->MIX
        else if (menuPosition == 6) breakBank = (breakBank + 1) % BREAK_BANK_COUNT;  //CUSTOM F13: ring slot 6 cycles the break pattern bank
        else if (menuPosition == 7) breakSpeed = (breakSpeed % BREAK_SPEED_MAX) + 1;  //CUSTOM F14: ring slot 7 cycles the break speed 1->2->3->4->1
      }
    }

    if (loopButtonActiveEdge && !secondaryMenu) loopInputBehavior = !loopInputBehavior;  //loop-gate-behaviour toggle stays on page 1
  }

  cycleInOld = cycleIn;  //CUSTOM (F9): track the CYCLE switch every loop (even outside the menu) so entering the menu can't see a stale edge
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F9): input handling for the editor overlay (page 3).  The LOOP button steps the channel cursor 1->2->3->4->wrap and re-arms the
//knob pickup; the mode dial sets the current channel's value, but only after it has physically moved since you landed on the channel
//(pickup), so switching channels never clobbers a stored value with wherever the knob happens to sit.  The MODE button is inert here (its
//option-toggle is suppressed by the caller), and exiting is the usual long MODE hold, which saves everything to EEPROM.

void handleF9Editor() {

  if (loopButtonActiveEdge) {                //LOOP steps to the next channel
    f9EditChannel = (f9EditChannel + 1) % 4;
    f9KnobLatched = 0;                       //new channel -> disarm pickup until the knob moves again
    f9KnobPosOld = modeDialPosition;         //re-anchor so the first real move is what arms it (not the residual offset from the last channel)
  }

  if (modeDialPosition != f9KnobPosOld) f9KnobLatched = 1;   //the knob moved -> it takes over this channel's value from here on
  if (f9KnobLatched) f9Value[f9EditChannel] = modeDialPosition;  //live-edit the current channel's value from the dial (0-7 -> value slot)
  f9KnobPosOld = modeDialPosition;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Move a channel (1-4) to the top of the linear-drumming priority order, shifting everyone above it down one.  Repeated taps can build
//any ordering (tap the channels in reverse priority order to land on an exact ranking).

void moveToFront(unsigned int ch) {
  unsigned int idx = 0;
  for (unsigned int i = 0; i < 4; i++) {
    if (priorityOrder[i] == ch) { idx = i; break; }  //find the channel's current rank
  }
  for (unsigned int i = idx; i > 0; i--) priorityOrder[i] = priorityOrder[i - 1];  //shift the higher ranks down into the gap
  priorityOrder[0] = ch;                                                            //and put this channel on top
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
