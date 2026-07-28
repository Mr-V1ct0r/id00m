# ID00M changelog

**ID00M** (a play on IDUM + 00 Modular) is a free community
firmware for the Mystic Circuits **IDUM**, based on Eli Pechman's **v.99** and released under the same
license (**CC BY-SA 4.0**). It keeps everything IDUM already does and adds bug fixes, new modes and
options, and a few opinionated changes to how some modes behave.

> Original IDUM firmware © Eli Pechman / Mystic Circuits, licensed CC BY-SA 4.0.
> ID00M modifications by 00 Modular, released under the same license. The changes are listed below, both
> because CC BY-SA asks you to indicate what you changed, and because it's the interesting part.


## v1.0.3 - 2026-07-28  *(patch; based on v1.0.2)*

### Bug fixes

- **Linear drumming no longer leaks delayed ghost hits.** With a busy beat, a lower-priority channel whose
  gate *outlived* the higher-priority gate that masked it would re-appear the instant the higher gate fell:
  its output went high again a few ms late, so a sampler saw a fresh trigger and fired a delayed, shortened
  phantom hit: snares and hats "still getting through, off the beat" under priority 1>2>3>4. The mask was
  decided purely per-instant, with no memory that the hit had already lost its moment. Linear drumming now
  **latches its mute for the whole of a channel's gate**: once a hit is masked at its onset it stays muted
  until its own gate falls, so it can never resurface mid-gate. The channel's next hit is judged fresh, and
  the winner/priority logic is unchanged; only the tail leak is closed.
- **Linear drumming honors priority even when hits aren't perfectly simultaneous.** De-conflicting the live
  gate per-instant wasn't enough for a one-shot sampler: if a lower-priority hit's edge reached the jack even a
  hair before the higher-priority one (a kick landing microseconds ahead of a higher-priority snare, say), the
  sampler had already fired it and priority couldn't reach back, so both voices sounded. Linear drumming now
  uses a small **look-ahead / coincidence window** (2 ms default): every output is delayed by the window, and a
  lower-priority hit that loses to a higher one within it is dropped before its delayed onset is ever emitted,
  so the loser never reaches the sampler. The cost is a **constant ~2 ms latency while linear drumming is on**,
  applied uniformly to all four voices so they stay phase-locked to each other and the shift is inaudible
  against the clock. Short triggers keep their full width (just shifted); the window exists only while linear
  drumming is on.
- **MERGE CUT on ROTATE no longer flams at modification boundaries.** Under CUT, ROTATE outputs your OWN
  channel's gate (hocket/duck), but it was still gated by the *rotated source* channel's choke. At a
  modification boundary the source choke could briefly drop and chop your held gate into a flam. CUT now
  follows the channel's **own** choke, the gate it actually outputs; REPLACE and ADD, which carry the routed
  source gate, keep the source choke as before. This completes the v1.0.1 "CUT no longer flams" work for the
  one path it didn't cover. With the chopped gates gone, the glitchy retriggering they could provoke on a
  downstream sample player clears up along with them.

*On-module version readout: this build lights the **third** ring LED after the "00" boot signature
(release 3). Power-cycle and read the dot; see the readout note under v1.0.1 below.*


## v1.0.2 - 2026-07-24  *(patch; based on v1.0.1)*

### Bug fixes: faithful (present in stock v.99; offered back to Eli)

*These restore behavior IDUM was meant to have; the bug is in stock v.99, not something ID00M introduced.*

- **Length-1 modifications now behave.** In Eli's v.99, whether a modification counts as *active* is
  decided each clock at a moment when its length has briefly ticked to zero, before it is renewed, so at
  a **LENGTH of 1**, where that happens every clock, the module reads the modification as inactive on each
  clock edge and skips the effect on the very trigger it should land on. On ID00M you catch it as **GATE
  DELAY** stuck at a fixed offset no matter where PARAM sits, **MULTIPLY / DIVIDE** counter-clockwise not
  dividing, or **BURST** counter-clockwise flickering between silence and full passthrough (it could even
  change on its own after sitting a while), but stock IDUM's own modes were skipped the same way; these
  are simply where it is easiest to hear. Fixed by settling the active state *after* the modification
  renews, so a continuous modification stays reliably active every clock. Longer lengths were never affected.
- **ROTATE now scrambles onto silent channels.** ROTATE re-routes an input to another channel's output,
  but the destination output was held muted unless *that* channel had its own input, so routing a single
  input onto otherwise-empty channels came out silent. ROTATE now carries the source channel's gating along
  with the routed signal, so one input scrambles across all four outputs as intended, with no signal needed
  on the destination channels. Also present in stock v.99.

*On-module version readout: this build lights the **second** ring LED after the "00" boot signature
(release 2). Power-cycle and read the dot; see the readout note under v1.0.1 below.*


## v1.0.1 - 2026-07-21  *(patch; based on v1.0.0)*

### Bug fixes

