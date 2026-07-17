//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// PURPLE LED mode - CUSTOM: SCATTER.  Holds slot 7 (the "purple" custom slot).  Replaces CONTAMINATION, which is kept dead-but-
// present in b81_contaminationMode for easy restore.
//
// Randomly displaces each incoming trigger OFF its spot to a QUANTIZED sub-position of that channel's own input gap, so hits land
// off-grid for a glitchy / IDM feel.  Because it moves hits into the GAPS between other channels' hits (rather than stacking them
// onto the same instant like contamination did), it plays nicely with linear drumming - a displaced hit that lands in a gap is the
// only thing firing that instant, so linear passes it.
//
//   PARAM is BIPOLAR:  center = off (pass-through).
//                      CCW = SINGLE  scatter - each triggered hit can be thrown to one quantized sub-slot.
//                      CW  = CASCADE scatter - a thrown hit can chain into up to SCATTER_MAXHOPS hits (stutters/rolls).
//          Distance from center = a DISTANCE BIAS that OPENS THE RANGE.  The knob sets how far a hit CAN reach (with a fat tail, so
//          even low settings occasionally throw far), but the actual distance is then picked uniformly up to that ceiling - so short,
//          medium, and long throws stay possible at every setting and high PARAM opens the full spread instead of locking to "far".
//          Because a hit whose reach lands on 0 stays on its spot, the one knob sets BOTH how far AND how many hits move - so there is
//          no separate "amount" control; CHANCE handles how OFTEN the whole effect is active (its bursty on/off), the orthogonal axis.
//
// Displacements snap to a ÷D subdivision of the channel's own trigger gap, where D is drawn from the SAME odd/even/powerOf2 table the
// ratchet-family modes read (see scatterDivisor) - so the shared paramResolution menu option means the same subdivisions here as it
// does everywhere else (odd = wonky ÷3/÷5/÷7 available; powerOf2 = tight/binary).  Self-scaling, same philosophy as delay/ratchet.
// A scheduled hit is a 10 ms pulse, timed off the same lastTrigNTime the delay mode uses; a hit that never walks stays a clean
// pass-through.  Rolls use the seeded RNG.  Only a real input edge builds a schedule, so it can get busy but never runs away.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#define SCATTER_BIASSTEPS 8   //resolution of the PARAM distance-bias curve (fixed, so throw distance feels the same at every grid)
#define SCATTER_MAXHOPS   3   //cascade cap: one input can spawn at most this many scattered hits (must match scatterOffsetN[] size in a00)

//Landing GRID for one hit: a divisor D drawn from the SAME odd/even/powerOf2 table the ratchet-family modes read, so the shared
//paramResolution menu option means the same subdivisions here as everywhere else.  We sample the table's value set (random index over
//the symmetric 16-entry curve, so it's weighted by how often each subdivision appears) rather than indexing by knob position - because
//scatter spends PARAM on the bias, not on picking a count.  powerOf2 -> {1,2,4,8} (binary/tight); even -> {1,2,4,6,8}; odd -> {1..8}
//(adds ÷3,÷5,÷7 = triplet/quintuplet/septuplet = wonky).  A divisor of 1 means "no subdivision" -> the hit stays on its spot.
unsigned int scatterDivisor() {
  unsigned int i = random(16);
  if (paramResolution == 2) return powerOf2[i];
  if (paramResolution == 1) return even[i];
  return odd[i];
}

//One quantized displacement whose DISTANCE is biased by PARAM.  Two stages so the range OPENS UP with PARAM without ever locking to
//one distance: (1) grow a CEILING outward one sub-slot at a time, each step gated by 'bias' (0..~1024 = distance from PARAM center) -
//so how far a hit CAN go rises with the knob (with a geometric tail: even low settings occasionally stretch the ceiling far).  Then
//(2) pick the actual reach UNIFORMLY in 0..ceiling - so short, medium, and long stay possible at every setting, and high PARAM opens
//the full spread instead of forcing max.  Returns 0 when the hit stays on its spot (reach 0), which happens most often near center.
unsigned long biasedOffset(unsigned long gap, unsigned int bias) {
  unsigned int D = scatterDivisor();                           //grid for this hit, from the active resolution's table (odd/even/pwr2)
  if (D < 2) return 0;                                         //divisor 1 = "no subdivision" -> the hit stays on its spot

  //PARAM bias -> a ceiling as a FRACTION of the gap (0..SCATTER_BIASSTEPS), independent of the grid so throw distance feels the same at
  //every resolution.  A uniform target up to that ceiling keeps the short/med/long spread; then snap it onto this hit's D-grid.
  unsigned int c = 0;
  while ((c < SCATTER_BIASSTEPS) && (random(1024) < bias)) c++;
  unsigned int target = random(c + 1);                         //0..ceiling, in 1/SCATTER_BIASSTEPS units
  unsigned int slot = ((unsigned long)target * D) / SCATTER_BIASSTEPS;
  if (slot > D - 1) slot = D - 1;                              //keep displacement inside the gap (never lands on the next spot)
  return gap * (unsigned long)slot / D;
}

