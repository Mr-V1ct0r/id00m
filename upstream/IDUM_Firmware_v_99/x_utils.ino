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
  //bitWrite ( optionByte, 4, loopEnable ) ;    <<needs to be enabled once we allow the loop being saved over power cycles

  return optionByte;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Write all of the relevant option data to the EEPROM

void saveOptions() {

  byte optionByte = 0;

  EEPROM.write(3, paramResolution);
  EEPROM.write(4, lengthResolution);

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
  //loopEnable = bitRead ( optionByte, 4 );
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void saveLoopData() {  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<this needs to be finalized

  //optionByte = getOptionByte() ;

  //EEPROM.write(1, optionByte );

  //should also save history index
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
