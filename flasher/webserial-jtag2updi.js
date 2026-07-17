// webserial-jtag2updi.js
// WebSerial flasher for the Arduino Nano Every (ATmega4809) - the ID00M / stock-v.99 firmware updater.
// Speaks avrdude's `jtag2updi` protocol (a JTAGICE-mkII subset) to the board's onboard mEDBG bridge over
// the Web Serial API. Reverse-engineered from a captured avrdude -vvvv flash; see flasher/protocol/FINDINGS.md.
//
// The pure protocol layer (crc16 / encodeFrame / FrameDecoder) is runtime-agnostic and is unit-tested against
// captured real frames (flasher/test/frame-test.html). The Jtag2Updi transport uses navigator.serial and only
// runs in a Chromium-family desktop browser.

export const MSG_START = 0x1b;
export const TOKEN     = 0x0e;

// ---- CRC-16/MCRF4XX (poly 0x1021, init 0xFFFF, reflected; reflected-table poly 0x8408). CONFIRMED on real frames.
export function crc16(bytes) {
  let c = 0xFFFF;
  for (const b of bytes) {
    c ^= b;
    for (let i = 0; i < 8; i++) c = (c & 1) ? ((c >>> 1) ^ 0x8408) : (c >>> 1);
  }
  return c & 0xFFFF;
}

// ---- Encode a JTAGICE-mkII frame: 1B | seq(2 LE) | size(4 LE) | 0E | body | crc(2 LE)
export function encodeFrame(seq, body) {
  const size = body.length;
  const f = new Uint8Array(8 + size + 2);
  f[0] = MSG_START;
  f[1] = seq & 0xff;           f[2] = (seq >>> 8) & 0xff;
  f[3] = size & 0xff;          f[4] = (size >>> 8) & 0xff;
  f[5] = (size >>> 16) & 0xff; f[6] = (size >>> 24) & 0xff;
  f[7] = TOKEN;
  f.set(body, 8);
  const c = crc16(f.subarray(0, 8 + size));
  f[8 + size] = c & 0xff;      f[9 + size] = (c >>> 8) & 0xff;
  return f;
}

// ---- Streaming decoder: push received bytes, get back complete, CRC-checked frames.
export class FrameDecoder {
  constructor() { this.buf = []; }
  push(chunk) {
    for (const b of chunk) this.buf.push(b);
    const out = [];
    for (;;) {
      while (this.buf.length && this.buf[0] !== MSG_START) this.buf.shift(); // resync to start byte
      if (this.buf.length < 8) break;
      const size = this.buf[3] | (this.buf[4] << 8) | (this.buf[5] << 16) | (this.buf[6] << 24);
      const total = 8 + size + 2;
      if (this.buf.length < total) break;
      const frame = this.buf.slice(0, total);
      const seq = frame[1] | (frame[2] << 8);
      const body = frame.slice(8, 8 + size);
      const rxcrc = frame[8 + size] | (frame[9 + size] << 8);
      const crcOk = frame[7] === TOKEN && crc16(frame.slice(0, 8 + size)) === rxcrc;
      this.buf = this.buf.slice(total);
      out.push({ seq, body, crcOk });
    }
    return out;
  }
}

// ---- Protocol constants (from the capture) ----
export const CMD = {
  SIGN_ON: 0x01, SET_PARAM: 0x02, GET_PARAM: 0x03, WRITE_MEM: 0x04, READ_MEM: 0x05,
  RESET: 0x0b, SET_DEVDESCR: 0x0c, GET_SYNC: 0x0f, ENTER_PROGMODE: 0x14, LEAVE_PROGMODE: 0x15,
  CHIP_ERASE: 0x34, GO: 0x08, SIGN_OFF: 0x00,
};
export const RSP = { OK: 0x80, MEMORY: 0x82, SIGN_ON: 0x86, PARAMETER: 0x87, FAILED: 0xa0 };
export const MTYPE = { SIGNATURE: 0xb4, FLASH: 0xc0, FUSE: 0xb2 };
export const FLASH_BASE = 0x4000;   // ATmega4809 flash is mapped at 0x4000 in the UPDI/NVM space
export const PAGE_SIZE  = 128;
export const SIG_ADDR   = 0x1100;
export const EXPECTED_SIGNATURE = [0x1e, 0x96, 0x51]; // ATmega4809 (per avrdude.conf; 0x96 = 48KB flash class)
export const FUSES = [ { addr: 0x1282, value: 0x01 },  // fuse2 OSCCFG
                       { addr: 0x1285, value: 0xC9 },  // fuse5 SYSCFG0
                       { addr: 0x1288, value: 0x00 } ];// fuse8 BOOTEND

