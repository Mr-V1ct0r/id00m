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
  //CUSTOM (MERGE CUT choke alignment): under CUT (op==2) the output is no longer the routed SOURCE gate - applyMerge (a90) replaces it with this
  //channel's OWN input (trigNIn && cutRotatePassN, hocket/duck).  So CUT must follow the OWN choke (choke[N-1]), not the source's: pairing the
  //own gate with a foreign choke let the source choke drop at a modification boundary and CHOP the own gate into a flam.  REPLACE/ADD still
  //carry the source gate, so they keep the source choke.  The condition mirrors applyMerge's guard exactly: CUT only actually swaps the output
  //when !loopEnable && mergeOpForMode(modeN)==2, so we swap the choke on the same terms (during loop playback applyMerge bails and the output
  //stays the source gate, which must keep the source choke).
  if ((modifyLength1 > 0) && (mode1 == 4)) { unsigned int s = rotateSrc[0][rotation1]; trig1State = in[s - 1]; trig1Choke = (!loopEnable && mergeOpForMode(mode1) == 2) ? choke[0] : choke[s - 1]; }

  if ((modifyLength2 > 0) && (mode2 == 4)) { unsigned int s = rotateSrc[1][rotation2]; trig2State = in[s - 1]; trig2Choke = (!loopEnable && mergeOpForMode(mode2) == 2) ? choke[1] : choke[s - 1]; }

  if ((modifyLength3 > 0) && (mode3 == 4)) { unsigned int s = rotateSrc[2][rotation3]; trig3State = in[s - 1]; trig3Choke = (!loopEnable && mergeOpForMode(mode3) == 2) ? choke[2] : choke[s - 1]; }

  if ((modifyLength4 > 0) && (mode4 == 4)) { unsigned int s = rotateSrc[3][rotation4]; trig4State = in[s - 1]; trig4Choke = (!loopEnable && mergeOpForMode(mode4) == 2) ? choke[3] : choke[s - 1]; }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\