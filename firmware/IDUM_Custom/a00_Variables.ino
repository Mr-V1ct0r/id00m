//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

///////////////////////////////////////////////////////////////////////gate input pins
#define clockPin  15
#define trig1Pin  14
#define trig2Pin  9
#define trig3Pin  11
#define trig4Pin  10

///////////////////////////////////////////////////////////////////////gate outputs pins
#define clockOut  13
#define trig1Out  7
#define trig2Out  8
#define trig3Out  6
#define trig4Out  5

///////////////////////////////////////////////////////////////////////display output pins
#define mode1LED  2
#define mode2LED  3
#define mode3LED  4

///////////////////////////////////////////////////////////////////////analog input pins
#define modePin 21
#define probPin  20
#define lengPin  18
#define paramPin 19
#define loopButton 16

#define loopLED 1

///////////////////////////////////////////////////////////////////////switch - button inputs
#define loopGate  12
#define cycleSw  17
#define modeBtn  0

/////////////////////////////////////////////////////////////////////edge detection
bool clockOld = 0;
bool trig1Old = 0;
bool trig2Old = 0;
bool trig3Old = 0;
bool trig4Old = 0;
bool modeBtnInOld = 0;
bool modeBtnIn = 0;
unsigned long modeBtnDebounceTime = 0;
bool modeBtnReset = 0;
bool menuModeReset = 0;

unsigned int clockTick2 = 0;
unsigned int clockTick2Old = 0;
unsigned long tickInterval = 61;     //I think this is the value makes 120 BPM around 64 ticks
unsigned long tickCounter = 0;


bool times2Clock = 0;
bool times4Clock = 0;
bool times2ClockEdge = 0;
bool times4ClockEdge = 0;
bool times2ClockOld = 0;
bool times4ClockOld = 0;
unsigned long times2ClockTime = 0;
unsigned long times4ClockTime = 0;
unsigned int clockTick = 0;

bool IDUMLEDebug = 0;

bool loopGateIn = 0;
bool loopGateInOld = 0;
bool loopButtonOld = 0;
bool loopButtonIn = 0;

unsigned long lastClockInterval = 0;
unsigned long lastClockTime = 0;
unsigned long lastTrig1Interval = 0;
unsigned long lastTrig1Time = 0;
unsigned long lastTrig2Interval = 0;
unsigned long lastTrig2Time = 0;
unsigned long lastTrig3Interval = 0;
unsigned long lastTrig3Time = 0;
unsigned long lastTrig4Interval = 0;
unsigned long lastTrig4Time = 0;

unsigned long lastTrig1FallingTime = 0 ;
unsigned long lastTrig2FallingTime = 0 ;
unsigned long lastTrig3FallingTime = 0 ;
unsigned long lastTrig4FallingTime = 0 ;

bool clockEdge = 0;
bool clockFallingEdge = 0;
bool trig1Edge = 0;
bool trig2Edge = 0;
bool trig3Edge = 0;
bool trig4Edge = 0;
bool trig1FallingEdge = 0;
bool trig2FallingEdge = 0;
bool trig3FallingEdge = 0;
bool trig4FallingEdge = 0;
bool modeBtnEdge = 0;
bool modeBtnFallingEdge = 0;

bool clockState = 0;
bool clockStateOld = 0; //helpful for keeping weird clock modes in time with the maintainCycle function
bool trig1State = 0;
bool trig2State = 0;
bool trig3State = 0;
bool trig4State = 0;

bool clockIn = 0;
bool trig1In = 0;
bool trig2In = 0;
bool trig3In = 0;
bool trig4In = 0;

bool clockChoke = 0;
bool trig1Choke = 0;
bool trig2Choke = 0;
bool trig3Choke = 0;
bool trig4Choke = 0;

////////////////////////////////////////////////////////////////////CV input variables

unsigned int modeRead = 0;
unsigned int probRead = 0;
unsigned int lengRead = 0;
unsigned int paramRead = 0;

unsigned int unscaledParam = 0;

///////////////////////////////////////////////////////////////////resolutions

bool menuMode = 0;

