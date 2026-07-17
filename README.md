# ID00M

**ID00M** (a play on **IDUM** + **00 Modular**) is a free, community firmware for the Mystic Circuits
**IDUM** Eurorack module. It is based on Eli Pechman's **v.99** and is released under the same license
(**CC BY-SA 4.0**). It keeps everything IDUM already does and adds bug fixes, new modes and options, and a
few opinionated changes to how some modes behave.

If you own an IDUM, you can flash ID00M onto it in your browser (no software to install), try it out, and
flash Eli's stock v.99 right back if you prefer the original. It is completely reversible.

> **Not affiliated with Mystic Circuits.** IDUM and the IDUM name are by Mystic Circuits. ID00M is an
> independent community modification and is not affiliated with or endorsed by Mystic Circuits.

## What's new

The full, musician-facing list is in [`docs/CHANGELOG.md`](docs/CHANGELOG.md). The headlines:

- **Per-channel clock multiply / divide.** Each of the four channels can run its bursts and bouncing balls
  at its own rate (from divide-by-4 up to times-8), so every voice can ratchet or bounce at a different
  speed against the same clock.
- **MERGE (OFF / ADD / CUT / MIX).** Modes can now layer on top of your triggers (ADD) or subtract from
  them (CUT) instead of only replacing them. ADD on the delay mode gives a true dry-plus-delayed echo.
  Concept inspired by MorphWorx's "Glitch, Please!"; see the [changelog credits](docs/CHANGELOG.md#credits-and-thanks).
- **Linear drumming.** An optional one-voice-at-a-time filter that turns dense chaos into a tight, hocketed
  groove.
- **A cleaner, musical delay**, a second bank of break patterns (clave and world grooves plus a few true
  polyrhythms), a break speed multiplier, a factory reset, a menu toggle for split-mode PARAM "freeze",
  and a "00" boot indicator so you can tell at a glance the module is running ID00M.
- **Bug fixes** that restore behavior IDUM was meant to have (real randomness at boot, faithful loop
  playback, and more). These are offered back upstream.

## User manual

A full user manual is included in [`manual/`](manual/ID00M_User_Manual.html). It is a single
self-contained HTML file (no external dependencies) - download it and open it in any browser. The
same manual is also rendered online on the [00 Modular resources page](https://00modular.com/id00m/resources/).

## Install it (the easy way)

The simplest path is the browser-based firmware updater on the 00 Modular website:

**[00modular.com/id00m/resources/#firmware](https://00modular.com/id00m/resources/#firmware)**

Plug your IDUM into a computer with a USB cable, open that page in a Chromium-family desktop browser
(Chrome, Edge, Brave, Opera), pick **ID00M** or **stock v.99**, and press update. The updater handles the
flaky programmer automatically and will only ask you to unplug/replug in the rare case it truly gets stuck.

You can also run the updater locally from this repo, see [`flasher/`](flasher/).

## Install it (from source)

ID00M is an Arduino sketch for the **Arduino Nano Every (ATmega4809)** inside the IDUM.

1. Install the Arduino IDE and the **Arduino megaAVR Boards** package (board: *Arduino Nano Every*,
   registers: *ATMEGA4809*).
2. Open [`firmware/IDUM_Custom/IDUM_Custom.ino`](firmware/IDUM_Custom/IDUM_Custom.ino). The sketch is split
   across many `.ino` files that the IDE concatenates alphabetically; the top of the main file explains the
   layout and how to spot the ID00M changes.
3. Upload to the module. (The onboard programmer can be flaky, retry if the first upload does not take.)

Prebuilt hex images are in [`releases/`](releases/) if you would rather flash directly with the updater or
avrdude.

## Reverting to stock

Prefer the original? Flash **`releases/idum_stock_v99.hex`** (or pick stock v.99 in the web updater). Eli's
untouched v.99 source is also included here under [`upstream/`](upstream/) for reference and diffing.

## Repository layout

```
firmware/IDUM_Custom/   the ID00M sketch (custom code tagged CUSTOM / F# / B#)
flasher/                browser-based WebSerial firmware updater (core + protocol notes + test pages)
releases/               prebuilt hex images (ID00M and stock v.99)
manual/                 self-contained HTML user manual (also online on the resources page)
upstream/               Eli Pechman's original IDUM v.99 source, verbatim, for attribution and diffing
docs/CHANGELOG.md       the full list of changes
LICENSE                 CC BY-SA 4.0 legal code
```

## License and attribution

Original IDUM firmware is **© Eli Pechman / Mystic Circuits**, licensed **CC BY-SA 4.0**. ID00M is a
derivative work by **00 Modular**, released under the **same license**. Under CC BY-SA you are free to use,
modify, and redistribute this, including commercially, as long as you give appropriate credit and share any
derivatives under the same license. The changes ID00M makes are documented in
[`docs/CHANGELOG.md`](docs/CHANGELOG.md) (CC BY-SA asks you to indicate what you changed).

See [`LICENSE`](LICENSE) for the full legal code, and [`upstream/README.md`](upstream/README.md) for Eli's
original notes.

## A note on your hardware

Flashing firmware and hacking on this code is at your own risk. Reassigning pins in the source (turning an
input into an output or vice versa) can damage your module. The web updater only writes the firmware image
and is safe to use, but as with any firmware update, do not unplug mid-write unless the tool tells you to.
