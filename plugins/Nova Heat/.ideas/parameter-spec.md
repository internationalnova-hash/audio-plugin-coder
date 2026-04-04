# Nova Heat — Parameter Specification

## Core Parameters

| ID | Name | Type | Range / Options | Default | Unit |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `drive` | Drive | Float | 0.0 - 10.0 | 2.0 | amount |
| `tone` | Tone | Float | 0.0 - 10.0 | 5.0 | tilt |
| `heat` | Heat | Float | 0.0 - 10.0 | 2.5 | character |
| `mix` | Mix | Float | 0.0 - 100.0 | 18.0 | % |
| `output_gain` | Output | Float | -12.0 - 12.0 | 0.0 | dB |
| `character_mode` | Mode | Choice | Soft, Medium, Hard | Medium | mode |
| `magic_mode` | Magic | Bool | Off / On | Off | toggle |

## Internal Mapping Notes

### `drive`
- User range: `0.0 → 10.0`
- Internal mapping: `0 dB → +24 dB` of gain before saturation
- Formula:
  - `driveGainDb = (drive / 10.0) * 24.0`
  - `driveGainLin = dB_to_linear(driveGainDb)`

### `heat`
- User range: `0.0 → 10.0`
- Controls **curve reshaping**, harmonic density, and light compression feel
- `heat` must **not** behave like simple gain; it should morph the nonlinear curve itself
- Formula concept:
  - `heatNorm = heat / 10.0`
  - `k = baseCurve + (heatNorm * curveRange)`
- Mode-specific mapping:
  - `Soft`: `k = 0.8 + heatNorm * 0.4`
  - `Medium`: `k = 1.2 + heatNorm * 1.0`
  - `Hard`: `k = 1.8 + heatNorm * 1.5`
- As `heat` rises, the processor should also apply more top-end smoothing to stay musical and expensive-sounding.

### `tone`
- User range: `0.0 → 10.0`
- Operates as a tilt EQ around roughly `1.8–2.5 kHz`
- Formula concept:
  - `toneNorm = tone / 10.0`
  - `tiltDb = -4.0 + toneNorm * 8.0`
- Feel:
  - `0.0` = darker / warmer
  - `5.0` = neutral
  - `10.0` = brighter / more present

### `mix`
- Must be a true post-saturation parallel blend
- Formula:
  - `mixNorm = mix / 100.0`
  - `output = dry * (1.0 - mixNorm) + wet * mixNorm`

### `output_gain`
- Final trim after saturation and blend
- Range: `-12.0 dB → +12.0 dB`
- Used for fair loudness-matched comparison

## Mode DSP Final Spec

### Core Rule
`character_mode` must change the **response and personality** of the saturator, not just the gain. Each mode should feel like different hardware.

| Mode | Visual Reference Curve | Harmonics | Transients | Tone Response | Internal Tweaks |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `Soft` | `y = tanh(0.8x)` | even harmonics high, odd low | `transientSoftening = 0.05` | low top smoothing, low low-mid bloom | `driveMultiplier = 0.7`, `heatMultiplier = 0.6` |
| `Medium` | `y = tanh(1.5x)` feel, implemented with `k = 1.2 + heatNorm * 1.0` | even medium-high, odd medium | `transientSoftening = 0.15` | medium top smoothing, medium bloom, small presence lift | `driveMultiplier = 1.0`, `heatMultiplier = 1.0` |
| `Hard` | `y = tanh(2.5x)` feel with asymmetry | even medium, odd high | `transientSoftening = 0.3` | darker top, high smoothing, thicker low mids | `driveMultiplier = 1.25`, `heatMultiplier = 1.3` |

### `Soft`
- **Sound:** invisible warmth, polish, expensive-record feel
- **Behavior:** very soft tanh curve, preserved transients, slight low-mid warmth, smooth highs
- **Use cases:** clean pop vocals, R&B, master bus, acoustic instruments
- **UX translation:** `clean, polished, safe`

### `Medium`
- **Sound:** warm, present, musical, obvious but controlled
- **Behavior:** balanced tanh / soft-clip hybrid with slight transient rounding and a subtle presence lift
- **Use cases:** lead vocals, drums, guitars, keys
- **UX translation:** `warm, analog, musical`

### `Hard`
- **Sound:** punchy, aggressive, character-rich, more compression feel
- **Behavior:** stronger soft-clip hybrid with slight asymmetry, darker top, thicker low mids, and fast-rising density
- **Use cases:** drums, 808s, bass, adlibs, creative vocal effects
- **UX translation:** `punchy, aggressive, character`

### Unified Curve Engine
```text
driveSample = input * driveGainLin * driveMultiplier

switch (mode)
  Soft:   k = 0.8 + heatNorm * 0.4
  Medium: k = 1.2 + heatNorm * 1.0
  Hard:   k = 1.8 + heatNorm * 1.5

output = tanh(driveSample * k)
```

### Asymmetry Sauce
For `Hard`, use slight asymmetry so the sound feels more analog and less digital:

```text
if (x > 0)
  y = tanh(2.5 * x)
else
  y = tanh(2.0 * x)
```