unsigned int odd[ 16 ] = { 8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8 } ;
unsigned int powerOf2[16] = { 8, 8, 4, 4, 2, 2, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8 } ;
unsigned int even[16] = { 8, 6, 6, 4, 4, 2, 2, 1, 1, 2, 2, 4, 4, 6, 6, 8 } ;
unsigned int paramResolution = 0;
unsigned int lengthResolution = 0;
unsigned int lengthScaled = 0;
unsigned int ratchetAmount = 0;

//////////////////////////////////////////////////////////////////////menu options

bool analogClock = 0;
bool doubleLength = 0 ; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<needs to be implemented
bool splitMode = 0;     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<needs to be implemented

//CUSTOM: SECONDARY setup menu (hold REMOVE MODE past 2.5s; LEDs 2-5 blink to show you're on it) and LINEAR DRUMMING.
bool secondaryMenu = 0;                       //true when the deeper "page 2" menu is active
bool linearDrumming = 0;                      //when on, only the highest-priority active trigger passes each instant
unsigned int priorityOrder[4] = { 1, 2, 3, 4 };  //rank -> channel: [0] is top priority.  Default TR1 > TR2 > TR3 > TR4.

//CUSTOM (F6 - MERGE): how the active mode's output combines with the incoming triggers, selected in the secondary menu at ring slot 5.
//0 = OFF (REPLACE, stock), 1 = ADD (delay/scatter/rotate/break layer onto the input), 2 = CUT (rotate/break subtract from the input),
//3 = MIX (delay/scatter ADD, rotate/break CUT).  Only ever touches those four modes - the trigger-anchored modes (hold/burst/ratchet/
//ball) stay REPLACE in every state.  Persisted in the option byte (bits 6-7).  See mergeOpForMode/applyMerge in a90_handleOutputs.
unsigned int mergeState = 0;
#define MERGE_STATE_COUNT 4
unsigned long secondaryBlinkTime = 0;         //timing base for the blink-count display
unsigned int secondaryBlinkPhase = 0;         //page-2 priority chase: which rank (0=top .. 3=bottom) the lit dot is pointing at

//CUSTOM (F9 - per-channel clock multiplier): each trigger channel can run its ratcheting modes at its own multiple of the master clock, so
//bursts differ per drum voice.  Edited on a THIRD menu page (the "F9 editor") reached by flipping the CYCLE switch while on page 2; the ring
//shows the current channel's value as a steady dot, the LOOP button steps the channel and the loop LED blinks its number.  f9Value[ch]
//indexes the value tables below (default 3 = x1, so F9 is inert until dialled).  Wired into burst first (b20); ratchet is deliberately left
//out (it already tracks each channel's own input rhythm, so a master-clock multiply would change its character).  Persisted at EEPROM 7-8.
unsigned int f9Value[4] = { 3, 3, 3, 3 };     //per-channel value index; 3 = x1 (no change)
#define F9_VALUE_COUNT 8
//the 8 ring slots -> a rational scale of the clock interval: slot0 /4  /3  /2  x1  x2  x3  x4  x8 (slot7).  A multiply divides the interval
//(faster ticks); a divide multiplies it (slower ticks).  chanClockInterval() in b20 returns lastClockInterval * num / den.
const unsigned int f9Num[F9_VALUE_COUNT] = { 4, 3, 2, 1, 1, 1, 1, 1 };
const unsigned int f9Den[F9_VALUE_COUNT] = { 1, 1, 1, 1, 2, 3, 4, 8 };
#define F9_MIN_PULSE 1000                     //us floor for the proportional pulse width (keeps fast sub-triggers audible without smearing)

bool f9EditPage = 0;                          //true when the F9 editor overlay is showing (only reachable from page 2 via a CYCLE flip)
unsigned int f9EditChannel = 0;               //which channel (0-3) the editor is currently pointed at
bool f9KnobLatched = 0;                       //pickup: the mode dial only takes over a channel's value once it has moved since landing on it
unsigned int f9KnobPosOld = 0;                //last mode-dial detent the editor saw, for the pickup move-detect
bool cycleInOld = 0;                          //previous CYCLE switch state, for detecting a flip while in the menu

//////////////////////////////////////////////////////////////////////mode stuff

