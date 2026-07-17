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

    if (modeBtnFallingEdge && !menuModeReset) {  //and the mode button just fell a second time after we enabled the menu
      switch (menuPosition) {                    //change the option being pointed to by the mode dial
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
          splitMode = !splitMode;
          break;
      }
    }

    if (loopButtonActiveEdge) loopInputBehavior = !loopInputBehavior;  //if we press the loop button then change the loop gate behavior
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
