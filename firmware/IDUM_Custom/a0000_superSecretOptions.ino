//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Extra under the hood options.  You can change these values and modify how your IDUM works easily!  Read the description for each option to
//learn what it does.

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//!!!!!!!!!!!!!THIS IS A NEW AND IMPORTANT VARIABLE.  IF YOUR LED RING LOOKS LIKE THE LEDS ARE OUT OF ORDER READ THIS SECTION!!!!!!!!!!!!!!!!!!!

//There are x2 slightly different hardware versions of IDUM that require a software fix to handle.  Basically the LEDs in the main LED ring are 
//in a different order for IDUMs from batch 1 & 2 (serial number 613 and below) than ones after that batch.  This is for ease of routing because
//some of these IDUMS were on 2 layer boards with single-sided SMD assembly.  Basically setting "LEDTweakSetEEPROM" to '1' will make the EEPROM set
//a flag high that tells the hardware to use the new LED order but this only happens when the EEPROM is re-initialized. Setting "forceEEPROMReset" 
//to '1' will make sure that the EEPROM re-initializes when this firmware is flashed.  The actual variable that is used in IDUM's display code is
// "LEDTweak" which uses the new order when it's set to '1' and the old order when it's set to '0'.  This is all so that I can keep supporting older
//IDUMs with future firmware updates and without making you all look through these menus which is apparently above many people's abilities.  Don't
//say that I don't work hard for you folks.

//______________________________________________________________________________________________________________________________________________

//IF YOUR LED RING LOOKS OUT OF ORDER, SET "forceEEPROMReset" to 1 and then change "LEDTweakSetEEPROM" TO EITHER 1 OR 0 AND SEE WHICH ONE LOOKS
//CORRECT.  THEN CHANGE "forceEEPROMReset" TO 0 AND UPLOAD FIRMWARE ONE MORE TIME.  OTHERWISE YOU WILL NOT BE ABLE TO SAVE OPTIONS.

//First set this option to 1 in order to force the EEPROM to reset
bool forceEEPROMReset = 0;

//Then try either 1 or 0 in this setting and see which mode makes the LEDs the right order.  If you have hardware 1.3 or above you want this to
//be 1, lower than hardware 1.3 is a 0.  Hardware 1.3 is the first PCB version that has a big white square for the serial number, before I just
//wrote it on the bare PCB like a cave man.  Please make sure to upload this firmware again with "forceEEPROMReset" set to '0' once everything
//looks right.  You can wear out your EEPROM by forgetting this.

//CUSTOM (ID00M): this is the LED order ID00M starts a module on before anyone holds LOOP.  ID00M defaults to the OLD order (0) because its
//installed base is mostly pre-1.3 hardware; owners of revision 1.3 and up hold LOOP once to switch.  Eli's vanilla v.99 ships this as 1.
bool LEDTweakSetEEPROM = 0;

//This sets up the variable that handles the new LED order.  I just put it in the same place to hopefully make things easier, the EEPROM stores
// a flag set to 0 or 1 that I set myself when uploading the hardware test version of this firmware.  I'm leaving all of this available to
//you folks in case something goes wrong.

bool LEDTweak = 0;   //CUSTOM (ID00M): matches the old-order default above; initializeEEPROM sets the real value from EEPROM / LEDTweakSetEEPROM

bool changeLEDOrder = 0;    //needed this to be global, it's the trigger that is called when you hold a button down.

//_______________________________________________________________________________________________________________________________________________

//This controls the speed of the slow clock cycle function.
//Increase the value if your favorite sequencer is not playing nicely with IDUM's clock manipulation.
//Decrease the value if you think that your favorite sequencer might be able to run faster with IDUM's clock manipulation.

unsigned int slowClockSpeed = 2400;

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Normally after a modification is activated IDUM waits for a trigger to come in each channels gate input before adding new gates to that channels
//output.  This makes it so that for example during a burst you won't end up with bursts being added to steps where no triggers are present unless
//you are using a particularly long length setting and even then bursts are only added after incoming triggers and not before.  Turning this option
//on will disable the input trigger checking and make it so that IDUM will add bursts and stuff like that as soon as a modification becomes active.

bool chokeDisable = 0;        //default = 0

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This makes it so that the param function cant change while a modification is active.
//Turning this off will make it so that param is only sampled once at the beginning of each modification and stays the same over that modification.
//In "split" mode this makes it so that each channel can have a different param value as well as mode value.
//This doesn't affect how param interacts with the loop mode.
//CUSTOM (ID00M): this is now also a normal menu toggle - it's the SECOND click of the split slot (page 1, ring slot 7),
//shown by a blinking loop LED, and it persists in EEPROM (option-byte bit 4).  This value is just the pre-EEPROM default.

bool freezeParam = 0;         //default = 0

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//IDUM will sometimes treat modifications that are triggered one after the other as one long modification.  This works for stuff like calculating
//the individual clock chokes, the step in the preset rhythms in break mode, the start time of various burst modes etc.  I felt like this was 
//useful because sometimes you would trigger a modification at one time during the cycle of your incoming pattern and it would sound different than
//if a modification activated at another time in the pattern.  This option disables the carryover behavior and makes IDUM treat each modification
//as it's own individual modification.

bool carryOverDisable = 0;    //default = 0

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\