//The mode that manipulates the CLOCK output rather than the trigger channels.  Index 7 is the "purple" slot, which the original
//author asks custom firmwares to occupy so users know a mode has been swapped out.  It is also the only mode wired into the
//cycle/follow engine.  The four sites that used to test 'mode == 7' literally now call modeOwnsClock() (see a91_followUtils) so that
//a new NON-clock mode can safely take this slot.  If you write a NEW mode that drives the clock, point SKIP_MODE at its index instead.
const unsigned int SKIP_MODE = 7;

//Set once the RNG has been re-seeded from the arrival time of the first external clock (see seedRNGOnFirstClock in a10_Setup).
//Stock firmware never seeded random() at all, so every power up replayed the identical "random" sequence.
bool rngSeeded = 0;

unsigned int randomValue = 0;
unsigned int testIndex = 0;
unsigned int modeCounter = 0;
unsigned int modeCounterOld = 0;
unsigned int displayedMode = 0;
unsigned int splitModeDisplayCounter = 0;
unsigned int LEDCounter = 100;
bool LEDCounterReset = 0;
bool activeModes[8] { 1, 1, 1, 1, 1, 1, 1, 1 };
unsigned int defaultModeBoundaries[9] { 0, 40, 230, 360, 520, 600, 820, 980, 1024 } ;
unsigned int modeBoundaries[9] { 0, 40, 230, 360, 520, 600, 820, 980, 1024 } ;
unsigned int mode = 0;
unsigned int modifyLength = 0;

bool modificationActive = 0;
bool carryOverActive = 0;

//split mode variables

unsigned int randomValue1 = 0;
unsigned int randomValue2 = 0;
unsigned int randomValue3 = 0;
unsigned int randomValue4 = 0;

unsigned int modifyLengthMain = 0;
unsigned int modifyLength1 = 0;
unsigned int modifyLength2 = 0;
unsigned int modifyLength3 = 0;
unsigned int modifyLength4 = 0;

unsigned int modeMain = 0;
unsigned int mode1 = 0;
unsigned int mode2 = 0;
unsigned int mode3 = 0;
unsigned int mode4 = 0;

unsigned int unscaledParam1 = 0;
unsigned int unscaledParam2 = 0;
unsigned int unscaledParam3 = 0;
unsigned int unscaledParam4 = 0;

unsigned int ratchetAmount1 = 0;
unsigned int ratchetAmount2 = 0;
unsigned int ratchetAmount3 = 0;
unsigned int ratchetAmount4 = 0;

unsigned int modifyParamMain = 0;
unsigned int modifyParam1 = 0;
unsigned int modifyParam2 = 0;
unsigned int modifyParam3 = 0;
unsigned int modifyParam4 = 0;

unsigned int originalModifyLength1 = 0;
unsigned int originalModifyLength2 = 0;
unsigned int originalModifyLength3 = 0;
unsigned int originalModifyLength4 = 0;

unsigned int probabilityModifier1 = 0;
unsigned int probabilityModifier2 = 0;
unsigned int probabilityModifier3 = 0;
unsigned int probabilityModifier4 = 0;

unsigned int modifyParam = 0;
unsigned long firstStepTime = 0;
unsigned int originalModifyLength = 0;
unsigned int lengthPosition = 0;
unsigned int modePosition = 0;
unsigned int modeDialPosition = 0 ;

unsigned int sampledUnscaledParam1 = 0;
unsigned int sampledUnscaledParam2 = 0;
unsigned int sampledUnscaledParam3 = 0;
unsigned int sampledUnscaledParam4 = 0;

unsigned int sampledModifyParam1 = 0;
unsigned int sampledModifyParam2 = 0;
unsigned int sampledModifyParam3 = 0;
unsigned int sampledModifyParam4 = 0;

unsigned int sampledRatchetAmount1 = 0;
unsigned int sampledRatchetAmount2 = 0;
unsigned int sampledRatchetAmount3 = 0;
unsigned int sampledRatchetAmount4 = 0;

///////////////////////////////////////////////////////////////////mode specific variables

bool startModification = 0;
bool endModification = 0;
bool carryOver = 0;

bool startModification1 = 0;
bool startModification2 = 0;
bool startModification3 = 0;
bool startModification4 = 0;

