# DSP Architecture Specification — Space By Nova

## Core Components

### 1. Input Conditioning
- Clean stereo input path
- Optional gentle high-pass conditioning to keep vocal mud out of the reverb feed
- Gain staging to prevent the tank from overloading

### 2. Pre-Delay Stage
- Dedicated pre-delay buffer for **0–150 ms** vocal separation
- Helps preserve intelligibility before the tail blooms
- Primarily influenced by `depth` and optionally fine-tuned by `pre_delay_ms`

### 3. Early Reflection Engine
- Short reflection cluster for near-room presence and articulation
- Used to give `Studio` and `Vintage` modes a defined front edge
- Controlled by `early_reflections`, `space`, and `depth`

### 4. Stereo Reverb Tank
- Dattorro / plate-inspired stereo diffuser and feedback network
- Tuned for **smooth**, **luxurious**, **non-harsh** vocal ambience
- One core algorithm with four macro-tuned personalities rather than four separate engines

### 5. Damping / Air Shaping
- High-frequency damping stage inside or after the feedback path
- Keeps the tail smooth while allowing `air` to brighten the top end musically
- Avoids brittle or splashy vocal reverb

### 6. Width Processor
- Mid/side stereo spread after the core tank
- Expands the perceived size for modern R&B and Latin vocals
- Should remain mono-safe at conservative settings

### 7. Wet/Dry Output Mixer
- Final blend stage controlled by `mix`
- Designed for direct insert use on vocals and pads

### 8. Macro Mapping + Smoothing Layer
- Maps the five visible macro controls onto the deeper DSP targets
- Applies **100–300 ms** smoothed transitions for premium-feeling `Nova Mode` changes
- Ensures both sonic movement and UI movement feel polished

## Processing Chain

```text
Input
  -> Input Conditioning / Tone Cleanup
  -> Pre-Delay
  -> Early Reflections
  -> Input Diffusers
  -> Stereo Reverb Tank
  -> Damping / Air Shaping
  -> Mid-Side Width Processing
  -> Wet/Dry Mix
  -> Output
```

## Parameter Mapping

| Parameter | Component | Function | Range |
|-----------|-----------|----------|-------|
| `space` | Reverb Tank + Diffusers | Increases room size, decay feel, and diffusion spread | 0.0 - 10.0 |
| `air` | Damping / Air Stage | Opens the top end and increases shimmer / brightness | 0.0 - 10.0 |
| `depth` | Pre-Delay + Wet Presence | Pushes the source deeper into the space and affects bloom timing | 0.0 - 10.0 |
| `mix` | Output Mixer | Controls wet/dry balance | 0 - 100% |
| `width` | M/S Width Processor | Expands or narrows stereo image | 0.0 - 10.0 |
| `nova_mode` | Preset / Macro Layer | Recalls the `Studio`, `Arena`, `Dream`, and `Vintage` voicings | Enum |
| `pre_delay_ms` | Pre-Delay Stage | Fine control of separation before the tail | 0 - 150 ms |
| `decay` | Reverb Tank | Direct tail length control | 0.3 - 8.0 s |
| `damping` | Damping Stage | Controls warmth vs. brightness in the tail | 0.0 - 1.0 |
| `early_reflections` | Reflection Engine | Controls near-room density and articulation | 0.0 - 1.0 |

## Nova Mode Strategy
Space By Nova should use **one premium vocal reverb engine** that is retuned by mode-specific targets.

- **Studio**: short, tight, clean, vocal-forward
- **Arena**: larger, wider, brighter, emotional
- **Dream**: long, soft, ambient, cinematic
- **Vintage**: warmer, darker, plate-like, musical

When a `Nova Mode` is selected, all affected visible controls should glide to their target values over roughly **100–300 ms** instead of snapping instantly.

## Final Control Intent
- **Space** = size + decay
- **Air** = brightness / air
- **Depth** = distance / pre-delay / front-back positioning
- **Mix** = wet/dry amount
- **Width** = stereo spread of the **wet signal only**

## Premium Hidden Behavior
- As `space` increases, slightly increase diffusion and apply a bit more damping so large reverbs stay smooth.
- As `air` increases, the tail should feel clearer and more present without becoming brittle.
- As `depth` increases, increase pre-delay and reduce early reflections for stronger vocal separation.
- As `width` increases, only widen the wet path and keep the dry vocal centered.
- At higher `mix` settings, allow a small wet trim if needed to keep output level stable.

## Complexity Assessment

**Score: 3 / 5**

### Rationale
Space By Nova is more advanced than a simple effect because it combines:
- a stereo modulated reverb architecture,
- macro-to-internal parameter mapping,
- four curated mode personalities,
- smooth preset transitions,
- and premium UI expectations.

However, it remains a **single-purpose vocal reverb** rather than a research-level multi-engine processor, so it fits well at **advanced-but-manageable complexity**.

## Implementation Risks

### High Risk
- Achieving a reverb tail that stays lush without becoming harsh or cloudy
- Keeping vocal clarity intact while increasing `depth` and `space`

### Medium Risk
- Tuning macro curves so the five main controls feel musical across the full range
- Matching UI animation timing to DSP smoothing for `Nova Mode`
- Maintaining mono compatibility at wider `width` settings

### Low Risk
- Wet/dry mixing
- Hidden advanced panel parameter exposure
- Basic preset recall structure