- **MERGE CUT no longer flams the incoming beat.** On both CUT flavors, ROTATE (hocket/duck) and
  BREAK (pattern stencil), a held input gate could be chopped into several output pulses whenever the
  mode's own signal flickered mid-gate, so kicks, snares and hats came out flammed and off the beat
  instead of cleanly gated. CUT now **latches its pass/drop decision at the instant your hit lands** and
  holds it, so your gate keeps its full width and can no longer be split. The intended masking (the
  dropped hits) is unchanged; only the flamming is gone. IDUM stays a gate processor: CUT never
  re-pulses your input to a fixed width.

### New features

- **On-module version readout.** After the **"00"** boot signature, a single ring LED is held for about
  a second; its **color** tells you which release is on the module, handy once more than one build
  exists. It's a running release index (the first ring LED = release 1), so a patch, minor or major bump
  is simply the next color. Power-cycle and read the color, then look it up here:
  - **Release 1 (first ring LED) = v1.0.1**

*MERGE and its ADD / CUT concept remain credited to **"Glitch, Please!"** by Patrik Andersson
(MorphWorx); see the v1.0.0 credits below.*


## v1.0.0 - 2026-07-17  *(first public release; based on IDUM v.99)*

### Bug fixes: faithful (these just restore the behavior IDUM was meant to have)

*Nothing here changes IDUM's character; these are offered back to Eli.*

- **Randomness is actually random now.** Stock IDUM never seeded its random number generator, so it
  replayed the *identical* "random" sequence of modifications on every power-up. ID00M seeds from hardware
  entropy at boot, so the module genuinely varies each time.
- **The looper plays back what you recorded.** Three separate fixes:
  1. In split mode it no longer fires a modification on steps that recorded none.
  2. Recorded gates and their modifications are no longer rotated out of alignment: looped bursts and
     ratchets now land on the steps you actually played them on.
  3. A looped ratchet now uses the same PARAM-to-speed mapping as live playing (it previously used a
     different, mirrored-backwards mapping, so a looped ratchet could come out at a different speed than
     you recorded).
- In **bouncing-ball mode**, output 3 no longer inherits a stale timing value from the previous modification.
- **Rotate** in split mode now honors each channel's own PARAM (channels 2-4 used to all follow channel 1).
- **Back-to-back modifications** now use the current MODE-dial position instead of reusing the previous
  modification's mode.
- **Power-on with modes removed** no longer strands the module on a removed mode (it could boot stuck on
  "hold" until you nudged the knob). The mode-boundary logic was hardened and cleaned up.
- **Under the hood:** interrupt flags made safe against optimizer caching and mid-read tearing; a
  divide-by-zero guarded in ratchet mode; and ~2 KB of RAM reclaimed by moving the break-pattern tables
  into flash where they belong (stock kept them in RAM).

### New features

- **Per-channel clock multiply / divide: the headline.** Each of the four channels can run its **bursts
  and bouncing balls** at its own rate: **÷4 ÷3 ÷2 ×1 ×2 ×3 ×4 ×8**. So every drum voice can ratchet or
  bounce at a different speed against the same clock, the long-requested "ch1 ×1, ch2 ×2, ch3 ×4, ch4 ×8"
  idea. Set it on a dedicated **page-3 editor**, reached by flipping the CYCLE switch while you're in the
  page-2 menu.
- **Linear drumming (one-voice-at-a-time).** An optional output filter: when two channels would fire at
  the same instant, only the highest-priority one passes. Priority is editable per channel. Turns dense
  chaos into a tight, hocketed groove.
- **MERGE: OFF / ADD / CUT / MIX.** Every stock IDUM mode *replaces* your incoming triggers with its own
  output, a big part of why IDUM feels destructive. MERGE lets a mode instead **ADD** to the input (keep your
  hits, layer the mode on top) or **CUT** it (keep your hits, but subtract / re-gate them where the mode fires).
  Set it in the secondary menu at ring slot 5 (solid LED 5 = active; the loop LED blinks the state).
  - **It only applies to the four "movement/pattern" modes: ROTATE, DELAY, BREAK, SCATTER (the last four ring
    slots).** The trigger-anchored modes (HOLD, BURST, RATCHET, BALL) always replace: they're already built
    around your incoming triggers, so ADD would just double the input and CUT would work against the mode
    rather than with it.
  - **OFF**: stock replace everywhere.
  - **ADD**: all four eligible modes layer on top of your input. Notably, **ADD on DELAY is a true echo**:
    your dry hit *plus* the delayed one, the dry+delayed echo people always asked for. (Rotate/break/scatter
    likewise become "input + the effect.")
  - **CUT**: **affects ROTATE and BREAK only.** They subtract from / re-gate your input (hocket; pattern-gated
    holes in the beat). DELAY and SCATTER are unaffected by CUT; they stay on plain replace (CUT isn't musically
    useful on them).
  - **MIX**: a combination of MERGE behaviors at once: DELAY/SCATTER **ADD**, ROTATE/BREAK **CUT**.
  Note: MERGE currently affects live playing, not loop playback (a later refinement).
  - *Credit:* the MERGE control and its ADD / CUT naming come from **"Glitch, Please!"** by Patrik
    Andersson (MorphWorx). See the Credits and thanks section at the end of this changelog. Our implementation is
    independent and behaves differently (OFF and MIX states, and it only touches some modes), so if you
    also use his module, treat this as inspired by rather than a 1:1 match.
