//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// GREEN LED mode that scrambles each input and output
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void rotate() {

  unsigned int rotation1 = unscaledParam1 / 2;
  unsigned int rotation2 = unscaledParam2 / 2;
  unsigned int rotation3 = unscaledParam3 / 2;
  unsigned int rotation4 = unscaledParam4 / 2;

  if ((modifyLength > 0) && (mode == 4)) clockState = ((micros() - firstStepTime) < 10000);  // set the clock to make a trigger once at the start of the modification

  //check if each channel is performing the current modification
  if ((modifyLength1 > 0) && (mode1 == 4)) {
    switch (rotation1) {  //we now switch the rotation variable per channel, this is probably the most channel dependent of all modifications
      case 0:
        {
          trig1State = trig1In;
          break;
        }

      case 1:
        {
          trig1State = trig3In;
          break;
        }

      case 2:
        {
          trig1State = trig4In;
          break;
        }

      case 3:
        {
          trig1State = trig2In;
          break;
        }

      case 4:
        {
          trig1State = trig1In;
          break;
        }

      case 5:
        {
          trig1State = trig4In;
          break;
        }

      case 6:
        {
          trig1State = trig3In;
          break;
        }

      case 7:
        {
          trig1State = trig2In;
          break;
        }

      default: break;
    }
  }

  //check if each channel is performing the current modification
  if ((modifyLength2 > 0) && (mode2 == 4)) {
    switch (rotation1) {
      case 0:
        {
          trig2State = trig3In;
          break;
        }

      case 1:
        {
          trig2State = trig4In;
          break;
        }

      case 2:
        {
          trig2State = trig3In;
          break;
        }

      case 3:
        {
          trig2State = trig1In;
          break;
        }

      case 4:
        {
          trig2State = trig2In;
          break;
        }

      case 5:
        {
          trig2State = trig1In;
          break;
        }

      case 6:
        {
          trig2State = trig4In;
          break;
        }

      case 7:
        {
          trig2State = trig3In;
          break;
        }

      default: break;
    }
  }

  //check if each channel is performing the current modification
  if ((modifyLength3 > 0) && (mode3 == 4)) {
    switch (rotation1) {
      case 0:
        {
          trig3State = trig2In;
          break;
        }

      case 1:
        {
          trig3State = trig1In;
          break;
        }

      case 2:
        {
          trig3State = trig2In;
          break;
        }

      case 3:
        {
          trig3State = trig4In;
          break;
        }

      case 4:
        {
          trig3State = trig3In;
          break;
        }

      case 5:
        {
          trig3State = trig2In;
          break;
        }

      case 6:
        {
          trig3State = trig1In;
          break;
        }

      case 7:
        {
          trig3State = trig4In;
          break;
        }

      default: break;
    }
  }

  //check if each channel is performing the current modification
  if ((modifyLength4 > 0) && (mode4 == 4)) {
    switch (rotation1) {
      case 0:
        {
          trig4State = trig4In;
          break;
        }

      case 1:
        {
          trig4State = trig2In;
          break;
        }

      case 2:
        {
          trig4State = trig1In;
          break;
        }

      case 3:
        {
          trig4State = trig3In;
          break;
        }

      case 4:
        {
          trig4State = trig4In;
          break;
        }

      case 5:
        {
          trig4State = trig3In;
          break;
        }

      case 6:
        {
          trig4State = trig2In;
          break;
        }

      case 7:
        {
          trig4State = trig1In;
          break;
        }

      default: break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/*

Here is the previous code just to compare.  It's a bit more legible and shows the general pattern behind the trigger input/ output reassignments

switch ( rotation ) {   //selects which version of scramble we perform

    case 0: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig1In ;    //You can see that each of these entries is just a different version of assigning inputs to outputs
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig3In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig2In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig4In ;
      break;
    }

    case 1: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig3In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig4In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig1In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig2In ;
      break;
    }

    case 2: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig4In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig3In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig2In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig1In ;
      break;
    }

    case 3: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig2In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig1In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig4In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig3In ;
      break;
    }

    case 4: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig1In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig2In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig3In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig4In ;
      break;
    }

    case 5: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig4In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig1In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig2In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig3In ;
      break;
    }

    case 6: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig3In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig4In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig1In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig2In ;
      break;
    }

    case 7: {
      if ( ( modifyLength1 > 0 ) && ( mode1 == 4 ) ) trig1State = trig2In ;
      if ( ( modifyLength2 > 0 ) && ( mode2 == 4 ) ) trig2State = trig3In ;
      if ( ( modifyLength3 > 0 ) && ( mode3 == 4 ) ) trig3State = trig4In ;
      if ( ( modifyLength4 > 0 ) && ( mode4 == 4 ) ) trig4State = trig1In ;
      break;
    }
  }
  */