# UI Specification v1

## Layout
- Window: 820x560 px
- Framework: WebView
- Sections:
  - Header with plugin name and descriptive subtitle
  - Top row with `Space`, `Air`, `Depth`, and `Mix`
  - Center hero area for `Width`
  - Bottom row with `Nova Mode` selector buttons
  - Footer status strip for current mode description

## Grid
- Outer shell centered within a fixed-size panel.
- Four compact rotary controls across the top band.
- One oversized hero rotary control in the center.
- Four equal-width mode buttons across the bottom.

## Controls

| Parameter | Type | Position | Range | Default |
|-----------|------|----------|-------|---------|
| `space` | Rotary knob | Top row, left | 0.00 - 1.00 | 0.45 |
| `air` | Rotary knob | Top row, second | 0.00 - 1.00 | 0.60 |
| `depth` | Rotary knob | Top row, third | 0.00 - 1.00 | 0.40 |
| `mix` | Rotary knob | Top row, right | 0.00 - 1.00 | 0.18 |
| `width` | Hero rotary knob | Center | 0.00 - 1.00 | 0.80 |
| `nova_mode` | Segmented buttons | Bottom row | Studio / Arena / Dream / Vintage | Studio |

## Color Palette
- Background: `#F3EFE6`
- Surface: `#FFF9F1`
- Primary Ink: `#20160D`
- Secondary Ink: `#6C5844`
- Gold Accent: `#B98934`
- Warm Gold Highlight: `#E3C27A`
- Outline: `#D9C9AF`

## Style Notes
- Bright interface intended to stand out in dark DAWs.
- Gold controls should feel like boutique hardware, not a flat software skin.
- Width is visually treated as the signature control.
- Mode selector should read like a premium hardware mode bank rather than generic tabs.