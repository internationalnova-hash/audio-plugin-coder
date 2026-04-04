# DSP Architecture Specification

## Core Components
Nova Heat should be built from these main DSP blocks:

- **Input drive stage**
  - Applies gain before the nonlinear processor
  - Smoothed to avoid zipper noise
  - Maps `drive` from `0 dB` up to `+24 dB`

- **Tone pre-shaper / tilt EQ**
  - Shapes the signal before it hits the saturator
  - Lets the harmonic generation feel darker/warmer or brighter/more present
  - Pivot should sit around `1.8–2.5 kHz`

- **Vocal harmonic focus stage**
  - Light pre-emphasis around `~2.5 kHz` before saturation
  - Matching de-emphasis after saturation
  - Adds vocal presence and articulation without harshness

- **Dynamic drive detector**
  - Envelope follower that increases effective drive on louder material
  - Makes Nova Heat react more like analog hardware under real program dynamics

- **Saturation core**
  - Main nonlinear stage based on smooth musical waveshaping
  - Controlled by `drive`, `heat`, `character_mode`, and signal-dependent dynamic push
  - Should feel warm first, musical second, aggressive only when pushed

- **Harmonic smoothing stage**
  - Slightly smooths the top as `heat` increases
  - Prevents brittle or fizzy high-end behavior

- **Stereo smart width stage**
  - Applies mid/side widening to the **wet path only**
  - Keeps the dry signal centered and more mono-compatible
  - Width amount should increase with `heat` and mode intensity

- **Parallel mix stage**
  - Blends dry and processed paths after saturation
  - Critical for vocals, bass, drums, and mix-bus use

- **Auto compensation + output trim**
  - Applies light automatic level trim as drive increases
  - Final `output_gain` stage provides level matching

- **Output meter stage**
  - Displays output level / saturation activity in a simple, premium way

- **Preset recall stage**
  - Recalls fully voiced combinations of `Mode`, `Drive`, `Heat`, `Tone`, `Mix`, and `Output`
  - Must feel level-matched and ready to use immediately

- **Magic polish layer**
  - Optional enhancement toggle that biases the processor toward a more finished result
  - Increases dynamic drive, vocal focus, smoothing, wet stereo width, and auto trim without overriding user settings

## Processing Chain

```text
Input → Tone Pre-Shaping → Vocal Harmonic Focus (Pre-Emphasis) → Dynamic Drive → Saturation Stage → Light Top Smoothing → Vocal Harmonic Focus (De-Emphasis) → Stereo Smart Width (Wet Only) → Mix Blend → Output Trim → Meter → Output
```

When `magic_mode` is ON, it should **bias** the highlighted pro-engine stages rather than replace the user’s chosen settings.

This order is important because the `tone` control should influence what enters the saturator, not just the final output EQ.

## Control Mapping

| Parameter | DSP Component | Function | Internal Mapping |
|-----------|---------------|----------|------------------|
| `drive` | Input drive stage | Pushes the signal into the saturator | `0.0–10.0` → `0 dB to +24 dB` |
| `heat` | Saturation core | Controls curve intensity, density, and harmonic richness | `curveAmount = 0.15 + heatNorm * 0.85` |
| `tone` | Tilt EQ | Controls warm/dark ↔ bright/present balance before saturation | `tiltDb = -4.0 + toneNorm * 8.0` |
| `mix` | Parallel mix stage | Blends dry and wet after processing | `mixNorm = mix / 100.0` |
| `output_gain` | Output trim | Final loudness compensation | `-12 dB to +12 dB` |
| `character_mode` | Mode voicing system | Selects the full response profile of the saturator | `Soft / Medium / Hard` |
| `preset` | Preset recall layer | Recalls musical starting points across all key controls | `Vocal / Drums / Bass / Master` |
| `magic_mode` | Magic polish layer | Applies smart polish on top of the current settings | `Off / On` |

## Saturation Math Direction

### Drive
```text
driveNorm = drive / 10.0
driveGainDb = driveNorm * 24.0
driveGainLin = dB_to_linear(driveGainDb)
```

### Tone
```text
toneNorm = tone / 10.0
tiltDb = -4.0 + toneNorm * 8.0
```

### Heat
```text
heatNorm = heat / 10.0
```

### Unified nonlinear curve engine
```text
driven = preShaped * driveGainLin * mode.driveMultiplier

switch (mode)
  Soft:   k = 0.8 + heatNorm * 0.4
  Medium: k = 1.2 + heatNorm * 1.0
  Hard:   k = 1.8 + heatNorm * 1.5
```

### Core nonlinear shape
```text
Soft / Medium:
  saturated = tanh(driven * k)

Hard:
  if (driven > 0.0)
      saturated = tanh(driven * (k + 0.15))
  else
      saturated = tanh(driven * (k - 0.15))
```

This gives Nova Heat one shared foundation with three distinct personalities: gentle tanh, stronger analog tanh, and steeper asymmetric tanh.

### Dynamic drive
```text
env = envelopeFollower(input)
dynamicBoost = env * mode.dynamicSensitivity
driveEffective = drive + dynamicBoost
```

### Vocal harmonic focus
```text
preFocused = preEmphasis(input, 2.5kHz, focusAmount)
postFocused = deEmphasis(smoothed, 2.5kHz, focusAmount)
```