bool endModification1 = 0;
bool endModification2 = 0;
bool endModification3 = 0;
bool endModification4 = 0;

bool carryOver1 = 0;
bool carryOver2 = 0;
bool carryOver3 = 0;
bool carryOver4 = 0;

bool hold1Choke = 0;
bool hold2Choke = 0;
bool hold3Choke = 0;
bool hold4Choke = 0;

int breakIndex1 = 0;
int breakIndex2 = 0;
int breakIndex3 = 0;
int breakIndex4 = 0;

//CUSTOM (F14 - break speed multiplier): how fast the break pattern advances.  1 = stock (2 steps per clock, uses times2ClockEdge
//exactly, so bank 0 is unchanged); 2/3/4 = play the pattern that many times faster for wilder fills.  Selected in the secondary menu at
//ring slot 7 (scatter's LED); persisted in the break byte (EEPROM addr 6, bits 2-3).  For >1x we derive our own finer tick by
//subdividing the measured clock interval into 2*breakSpeed parts (see b70) - breakSubIndexOld tracks the current sub-slot for edge
//detection and breakTickTime anchors the single-trigger pulse window in place of times2ClockTime.
unsigned int breakSpeed = 1;
#define BREAK_SPEED_MAX 4
unsigned int breakSubIndexOld = 0;
unsigned long breakTickTime = 0;
bool breakTickEdge = 0;       //CUSTOM: the shared break advance tick for THIS loop - computed once in updateBreakClock (a20 order), read by breakBeat
bool breakClockStarted = 0;   //CUSTOM (F6): cleared at power-on; set on the first clock edge, where we snap breakFreeIndex to step 0 to lock phase
unsigned long breakStableInterval = 0;  //CUSTOM (F6): running clock period, used to spot a clock stop/restart and re-lock the sculpt-gate phase
#define BREAK_RESYNC_MULT 4   //a clock gap longer than this many normal intervals counts as "stopped" -> the next pulse re-snaps step 0 (raise to loosen)

//CUSTOM (F6 break-CUT sculpt): per-channel "the current pattern step is a hit-step" flag (cell != 0), held for the whole step.  Unlike the
//5ms trigger pulse, this is a step-length gate, so MERGE's CUT can gate the incoming beat INTO the pattern's rhythm (input plays only on
//pattern hit-steps) instead of just blanking a 5ms sliver.  Set in b70, consumed by applyMerge (a90).  It reads breakFreeIndex, NOT the
//normal breakIndexN: the normal index resets to step 0 (a hit) on every input trigger, so an input-anchored gate would pass every hit and
//do nothing.  breakFreeIndex free-runs on the break tick and is never reset by input, so incoming hits fall on the pattern's true phase.
unsigned int breakFreeIndex = 0;
bool breakStepGate1 = 0;
bool breakStepGate2 = 0;
bool breakStepGate3 = 0;
bool breakStepGate4 = 0;

unsigned int trig1Divide = 0;
unsigned int trig2Divide = 0;
unsigned int trig3Divide = 0;
unsigned int trig4Divide = 0;
unsigned int clockDivide = 0;

bool clockRatchet = 0;
bool clockSkip = 0;
bool clockRatchetState = 0;
bool clockRatchetStateOld = 0;
bool forceSkip = 0;
unsigned int skipRatchetAmount = 0;

unsigned long bounce1Time = 0;
unsigned long bounce2Time = 0;
unsigned long bounce3Time = 0;
unsigned long bounce4Time = 0;

//fixes a bug where triggers received right before the clock don't correctly trigger a bounce
bool forceBounce1 = 0;
bool forceBounce2 = 0;
bool forceBounce3 = 0;
bool forceBounce4 = 0;

//CUSTOM (burst-mode thinning): per-channel "drop this trigger" decision, rolled once per incoming trigger.  See b20_burstMode.
bool burstDrop1 = 0;
bool burstDrop2 = 0;
bool burstDrop3 = 0;
bool burstDrop4 = 0;

