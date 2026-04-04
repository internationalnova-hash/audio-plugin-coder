# Implementation Plan

## Complexity Score: 3 / 5

## Framework Selection
**Decision:** `webview`

**Rationale:**
Nova Heat should match the premium Nova family look established by Space By Nova and Nova Tone. The requested champagne-and-gold hardware presentation, bottom preset buttons, and rich control styling are a strong fit for the WebView workflow.

## Implementation Strategy
**Phased implementation**

### Phase 2.1.1 — Core Processing
- [ ] Implement input drive mapping from `0 dB` to `+24 dB`
- [ ] Add parameter smoothing for `drive`, `heat`, `tone`, `mix`, and `output_gain`
- [ ] Build the unified tanh-based curve engine:
  - `Soft: k = 0.8 + heatNorm * 0.4`
  - `Medium: k = 1.2 + heatNorm * 1.0`
  - `Hard: k = 1.8 + heatNorm * 1.5`
- [ ] Add slight asymmetry in `Hard` mode for analog-feeling odd harmonics
- [ ] Add post-saturation dry/wet blend and final output trim
- [ ] Add light automatic output compensation tied to `drive`

### Phase 2.1.2 — Tone + Heat Behavior
- [ ] Implement the tilt response for `tone` around `~2 kHz`
- [ ] Ensure `tone` shapes the signal **before** it reaches the saturator
- [ ] Make `heat` reshape the curve itself, not simply add gain
- [ ] Add mode-aware top-end smoothing after saturation
- [ ] Keep the default behavior warm and polished, never harsh

### Phase 2.1.3 — Pro Engine Behaviors
- [ ] Add `Dynamic Drive` using an envelope follower and mode-based sensitivity
- [ ] Add `Stereo Smart Width` on the wet path only using M/S processing
- [ ] Add `Vocal Harmonic Focus` with pre-emphasis before saturation and de-emphasis after
- [ ] Add subtle hidden transient shaping tied to the selected mode
- [ ] Add a `Magic Mode` toggle that biases dynamic drive, focus, smoothing, width, and trim
- [ ] Verify these systems feel premium but remain invisible in the UI

### Phase 2.1.4 — Mode Voicing System
- [ ] Build the three core DSP modes: `Soft`, `Medium`, and `Hard`
- [ ] Tune curve scale, asymmetry, harmonic balance, and transient softening per mode
- [ ] Ensure each mode feels like different hardware, not a simple gain change
- [ ] Keep low-drive settings flattering and musical in every mode

### Phase 2.1.5 — UI & Preset Polish
- [ ] Bind the five main controls and mode selection to the Nova Heat UI
- [ ] Add a premium `MAGIC` toggle above the preset row
- [ ] Add bottom preset buttons for `Vocal`, `Drums`, `Bass`, and `Master`
- [ ] Make each preset recall `Mode`, `Drive`, `Heat`, `Tone`, `Mix`, and `Output`
- [ ] Level-match presets using stored output trim plus `autoTrimDb = -(drive * 0.3)`
- [ ] Add output metering and readable numeric values
- [ ] Tune default gain compensation for fair A/B evaluation


## DSP Targets

### Default starting point
- Mode: `Medium`
- Drive: `2.0`
- Heat: `2.5`
- Tone: `5.0`
- Mix: `18`
- Output: `0.0`

### Final preset targets
| Preset | Mode | Drive | Heat | Tone | Mix | Output | Character |
|------|------|------:|-----:|-----:|----:|-------:|-----------|
| `VOCAL` | `Medium` | 4.2 | 5.0 | 5.6 | 28 | -0.8 dB | warm, present |
| `DRUMS` | `Hard` | 5.8 | 6.5 | 4.8 | 35 | -1.5 dB | punchy, aggressive |
| `BASS` | `Medium` | 5.0 | 5.8 | 3.8 | 32 | -1.2 dB | thick, controlled |
| `MASTER` | `Soft` | 2.8 | 3.2 | 5.0 | 18 | 0.0 dB | subtle glue |

### Mode coefficient targets
| Mode | Curve Engine | Harmonics | Transients | Tone Bias | Internal Multipliers |
|------|--------------|-----------|------------|-----------|----------------------|
| `Soft` | `k = 0.8 + heatNorm * 0.4` | even-high / odd-low | mostly preserved | slight warmth, smooth highs | `drive x0.7`, `heat x0.6` |
| `Medium` | `k = 1.2 + heatNorm * 1.0` | even-medium-high / odd-medium | slightly rounded | small presence lift, controlled highs | `drive x1.0`, `heat x1.0` |
| `Hard` | `k = 1.8 + heatNorm * 1.5` + asymmetry | even-medium / odd-high | noticeably softened | darker top, thick low mids | `drive x1.25`, `heat x1.3` |

## Required JUCE Modules
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_dsp`
- `juce_gui_basics`
- `juce_gui_extra`

## Risk Assessment

### High Risk
- Keeping the processor smooth and musical at high drive values
- Avoiding brittle brightness or cheap-sounding fizz as `heat` rises
- Making mode changes feel premium rather than gimmicky

### Medium Risk
- Making `tone` and `heat` clearly different in feel and function
- Preserving clarity on vocals and mix bus material while still adding excitement
- Achieving reliable loudness compensation across character modes

### Low Risk
- Standard parameter binding
- Dry/wet parallel blending
- Output metering and preset button workflow

## Verification Priorities
- Compare `Soft`, `Medium`, and `Hard` at matched settings to confirm they feel meaningfully different
- Confirm `Dynamic Drive` reacts to song dynamics so louder phrases bloom more than quiet passages
- Verify `Stereo Smart Width` makes the wet signal feel wider without destabilizing mono playback
- Confirm `Vocal Harmonic Focus` adds presence in the `1k–4k` region without harshness
- Toggle `MAGIC` on/off across all presets and confirm it sounds more finished without overstepping the knobs
- Audition `VOCAL`, `DRUMS`, `BASS`, and `MASTER` presets and confirm they sound finished immediately
- Compare low vs high `heat` inside each mode to ensure the change is obvious but still smooth
- Verify that `mix` behaves like a true parallel saturator
- Confirm output trim and `autoTrimDb = -(drive * 0.3)` prevent misleading loudness jumps
- Test on vocals, drums, bass, keys, and a simple stereo bus

## Recommended Next Step
Proceed to the **implementation** phase and translate this DSP map into the real Nova Heat processor and WebView UI bindings.
