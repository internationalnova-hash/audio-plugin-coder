# DSP Architecture Specification

## Core Components

- Input conditioning stage for stereo-safe gain normalization and denormal protection.
- Macro parameter mapper that converts `space`, `air`, `depth`, `width`, and `mix` into reverb-engine targets.
- Mode profile system for `Studio`, `Arena`, `Dream`, and `Vintage` personalities.
- Pre-delay stage for front-to-back placement and vocal separation.
- Shared stereo reverb core using a diffuse algorithm with mode-dependent coefficients.
- Wet tone-shaping stage for brightness control and analog-style softening.
- Stereo width processor applied to the wet field only.
- Wet/dry mixer with output protection and session-state recall.

## Processing Chain

```text
Input
→ Input Conditioning
→ Macro Mapping
→ Pre-Delay
→ Shared Reverb Core
→ Wet Tone Shaping
→ Wet Stereo Width
→ Wet/Dry Mix
→ Output Protection
→ Output
```

## Parameter Mapping

| Parameter | Component | Function | Range |
|-----------|-----------|----------|-------|
| `space` | Macro Mapper -> Reverb Core | Controls room size, tail growth, and reflection density | 0.00 - 1.00 |
| `air` | Macro Mapper -> Tone Shaper | Controls damping, brightness, and modern sheen | 0.00 - 1.00 |
| `depth` | Macro Mapper -> Pre-Delay / Reverb Mix Bias | Controls front-back placement, pre-delay, and tail emphasis | 0.00 - 1.00 |
| `width` | Wet Stereo Width | Controls wet stereo expansion and mid/side emphasis | 0.00 - 1.00 |
| `mix` | Wet/Dry Mixer | Controls final wet/dry balance | 0.00 - 1.00 |
| `nova_mode` | Mode Profile System | Selects Studio, Arena, Dream, or Vintage coefficient profile | 0 - 3 |

## Mode Profiles

### Studio
- Tight, clean ambience for lead vocals.
- Shorter pre-delay and restrained width.
- Lower diffusion and more center focus.

### Arena
- Larger hall profile with broader stereo tail.
- Longer decay target and stronger width multiplier.
- Tuned for modern vocal size without losing articulation.

### Dream
- Highest diffusion and longest tail.
- More bloom and softer transients.
- Intended for ambient and pad-like vocal trails.

### Vintage
- Plate-inspired profile with darker damping and mild saturation.
- Slightly narrower top end and warmer decay.
- Tuned to feel more hardware-like than pristine.

## Complexity Assessment

**Score: 3/5**

**Rationale:**
NovaSpace is an advanced effect rather than a research-grade engine. The DSP remains manageable because it uses one shared reverb architecture with mode-dependent tuning, but the plugin still requires coordinated macro mapping, smooth mode transitions, stereo wet processing, parameter smoothing, state management, and a WebView UI. That pushes it beyond a simple single-pass effect and justifies phased implementation.