//CUSTOM (ratchet-mode CCW divider, b30): per-channel incoming-trigger counter + current drop decision.  On the CCW half of the
//PARAM knob, ratchet mode passes 1 of every ratchetAmount triggers and drops the rest; the counter resets at each modification
//start (a60_ModifyTest) so the first hit always passes.  Replaces the stock interval-multiply "divide" that was inaudible on a groove.
unsigned int ratchetDivCount1 = 0;
unsigned int ratchetDivCount2 = 0;
unsigned int ratchetDivCount3 = 0;
unsigned int ratchetDivCount4 = 0;
bool ratchetDrop1 = 0;
bool ratchetDrop2 = 0;
bool ratchetDrop3 = 0;
bool ratchetDrop4 = 0;

//CUSTOM (scatter mode, slot 7 - b82): per-channel schedule of displaced-hit offsets (microseconds AFTER the channel's input edge)
//plus how many are active for the current gap.  Rebuilt on each trigger edge; count 0 = pass the hit on its spot.  Array size (3)
//must match SCATTER_MAXHOPS in b82_scatterMode.  See b82.
unsigned long scatterOffset1[3];
unsigned long scatterOffset2[3];
unsigned long scatterOffset3[3];
unsigned long scatterOffset4[3];
unsigned int scatterCount1 = 0;
unsigned int scatterCount2 = 0;
unsigned int scatterCount3 = 0;
unsigned int scatterCount4 = 0;

//CUSTOM (contamination mode, slot 7): micros timestamp of the last time each channel was "infected" by its neighbor.  See b81.
//NOTE: contamination is retired from slot 7 (replaced by scatter, b82) but kept dead-but-present for easy restore - these stay.
unsigned long contaminate1 = 0;
unsigned long contaminate2 = 0;
unsigned long contaminate3 = 0;
unsigned long contaminate4 = 0;

//CUSTOM (F13 - break pattern banks): which bank of 16 break patterns is active.  Selected in the secondary menu by pressing MODE while
//pointing at ring slot 6 (break's own LED): 0 = default drum kit (breakBeat1), 1 = polyrhythm/clave (breakBeat2).  Persisted in the
//break-settings byte at EEPROM addr 6 (see x_utils getBreakByte/setBreakSettings, a10_Setup).  b70_breakMode reads whichever bank is live.
unsigned int breakBank = 0;
#define BREAK_BANK_COUNT 2  //number of banks below; bump when you add a breakBeatN.  Also caps the EEPROM clamp (2 banks -> 1 bit).

//Preset rhythms for break mode.  These are also an easy thing to modify for yourself.
//Keep in mind that these rhythms are reset whenever a trigger is received at the relevant trigger input.
//So try not to think of them as a sequence that plays once the modification starts, rather they are like
//a "mask" that is modifying each input in a specific pattern.  That's why most of them start with a '1'
//on the first step.  Try turning these off and see what the effect is to see what I'm talking about.
//A '1' in each cell activates a single trigger whereas a '0' is silent at that step.  Numbers higher than
//'1' triggers a ratchet on that step.  Also worth mentioning that these sequences run at twice the resolution
//of the incoming clock so think of every two steps being one clock.
//CUSTOM: declared 'const' so the banks live in the ATmega4809's memory-mapped flash (.rodata) instead of RAM.  This frees the ~2 KB
//of SRAM the stock non-const array wasted AND lets extra banks cost only flash.  The array is read-only at runtime (b70 only reads it).

