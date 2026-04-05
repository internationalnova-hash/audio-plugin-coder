# Nova Master — Implementation Plan

## Complexity Score: 3 / 5

## Framework Decision
**Selected UI framework:** `webview`

### Rationale
WebView is the best fit for Nova Master because it should feel like a premium flagship processor with:
- elegant five-knob layout
- boutique Nova-family presentation
- polished meter feedback
- interactive preset / mode buttons
- visually elevated, mastering-room styling

## Implementation Strategy
**Phased implementation**

### Phase 2.1.1 — Core Tonal Engine
- [ ] Implement `tone` as a broad mastering tilt around roughly `2 kHz`
- [ ] Implement `weight` as a low contour centered around roughly `80 Hz`
- [ ] Add subtle low-mid tightening around `180–250 Hz` so higher `Weight` stays clean
- [ ] Implement `air` as a smooth high shelf around roughly `10–14 kHz`
- [ ] Add output trim and fixed ceiling safety target around `-0.5 dBFS`

### Phase 2.1.2 — Glue + Finish Engine
- [ ] Implement `glue` threshold mapping from about `-6 dB` to `-18 dB`
- [ ] Implement `glue` ratio mapping from about `1.5:1` to `3.5:1`
- [ ] Use about `30 ms` attack with auto / smooth release behavior
- [ ] Add a soft-knee, mix-bus-safe compressor feel
- [ ] Add subtle harmonic finish using low tanh-style density
- [ ] Implement `finish_mode` with micro dynamic smoothing, slight stereo polish, top smoothing, and small auto trim

### Phase 2.1.3 — Stereo + Metering
- [ ] Implement `width` mostly above `180–250 Hz`
- [ ] Keep the low end centered and stable as width rises
- [ ] Add optional subtle decorrelation up to roughly `6 ms`
- [ ] Add output / loudness metering with restrained mastering-style behavior
- [ ] Add mode recall for `Clean`, `Warm`, `Wide`, and `Loud`

### Phase 2.1.4 — UI Binding + Polish
- [ ] Bind all parameters to the WebView UI
- [ ] Keep the plugin simple and confidence-inspiring, not technical
- [ ] Make `FINISH` feel valuable without creating a big loudness jump
- [ ] Validate gain staging, loudness consistency, and mastering-safe top-end behavior
- [ ] Confirm that the plugin is hard to make sound bad across a full mix bus

## Required JUCE Modules
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_dsp`
- `juce_gui_basics`
- `juce_gui_extra`

## Risk Assessment

### High Risk
- making the mastering chain feel stronger without becoming harsh or over-processed
- keeping `width` safe and stable in the low end
- making `finish_mode` clearly valuable but not gimmicky

### Medium Risk
- gain staging between glue compression, harmonic finish, and output balance
- keeping presets meaningfully different while still mastering-safe
- ensuring the loudness result feels better, not just louder

### Low Risk
- parameter binding
- WebView layout and interaction wiring
- preset recall logic

## DSP Lock Notes

### Preset personalities
- `Clean` = safest and most transparent
- `Warm` = richer, smoother, slightly darker, more harmonic body
- `Wide` = more open and spacious with extra side polish
- `Loud` = densest and most forward while staying mastering-safe

### Critical hidden rules
- more `Glue` should slightly tighten the low end
- more `Air` should slightly smooth the top end
- more `Width` should never destabilize bass mono stability
- `Finish` should improve polish without causing an obvious loudness jump

## Success Criteria
Nova Master is successful when:
- a user can open it and understand it in seconds
- a mix sounds more polished and connected with only a few moves
- the plugin feels premium, simple, and trustworthy
- it stays subtle, musical, and hard to make sound bad
- it feels more finished and balanced, not just louder or brighter
- the suite positioning is clear:
  - `Tone` = shaping
  - `Heat` = color
  - `Level` = control
  - `Master` = finish

## Final Identity Lock
If another finishing plugin says:
> “make the mix hit harder”

Nova Master should say:
> “make the mix feel complete.”
