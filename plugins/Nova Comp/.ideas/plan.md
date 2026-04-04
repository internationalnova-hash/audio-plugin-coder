# Implementation Plan

## Complexity Score: 3 / 5

## Framework Selection
**Decision:** `webview`

**Rationale:**
Nova Comp should match the premium Nova family presentation with polished hardware-style knobs, a refined analog-style meter, mode buttons, and a clean preset strip. A WebView UI is the best fit for that boutique hardware look.

## Implementation Strategy
**Phased implementation**

### Phase 2.1.1 — Core Compression Engine
- [ ] Build the detector and gain computer around the `COMPRESSION` macro
- [ ] Keep the front-panel identity centered on three main controls: `COMPRESSION`, `TONE SHAPING`, and `OUTPUT`
- [ ] Do **not** add a separate `Input` knob in v1; preserve the simplified Nova workflow
- [ ] Map `compression` into threshold behavior using:
  ```text
  thresholdDb = -10.0 - (compression / 10.0) * 22.0
  ```
- [ ] Add slight mode-based ratio bias:
  - `Smooth`: `2.5:1 → 3.5:1`
  - `Punch`: `3.5:1 → 4.5:1`
  - `Limit`: `6:1 → 8:1`
- [ ] Add click-free smoothing for `compression`, `output`, `mix`, and `tone`
- [ ] Add makeup gain and true post-compression dry/wet blend
- [ ] Add subtle auto trim so louder does not automatically sound better

### Phase 2.1.2 — Optical / Program-Dependent Feel
- [ ] Implement the program-dependent release system:
  ```text
  grNorm = currentGainReduction / maxGainReduction
  effectiveReleaseMs = releaseFastMs + grNorm * (releaseSlowMs - releaseFastMs)
  ```
- [ ] Tune `Smooth`, `Punch`, and `Limit` as clearly different compressor personalities
- [ ] Keep the default sound forgiving, musical, and fast to dial in

### Phase 2.1.3 — Tone + Warmth System
- [ ] Add the compression-aware `tone` tilt stage using:
  ```text
  tiltDb = -3.0 + toneNorm * 6.0
  ```
- [ ] Add sidechain emphasis from the tone control:
  ```text
  sidechainTiltDb = -2.0 + toneNorm * 4.0
  ```
- [ ] Make `tone` interact with the compressed signal, not just the final output
- [ ] Add subtle harmonic enhancement tied to gain reduction:
  ```text
  warmthAmount = gainReductionDb * 0.025
  colored = tanh(compressed * (1.0 + warmthAmount))
  ```
- [ ] Add slight top softening and presence recovery after compression

### Phase 2.1.4 — Magic + Metering
- [ ] Add `Magic` mode as **automatic leveling + smoothness + polish**, not extra color
- [ ] Implement smarter program-dependent release inside `Magic`
- [ ] Add slight vocal leveling bias, soft transient control, tone-aware smoothing, tiny stereo polish, and auto output balance
- [ ] Scale `Magic` by mode so it stays strongest in `Smooth`, reduced in `Punch`, and light in `Limit`
- [ ] Use mode scaling:
  - `Smooth = 1.0x`
  - `Punch = 0.8x`
  - `Limit = 0.65x`
- [ ] Add `GR` / `OUT` meter switching
- [ ] Make the meter feel slower and smoother than Nova Heat for a more analog compressor identity

### Phase 2.1.5 — UI + Presets
- [ ] Implement the `NOVA COMP` WebView interface
- [ ] Add bottom preset buttons: `VOCAL`, `BASS`, `DRUMS`, `MASTER`
- [ ] Ensure presets are level-balanced and feel mix-ready immediately
- [ ] Preserve the simple workflow:
  1. `LEVEL`
  2. `GAIN`
  3. `MIX`
  4. `MAGIC`

## Mode Targets

| Mode | Ratio | Attack | Release Fast | Release Slow | Knee | Character |
|------|------:|-------:|-------------:|-------------:|-----:|-----------|
| `Smooth` | ~2.5:1 to 3.5:1 | 10 ms | 90 ms | 1300 ms | 8.0 dB | smooth, forgiving, vocal-friendly |
| `Punch` | ~3.5:1 to 4.5:1 | 5 ms | 60 ms | 500 ms | 5.0 dB | tighter, more forward, more control |
| `Limit` | ~6:1 to 8:1 | 2.5 ms | 80 ms | 700 ms | 2.5 dB | strong peak control and density |

## Final Preset Targets

| Preset | Mode | Compression | Tone Shaping | Output | Mix | Magic |
|------|------|------------:|-------------:|-------:|----:|------|
| `VOCAL` | `Smooth` | 4.2 | 5.8 | +1.8 dB | 72% | ON |
| `DRUMS` | `Punch` | 5.8 | 5.0 | +1.2 dB | 58% | OFF |
| `BASS` | `Smooth` | 5.3 | 4.0 | +2.3 dB | 80% | OFF |
| `MASTER` | `Smooth` | 2.6 | 5.0 | 0.0 dB | 45% | ON |

## Required JUCE Modules
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_dsp`
- `juce_gui_basics`
- `juce_gui_extra`

## Risk Assessment

### High Risk
- Getting the release behavior to feel truly optical and premium instead of generic
- Making `Limit` stronger without becoming harsh or lifeless
- Making one `level` knob feel powerful but still instantly musical

### Medium Risk
- Blending `tone` with compression in a way that feels useful, not EQ-like
- Keeping `Magic` helpful without making the interface feel misleading
- Matching gain and auto trim so presets audition fairly

### Low Risk
- Parameter binding
- Parallel mix
- Preset recall
- Meter view switching

## Verification Priorities
- Confirm `Smooth` feels vocal-friendly, forgiving, and musical
- Confirm `Punch` adds tighter control without over-choking the transient
- Confirm `Limit` catches peaks decisively but still sounds expensive
- Verify heavier GR releases more slowly and musically than light GR
- Verify `Magic` adds polish without changing the visible control positions
- Verify the `VOCAL`, `BASS`, `DRUMS`, and `MASTER` presets sound right immediately
- Check `GR` and `OUT` meter modes for readable, smooth analog-style motion
- Confirm auto trim prevents compression from winning only by loudness

## Recommended Next Step
Proceed to the **implementation** phase and turn this finalized optical compression map into the real Nova Comp processor and WebView UI bindings.