- **Break pattern banks.** A second bank of 16 break patterns, mostly **grooves based on clave and world
  rhythms** (son, rumba, bossa, tresillo, cinquillo, cáscara, bembé, songo, baião, samba) plus a handful of
  **true polyrhythms** (3:2, 4:3, 5:4, hemiola), alongside the original drum patterns.
- **Break speed multiplier.** Run the break patterns at 1× / 2× / 3× / 4×.
- **Factory reset.** Hold REMOVE MODE while powering on to wipe back to defaults.
- **Split mode can now "freeze" PARAM.** Click the split menu slot once for split, **twice for split + freeze**.
  A third click turns off split mode. With freeze on, the PARAM knob is snapshotted the instant each channel starts a
  modification and held for its duration, so in split mode you can dial a *different* PARAM (and mode) into each
  of the four channels by moving the knob as they trigger, instead of all four tracking the live knob. This was
  a hidden, source-only "secret option" in stock IDUM; ID00M puts it on the menu, shown by a **blinking loop LED**
  at the split slot (the ring LED stays solid for split; the loop-LED blink means freeze is engaged). Because each
  channel commits to one setting per modification instead of everything morphing with the live knob, freeze tends
  to **settle the output**, a handy way to tame the chaos when you want it a bit more composed. Heads-up: with
  freeze on, turning PARAM mid-modification does nothing until the next one; that's the point, not a fault.

### Changed mode behavior

- **SCATTER replaces CLOCK SKIP** in the purple slot: it randomly nudges triggers off-grid to a quantized
  sub-slot of their own gap.
- **Burst, counter-clockwise** is now probabilistic thinning (it randomly drops incoming triggers)
  instead of the old slow clock-divide echo.
- **Ratchet, counter-clockwise** is now a true trigger divider (passes 1 of every N triggers) instead of
  an interval-stretch that was effectively inaudible on a steady beat.
- **Delay: a proper monotonic delay now.** *How it used to work:* a bipolar knob whose two halves scaled
  differently, with each of the four channels hard-wired to a *different* divisor (÷8 / ÷12 / ÷14 / ÷16) that
  even swapped between the knob's two halves, so the four outputs were artificially fanned apart and the knob
  was more of a lucky-dip than a straight short→long control. And because ÷12/÷14/÷16 aren't clean eighths, the
  shifted hits landed on mismatched, *non-musical* fractions of the gap, never lining up to a tidy grid (Eli's
  own comment calls them ratios that get "sort of close to actual beats"). *Now:* one clean sweep, CCW = short
  (an eighth of the beat) up to CW = long (a full beat), where every shifted hit **snaps to a clean, quantized
  subdivision** of the gap (n/8, or the pow2 / even subset you pick in the resolution menu), and **all four
  channels share the same grid** (the per-channel fan is gone; spread now comes from feeding them different inputs). The delay is a fraction of each channel's *own input gap*, so it stays
  proportional as the tempo changes, and it's capped at one full gap so a delayed hit never runs past (and
  swallows) the next trigger. It *shifts* each hit later rather than adding a dry copy. For a "dry+delayed
  **echo**", use MERGE ADD on this mode (see MERGE, above).
- **Bursts stay crisp at high multiplication**: the trigger pulse width now scales with the subdivision,
  so fast ratchets don't smear together into one sustained gate as readily.
- **Boot indicator:** ID00M lights the **first and last ring LEDs together** (a "00" signature) for about
  a second at power-on, so you can tell at a glance the module is running ID00M and not stock.

### Reverting

Prefer the original, or want to go back? The updater can flash **Eli's stock v.99** onto the module at any
time; ID00M is completely reversible.


## Credits and thanks

ID00M stands on the work of two people worth supporting directly, whether IDUM related or not:

- **Eli Pechman / Mystic Circuits** wrote IDUM and its v.99 firmware, the entire foundation this fork
  builds on, and openly invited the community to make custom firmware. If you enjoy what ID00M does,
  the module itself is his: buy IDUM from Mystic Circuits and follow what he builds next.
- **Patrik Andersson (MorphWorx, [github.com/vurt72](https://github.com/vurt72/MorphWorx))** made
  **"Glitch, Please!"**, a VCV Rack / MetaModule port of IDUM. The **MERGE** control and its **ADD / CUT**
  naming are his idea; we thought it was a genuinely good one and brought the concept to the hardware.
  Our version is an independent implementation and behaves differently (ID00M adds OFF and MIX states and
  applies MERGE to only some modes), so it is *inspired by* his work rather than a match to it. If you
  patch in VCV Rack or on the 4ms MetaModule, go check out MorphWorx and support what he builds.

Concept and naming credit for MERGE goes to Patrik; the firmware implementation here is our own.

*IDUM and the IDUM name are by Mystic Circuits. ID00M is an independent community modification and is not
affiliated with or endorsed by Mystic Circuits. "Glitch, Please!" and MorphWorx are by Patrik Andersson;
ID00M is not affiliated with or endorsed by MorphWorx.*
