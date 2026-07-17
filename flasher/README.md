# ID00M firmware flasher (WebSerial `jtag2updi`)

A browser-based firmware updater for the Mystic Circuits IDUM (Arduino Nano Every / ATmega4809), for the
[ID00M](../docs/CHANGELOG.md) fork + stock-v.99 revert. It speaks avrdude's `jtag2updi` protocol to the
board's onboard mEDBG bridge over the **Web Serial API** - no install, no drivers (on Mac/Linux; Windows may
need the usual COM driver). This is the reusable **core**; the website session wraps it in the resources-page
FW-updater tab UI (variant picker, progress, browser check).

## Files

- **`webserial-jtag2updi.js`** - the module. Pure protocol layer (`crc16`, `encodeFrame`, `FrameDecoder`,
  `parseIntelHex`) + the `Jtag2Updi` transport/sequence class + a `withRetry` helper.
- **`protocol/`** - the reverse-engineering record: `FINDINGS.md` (the spec) and the raw avrdude `-vvvv`
  captures (`trace_flash_vvvv.txt` = ground truth for every command).
- **`test/frame-test.html`** - offline codec self-test (no hardware). Verifies crc/encode/decode/hex-parse
  against real captured frames.
- **`test/identify.html`** - live **read-only** milestone: touch → sign-on → enter progmode → read signature.
  Proves the whole handshake without writing flash.

## How to test

Web Serial needs a **Chromium-family desktop browser** (Chrome, Edge, Opera, Brave) and a **localhost**
origin (ES modules + Web Serial are blocked on `file://`). From this `flasher/` directory:

```
python -m http.server 8000
```

- Codec (no module needed): open <http://localhost:8000/test/frame-test.html> → expect "ALL TESTS PASSED".
- Live handshake (plug the module in): open <http://localhost:8000/test/identify.html> → **Connect & Identify**
  → expect signature `1E 95 51` and "ATmega4809 identified".

The flaky programmer is handled automatically (`withRetry` re-touches and re-tries silently); if it truly
wedges, the page tells you to unplug/replug - the same failure mode we see with avrdude.

## Status

- ✅ Protocol fully reverse-engineered + documented (`protocol/FINDINGS.md`). CRC = CRC-16/MCRF4XX (confirmed).
- ✅ Module written: framing/CRC/decoder verified against 20 captured frames (Python mirror); browser codec
  self-test provided.
- ✅ **Live handshake + read path VALIDATED ON HARDWARE** (2026-07-17): `identify.html` ran touch → sign-on →
  device descriptor → enter progmode → read signature end-to-end, returning `1E 96 51` (correct ATmega4809 sig
  per avrdude.conf), matching avrdude's own captured read byte-for-byte.
- ✅ **FULL WRITE PATH VALIDATED ON HARDWARE** (2026-07-17): `program.html` flashed + read-back-verified the real
  firmware end-to-end (erase → paged write → per-page verify). **Survives the flaky programmer** via a recovery
  ladder - GET_SYNC resync for glitches, full reconnect-and-resume for deep stalls (`_cmdRecover`), on both reads
  and writes. **The flasher is functionally complete.**
- ⏳ **Follow-ups (not blockers):** (1) speed - it's slow on a degraded programmer (many resyncs/reconnects); a
  fresh programmer is much faster, and read/write batching could cut round-trips. (2) Hand the core to the
  website session for the FW-updater tab UI (variant picker, progress, browser check). (3) fuse-writing is
  implemented but opt-in and unvalidated (fuses are already correct, so flash-only is the safe default).

## Notes for whoever builds the UI

- `Jtag2Updi.requestPort()` must be called from a user gesture (click). `identify()` is the safe read-only
  entry point; `program(hexText, {onProgress})` does erase → paged flash write → fuse write → verify.
- Flash is mapped at `0x4000` (UPDI space), 128-byte pages; fuses via memtype `0xB2`. Constants are exported.
- Ship two hex variants (ID00M / stock v.99) + always offer the stock-restore as the "revert" path.
