//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/*   
          _____                    _____                    _____                    _____          
         /\    \                  /\    \                  /\    \                  /\    \         
        /::\    \                /::\    \                /::\____\                /::\____\        
        \:::\    \              /::::\    \              /:::/    /               /::::|   |        
         \:::\    \            /::::::\    \            /:::/    /               /:::::|   |        
          \:::\    \          /:::/\:::\    \          /:::/    /               /::::::|   |        
           \:::\    \        /:::/  \:::\    \        /:::/    /               /:::/|::|   |        
           /::::\    \      /:::/    \:::\    \      /:::/    /               /:::/ |::|   |        
  ____    /::::::\    \    /:::/    / \:::\    \    /:::/    /      _____    /:::/  |::|___|______  
 /\   \  /:::/\:::\    \  /:::/    /   \:::\ ___\  /:::/____/      /\    \  /:::/   |::::::::\    \ 
/::\   \/:::/  \:::\____\/:::/____/     \:::|    ||:::|    /      /::\____\/:::/    |:::::::::\____\
\:::\  /:::/    \::/    /\:::\    \     /:::|____||:::|____\     /:::/    /\::/    / ~~~~~/:::/    /
 \:::\/:::/    / \/____/  \:::\    \   /:::/    /  \:::\    \   /:::/    /  \/____/      /:::/    / 
  \::::::/    /            \:::\    \ /:::/    /    \:::\    \ /:::/    /               /:::/    /  
   \::::/____/              \:::\    /:::/    /      \:::\    /:::/    /               /:::/    /   
    \:::\    \               \:::\  /:::/    /        \:::\__/:::/    /               /:::/    /    
     \:::\    \               \:::\/:::/    /          \::::::::/    /               /:::/    /     
      \:::\    \               \::::::/    /            \::::::/    /               /:::/    /      
       \:::\____\               \::::/    /              \::::/    /               /:::/    /       
        \::/    /                \::/____/                \::/____/                \::/    /        
         \/____/                  ~~                       ~~                       \/____/    


  v.99 - Written by Eli Pechman

  This code is released under a Creative Commons Attribution-ShareAlike 4.0 Licence

  https://creativecommons.org/licenses/by-sa/4.0/legalcode

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

  Here it is folks, this is the code used in the IDUM Eurorack module sold by Mystic Circuits.  For a good
  overview of this module and how it works check out the product website at:

  www.MysticCircuits.com/product/idum

  In short IDUM works as a sort of multi-fx processor for gates that turns your patterns effortlessly into IDM.  It can add little bits of 
  variation to incoming patterns or it can completely destroy everything going through it.  It also does some fairly complex clock manipulation 
  to an external sequencer allowing you to make it run backwards, loop subsections of the sequence etc.

  I did a my best to make this code as simple to understand and as human-readable as possible.  From the beginning I wanted this module to be as
  easy as possible for people to hack even if they are just messing with a few numbers or changing the under-the-hood options on this page to
  slightly tweak how the module works.  Using the arduino environment felt like a good choice because it is designed to be beginner friendly,
  it is extremely well documented and widely used and therefor easily searchable, and it is easy to install the necessary programs to update your board.

  Please understand that although I am doing my best to encourage people to tweak your IDUM code that doing so can damage your module, especially
  if you reassign pins that are inputs to outputs or vice versa.  Also keep in mind that if you contact me with questions I will do my best to
  answer them but that I am under no obligation to help you alter this program.  Mystic Circuits is a small company and I do not have a lot
  of free time.  Please be patient for me to respond and keep in mind the nicer you ask me for help the more enthusiastic I will be to help.

  If you have suggestions for improvements that can be made or things that can be commented/ explained more clearly in here please do not
  hesitate to get in touch with me through the contact page on my website. I'm not great with Github and I don't check my messages here that
  often so the website is the best way to get a hold of me.  I make no claims to being a great programmer and will not be offended if you tell 
  me a better way to do what I am doing here.
  
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\                                                                                                  
   
  Hi Uli!  I know there's not much that I can do to stop you or someone else from using this code or my hardware in their products.  All I'll 
  say is that if you or anyone else ends up using anything here in a way that makes you money it is my wish that you: 
  
  a) Change things enough so that your product/ device is different enough from and doesn't compete with IDUM.
  
  b) Try to send some love my way (I'd love a reel of bucket brigade chips).

  If you the reader are at all unsure about whether something will step on my toes again do not hesitate to contact me through my contact form
  on my website.
 
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

  CHANGELOG:

  - Implemented Split Mode to allow each channel to calculate it's modification status and mode separately.
  - Increased resolution of the looper to 4 pulses-per-clock instead on one pulse-per-clock.  This allows the looper to record triggers and 
    gates differently and also capture ratchets pretty accurately.  When recording once per clock the looper would just smoosh everything into
    a long gate.
  - Changed the dead zones at the top and bottom of the prob slider to allow for lower and higher values of probability before entering a dead zone
  - Changed the odd/ even/ power of 2 param knob arrays to make them all treated the same.  This improved the accuracy especially of divided clock
    speeds across various modes that would use the standard ratchet calculations.
  - added a new page specifically for the secret option flags to improve legibility
  - General improvments to the looper modificaiton status and index calculations.  Allows for the looper to more accurately capture and replay 
    an 8 step loop in time with the original content when the length and speed parameters are at their relative defaults.  
  - The loop code now starts with a relative offset of 0 no matter which mode we were in before engaging a loop.  Important because the looper
    offset control is the mode knob normally.
  - Changed the relative brightness of the selected mode and available modes to decrease LED flickering in the display
  - 

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

   TO DO:

   add 16 step code for looping and maintain cycle functions
   update timing sensitive parts of code to be interrupt driven
   improve slow clock mode reliability and reducing number of triggers when a lot of follow steps are added.
   add code to save loops between power downs
   record param variables to the loop per tick instead of per clock
   increase number of ticks in the loop even more?
   allow sending a trigger to the prob input to manually trigger a modification between clocks?
   add calibration code for mode and length pots?

*/

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

/*
  ========================================  ID00M  ========================================

  ID00M is a community fork of the IDUM firmware whose original header is above (Eli Pechman /
  Mystic Circuits, v.99).  Everything up to here is Eli's original text, kept intact for attribution.
  ID00M is released under the SAME license as the original - Creative Commons Attribution-ShareAlike
  4.0 (CC BY-SA 4.0) - and the full list of what it changes versus stock v.99 lives in CHANGELOG.md
  next to this sketch (CC BY-SA asks you to indicate your changes; the changelog is that).  ID00M is
  an independent modification, not affiliated with or endorsed by Mystic Circuits.

  =====  concept credit: MERGE  =====

  The MERGE control (and its ADD / CUT naming) is inspired by "Glitch, Please!" by Patrik Andersson
  (MorphWorx, github.com/vurt72), his VCV Rack / MetaModule port of IDUM.  The idea is his; we
  thought it was a good one and brought it to the hardware.  This firmware implementation is our own
  and it diverges from his. ID00M adds OFF and MIX states and applies MERGE to only some modes, so
  treat it as inspired by, not a behavioral match to Glitch, Please!.  If you patch in VCV Rack, go
  support MorphWorx; and, of course, support Mystic Circuits for IDUM itself.

  =====  finding your way around  =====

  The Arduino IDE concatenates these files in alphabetical order into one program:

    a0000   the "super secret" under-the-hood option flags (source-level toggles)
    a00     all global variables + the break-pattern data tables
    a01     timer helpers
    a10     setup()  - runs once at power-on (EEPROM load, pin setup, boot LED)
    a20     loop()   - the main superloop; this is the top-level flow, read it first
    a30/a31 read the knobs/gates + edge detection (clock, triggers, buttons)
    a32     pin interrupts
    a40..a42 the looper (play back / record)
    a50/a51 choose the current mode from the MODE dial (+ removed-modes boundary math)
    a60     modifyTest() - decides when a modification starts/stops (independently per channel in split mode)
    a70     the LED display / mode ring / menu drawing
    a80     menu button handling (the option pages)
    a90     combine everything into the final output gates (MERGE + linear drumming live here)
    a91     clock-follow / cycle-maintenance utilities
    b10..b82 one file per modification MODE - hold, burst, ratchet(mult), ball, rotate, delay, break,
             skip, contamination, scatter.  (skip=b80 and contamination=b81 are RETIRED but kept in place
             so you can restore them in one line; scatter=b82 is what lives in the purple slot now.)
    x_utils EEPROM pack/unpack + small helpers

  =====  how to spot the ID00M changes  =====

  Every ID00M change is tagged in a comment with the word CUSTOM - usually "CUSTOM (F9)",
  "CUSTOM (F6 - MERGE)", etc., where F# is a feature and B# is a bug fix (the same IDs used in the
  changelog).  Grep the codebase for CUSTOM to see every divergence from stock, each with a note on
  what it does and why; bug fixes are tagged B#.  The modes reworked the most (and with the most
  detailed comments) are b20 burst, b30 ratchet, b60 delay, and b82 scatter.  The browser-based
  firmware updater is in the flasher/ folder with its own reverse-engineered-protocol notes.

  =========================================================================================
*/

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Library inclusions and defining the timer interrupts

#include "fastdigitalIO.h"
#include <EEPROM.h>
#include "arduino-timer.h"

Timer<1, micros> dTimer;
Timer<1, micros> cTimer;

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//DEBUG SPECIFIC VARIABLES
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//This lets us write directly to the serial monitor in the arduino IDE for better debugging info.  
//Remember to turn it off after you mess around with the code as it will mess with the timing of your programs.

bool printEnable = 0;         //default = 0 < make sure this is turned off when you are done!

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Used to let us manually turn on and off the looper LED for faster debugging feedback.  
//This can be useful in cases where using the serial monitor messes up your program timing in a way that makes it difficult to replicate bugs.
//Don't name your own variables "debug" folks.  Remember to turn this option off after you are done messing with your program.
//Variable to save to to make LED light up is called "IDUMLEDebug"

bool IDUMLEDebugEnable = 0; //default = 0 < make sure this is turned off when you are done!

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
