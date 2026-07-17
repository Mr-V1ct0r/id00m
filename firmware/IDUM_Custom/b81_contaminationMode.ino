//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// RETIRED 2026-07-15 - kept dead-but-present for easy restore.  Slot 7 now hosts SCATTER (b82_scatterMode); contaminate() below is
// no longer dispatched from a90_handleOutputs.  It fought linear drumming (its ghosts fire on the SAME instant as their source, so
// the lowest-priority channel was always muted), which is why scatter - a time-DISPLACEMENT mode that moves hits into the gaps -
// replaced it.  To restore: point a90 case 7 back at contaminate().  (contaminate1..4 state vars are still declared in a00.)
//
// PURPLE LED mode - CUSTOM: CONTAMINATION.  Replaced the old CLOCK SKIP in slot 7 (the "purple" slot reserved for custom modes).
// Each incoming trigger can "infect" a RANDOM other channel with a 10 ms ghost hit, so triggers spread virally across the panel.
//   PARAM is BIPOLAR:  center = off.
//                      CCW  = SINGLE-HOP  (one random channel gets infected) - controlled scatter.
//                      CW   = CASCADE     (the infection chains through up to 3 random channels) - viral.
//          Distance from center sets the spread probability (and, on the CW side, how deep the chain tends to reach).
//   CHANCE gates the whole effect on/off, like every mode.
// Rolls use the seeded RNG.  A real input edge is what starts a spread; ghost hits do not themselves start new spreads (the cascade is
// bounded by the chain length only), so it can get dense but never runs away infinitely.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Pick a random channel (1-4) that is NOT 'cur'.
unsigned int randomOtherChannel(unsigned int cur) {
  unsigned int r = random(3) + 1;  //1..3
  if (r >= cur) r += 1;            //shift past 'cur' to skip it -> gives 1..4 excluding cur
  return r;
}

//Infect a channel right now: stamp its ghost time and arm its choke so the ghost isn't muted on a channel that got no input of its own.
void applyInfection(unsigned int ch, unsigned long t) {
  switch (ch) {
    case 1: contaminate1 = t; trig1Choke = 1; break;
    case 2: contaminate2 = t; trig2Choke = 1; break;
    case 3: contaminate3 = t; trig3Choke = 1; break;
    case 4: contaminate4 = t; trig4Choke = 1; break;
  }
}

//Spread an infection out from 'source'.  Single-hop infects one random target; cascade chains through random targets, each further
//hop continuing only if it passes another 'prob' roll, capped at 3 hops so a single trigger can never light up more than a few channels.
void spreadInfection(unsigned int source, unsigned long t, bool cascade, unsigned int prob) {
  unsigned int cur = source;
  unsigned int maxHops = cascade ? 3 : 1;
  for (unsigned int h = 0; h < maxHops; h++) {
    unsigned int target = randomOtherChannel(cur);
    applyInfection(target, t);
    cur = target;
    if (!cascade) break;
    if (random(1024) >= prob) break;  //chain stops the moment a hop's roll fails
  }
}

void contaminate() {

  unsigned long currentMicros = micros();  //sample the system time once

  //clock channel: standard one-shot at the start of the modification, so the cycle/follow behaviour matches every other mode
  if ((modifyLength > 0) && (mode == 7)) clockState = ((currentMicros - firstStepTime) < 10000);

  //Each channel, on its own input edge, rolls to spread.  PARAM bipolar around center (512): CCW single-hop, CW cascade; magnitude
  //= probability.  Then the channel passes its own input OR any ghost it has been infected with in the last 10 ms.

  if ((modifyLength1 > 0) && (mode1 == 7)) {
    if (trig1Edge) {
      if (modifyParam1 <= 512) { unsigned int p = (512 - modifyParam1) * 2; if (random(1024) < p) spreadInfection(1, currentMicros, false, p); }
      else                     { unsigned int p = (modifyParam1 - 512) * 2; if (random(1024) < p) spreadInfection(1, currentMicros, true,  p); }
    }
    trig1State = trig1In || ((currentMicros - contaminate1) < 10000);
  }

  if ((modifyLength2 > 0) && (mode2 == 7)) {
    if (trig2Edge) {
      if (modifyParam2 <= 512) { unsigned int p = (512 - modifyParam2) * 2; if (random(1024) < p) spreadInfection(2, currentMicros, false, p); }
      else                     { unsigned int p = (modifyParam2 - 512) * 2; if (random(1024) < p) spreadInfection(2, currentMicros, true,  p); }
    }
    trig2State = trig2In || ((currentMicros - contaminate2) < 10000);
  }

  if ((modifyLength3 > 0) && (mode3 == 7)) {
    if (trig3Edge) {
      if (modifyParam3 <= 512) { unsigned int p = (512 - modifyParam3) * 2; if (random(1024) < p) spreadInfection(3, currentMicros, false, p); }
      else                     { unsigned int p = (modifyParam3 - 512) * 2; if (random(1024) < p) spreadInfection(3, currentMicros, true,  p); }
    }
    trig3State = trig3In || ((currentMicros - contaminate3) < 10000);
  }

  if ((modifyLength4 > 0) && (mode4 == 7)) {
    if (trig4Edge) {
      if (modifyParam4 <= 512) { unsigned int p = (512 - modifyParam4) * 2; if (random(1024) < p) spreadInfection(4, currentMicros, false, p); }
      else                     { unsigned int p = (modifyParam4 - 512) * 2; if (random(1024) < p) spreadInfection(4, currentMicros, true,  p); }
    }
    trig4State = trig4In || ((currentMicros - contaminate4) < 10000);
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
