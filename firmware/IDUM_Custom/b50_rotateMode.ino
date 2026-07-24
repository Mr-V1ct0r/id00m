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

  //B15: single source of truth for the scramble permutation.  rotateSrc[outputChannel-1][rotation] = the INPUT channel (1-4) that feeds
  //that output at the given rotation.  This replaces four hand-written switch statements that were a copy-paste hazard (see the B9 fix,
  //where one channel switched on the wrong rotation).  Values are exactly the old case tables, verified column by column.
  static const unsigned int rotateSrc[4][8] = {
    { 1, 3, 4, 2, 1, 4, 3, 2 },  //output 1
    { 3, 4, 3, 1, 2, 1, 4, 3 },  //output 2
    { 2, 1, 2, 4, 3, 2, 1, 4 },  //output 3
    { 4, 2, 1, 3, 4, 3, 2, 1 },  //output 4
  };

  //Snapshot inputs and chokes before routing so a destination we overwrite cannot become a source another channel reads mid-pass.
  bool in[4]    = { trig1In,    trig2In,    trig3In,    trig4In    };
  bool choke[4] = { trig1Choke, trig2Choke, trig3Choke, trig4Choke };

  //B15: each output takes both the routed input AND that SOURCE channel's choke.  Rotate only re-routes real input gates (it never generates
  //new ones), so the choke's usual job - "wait for THIS channel's own input before letting burst/ratchet embellishments out" - is wrong
  //here: it muted a rotated signal on any destination channel that had no input of its own.  Following the source choke keeps the "wait for
  //the first hit" behaviour keyed to the voice actually being routed, so a single input scrambles onto every output as intended.
  if ((modifyLength1 > 0) && (mode1 == 4)) { unsigned int s = rotateSrc[0][rotation1]; trig1State = in[s - 1]; trig1Choke = choke[s - 1]; }

  if ((modifyLength2 > 0) && (mode2 == 4)) { unsigned int s = rotateSrc[1][rotation2]; trig2State = in[s - 1]; trig2Choke = choke[s - 1]; }

  if ((modifyLength3 > 0) && (mode3 == 4)) { unsigned int s = rotateSrc[2][rotation3]; trig3State = in[s - 1]; trig3Choke = choke[s - 1]; }

  if ((modifyLength4 > 0) && (mode4 == 4)) { unsigned int s = rotateSrc[3][rotation4]; trig4State = in[s - 1]; trig4Choke = choke[s - 1]; }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\