const int breakBeat1[16][4][16] =
//////X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |   <<<< This helps a lot for writing the patterns
{ { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Bassdrum   - Pattern 0
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 0
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }, // HighHate   - Pattern 0
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0 } // Percussion - Pattern 0
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bassdrum   - Pattern 1
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 1
    { 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 }, // HighHate   - Pattern 1
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // Percussion - Pattern 1
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Bassdrum   - Pattern 2
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 2
    { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0 }, // HighHate   - Pattern 2
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }  // Percussion - Pattern 2
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Bassdrum   - Pattern 3
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 3
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // HighHate   - Pattern 3
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }  // Percussion - Pattern 3
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bassdrum   - Pattern 4
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Snare      - Pattern 4
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 2 }, // HighHate   - Pattern 4
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }  // Percussion - Pattern 4
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bassdrum   - Pattern 5
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Snare      - Pattern 5
    { 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 }, // HighHate   - Pattern 5
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }  // Percussion - Pattern 5
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bassdrum   - Pattern 6
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 }, // Snare      - Pattern 6
    { 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 2, 2 }, // HighHate   - Pattern 6
    { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 }  // Percussion - Pattern 6
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0 }, // Bassdrum   - Pattern 7
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Snare      - Pattern 7
    { 1, 0, 1, 0, 1, 1, 1, 0, 4, 4, 1, 0, 1, 1, 1, 0 }, // HighHate   - Pattern 7
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 }  // Percussion - Pattern 7
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bassdrum   - Pattern 8
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 8
    { 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 }, // HighHate   - Pattern 8
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0 }  // Percussion - Pattern 8
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Bassdrum   - Pattern 9
    { 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Snare      - Pattern 9
    { 2, 2, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 4, 4 }, // HighHate   - Pattern 9
    { 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 }  // Percussion - Pattern 9
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Bassdrum   - Pattern 10
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Snare      - Pattern 10
    { 2, 2, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 4 }, // HighHate   - Pattern 10
    { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 }  // Percussion - Pattern 10
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0 }, // Bassdrum   - Pattern 11
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Snare      - Pattern 11
    { 4, 0, 1, 0, 1, 0, 1, 0, 4, 0, 1, 0, 1, 0, 1, 4 }, // HighHate   - Pattern 11
    { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 4, 4, 0, 0, 1, 1 }  // Percussion - Pattern 11
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0 }, // Bassdrum   - Pattern 12
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0 }, // Snare      - Pattern 12
    { 1, 1, 2, 2, 4, 4, 2, 2, 1, 1, 2, 2, 4, 4, 0, 0 }, // HighHate   - Pattern 12
    { 1, 0, 2, 2, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 1, 0 }  // Percussion - Pattern 12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 }, // Bassdrum   - Pattern 13
    { 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, // Snare      - Pattern 13
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 4, 4, 0, 0 }, // HighHate   - Pattern 13
    { 1, 0, 2, 2, 1, 0, 1, 0, 4, 4, 1, 0, 2, 2, 1, 0 }  // Percussion - Pattern 13
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 4, 4 }, // Bassdrum   - Pattern 14
    { 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Snare      - Pattern 14
    { 4, 4, 2, 2, 1, 1, 1, 0, 4, 4, 2, 2, 1, 1, 4, 4 }, // HighHate   - Pattern 14
    { 4, 4, 0, 4, 4, 0, 1, 1, 0, 2, 2, 0, 1, 1, 1, 1 }  // Percussion - Pattern 14
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 2, 2, 1, 1, 0, 0 }, // Bassdrum   - Pattern 15
    { 4, 4, 1, 1, 1, 0, 1, 0, 2, 2, 2, 2, 1, 0, 0, 0 }, // Snare      - Pattern 15
    { 4, 4, 2, 2, 1, 1, 0, 0, 4, 4, 2, 2, 1, 0, 1, 0 }, // HighHate   - Pattern 15
    { 4, 4, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 }  // Percussion - Pattern 15
  }
};