//Build a channel's scatter schedule on its input edge.  Distance-from-center biases HOW FAR each hit is thrown; because a hit that
//doesn't walk stays on its spot, the same knob also sets HOW MANY hits move (no separate amount control - CHANCE handles how often).
//Only DISPLACED hits (offset > 0) are scheduled; an all-on-spot result returns count 0 = clean pass-through.  CCW = at most one hit;
//CW = a cascade of up to SCATTER_MAXHOPS, each hop continuing only if another bias roll passes (so it also deepens toward full CW).
unsigned int buildScatterSchedule(unsigned long gap, unsigned int param, unsigned long *offsets) {
  unsigned int count = 0;

  if (param <= 512) {                                  //CCW: single throw, distance biased further out as you go CCW
    unsigned int bias = (512 - param) * 2;
    unsigned long off = biasedOffset(gap, bias);
    if (off > 0) offsets[count++] = off;               //never walked -> leave it on the spot (pass-through)
  }

  else {                                               //CW: cascade - deeper AND further as you go CW
    unsigned int bias = (param - 512) * 2;
    for (unsigned int h = 0; h < SCATTER_MAXHOPS; h++) {
      unsigned long off = biasedOffset(gap, bias);
      if (off > 0) offsets[count++] = off;             //on-spot hops aren't scheduled; the input still passes if count stays 0
      if (random(1024) >= bias) break;                 //chain length grows with CW too, bounded to SCATTER_MAXHOPS
    }
  }

  return count;
}

//True if 'elapsed' (micros since this channel's trigger) falls in the 10 ms window of any scheduled scattered hit.
bool scatterFiring(unsigned long elapsed, unsigned long *offsets, unsigned int count) {
  for (unsigned int k = 0; k < count; k++)
    if ((elapsed > offsets[k]) && (elapsed < offsets[k] + 10000)) return true;
  return false;
}

void scatter() {

  unsigned long currentMicros = micros();  //sample the system time once

  //clock channel: standard one-shot at the start of the modification, so the cycle/follow behaviour matches every other mode
  if ((modifyLength > 0) && (mode == 7)) clockState = ((currentMicros - firstStepTime) < 10000);

  //Each channel: on its input edge, (re)build the scatter schedule from PARAM.  Then either pass the input on its spot (roll failed,
  //count 0) or fire ONLY at the scheduled sub-slots - the on-spot hit is displaced away, not doubled.

  if ((modifyLength1 > 0) && (mode1 == 7)) {
    if (trig1Edge) scatterCount1 = buildScatterSchedule((unsigned long)lastTrig1Interval, modifyParam1, scatterOffset1);
    trig1State = (scatterCount1 == 0) ? trig1In : scatterFiring(currentMicros - lastTrig1Time, scatterOffset1, scatterCount1);
  }

  if ((modifyLength2 > 0) && (mode2 == 7)) {
    if (trig2Edge) scatterCount2 = buildScatterSchedule((unsigned long)lastTrig2Interval, modifyParam2, scatterOffset2);
    trig2State = (scatterCount2 == 0) ? trig2In : scatterFiring(currentMicros - lastTrig2Time, scatterOffset2, scatterCount2);
  }

  if ((modifyLength3 > 0) && (mode3 == 7)) {
    if (trig3Edge) scatterCount3 = buildScatterSchedule((unsigned long)lastTrig3Interval, modifyParam3, scatterOffset3);
    trig3State = (scatterCount3 == 0) ? trig3In : scatterFiring(currentMicros - lastTrig3Time, scatterOffset3, scatterCount3);
  }

  if ((modifyLength4 > 0) && (mode4 == 7)) {
    if (trig4Edge) scatterCount4 = buildScatterSchedule((unsigned long)lastTrig4Interval, modifyParam4, scatterOffset4);
    trig4State = (scatterCount4 == 0) ? trig4In : scatterFiring(currentMicros - lastTrig4Time, scatterOffset4, scatterCount4);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
