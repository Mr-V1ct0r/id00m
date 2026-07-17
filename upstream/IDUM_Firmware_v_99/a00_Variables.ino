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

//////////////////////////////////////////////////////////////////////mode stuff

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

//Preset rhythms for break mode.  These are also an easy thing to modify for yourself.
//Keep in mind that these rhythms are reset whenever a trigger is received at the relevant trigger input.
//So try not to think of them as a sequence that plays once the modification starts, rather they are like
//a "mask" that is modifying each input in a specific pattern.  That's why most of them start with a '1'
//on the first step.  Try turning these off and see what the effect is to see what I'm talking about.
//A '1' in each cell activates a single trigger whereas a '0' is silent at that step.  Numbers higher than
//'1' triggers a ratchet on that step.  Also worth mentioning that these sequences run at twice the resolution
//of the incoming clock so think of every two steps being one clock.

int breakBeat1[16][4][16] =
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

bool forceTrig1LoopRecord = 0;
bool forceTrig2LoopRecord = 0;
bool forceTrig3LoopRecord = 0;
bool forceTrig4LoopRecord = 0;

unsigned long forceTrig1LoopRecordTime = 0;
unsigned long forceTrig2LoopRecordTime = 0;
unsigned long forceTrig3LoopRecordTime = 0;
unsigned long forceTrig4LoopRecordTime = 0;

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
