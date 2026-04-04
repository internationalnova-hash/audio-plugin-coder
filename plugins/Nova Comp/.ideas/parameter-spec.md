# Nova Comp — Parameter Spec

## Core Controls

### Main 3-Knob Identity
Nova Comp should feel like a **signature 3-knob compressor** built around:

1. **COMPRESSION** → how much control happens
2. **TONE SHAPING** → how the compressor feels and reacts
3. **OUTPUT** → final level matching

These are the main front-panel ideas the user should understand instantly.

| ID | Name | Type | Range | Default | Unit | Behavior |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `level` | Compression | Float | 0.0 - 10.0 | 4.0 | amt | Main smart compression control. Replaces input, threshold, ratio feel, and part of the timing behavior depending on mode. |
| `tone` | Tone Shaping | Float | 0.0 - 10.0 | 5.0 | tilt | Compression-aware tonal shaping. Changes both tone balance and what frequencies drive the compressor most strongly. |
| `gain` | Output | Float | -12.0 - 12.0 | 0.0 | dB | Makeup gain after compression for loudness matching and preset balancing. |
| `mix` | Mix | Float | 0.0 - 100.0 | 70.0 | % | Supporting modern upgrade for built-in parallel compression. Keep it secondary in the hierarchy. |
| `mode` | Mode | Choice | Smooth / Punch / Limit | Smooth | enum | Compression personality and hidden timing model. |
| `magic_mode` | Magic | Bool | Off / On | Off | bool | Adds automatic leveling, smoothness, and polish: smarter release, slight vocal focus, transient control, tone-aware smoothing, subtle stereo finish, and extra level balancing. |
| `meter_view` | Meter View | Choice | GR / Output | GR | enum | Switches the meter between gain reduction and output level display. |

## Control Mapping Notes

### `level` / `COMPRESSION`
`COMPRESSION` is intentionally the **single smart compressor knob** for v1.

It replaces the traditional combination of:
- input drive into the compressor
- threshold feel
- ratio feel
- peak-reduction amount
- part of the timing feel depending on mode

Suggested internal mapping:
```text
compressionNorm = compression / 10.0
thresholdDb = -10.0 - compressionNorm * 22.0
```

Feel target:
- low values = light compression (`~1–2 dB GR`)
- mid values = musical compression (`~3–6 dB GR`)
- high values = heavy control (`~6–10 dB GR`)

Optional ratio bias by knob:
- `Smooth`: `2.5:1 → 3.5:1`
- `Punch`: `3.5:1 → 4.5:1`
- `Limit`: `6:1 → 8:1`

This keeps the workflow fast and beginner-friendly while still feeling like a real optical leveler.

### `gain`
```text
outputGainLin = dB_to_linear(gainDb)
```

### `mix`
```text
mixNorm = mix / 100.0
final = dry * (1.0 - mixNorm) + wet * mixNorm
```

### `tone` / `TONE SHAPING`
This is **not just EQ**.

It controls:
- tonal balance (`dark ↔ bright`)
- what frequencies trigger compression more strongly
- post-compression polish and vocal clarity

```text
toneNorm = tone / 10.0
tiltDb = -3.0 + toneNorm * 6.0
```

Behavior target:
- low = warmer, smoother, less harsh
- mid = neutral
- high = brighter, more present, more vocal clarity

It should also influence compressor sensitivity:
- higher `tone` = reacts more to vocals / mids
- lower `tone` = reacts more to body / lows

## Mode Behavior Targets

### `Smooth`
- ratio: ~`3:1`
- attack: `10 ms`
- release: program-dependent, roughly `80 ms → 1200 ms`
- knee: `8.0 dB`
- classic optical leveling feel

### `Punch`
- ratio: ~`4:1`
- attack: `5 ms`
- release: `60 ms → 500 ms`
- knee: `5.0 dB`
- tighter, more forward, more modern

### `Limit`
- ratio: ~`8:1 → 10:1`
- attack: `2 ms`
- release: `80 ms → 700 ms`
- knee: `2.5 dB`
- strong peak control and firmer leveling

## Final Presets

| Preset | Mode | Compression | Tone Shaping | Output | Mix | Magic |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `VOCAL` | Smooth | 4.2 | 5.8 | +1.8 dB | 72% | ON |
| `DRUMS` | Punch | 5.8 | 5.0 | +1.2 dB | 58% | OFF |
| `BASS` | Smooth | 5.3 | 4.0 | +2.3 dB | 80% | OFF |
| `MASTER` | Smooth | 2.6 | 5.0 | 0.0 dB | 45% | ON |

## Magic Mode Intent
`Magic` in Nova Comp is **not** about extra color like Nova Heat.

It is about:
- automatic leveling
- smoothness
- consistency
- polish
- control

When ON, it should feel like a pro engineer gently riding the fader and compression.

Suggested hidden behavior:
- smarter program-dependent release
- slight automatic vocal leveling bias
- subtle transient smoothing for peaks
- tone-aware smoothing to avoid harsh highs and muddy lows
- gentle auto output trim based on gain reduction
- optional tiny stereo widening on the wet path only

Mode scaling:
- `Smooth = 1.0x`
- `Punch = 0.8x`
- `Limit = 0.6x`

## Hidden DSP Intent
- program-dependent release is mandatory
- heavier gain reduction should release more slowly
- more compression should produce slightly more warmth
- `tone` should interact with the compression stage, not just act as a final EQ
- `Magic` should enhance polish without overriding the front-panel settings
- auto trim should stop louder settings from always seeming better
- overall workflow must remain fast, musical, and simple