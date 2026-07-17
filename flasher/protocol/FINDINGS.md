# ID00M WebSerial flasher - protocol spike findings

Ground truth for the WebSerial `jtag2updi` flasher, captured against the real module (Nano Every /
ATmega4809, COM6) on 2026-07-17. Source of the raw capture: `trace_readsig.txt` (avrdude `-vvvv`).

## Confirmed

- **Transport:** plain CDC serial. avrdude programmer `jtag2updi`, **115200 baud**, 8N1. → **WebSerial**
  (`navigator.serial`) is the correct browser API. Target MCU `atmega4809`, expected signature `1E 95 51`.
- **Precondition - the 1200 bps touch is REQUIRED.** Calling avrdude directly (skipping the touch that
  arduino-cli normally does) → every sign-on times out with "programmer is not responding". The flasher
  must **open the port at 1200 baud and close it** to reset the target before connecting at 115200.
- **No re-enumeration.** After the 1200 touch the port **stays COM6** (verified: same DeviceID before/after).
  Unlike a Leonardo, the Nano Every does NOT switch to a separate bootloader port. So the flasher reconnects
  to the *same* WebSerial port after the touch - no device-repick needed. (Still confirm timing on hardware.)
- **Frame format (JTAGICE mkII):**
  ```
  0x1B  | seq (2 bytes, LE) | size (4 bytes, LE, = body length) | 0x0E (TOKEN) | body... | CRC (2 bytes, LE)
  ```
- **Sign-on command** (captured, known-good):
  ```
  1b 00 00  01 00 00 00  0e  01  f3 97
  ^START ^seq=0  ^size=1     ^TOK ^body=CMND_SIGN_ON(0x01)  ^CRC=0x97f3 (LE f3 97)
  ```
- **CRC algorithm: CRC-16/MCRF4XX - CONFIRMED** on 3 frames (sign-on cmd 0x97f3, sign-on response 0x566f,
  getparam cmd 0x3124). Params: poly `0x1021`, init `0xFFFF`, reflected in/out, no final XOR (reflected-table
  poly `0x8408`). JS:
  ```js
  function crc16(data){let c=0xFFFF;for(const b of data){c^=b;for(let i=0;i<8;i++)c=(c&1)?(c>>>1)^0x8408:(c>>>1);}return c&0xFFFF;}
  ```

## Full command map (from `trace_flash_vvvv.txt`, a complete successful -vvvv flash)

The whole flash sequence is captured. Command byte is the first body byte; response frames start with a
response code (`0x80`=OK, `0x86`=SIGN_ON, `0x87`=PARAMETER, `0x82`=MEMORY, `0xA0`=FAILED).

| Cmd | Name | avrdude fn | body len (ex.) | notes |
|----|----|----|----|----|
| 0x01 | SIGN_ON | getsync | 1 | first command; response carries "JTAGICE mkII" string |
| 0x0f | GET_SYNC | getsync | 1 | |
| 0x02 | SET_PARAMETER | setparm | 3 | `[02, paramID, value]` |
| 0x03 | GET_PARAMETER | getparm | 2 | `[03, paramID]` |
| 0x0c | SET_DEVICE_DESCRIPTOR | set_devdescr | **299** | device-specific; exact 298-byte payload captured in trace |
| 0x0b | RESET | reset | 2 | `[0b, 01]` |
| 0x14 | ENTER_PROGMODE | program_enable | 1 | |
| 0x05 | READ_MEMORY | read_byte | 10 | `[05, memtype, size(4LE), addr(4LE)]`; signature memtype `0xB4` @ addr 0x1100.. |
| 0x34 | CHIP_ERASE (UPDI) | chip_erase | 6 | |
| 0x04 | WRITE_MEMORY | paged_write / write_byte | 138 (page) / 11 (fuse) | `[04, memtype, size(4LE), addr(4LE), data...]` |
| 0x15 | LEAVE_PROGMODE | program_disable | 1 | |
| 0x08 / 0x00 | GO / SIGN_OFF | close | 1 | teardown |

