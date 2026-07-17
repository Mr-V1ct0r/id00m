//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//a space for extra little functions that are helpful
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//its like modulo but when a number turns negative it wraps around to the top of the range
int wrapAround(int number, int range) {

  if (range == 0) return 0;
  if (range < 0) range = range * -1;
  if (number >= 0) return number % range;
  else
    while (number < 0) number += range;
  return number % range;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//turns the bits in the mode array into a single byte for saving it more easily in the EEPROM

byte getModeByte() {

  byte modeByte = 0;

  for (int i = 0; i < 8; i++) {
    modeByte += (activeModes[i] << (7 - i));
  }

  return modeByte;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Take the byte of saved modes from the EEPROM and puts each bit into the mode array

void setModeArray(byte modeByte) {

  bool modeBit = 0;

  for (int i = 0; i < 8; i++) {
    modeBit = bitRead(modeByte, (7 - i));
    activeModes[i] = modeBit;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//returns a byte made out of all of the options combined as bits.

byte getOptionByte() {

  byte optionByte = 0;

  bitWrite(optionByte, 0, analogClock);
  bitWrite(optionByte, 1, splitMode);
  bitWrite(optionByte, 2, doubleLength);
  bitWrite(optionByte, 3, loopInputBehavior);
  bitWrite(optionByte, 4, freezeParam);     //CUSTOM: freeze (split-mode PARAM snapshot) - reuses the old reserved loopEnable bit
  bitWrite(optionByte, 5, linearDrumming);  //CUSTOM: linear-drumming on/off
  optionByte |= (mergeState & 3) << 6;      //CUSTOM F6: MERGE state (0-3) in the two spare top bits

  return optionByte;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Pack the 4-channel linear-drumming priority order into one byte (2 bits per rank).  Stored at EEPROM address 5.
byte getPriorityByte() {
  byte b = 0;
  for (unsigned int i = 0; i < 4; i++) b |= ((priorityOrder[i] - 1) & 3) << (2 * i);
  return b;
}

//Unpack the priority byte.  Validates that it is a real permutation of 1-4 (an uninitialised/old EEPROM won't be) and falls back to the
//default TR1 > TR2 > TR3 > TR4 if not.
void setPriorityArray(byte b) {
  bool seen[5] = { false, false, false, false, false };
  bool valid = true;
  for (unsigned int i = 0; i < 4; i++) {
    unsigned int ch = ((b >> (2 * i)) & 3) + 1;
    priorityOrder[i] = ch;
    if (ch < 1 || ch > 4 || seen[ch]) valid = false;
    else seen[ch] = true;
  }
  if (!valid) { priorityOrder[0] = 1; priorityOrder[1] = 2; priorityOrder[2] = 3; priorityOrder[3] = 4; }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F13/F14): pack the break-mode secondary-menu settings into one EEPROM byte (addr 6).  bits 0-1 = breakBank (0-3),
//bits 2-3 = breakSpeed-1 (0-3 -> 1-4x).  The top nibble is always written 0, which doubles as an "initialised" marker (see below).
//Kept out of the option byte so break settings have room to grow.
byte getBreakByte() {
  byte b = 0;
  b |= (breakBank & 3);
  b |= ((breakSpeed - 1) & 3) << 2;
  return b;
}

//Unpack the break byte.  A never-written addr 6 (old or first-run EEPROM, or a module flashed with pre-F13 firmware) reads 0xFF; since we
//always write the top nibble as 0, any set high bit up there means "uninitialised" -> fall back to the defaults (bank 0, 1x).  Otherwise
//split out the fields, defensively clamping the bank to one that actually exists.
void setBreakSettings(byte b) {
  if (b & 0xF0) { breakBank = 0; breakSpeed = 1; return; }  //top nibble set -> uninitialised byte -> defaults
  breakBank = b & 3;
  if (breakBank >= BREAK_BANK_COUNT) breakBank = 0;
  breakSpeed = ((b >> 2) & 3) + 1;                          //1-4x (2 bits can't exceed 4, so no clamp needed)
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//CUSTOM (F9): pack/unpack the four per-channel clock-multiplier values.  Each value is 0-7 (index into f9Num/f9Den) and lives in a nibble,
//so bit 3 of every nibble is always 0 - that doubles as the "initialised" marker: a never-written addr 7/8 reads 0xFF, whose nibble-high
//bits are set, so we fall back to all-x1.  Two bytes: addr 7 = ch1|ch2, addr 8 = ch3|ch4.
byte getF9ByteLow()  { return (f9Value[0] & 7) | ((f9Value[1] & 7) << 4); }
byte getF9ByteHigh() { return (f9Value[2] & 7) | ((f9Value[3] & 7) << 4); }

void setF9Settings(byte lo, byte hi) {
  if ((lo & 0x88) || (hi & 0x88)) {                             //any nibble-high bit set -> uninitialised byte -> default every channel to x1
    f9Value[0] = f9Value[1] = f9Value[2] = f9Value[3] = 3;      //3 = x1
    return;
  }
  f9Value[0] = lo & 7;  f9Value[1] = (lo >> 4) & 7;
  f9Value[2] = hi & 7;  f9Value[3] = (hi >> 4) & 7;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Write all of the relevant option data to the EEPROM

void saveOptions() {

  byte optionByte = 0;

  EEPROM.write(3, paramResolution);
  EEPROM.write(4, lengthResolution);
  EEPROM.write(5, getPriorityByte());  //CUSTOM: save the linear-drumming priority order
  EEPROM.write(6, getBreakByte());     //CUSTOM F13: save the active break pattern bank
  EEPROM.write(7, getF9ByteLow());     //CUSTOM F9: save per-channel clock multipliers (ch1|ch2)
  EEPROM.write(8, getF9ByteHigh());    //CUSTOM F9: save per-channel clock multipliers (ch3|ch4)

  optionByte = getOptionByte();
  EEPROM.write(1, optionByte);
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Takes the option byte of combined option bits from the EEPROM and sets each option accordingly

void setOptions(byte optionByte) {

  analogClock = bitRead(optionByte, 0);
  splitMode = bitRead(optionByte, 1);
  doubleLength = bitRead(optionByte, 2);
  loopInputBehavior = bitRead(optionByte, 3);
  freezeParam = bitRead(optionByte, 4);     //CUSTOM: freeze (0 on any pre-freeze EEPROM, since bit 4 was always written 0)
  linearDrumming = bitRead(optionByte, 5);  //CUSTOM: linear-drumming on/off
  mergeState = (optionByte >> 6) & 3;        //CUSTOM F6: MERGE state from the two spare top bits (0 on any pre-F6 EEPROM -> OFF)
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void saveLoopData() {  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<this needs to be finalized

  //optionByte = getOptionByte() ;

  //EEPROM.write(1, optionByte );

  //should also save history index
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