// ATmega4809 UPDI device descriptor (298 bytes), captured verbatim from avrdude -c jtag2updi. Opaque; replay as-is.
export const DEVICE_DESCRIPTOR = new Uint8Array([
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x80,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x00,0x00,0x00,
  0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
]);

// The handshake command bodies avrdude sends, in order, before enter-progmode (SET_DEVDESCR spliced in by code).
export const HANDSHAKE = [
  [CMD.SIGN_ON],
  [CMD.SET_PARAM, 0x03, 0x06],
  [CMD.GET_SYNC],
  [CMD.GET_PARAM, 0x01],
  [CMD.GET_PARAM, 0x02],
  [CMD.GET_PARAM, 0x06],
  [CMD.SET_PARAM, 0x05, 0x07],
];

const u32le = (n) => [n & 0xff, (n >>> 8) & 0xff, (n >>> 16) & 0xff, (n >>> 24) & 0xff];
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

// ---- Intel HEX parser -> flat Uint8Array (program bytes at their load addresses, 0-based). ----
export function parseIntelHex(text) {
  let max = 0; const recs = [];
  for (const raw of text.split(/\r?\n/)) {
    const line = raw.trim();
    if (!line || line[0] !== ':') continue;
    const bytes = [];
    for (let i = 1; i < line.length; i += 2) bytes.push(parseInt(line.substr(i, 2), 16));
    const len = bytes[0], addr = (bytes[1] << 8) | bytes[2], type = bytes[3];
    if (type === 0x01) break;                 // EOF
    if (type === 0x00) { recs.push({ addr, data: bytes.slice(4, 4 + len) }); max = Math.max(max, addr + len); }
    // (no extended-address records in the <64K Nano Every image)
  }
  const out = new Uint8Array(max).fill(0xff);
  for (const r of recs) out.set(r.data, r.addr);
  return out;
}

// ---- Transport / sequence over Web Serial ----
export class Jtag2Updi {
  constructor(port) { this.port = port; this.seq = 0; this.dec = new FrameDecoder(); this.frames = []; this._reader = null; this._readLoop = null; }

  static async requestPort() {
    if (!('serial' in navigator)) throw new Error('Web Serial not supported - use a Chromium-based desktop browser (Chrome, Edge, Opera, Brave).');
    return navigator.serial.requestPort();
  }

  // 1200bps touch resets the target; the Nano Every does NOT re-enumerate (same port), so we reopen the same port.
  async touch1200() {
    await this.port.open({ baudRate: 1200 });
    try { await this.port.setSignals({ dataTerminalReady: false }); } catch (e) {}
    await sleep(150);
    await this.port.close();
    await sleep(1200); // let the target come back up
  }

  async open() {
    await this.port.open({ baudRate: 115200 });
    this._reader = this.port.readable.getReader();
    this._readLoop = (async () => {
      try { for (;;) { const { value, done } = await this._reader.read(); if (done) break; if (value) { if (this.onRaw) this.onRaw(value); this.frames.push(...this.dec.push(value)); } } }
      catch (e) {}
      finally { try { this._reader.releaseLock(); } catch (e) {} } // release the lock so port.close() can actually close
    })();
  }

  async close() {
    try { if (this._reader) await this._reader.cancel(); } catch (e) {} // ends read() -> loop finally releases the lock
    try { await this._readLoop; } catch (e) {}
    this._reader = null; this._readLoop = null;
    try { await this.port.close(); } catch (e) {}
  }

  async _write(bytes) { const w = this.port.writable.getWriter(); try { await w.write(bytes); } finally { w.releaseLock(); } }

  async _nextFrame(timeout = 2000) {
    const t0 = Date.now();
    for (;;) {
      if (this.frames.length) return this.frames.shift();
      if (Date.now() - t0 > timeout) throw new Error('timeout waiting for response frame');
      await sleep(2);
    }
  }

  // Send a command body, await one response frame, optionally require a response code.
  async cmd(body, opts) {
    opts = opts || {};
    const timeout = opts.timeout || 2000;
    this.dec.buf.length = 0; this.frames.length = 0; // start clean: drop any stale bytes from a prior slow/failed response
    await this._write(encodeFrame((this.seq++) & 0xffff, body));
    const f = await this._nextFrame(timeout);
    if (!f.crcOk) throw new Error('bad CRC in response');
    if (opts.expect != null && f.body[0] !== opts.expect)
      throw new Error('unexpected response 0x' + f.body[0].toString(16) + ' (wanted 0x' + opts.expect.toString(16) + ')');
    return f;
  }