**Ordered sequence (read-only milestone bolded):** **SIGN_ON** → SET/GET_PARAMETER ×several → GET_SYNC →
**SET_DEVICE_DESCRIPTOR** → RESET → **ENTER_PROGMODE** → **READ_MEMORY(signature)** → [ CHIP_ERASE →
WRITE_MEMORY ×pages → WRITE_MEMORY ×3 fuses (2=0x01, 5=0xC9, 8=0x00) ] → LEAVE_PROGMODE → SIGN_OFF.

Exact captured frames for every command (verbatim bytes to replay, incl. the 298-byte descriptor) are in
`trace_flash_vvvv.txt` (grep the `Send:`/`Recv:` lines). Fuse memtypes and the paged_write header layout are
all there. **Nothing about the protocol is unknown now - the remaining work is implementation, not RE.**

### Traces on file
- `trace_readsig.txt` - the failed direct-avrdude attempts (proved the 1200-touch requirement + sign-on frame).
- `trace_flash_vvvv.txt` - a complete successful flash at -vvvv (the full protocol ground truth). Captured by
  temporarily setting `tools.avrdude.upload.params.verbose=-v -v -v -v` in the core's platform.txt and running
  `arduino-cli upload --verbose` (the working path does the 1200 touch); platform.txt restored afterward.

## Programmer flakiness (informs the flasher's retry design)

- Under repeated failed sign-ons the onboard mEDBG **wedges** and stops responding entirely - a physical
  **USB unplug/replug** clears it (matches what we saw during bring-up). The flasher's auto-retry should attempt N
  silent reconnects (re-touch + reopen), then surface a single "unplug/replug" prompt - exactly this case.
- avrdude's own `jtagmkII_getsync()` retries the sign-on many times before giving up; our JS retry loop
  should behave similarly but with the 1200 touch re-done each attempt.

## Status: protocol RE complete ✅ + live handshake VALIDATED ON HARDWARE ✅ (2026-07-17)

Everything needed is captured and confirmed: transport, framing, CRC, the full command set, exact frames
(incl. the device descriptor), the 1200-touch precondition, and the no-re-enumeration behavior.

**The WebSerial JS core ran end-to-end against the real module on the first try** (`test/identify.html`):
1200 touch → sign-on → set-devdescr → enter-progmode → read signature all succeeded (~1.8 s), returning
**`1E 96 51`** - which is **correct**: the ATmega4809 signature is `1E 96 51` per avrdude.conf (line 16062;
the `0x96` middle byte is the 48 KB-flash class, not the `0x95` I'd misremembered from the datasheet). The JS
read matches avrdude's own captured read byte-for-byte. So `EXPECTED_SIGNATURE` was fixed to `1E 96 51` and
the read was confirmed rock-solid (no glitch - the majority-vote experiment was reverted). **The entire
handshake + read path is validated against ground truth.** Write path is next (with read-back verify).

## Next steps (build)

1. **WebSerial JS core** (`flasher/webserial-jtag2updi.js`), milestone order:
   a. Frame encode/decode + CRC (MCRF4XX) - unit-test the encoder against the captured frames offline.
   b. 1200-bps touch (open@1200 / close) → reopen @115200.
   c. **Read-only milestone:** SIGN_ON → params → SET_DEVICE_DESCRIPTOR → RESET → ENTER_PROGMODE →
      READ_MEMORY(signature); assert `1E 95 51`. Safe to iterate - no writes. User tests in a Chromium browser.
   d. Write path: CHIP_ERASE → paged WRITE_MEMORY from parsed Intel HEX → fuse writes → LEAVE_PROGMODE →
      read-back verify.
   e. Auto-retry state machine (silent re-touch/reopen; single "unplug/replug" prompt only if truly stuck).
2. Hand the core to the **website session** to wrap in the FW-updater tab UI (variant picker, progress,
   browser-support check).

**Test harness note:** the JS core needs a real Chromium browser + the module - this CLI can't run WebSerial.
Build the core here (offline-unit-test the framing/CRC against the captured frames), then the user runs a
minimal test page in-browser against the module to validate the live handshake.
