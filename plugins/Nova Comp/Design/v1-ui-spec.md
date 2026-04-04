# Nova Comp — UI Specification v1

## Overview
**Internal project name:** `Nova Comp`  
**Display branding:** `NOVA COMP`  
**Subtitle:** `Optical Dynamics Processor`

This design locks the product presentation as **Nova Comp**.

## Window
- **Recommended size:** `980 x 620 px`
- **Framework:** `webview`
- **Layout style:** premium one-page hardware compressor panel

## Top Bar
### Left
- Main title: `NOVA COMP`
- Subtitle: `Optical Dynamics Processor`

### Right
- Branding line: `by International Nova`
- Tone should feel refined, premium, and consistent with the rest of the Nova family

## Main Layout
Use a three-panel center layout with a bottom preset strip.

### Left Panel — `COMPRESSION`
Main purpose: make the amount of control obvious and immediate.

**Controls:**
- `LEVEL` — **largest hero knob** on the left
- `SMOOTH / PUNCH / LIMIT` mode buttons below the section title
- readout below the knob: `0.0`

**UX intent:**
Turn `LEVEL` first, then choose the mode flavor.

### Center Panel — `TONE SHAPING`
Main purpose: let the user shape how the compression feels in the mix.

**Controls:**
- `TONE` knob — medium-sized
- `MIX` horizontal slider — with pill readout like Nova Heat

**UX intent:**
- `TONE` changes warmth vs presence
- `MIX` blends in parallel compression quickly

### Right Panel — `OUTPUT`
Main purpose: final level matching and confidence monitoring.

**Controls:**
- `MAKEUP GAIN` knob
- vertical analog-style meter
- toggle buttons:
  - `GR` for gain reduction
  - `OUT` for output level
- `MAGIC` button in the bottom-right area

**UX intent:**
The right side should feel like the finishing section: verify reduction, match gain, add polish.

## Bottom Preset Bar
Buttons:
- `VOCAL`
- `BASS`
- `DRUMS`
- `MASTER`

Each should recall a ready-to-mix starting point and remain visually secondary to the main compressor controls.

## Visual Hierarchy
### Primary
- `LEVEL` knob
- `MAKEUP GAIN` knob
- vertical meter

### Secondary
- `TONE`
- `MIX`
- mode buttons

### Tertiary
- `MAGIC`
- preset buttons
- meter mode toggle

## Control Inventory
| Control | Type | Position | Range | Default |
|--------|------|----------|-------|---------|
| Level | Large knob | Left panel center | `0.0–10.0` | `4.0` |
| Mode | 3-button group | Left panel top | `Smooth / Punch / Limit` | `Smooth` |
| Tone | Medium knob | Center panel | `0.0–10.0` | `5.0` |
| Mix | Horizontal slider | Center panel lower | `0–100%` | `70%` |
| Gain | Medium knob | Right panel left | `-12 to +12 dB` | `0.0 dB` |
| Meter View | 2-button toggle | Right panel top | `GR / OUT` | `GR` |
| Meter | Vertical analog-style meter | Right panel edge | visual | n/a |
| Magic | Premium toggle button | Right panel bottom-right | `Off / On` | `Off` |
| Presets | 4 buttons | Bottom strip | `VOCAL / BASS / DRUMS / MASTER` | none |

## Color Palette
- **Faceplate:** `#EFE1C7`
- **Faceplate shade:** `#E3D2B4`
- **Inner panel:** `#D8C5A0`
- **Dark text:** `#473D33`
- **Soft text:** `#6B6258`
- **Cool gold:** `#BE9440`
- **Deep gold:** `#9E7330`
- **Pale highlight:** `#E8CC86`
- **Cream white:** `#FFF9F1`

## Style Notes
- Must feel **controlled, refined, and expensive**
- Cooler and more neutral than `Nova Heat`
- No copper energy emphasis
- Meter movement should feel smoother and more analog than Heat
- Button glows should be subtle, not flashy

## UX Notes
The plugin should communicate this flow clearly:
1. Turn `LEVEL`
2. Match with `GAIN`
3. Blend with `MIX`
4. Tap `MAGIC` for instant polish

The overall impression should be:
> **I don’t need to think — it just works.**