  // Recover after a comms glitch on the flaky link. A single corrupted TX byte makes the device ignore a frame
  // (no response) and, if the size field was hit, leaves it stuck mid-body. Pause long enough for the device's
  // frame parser to time out on a partial frame, flush our buffers, then GET_SYNC to re-establish alignment.
  async resync() {
    await sleep(250);
    this.dec.buf.length = 0; this.frames.length = 0;
    try { await this.cmd([CMD.GET_SYNC], { timeout: 1500 }); } catch (e) {}
  }

  // Deep recovery: fully re-establish the link (close, 1200-touch, re-open, re-handshake into progmode). Used when
  // a GET_SYNC resync can't un-stick the device. The flash keeps whatever was already written (erase happened once
  // at the start), so writeFlash resumes from the same page afterwards.
  async reconnect() {
    try { await this.close(); } catch (e) {}
    await this.touch1200();
    await this.open();
    await this.signOnAndEnterProgmode();
  }

  // Send a command, re-syncing between attempts. This is what makes sustained page writes survive the flaky
  // programmer (blind re-sends don't recover a stuck device parser; a resync does).
  async _txRetry(frame, opts, tries = 6) {
    for (let a = 0; ; a++) {
      try { return await this.cmd(frame, opts); }
      catch (e) { if (a >= tries - 1) throw e; await this.resync(); }
    }
  }

  // Full recovery ladder for the sustained read/write loops: GET_SYNC resyncs first (_txRetry), then escalate to a
  // full reconnect (re-touch + re-handshake) if the device is stuck deeper. Used by both readFlash and writeFlash so
  // verify survives the flaky link exactly like writing does. `label` is for the error/log; `onEvent` reports reconnects.
  async _cmdRecover(frame, opts, label, onEvent) {
    for (let deep = 0; ; deep++) {
      try { return await this._txRetry(frame, opts); }
      catch (e) {
        if (deep >= 3) throw new Error(`${label} failed: ${e.message}`);
        if (onEvent) onEvent(`link stalled (${label}) - full reconnect…`);
        await this.reconnect();
      }
    }
  }

  async signOnAndEnterProgmode() {
    this.seq = 0;
    await this.cmd(HANDSHAKE[0], { expect: RSP.SIGN_ON });     // SIGN_ON
    for (let i = 1; i < HANDSHAKE.length; i++) await this.cmd(HANDSHAKE[i]);
    await this.cmd([CMD.SET_DEVDESCR, ...DEVICE_DESCRIPTOR]);  // SET_DEVICE_DESCRIPTOR
    await this.cmd([CMD.RESET, 0x01]);                         // RESET
    await this.cmd([CMD.ENTER_PROGMODE], { expect: RSP.OK });  // ENTER_PROGMODE
  }

  async readByteAt(memtype, addr) {
    const f = await this.cmd([CMD.READ_MEM, memtype, ...u32le(1), ...u32le(addr)], { expect: RSP.MEMORY });
    return f.body[1];
  }

  async readSignature() {
    const sig = [];
    for (let i = 0; i < 3; i++) sig.push(await this.readByteAt(MTYPE.SIGNATURE, SIG_ADDR + i));
    return sig; // ATmega4809 => 1E 96 51 (validated byte-for-byte against avrdude's own read)
  }

  async leave() {
    try { await this.cmd([CMD.LEAVE_PROGMODE], { expect: RSP.OK }); } catch (e) {}
    try { await this.cmd([CMD.SIGN_OFF]); } catch (e) {}
  }

  // READ-ONLY MILESTONE: connect + handshake + read & check signature. No writes. Safe to iterate.
  async identify() {
    await this.touch1200();
    await this.open();
    await this.signOnAndEnterProgmode();
    const sig = await this.readSignature();
    const match = sig.length === 3 && sig.every((b, i) => b === EXPECTED_SIGNATURE[i]);
    await this.leave();
    return { signature: sig, match };
  }

  // WRITE PATH (implemented from the captured frame structure; validate the read milestone first).
  async chipErase() { await this.cmd([CMD.CHIP_ERASE, 0, 0, 0, 0, 0]); }

