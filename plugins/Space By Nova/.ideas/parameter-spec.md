# Space By Nova — Parameter Spec

## Design Philosophy
Use a **macro-first** control system so producers can shape space quickly without needing traditional reverb jargon. Advanced parameters remain available in a hidden panel if needed.

## Main Controls

| ID | Name | Type | Range | Default | Unit | Purpose |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `space` | Space | Float | 0.0 - 10.0 | 5.0 | macro | Controls room size, decay feel, and diffusion spread |
| `air` | Air | Float | 0.0 - 10.0 | 5.0 | macro | Controls shimmer, brightness, and high-frequency openness |
| `depth` | Depth | Float | 0.0 - 10.0 | 4.5 | macro | Controls perceived distance, tail presence, and linked pre-delay behavior |
| `mix` | Mix | Float | 0 - 100 | 24 | % | Wet/dry blend |
| `width` | Width | Float | 0.0 - 10.0 | 7.0 | macro | Controls stereo spread and mid/side feel |
| `nova_mode` | Nova Mode | Choice | `Studio`, `Arena`, `Dream`, `Vintage` | `Studio` | enum | Instantly recalls signature sound personalities |

## Advanced / Hidden Panel

| ID | Name | Type | Range | Default | Unit | Purpose |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `pre_delay_ms` | Pre-Delay | Float | 0 - 150 | 28 | ms | Keeps lead vocals clear before the tail blooms |
| `decay` | Decay | Float | 0.3 - 8.0 | 2.2 | s | Direct control over tail length |
| `damping` | Damping | Float | 0.0 - 1.0 | 0.45 | norm | Softens high-end and controls warmth |
| `early_reflections` | Early Reflections | Float | 0.0 - 1.0 | 0.35 | norm | Controls density and near-room presence |

## Macro Mapping Notes
- **Space** = room size + decay impression + diffusion spread
- **Depth** = tail push + pre-delay feel + wet presence
- **Air** = high-frequency damping inverse + shimmer character
- **Width** = stereo spread + mid/side emphasis
- **Mix** = standard wet/dry blend

## Final DSP Mapping Reference

### `space` — Size + Decay
**User range:** `0.0 - 10.0`

Maps to:
- `decay time (RT60)`: `0.35s -> 7.0s`
- `room size`: `0.15 -> 1.00`
- `diffusion`: `0.45 -> 0.82`

Suggested formulas:
- `decay = 0.35 + (space / 10.0) * 6.65`
- `roomSize = 0.15 + (space / 10.0) * 0.85`
- `diffusion = 0.45 + (space / 10.0) * 0.37`

### `air` — Brightness + Tail Openness
**User range:** `0.0 - 10.0`

Maps to:
- `reverb high cut`: `4.5 kHz -> 18 kHz`
- `presence tilt`: `-1.5 dB -> +2.0 dB`

Suggested formulas:
- `highCutHz = 4500 + (air / 10.0) * 13500`
- `presenceTiltDb = -1.5 + (air / 10.0) * 3.5`

### `depth` — Distance + Pre-Delay
**User range:** `0.0 - 10.0`

Maps to:
- `pre-delay`: `0 ms -> 120 ms`
- `early reflections level`: `1.00 -> 0.55`
- `attack softness`: `0.0 -> 0.2`

Suggested formulas:
- `preDelayMs = (depth / 10.0) * 120`
- `earlyReflectionLevel = 1.0 - (depth / 10.0) * 0.45`
- `attackSoftness = (depth / 10.0) * 0.2`

### `mix` — Wet / Dry Balance
**User range:** `0 - 100`

Maps to:
- `wet = mix / 100.0`
- `dry = 1.0 - wet`

### `width` — Wet Stereo Spread Only
**User range:** `0.0 - 10.0`

Maps to:
- `side gain`: `0 dB -> +4.5 dB`
- `decorrelation delay`: `0 ms -> 12 ms`

Suggested formulas:
- `widthNorm = width / 10.0`
- `sideGainDb = widthNorm * 4.5`
- `decorrelationMs = widthNorm * 12.0`

**Important:** `width` should affect the **wet signal only**, keeping the dry vocal centered.

## Premium Interaction Rules
- As `space` increases, apply **slightly more damping** so large reverbs stay smooth and expensive.
- As `depth` increases, **reduce early reflections** slightly for better vocal separation.
- As `width` increases, **do not widen the dry signal**.
- As `mix` rises, use a **small wet gain trim** if needed to keep output level stable.

## Nova Mode Presets (Secret Sauce)
When a **Nova Mode** button is clicked, the main knobs should move to the target values below.

| Mode | Space | Air | Depth | Mix | Width | Character |
| :--- | ---: | ---: | ---: | ---: | ---: | :--- |
| `Studio` | 2.2 | 3.0 | 2.5 | 18% | 4.5 | Tight, clean, vocal-forward |
| `Arena` | 6.5 | 5.8 | 6.8 | 32% | 8.5 | Big, lush, wide, emotional |
| `Dream` | 8.8 | 7.2 | 8.5 | 45% | 9.5 | Ambient, floaty, cinematic |
| `Vintage` | 4.5 | 2.2 | 5.5 | 28% | 5.5 | Warm, plate-like, smooth |

### Premium Transition Behavior
- **Do not hard-snap values** unless explicitly required for testing.
- When a mode is selected, **ramp all affected controls over ~100–300 ms**.
- Use smoothed parameter transitions so the plugin feels polished and avoids abrupt visual or sonic jumps.
- Knob animation and DSP parameter movement should feel synchronized and premium.

## Mode Behavior Targets

### `Studio`
- Short decay: **~0.8–1.2s**
- Tight early reflections
- Slight top-end lift for clarity
- Best for lead vocals, rap vocals, pop hooks

### `Arena`
- Medium-long decay: **~2.5–3.5s**
- Wide stereo spread
- Bright modern tail
- Best for hooks, adlibs, melodic vocals

### `Dream`
- Long decay: **~5–8s**
- Heavy diffusion and washed tail
- Cinematic ambient character
- Best for background vocals, pads, FX, intros/outros

### `Vintage`
- Plate-style decay: **~1.8–2.5s**
- Darker tone with dense reflections
- Warm analog-inspired finish
- Best for R&B, soul, and classic-style mixes

## UX Notes
- Default workflow should feel **fast and musical**, not technical.
- The five visible controls should do most of the creative work.
- Advanced parameters are for fine-tuning only and should not clutter the main UI.
