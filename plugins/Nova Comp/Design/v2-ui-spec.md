# Nova Comp — UI Specification v2

## Overview
**Internal project name:** `Nova Comp`  
**Display branding:** `NOVA COMP`  
**Subtitle:** `Optical Dynamics Processor`

This version locks the final **3-knob centered triangle layout** so Nova Comp feels visually distinct from Nova Tone and Nova Heat while still belonging to the same family.

## Window
- **Recommended size:** `980 x 620 px`
- **Framework:** `webview`
- **Layout style:** centered iconic compressor layout

## Brand Vibe
- minimal
- premium
- confident
- fast
- "I don’t need 10 knobs" energy

## Top Bar
### Left
- Main title: `NOVA COMP`
- Subtitle: `Optical Dynamics Processor`

### Right
- Branding line: `by International Nova`
- understated, elegant, and consistent with the rest of the Nova suite

## Main Layout — Centered Triangle
The core layout should be centered and highly recognizable.

```text
        COMPRESSION

TONE SHAPING      OUTPUT
```

This immediately communicates:
- top = control
- bottom-left = tonal shaping
- bottom-right = final output / polish

## Hero Control — `COMPRESSION`
**Position:** top center  
**Style:** largest knob in the plugin  
**Label:** `COMPRESSION`  
**Value:** `0.0`

### Visual priority
- approximately `20–25%` larger than the other knobs
- thicker ring and slightly brighter accent than the others
- subtle feedback arc around the knob to imply gain reduction behavior

### Supporting controls below
- `SMOOTH`
- `PUNCH`
- `LIMIT`

These should appear as a compact button row under the hero knob.

## Secondary Control — `TONE SHAPING`
**Position:** bottom-left  
**Label:** `TONE SHAPING`  
**Value:** `0.0`

- medium-sized knob
- same Nova family metal language
- reads as the “make it sit in the mix” control

## Secondary Control — `OUTPUT`
**Position:** bottom-right  
**Label:** `OUTPUT`  
**Value:** `+0.0 dB`

- medium-sized knob
- paired visually with the vertical meter and `MAGIC` button
- should feel like the finishing stage

## Metering Area
**Location:** right side, near the `OUTPUT` area

### Meter behavior
- vertical meter
- slightly taller than Nova Heat’s meter
- slower movement for optical / analog feel
- subtle gold glow only

### Meter toggle
- `GR`
- `OUT`

Small switch above or below the meter.

## Magic Button
**Location:** bottom-right of the output area

```text
[ OUTPUT KNOB ]
[ METER ]
[ MAGIC ]
```

- compact premium button
- same family language as Nova Heat, but calmer and more refined
- should feel like “instant polish”

## Bottom Preset Bar
Buttons:
- `VOCAL`
- `BASS`
- `DRUMS`
- `MASTER`

The preset bar remains secondary to the centered main control triangle.

## Visual Hierarchy
### Primary
- `COMPRESSION` hero knob

### Secondary
- `TONE SHAPING` knob
- `OUTPUT` knob
- meter

### Tertiary
- mode buttons
- `MAGIC`
- preset buttons

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

## Signature Differences vs Other Nova Plugins
- `Nova Tone` = panel-based shaping
- `Nova Heat` = panel-based energy
- `Nova Comp` = **centered triangle control layout**

This creates consistency without sameness.

## UX Flow
The user should naturally do this:
1. Turn `COMPRESSION`
2. Adjust `TONE SHAPING`
3. Match with `OUTPUT`
4. Optionally tap `MAGIC`

The impression should be:
> **I just turn one knob and it works.**