  async writeFlash(bytes, onProgress, onEvent) {
    for (let off = 0; off < bytes.length; off += PAGE_SIZE) {
      const page = bytes.subarray(off, Math.min(off + PAGE_SIZE, bytes.length));
      const chunk = new Uint8Array(PAGE_SIZE).fill(0xff); chunk.set(page);
      const frame = [CMD.WRITE_MEM, MTYPE.FLASH, ...u32le(PAGE_SIZE), ...u32le(FLASH_BASE + off), ...chunk];
      await this._cmdRecover(frame, { expect: RSP.OK, timeout: 3000 }, `write @0x${(FLASH_BASE + off).toString(16)}`, onEvent);
      if (onProgress) onProgress(Math.min(off + PAGE_SIZE, bytes.length), bytes.length);
    }
  }

  async writeFuses() {
    for (const fz of FUSES) await this.cmd([CMD.WRITE_MEM, MTYPE.FUSE, ...u32le(1), ...u32le(fz.addr), fz.value], { expect: RSP.OK });
  }

  // Read `len` bytes of flash from `addr` (UPDI space = FLASH_BASE + offset). Read-only; safe to validate first.
  // The mEDBG dribbles the response back in many chunks and the programmer is flaky, so use a generous timeout
  // and retry a stalled read a couple of times (each cmd() flushes stale bytes first).
  async readFlash(addr, len, onEvent) {
    const f = await this._cmdRecover([CMD.READ_MEM, MTYPE.FLASH, ...u32le(len), ...u32le(addr)], { expect: RSP.MEMORY, timeout: 8000 }, `read @0x${addr.toString(16)}`, onEvent);
    return Uint8Array.from(f.body.slice(1, 1 + len));
  }

  // Read the flash back a page at a time and compare to `bytes`. Throws on the first mismatch.
  async verifyFlash(bytes, onProgress, onEvent) {
    for (let off = 0; off < bytes.length; off += PAGE_SIZE) {
      const n = Math.min(PAGE_SIZE, bytes.length - off);
      const got = await this.readFlash(FLASH_BASE + off, n, onEvent);
      for (let i = 0; i < n; i++) {
        if (got[i] !== bytes[off + i])
          throw new Error(`verify mismatch at 0x${(off + i).toString(16)}: wrote 0x${bytes[off + i].toString(16)}, read 0x${(got[i] ?? 0).toString(16)}`);
      }
      if (onProgress) onProgress(off + n, bytes.length);
    }
    return true;
  }

  // READ-ONLY: connect, read flash back, and compare to `bytes`. Validates the read-back path without writing.
  async verifyAgainst(bytes, opts) {
    opts = opts || {};
    await this.touch1200(); await this.open();
    await this.signOnAndEnterProgmode();
    const sig = await this.readSignature();
    if (!sig.every((b, i) => b === EXPECTED_SIGNATURE[i])) { await this.leave(); throw new Error('wrong chip signature'); }
    try { await this.verifyFlash(bytes, opts.onProgress, opts.onEvent); return { match: true, bytes: bytes.length }; }
    catch (e) { return { match: false, error: e.message }; }
    finally { await this.leave(); }
  }

  async program(hexText, opts) {
    opts = opts || {};
    const phase = opts.onPhase || (() => {});
    const bytes = parseIntelHex(hexText);
    await this.touch1200();
    await this.open();
    await this.signOnAndEnterProgmode();
    const sig = await this.readSignature();
    if (!sig.every((b, i) => b === EXPECTED_SIGNATURE[i])) { await this.leave(); throw new Error('wrong chip signature: ' + sig.map((x) => x.toString(16)).join(' ')); }
    phase('erase');  await this.chipErase();
    phase('write');  await this.writeFlash(bytes, opts.onProgress, opts.onEvent);
    phase('verify'); await this.verifyFlash(bytes, opts.onVerifyProgress || opts.onProgress, opts.onEvent);
    if (opts.writeFuses) { phase('fuses'); await this.writeFuses(); }  // opt-in: fuses rarely change and are already correct
    await this.leave();
    return { bytes: bytes.length, verified: true };
  }
}

// ---- Auto-retry wrapper: silent re-attempts; caller decides when to prompt the user to unplug/replug. ----
export async function withRetry(makeAttempt, opts) {
  opts = opts || {};
  const tries = opts.tries || 4;
  let last;
  for (let i = 0; i < tries; i++) {
    try { return await makeAttempt(i); }
    catch (e) { last = e; if (opts.onRetry) opts.onRetry(i + 1, tries, e); await new Promise((r) => setTimeout(r, 300 * (i + 1))); }
  }
  throw last;
}