### Stereo smart width and mix
```text
mid = (wetL + wetR) * 0.5
side = (wetL - wetR) * 0.5
sideBoost = 1.0 + (heatNorm * mode.widthAmount)
wetStereo = msToStereo(mid, side * sideBoost)

mixNorm = mix / 100.0
mixed = dry * (1.0 - mixNorm) + wetStereo * mixNorm
```

### Auto trim
```text
autoTrimDb = -(drive * 0.3)
finalGainDb = outputDb + autoTrimDb
finalOutput = mixed * dB_to_linear(finalGainDb)
```

### Magic Mode enhancement layer
```text
if (magicMode == ON)
{
    magicIntensity = (mode == Soft) ? 0.7 : (mode == Medium ? 1.0 : 1.15);

    dynamicBoost    += 0.15 * magicIntensity;
    focusAmount     += 0.10 * magicIntensity;
    topSmoothing    += 0.12 * magicIntensity;
    sideBoost       += 0.08 * magicIntensity;
    autoTrimDb      += -0.5 * magicIntensity;
}
```

This should feel like smart polish, not a separate effect layer.

## Mode Voicing System

### Coefficient Targets

| Mode | Base / Range | `asymmetry` | `transientSoftening` | `topSmoothing` | `driveMultiplier` | `heatMultiplier` | `dynamicSensitivity` | `widthAmount` | `focusStrength` |
|------|-------------:|------------:|---------------------:|---------------:|------------------:|-----------------:|---------------------:|--------------:|----------------:|
| `Soft` | `0.8 + heatNorm * 0.4` | 0.05 | 0.05 | 0.10 | 0.70 | 0.60 | 0.20 | 0.06 | 0.08 |
| `Medium` | `1.2 + heatNorm * 1.0` | 0.10 | 0.15 | 0.20 | 1.00 | 1.00 | 0.40 | 0.10 | 0.14 |
| `Hard` | `1.8 + heatNorm * 1.5` | 0.20 | 0.30 | 0.35 | 1.25 | 1.30 | 0.60 | 0.15 | 0.20 |

### `Soft`
- Invisible warmth / polish
- Prioritize even harmonics and transient preservation
- Add only a little low-mid bloom and high-end smoothing
- Keep saturation usable on vocals and mix bus at low drive

### `Medium`
- Classic analog saturation and the **default** Nova Heat personality
- Use a balanced tanh / soft-clip hybrid with richer harmonic content
- Round transients slightly for glue and add a very small presence lift
- This should be the “works on everything” mode

### `Hard`
- Aggressive character with musical restraint
- Increase odd-harmonic density and mild asymmetry
- Soften transients more noticeably for punch and compression feel
- Darken the top slightly so it never turns into harsh digital distortion

## Final Preset Targets

| Preset | Mode | Drive | Heat | Tone | Mix | Output |
|------|------|------:|-----:|-----:|----:|-------:|
| `VOCAL` | `Medium` | 4.2 | 5.0 | 5.6 | 28 | -0.8 dB |
| `DRUMS` | `Hard` | 5.8 | 6.5 | 4.8 | 35 | -1.5 dB |
| `BASS` | `Medium` | 5.0 | 5.8 | 3.8 | 32 | -1.2 dB |
| `MASTER` | `Soft` | 2.8 | 3.2 | 5.0 | 18 | 0.0 dB |

These presets should recall a complete musical posture, not just a tone snapshot.

## Pro-Level Enhancements
1. **High-frequency smoothing after saturation**
   - `smoothed = lowpass(saturated, cutoff based on mode + heat)`
   - This is especially important in `Hard` mode.
2. **Dynamic saturation response**
   - `kDynamic = k + (abs(input) * smallFactor)`
   - Louder signals saturate a bit harder and feel more alive.
3. **Optional pre-emphasis / de-emphasis path**
   - Lightly lift presence before the saturator, then rebalance after.
   - This can produce richer harmonics without brittle highs.
4. **Preset loudness matching**
   - Use stored output trim plus automatic compensation:
   - `autoTrimDb = -(drive * 0.3)`
   - Preset changes should feel consistent in loudness when auditioned.

## Premium Behavior Rules
1. **Mode must change response, not just gain.** `Soft`, `Medium`, and `Hard` should feel like different hardware.
2. **Dynamic drive must be signal-reactive.** Louder phrases should saturate more than quiet passages.
3. **Magic Mode must enhance, not override.** It should polish the current settings without replacing the user’s knob choices.
4. **Auto output compensation** should increase with `drive` to prevent misleading loudness jumps.
5. **Top smoothing** should increase with `heat`, especially in `Hard`, so the sound stays expensive and never brittle.
6. **Tone and harmonic focus must shape what enters the saturator**, not just the final output stage.
7. **Stereo width must apply only to the wet path** so the center stays solid and mono-safe.
8. **Mix must be post-saturation and post-tone-shape** so the dry/wet blend feels truly parallel.
9. **Low drive must stay usable** in every mode, including `Hard`.

## Complexity Assessment
**Score: 3 / 5**

### Rationale
Nova Heat is a moderate-to-advanced effect:
- simple user-facing control count
- but multiple nonlinear voicings and intelligent gain behavior under the hood
- requires smoothing, level compensation, and musical top-end control
- must remain flattering across vocals, drums, bass, keys, and mix bus work

This is more refined than a generic distortion plugin, but still practical for a phased JUCE implementation.