//CUSTOM (F13) - BANK 2: polyrhythm / clave patterns.  Same [pattern][channel][step] shape and same cell rules (0 = silent, 1 = trigger,
//>=2 = ratchet of that many hits).  16 steps = 8 clocks at 2x resolution, so on an 8th-note clock the grid is one 4/4 bar of 16ths.
//Channels are voiced as a world/Latin kit: ch0 = bass/tumbao, ch1 = the signature clave or bell, ch2 = shaker/hihat, ch3 = conga/perc -
//patch any single channel to get that voice alone, or several for the full cross-rhythm.  Claves are voiced starting on step 0 (the 3-2
//orientation) because the reset-on-trigger behaviour makes step 0 the downbeat.  Sources: son/rumba/bossa claves and the bembe 6/8 bell
//are the standard Afro-Cuban/Brazilian patterns; the 3:2 / 4:3 / 5:4 entries spread N hits evenly across the 16 steps to cross-rhythm.
const int breakBeat2[16][4][16] =
//////X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |   <<<< quarter-note grid falls on 0/4/8/12
{ { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Bass       - Son clave 3-2      (P0)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Son clave  - hits 0,3,6,10,12
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }  // Perc       - backbeat 4,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 }, // Bass       - Rumba clave 3-2     (P1)
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0 }, // Rumba clave- hits 0,3,7,10,12
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2 }, // Shaker     - 8ths, ratchet tail
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }  // Perc       - 4,10,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Bass       - Bossa nova clave    (P2)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0 }, // Bossa clave- hits 0,3,6,10,13
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 }  // Perc       - 2,7,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0 }, // Bass       - Tresillo (doubled)  (P3)
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Anchor     - half-note 0,8
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0 }  // Perc       - echoes 3,6,11,14
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Bass       - Cinquillo           (P4)
    { 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0 }, // Cinquillo  - 0,2,3,5,6 doubled
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0 }  // Perc       - 0,6,8,14
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Bass       - Cascara (timbale)   (P5)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Son under  - 0,3,6,10,12
    { 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0 }, // Cascara    - shell pattern
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }  // Perc       - backbeat 4,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bass       - Bembe 6/8 bell      (P6)
    { 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }, // 6/8 bell   - 7-stroke (approx)
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 }  // Perc       - cross 0,5,8,13
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0 }, // Bass       - Songo tumbao        (P7)
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0 }, // Rumba clave- 0,3,7,10,12
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }  // Perc       - 0,4,10,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // Bass       - 3:2 polyrhythm      (P8)
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }, // Three      - 0,5,11 (even thirds)
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }, // Four       - quarters
    { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 }  // Perc       - offset three
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }, // Bass       - 4:3 polyrhythm      (P9)
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }, // Three      - 0,5,11
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }  // Perc       - three (doubles it)
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 }, // Bass       - 5:4 polyrhythm      (P10)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0 }, // Five       - 0,3,6,10,13
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0 }  // Perc       - offset five
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 }, // Bass       - Hemiola 3-over-8    (P11)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Son clave  - 0,3,6,10,12
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }  // Perc       - half-note anchor
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0 }, // Bass       - Baiao               (P12)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, // Son clave  - 0,3,6,10,12
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, // Shaker     - straight 8ths
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }  // Perc       - backbeat 4,12
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bass       - Samba               (P13)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0 }, // Partido alto- syncopated accents
    { 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 2, 2 }, // Shaker     - busy 16ths + ratchet
    { 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0 }  // Tamborim   - 0,3,5,8,11,13
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // Bass       - Afro 6/8 dense      (P14)
    { 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }, // 6/8 bell   - 7-stroke
    { 1, 0, 2, 0, 1, 0, 1, 0, 4, 0, 1, 0, 1, 0, 2, 2 }, // Shaker     - ratchet flurries
    { 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0 }  // Perc       - cross rhythm
  },//X  -  -  -  X  -  -  -  0  -  -  -  X  -  -  - |
  { { 4, 4, 0, 1, 0, 0, 1, 0, 4, 4, 0, 0, 1, 0, 1, 0 }, // Bass       - Polyrhythm max      (P15)
    { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 2, 2 }, // Clave      - son + ratchet tail
    { 4, 4, 2, 2, 1, 1, 1, 0, 4, 4, 2, 2, 1, 0, 4, 4 }, // Shaker     - full ratchet cascade
    { 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 2, 2 }  // Perc       - dense cross
  }
};

///////////////////////////////////////////////////////////loop stuff

//This makes it easy to improve the resolution of the looper for me later, I do not advise changing this variable

#define numTicks 4

bool loopEnable = 0;
bool loopInputBehavior = 0;

unsigned int loopMinimumPulseWidth = 5000;
unsigned int loopLength = 0;
unsigned int loopOffset = 0 ;
unsigned int loopRatchetAmount = 0;
unsigned int loopClockSpeed = 0;

