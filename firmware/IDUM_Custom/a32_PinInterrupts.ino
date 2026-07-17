//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// A few external interrupts that make the looper recording more accurate.  This will be expanded in the next firmware.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void trig1InterruptHandler() {

  forceTrig1LoopRecord = 1;
  forceTrig1LoopRecordTime = micros();
}

void trig2InterruptHandler() {

  forceTrig2LoopRecord = 1;
  forceTrig2LoopRecordTime = micros();
}

void trig3InterruptHandler() {

  forceTrig3LoopRecord = 1;
  forceTrig3LoopRecordTime = micros();
}

void trig4InterruptHandler() {

  forceTrig4LoopRecord = 1;
  forceTrig4LoopRecordTime = micros();
}