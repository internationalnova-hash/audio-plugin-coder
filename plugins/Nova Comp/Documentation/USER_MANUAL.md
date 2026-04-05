# Nova Level User Manual

**Current V1 Build**  
**Optical Dynamics Processor**  
**Internal project/build name:** `Nova Comp`

---

## Introduction

`Nova Level` is a modern optical-style compressor designed to feel simple, premium, and fast to trust.

The idea is:
> **turn one main knob and the source sounds more controlled, polished, and expensive.**

---

## Quick Start

1. Start in `SMOOTH`
2. Raise `COMPRESSION` until the source sits where you want it
3. Adjust `TONE SHAPING` for brightness vs warmth
4. Use `OUTPUT` to level-match
5. Turn on `MAGIC` if you want extra finish and control
6. Use a preset button for a faster starting point if needed

---

## Main Controls

| Control | What you hear | What it changes in the DSP |
|---|---|---|
| `COMPRESSION` | More leveling, more control, more density | This is the main macro. It lowers the effective **threshold** and increases how much compression can happen inside the selected mode |
| `TONE SHAPING` | Left = warmer/smoother, Right = brighter/more present | Applies a **tilt EQ** and also changes how much the compressor detector reacts to upper content |
| `OUTPUT` | Matches loudness after compression | Adds final output gain from `-12 dB` to `+12 dB` |
| `GR / OUT` | Meter display choice | Changes the meter view only. It does **not** change the audio |

### Automation / host parameter
`MIX` is also implemented in the backend for dry/wet blending and preset behavior. In the current V1 front-panel concept, the core user workflow stays focused on `COMPRESSION`, `TONE SHAPING`, and `OUTPUT`.

---

## Mode Buttons

`SMOOTH`, `PUNCH`, and `LIMIT` are fully mapped DSP modes.

| Mode | Feel | What changes in the DSP |
|---|---|---|
| `SMOOTH` | Gentle, forgiving, leveling | Lower ratio, slower attack, longer program-dependent release, wider knee |
| `PUNCH` | Tighter, more assertive | Medium ratio, faster attack, shorter release, firmer knee |
| `LIMIT` | Strongest control | Higher ratio, fastest attack, tighter knee, more limiter-like behavior |

### Important note
These buttons change the **backend compressor behavior** directly. They do **not** need to move the main knobs to have an effect.

---

## MAGIC Button

`MAGIC` is a real DSP enhancement layer, not just a visual switch.

When `MAGIC` is ON, the processor adds:
- a little more top smoothing
- smarter detector behavior
- slightly more gain reduction when useful
- smoother release behavior
- a touch more harmonic warmth and saturation
- small stereo polish / width enhancement
- extra auto-trim balancing

### Important note
`MAGIC` changes the internal processing, but it does **not** trigger a hidden preset by itself and it does **not** automatically move the visible knobs.

---

## Preset Buttons

The preset buttons are musical starting points:

- `VOCAL`
- `DRUMS`
- `BASS`
- `MASTER`

These **do** recall full settings and may turn `MAGIC` on or off depending on the source.

---

## What Happens Under the Hood

`Nova Level` combines several behaviors at once:

1. **Threshold / ratio shaping** through the main `COMPRESSION` control
2. **Program-dependent release** so it recovers musically
3. **Tone-aware detection** so brightness can influence how the compressor reacts
4. **Warmth and saturation scaling** based on gain reduction
5. **Auto trim** to keep levels more balanced after compression

That is why the plugin feels more musical than a plain utility compressor.

---

## Practical Tips

### Vocals
- Start with `SMOOTH`
- Use moderate `COMPRESSION`
- Turn on `MAGIC` for polish and consistency

### Drums
- Try `PUNCH`
- Use more compression than on vocals
- Keep output matched so louder does not fool you

### Bass
- Use `SMOOTH` for levelling or `PUNCH` for definition
- Pull `TONE SHAPING` slightly left if the result gets too bright

### Master / mix bus
- Use low `COMPRESSION`
- Start from the `MASTER` preset
- Keep moves subtle

---

## Summary

`Nova Level` is designed around a simple idea:
- `COMPRESSION` = main control and movement
- `TONE SHAPING` = color of the control action
- `OUTPUT` = level match
- `SMOOTH / PUNCH / LIMIT` = real backend compressor personalities
- `MAGIC` = finishing polish layer