bool trigHistory [4][8][numTicks] ;
unsigned int paramHistory [5][8][4] ;
unsigned int historyIndex = 0;
unsigned int originalHistoryIndex = 0;
unsigned int originalLoopOffset = 0;
unsigned int tickIndex = 0;
unsigned int oldTickIndex = 0;
unsigned int oldTickTracker = 0;
unsigned int loopIndex = 0;
unsigned int loopIndexOld = 0;
unsigned int loopOffsetOld = 0;
unsigned int loopRandom = 0;
unsigned int loopRandom1 = 0;
unsigned int loopRandom2 = 0;
unsigned int loopRandom3 = 0;
unsigned int loopRandom4 = 0;
unsigned int loopCycleIndex = 0;
unsigned int loopDivide = 0;
bool loopClockMult = 0;
bool loopClockMultOld = 0;
bool clockStateLoop = 0;
bool loopIndexChanged = 0;

unsigned long loopClockMultTime = 0;
unsigned long loopClockMultInterval = 0;

int loopWrapCount = 0;
unsigned int loopWrapTime = 0;
bool loopWrapEnable = 0;
bool loopWrapClockState = 0;

bool loopClockOld = 0;
bool loopTrig1Old = 0;
bool loopTrig2Old = 0;
bool loopTrig3Old = 0;
bool loopTrig4Old = 0;

//B5 fix: these eight are written by the trig1-4 pin ISRs (a32_PinInterrupts) and read from the main loop (a42_RecordTriggers), so
//they must be 'volatile' or the compiler may cache a stale copy in a register and never see the ISR's update.  The 32-bit *Time
//values additionally can't be read atomically on an 8-bit AVR, so the main-loop reads snapshot them with interrupts briefly off
//(see atomicReadULong / storeTrigInterruptsInLoop) to avoid a torn read if a trigger fires mid-read.
volatile bool forceTrig1LoopRecord = 0;
volatile bool forceTrig2LoopRecord = 0;
volatile bool forceTrig3LoopRecord = 0;
volatile bool forceTrig4LoopRecord = 0;

volatile unsigned long forceTrig1LoopRecordTime = 0;
volatile unsigned long forceTrig2LoopRecordTime = 0;
volatile unsigned long forceTrig3LoopRecordTime = 0;
volatile unsigned long forceTrig4LoopRecordTime = 0;

unsigned long loopButtonTime = 0;
bool loopButtonActive = 0;
bool loopButtonActiveOld = 0;
bool loopButtonActiveEdge = 0;

unsigned long loopGateTime = 0;
bool loopGateActive = 0;
bool loopGateActiveOld = 0;
bool loopGateActiveEdge = 0;

unsigned int loopCycleCount = 0;

////////////////////////////////////////////////////////////follow stuff

bool cycleIn = 0;
bool cycleActivate = 0;
unsigned long cycleTimeStart = 0;
unsigned long cycleTimeEnd = 0;
bool cycleFlag = 0;
unsigned int cycleCountOut = 0;
unsigned int cycleMinimumPulseWidth = 5000;
unsigned int clockSkipCycleCount = 0;
unsigned int clockLoopCycleCount = 0;
unsigned int clockRatchetCycleCount = 0;
unsigned int skipModeCarryOverCount = 0;
unsigned int cycleClockIn = 0;
unsigned int cycleClockOut = 0;
unsigned int cycleCount = 0;
unsigned long cycleTime = 0;
bool cycleState = 0;
bool cycleActive = 0;
bool cycleComplete = 0;
bool forceMod = 0;
unsigned int probabilityModifier = 0;

//////////////////////////////////////////////////////////////////mode boundary calculation

struct Interval
{
  unsigned int left;
  unsigned int right;
  unsigned int mode;
};

unsigned int intervalCount = 8;
struct Interval intervals[8];
unsigned int boundariesSize = 15;
unsigned int possibleModeBoundaries [ 15 ] { 0, 40, 100, 230, 295, 360, 440, 520, 560, 600, 710, 820, 900, 980, 1024 };

const unsigned int MODE_COUNT = 8;
const unsigned int INTERVAL_COUNT = MODE_COUNT;
struct Interval INTERVALS[INTERVAL_COUNT];
const unsigned int MAX_INTERVAL_BOUNDARY = 1024;

const unsigned int BOUNDARIES_2X_SIZE = 15;
unsigned int BOUNDARIES_2X [ BOUNDARIES_2X_SIZE ] { 0, 40, 100, 230, 295, 360, 440, 520, 560, 600, 710, 820, 900, 980, 1024 };

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
