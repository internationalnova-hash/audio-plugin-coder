# Nova Master — Parameter Specification

## Overview
Nova Master is a mastering-style finalizer built around five main controls and one premium enhancement button.

## Parameters

| ID | Name | Type | Range | Default | Unit | Purpose |
|---|---|---:|---|---:|---|---|
| `tone` | Tone | Float | `0.0 - 10.0` | `5.0` | scalar | Overall tonal tilt from darker/smoother to brighter/present |
| `glue` | Glue | Float | `0.0 - 10.0` | `3.5` | scalar | Controls subtle mix-bus compression feel and density |
| `weight` | Weight | Float | `0.0 - 10.0` | `4.0` | scalar | Adds low-end body and foundation without mud |
| `air` | Air | Float | `0.0 - 10.0` | `5.0` | scalar | Adds top-end openness, sheen, and polish |
| `width` | Width | Float | `0.0 - 10.0` | `4.5` | scalar | Adds stereo polish while keeping the low end centered |
| `output_gain` | Output | Float | `-6.0 - 6.0` | `0.0` | dB | Final level trim / level match |
| `finish_mode` | Finish | Bool | `Off / On` | `Off` | bool | Adds subtle smoothing, harmonic finish, stereo refinement, and balancing |
| `mode_preset` | Mode | Choice | `Clean / Warm / Wide / Loud` | `Clean` | enum | Recalls musical mastering personalities |
| `meter_view` | Meter View | Choice | `LU / OUT` | `OUT` | enum | Changes the meter display mode |

---

## Control Intent

### `tone`
Controls the overall tonal tilt of the full mix.

**Range behavior**
- `0.0` = slightly darker / smoother
- `5.0` = neutral
- `10.0` = slightly brighter / more present

**Suggested internal mapping**
```text
tiltDb = -3.0 + (tone / 10.0) * 6.0
```
- effective range: about `-3 dB` to `+3 dB`
- best pivot: about `1.8 kHz` to `2.2 kHz`

The goal is a broad mastering tilt, not a narrow EQ move.

### `glue`
This should feel like hidden mix-bus compression and density.
It should influence:
- threshold
- ratio
- detector sensitivity
- release behavior

**Suggested threshold mapping**
```text
thresholdDb = -6.0 - (glue / 10.0) * 12.0
```
- `0.0` = about `-6 dB`
- `10.0` = about `-18 dB`

**Suggested ratio mapping**
```text
ratio = 1.5 + (glue / 10.0) * 2.0
```
- `0.0` = about `1.5:1`
- `10.0` = about `3.5:1`

**Timing target**
- attack: about `20–40 ms`
- release: about `100–400 ms`, auto-biased
- knee: soft

The goal is glue and cohesion, not obvious pumping.

### `weight`
Not a simple bass boost.
It should combine:
- broad low-shelf weight
- slight low-mid contouring
- low-end control

**Suggested internal mapping**
```text
lowShelfDb = -1.5 + (weight / 10.0) * 3.5
```
- `0.0` ≈ `-1.5 dB`
- `10.0` ≈ `+2.0 dB`
- main foundation area: about `80 Hz`
- optional gentle tightening around `180–250 Hz`

The goal is bigger foundation without boominess.

### `air`
This should add mastering-style openness, sheen, and polish.

**Suggested internal mapping**
```text
highShelfDb = -1.0 + (air / 10.0) * 4.0
```
- `0.0` ≈ `-1.0 dB`
- `10.0` ≈ `+3.0 dB`
- main shelf area: about `10–14 kHz`

As `Air` rises, the DSP should also add slight top smoothing so the result stays classy and never brittle.

### `width`
Controls stereo width enhancement mainly in the upper mids and highs.
The low end must stay centered.

**Suggested internal mapping**
```text
widthNorm = width / 10.0
sideGainDb = widthNorm * 3.5
decorrelationMs = widthNorm * 6.0
```
- widen mainly above roughly `180–250 Hz`
- keep sub and bass mostly mono / centered

### `finish_mode`
When ON, this should add a subtle premium finishing layer:
- micro dynamic smoothing
- low harmonic finish
- slight stereo polish
- slight top-end smoothing
- small output trim balancing

**Target behavior**
- `extraGlue` ≈ `+8% to +12%`
- `harmonicDrive` ≈ `1.03 to 1.08`
- `sideBoost` ≈ `+4% to +8%`
- `autoTrim` ≈ `-0.3 dB to -0.8 dB`

It should feel like:
> **the mix just became more expensive**

---

## Hidden Premium Rules

1. As `Glue` increases, slightly tighten the low end so more cohesion does not become mud.
2. As `Air` increases, slightly smooth the top so brightness stays premium.
3. As `Width` increases, keep the center anchored and the low end stable.
4. `Finish` should never create a big loudness jump — it must stay level-managed.

---

## Mode Defaults

| Mode | Tone | Glue | Weight | Air | Width | Finish |
|---|---:|---:|---:|---:|---:|---|
| `Clean` | `5.0` | `3.5` | `4.0` | `5.0` | `4.5` | `Off` |
| `Warm` | `4.2` | `4.8` | `6.0` | `4.0` | `4.5` | `On` |
| `Wide` | `5.5` | `3.8` | `4.0` | `6.2` | `6.8` | `On` |
| `Loud` | `5.2` | `5.5` | `5.2` | `5.8` | `5.0` | `On` |

---

## UX Rules
- The plugin must stay simple and confidence-inspiring
- No dense mastering-suite layout in V1
- The five main knobs must remain the core workflow
- `Finish` should enhance the result without overriding the front-panel settings
- Mode buttons should recall useful musical personalities quickly
