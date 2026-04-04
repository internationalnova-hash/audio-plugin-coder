# DSP Architecture Specification

## Core Components

Nova Tone is a **musical analog-style tone EQ** inspired by the broad-stroke behavior of a classic Pultec EQP-1A. The architecture is designed around smooth shaping, forgiving curves, and subtle analog character rather than precise digital correction.

### Main DSP Building Blocks
- **Input / calibration stage**
  - Normalizes level into the analog-style EQ path
  - Prepares the signal for consistent tone shaping

- **Low frequency resonant boost stage**
  - Selectable center frequencies: `20`, `30`, `60`, `100 Hz`
  - Broad low-end lift with rounded musical emphasis

- **Low frequency attenuation stage**
  - Uses the same selected low frequency options
  - Runs independently from the boost stage so boost and attenuation can happen at once
  - Creates the classic Pultec-style low-end contour

- **High boost stage**
  - Selectable frequency: `3k`, `4k`, `5k`, `8k`, `10k`, `12k`, `16k Hz`
  - Variable bandwidth from wide to narrow
  - Intended for presence, gloss, and air rather than narrow surgical peaks

- **High attenuation stage**
  - Selectable attenuation frequency: `5k`, `10k`, `20k Hz`
  - Soft high-frequency attenuation for smoothing edge or brightness

- **Analog finish / saturation stage**
  - Very subtle non-linear softening to reduce harsh digital feel
  - Should remain restrained and premium, never obvious distortion

- **Output gain stage**
  - Final trim from `-10 dB` to `+10 dB`
  - Supports level matching after tone shaping

- **Preset macro system**
  - `Vocal`, `Bass`, `Air`, `Master`
  - Updates all relevant parameters to musical starting points instantly

---

## Processing Chain

```text
Input
  → Input Calibration
  → Low Boost Filter
  → Low Attenuation Filter
  → High Boost Filter
  → High Attenuation Filter
  → Subtle Analog Saturation / Softening
  → Output Gain
  → Output
```

### Conceptual Behavior
- The **low boost** and **low attenuation** stages are intentionally separate so the user can shape low-end weight and cleanup at the same time.
- The **high boost** should feel silky and broad, especially at higher frequencies.
- The **high attenuation** should behave like a smooth top taming control, not a harsh low-pass.

---

## Parameter Mapping

| Parameter | DSP Component | Function | Range |
|-----------|---------------|----------|-------|
| `low_freq` | Low boost + low attenuation filters | Selects shared low contour frequency | 20 / 30 / 60 / 100 Hz |
| `low_boost` | Low boost filter | Adds rounded low-end energy | 0.0 - 10.0 |
| `low_attenuation` | Low attenuation filter | Reduces selected low area smoothly | 0.0 - 10.0 |
| `high_boost_freq` | High boost filter | Selects presence / air target | 3k - 16k Hz |
| `high_boost` | High boost filter | Adds musical presence or sheen | 0.0 - 10.0 |
| `bandwidth` | High boost filter Q mapping | Wide-to-narrow emphasis control | 1.0 - 10.0 |
| `high_attenuation_freq` | High attenuation filter | Selects the top-end dip region | 5k / 10k / 20k Hz |
| `high_attenuation` | High attenuation filter | Softens harshness or excess brightness | 0.0 - 10.0 |
| `output_gain` | Output stage | Final trim and gain matching | -10.0 to +10.0 dB |
| `mode_preset` | Preset system | Applies full musical parameter sets | Neutral / Vocal / Bass / Air / Master |

---

## Preset Intent Mapping

| Mode | Sonic Purpose | Intended Result |
|------|---------------|-----------------|
| `Vocal` | Presence + polish | Forward vocal tone with expensive top-end and controlled body |
| `Bass` | Weight + roundness | Fuller fundamentals with restrained upper bite |
| `Air` | Gloss + openness | High-end lift that stays soft and flattering |
| `Master` | Broad sweetening | Gentle overall tone enhancement for mix bus polish |

---

## Complexity Assessment

**Score: 3 / 5 — Advanced**

### Rationale
Nova Tone is more advanced than a basic EQ because it combines:
- multiple frequency-selectable analog-style filter sections
- simultaneous low boost / low attenuation behavior
- preset-mode macro mapping
- output gain staging
- analog-style smoothing / saturation polish
- a premium hardware-inspired visual interface

It is not a research-level processor, but it does require careful tuning to feel musical and premium rather than clinical.

---

## Recommended UI Framework

**Decision: `webview`**

### Why
- best match for a premium boutique hardware skin in the same family as `Space By Nova`
- ideal for warm metal textures, engraved labeling, gold knobs, and polished preset buttons
- easier to create a refined analog gear layout with subtle gradients and precise visual control