A more continuous option is:

```text
y = tanh(a * x + b * x^3)
```

### Heat Interaction
- `Soft + Heat` = gentle richness
- `Medium + Heat` = harmonic fullness
- `Hard + Heat` = saturation explosion

### Professional Guardrails
1. Protect the high end, especially in `Hard` mode.
2. Keep low-drive settings musical in every mode.
3. Use more smoothing as density rises so Nova Heat never feels harsh or cheap.
4. `Heat` must reshape the curve, not just add gain.
5. `Hard` should stay musical, never cheap digital distortion.

## Final Preset System
Each preset must lock these values together:
- `Mode`
- `Drive`
- `Heat`
- `Tone`
- `Mix`
- `Output`

All presets should be **gain-balanced and musical out of the box** so beginners get an instant result and pros get a strong starting point.

| Preset | Mode | Drive | Heat | Tone | Mix | Output | Intent |
| :--- | :--- | ---: | ---: | ---: | ---: | ---: | :--- |
| `VOCAL` | `Medium` | 4.2 | 5.0 | 5.6 | 28 | -0.8 dB | polished, warm, upfront vocal |
| `DRUMS` | `Hard` | 5.8 | 6.5 | 4.8 | 35 | -1.5 dB | punch, glue, and energy |
| `BASS` | `Medium` | 5.0 | 5.8 | 3.8 | 32 | -1.2 dB | thick, controlled low-end |
| `MASTER` | `Soft` | 2.8 | 3.2 | 5.0 | 18 | 0.0 dB | subtle glue and enhancement |

### Preset Notes
- `VOCAL` should feel radio-ready with warmth, slight presence, and zero harshness.
- `DRUMS` should hit harder immediately with thicker transient shaping and strong parallel attitude.
- `BASS` should add translation and harmonic weight without mud.
- `MASTER` should feel better, not louder.

### Preset Loudness Matching
Use the preset output trims above and also apply light automatic trim:

```text
autoTrimDb = -(drive * 0.3)
```

This helps keep preset auditioning fair and professional.

## Hidden Pro Engine Systems
These behaviors are mostly **invisible to the user** but should be clearly felt in the sound.

### 1. Dynamic Drive
Make saturation react to signal level so louder phrases and hits get a little more push:

```text
env = envelopeFollower(input)
dynamicBoost = env * modeSensitivity
driveEffective = drive + dynamicBoost
```

**Mode sensitivity:**
- `Soft` = `0.2`
- `Medium` = `0.4`
- `Hard` = `0.6`

Result:
- verses stay cleaner
- choruses bloom more naturally
- drums feel more alive and animated

### 2. Stereo Smart Width
Widen **only the wet path** so the center stays clean and mono-safe:

```text
mid = (L + R) * 0.5
side = (L - R) * 0.5
sideBoost = 1.0 + (heatNorm * widthAmount)
wetMid = mid
wetSide = side * sideBoost
```

**Mode width amount:**
- `Soft` = subtle
- `Medium` = moderate
- `Hard` = wide

### 3. Vocal Harmonic Focus
Add presence without harshness using a light pre/de-emphasis stage around `~2.5 kHz`:
- Pre-emphasis before saturation: `+1 to +2 dB`
- De-emphasis after saturation: reduce the same region slightly

**Mode focus strength:**
- `Soft` = light
- `Medium` = medium
- `Hard` = strong

This should help vocals sit forward in the mix while keeping sibilance smooth.

### 4. Hidden Transient Shaping
Use a light transient control based on mode:

```text
if (mode == HARD)
  transient *= 0.8

if (mode == SOFT)
  transient *= 0.95
```

Result:
- `Hard` = punchy and controlled
- `Soft` = natural and open

## Magic Mode
`magic_mode` is a **special polish toggle** that sits on top of the current settings. It must not replace the main knobs — it should bias Nova Heat toward a more finished, expensive result.

### Default State
- `Magic` should default to **OFF**
- Turning it on should feel like an upgrade moment for the sound

### Hidden boosts when ON
```text
if (magicMode == ON)
  dynamicDrive += 0.15
  vocalFocus += 0.10
  topSmoothing += 0.12
  wetStereoBoost += 0.08
  autoTrim += -0.5 dB
```

### Mode scaling
- `Soft` = `0.7x`
- `Medium` = `1.0x`
- `Hard` = `1.15x`

### Preset-aware behavior
- `VOCAL + Magic` = strongest vocal focus, moderate dynamic drive, medium smoothing, slight stereo lift
- `DRUMS + Magic` = stronger transient control, stronger dynamic drive, less stereo widening, more low-mid bloom
- `BASS + Magic` = almost no stereo widening, more low-mid harmonics, extra smoothing
- `MASTER + Magic` = most subtle behavior with tiny dynamic lift and tiny trim

### User-facing description
> `MAGIC adds smart polish, presence, and control without changing your core settings.`

## Default State
The default setting should feel flattering immediately, not harsh:
- Mode: `Medium`
- Drive: `2.0`
- Tone: `5.0`
- Heat: `2.5`
- Mix: `18%`
- Output: `0.0 dB`
