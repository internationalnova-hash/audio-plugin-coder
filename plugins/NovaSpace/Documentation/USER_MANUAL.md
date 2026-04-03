# Nova Space — User Manual

**Version 1.0.0**

---

## Overview

Nova Space is a vocal-first space reverb that turns a handful of musical controls into polished, expensive-sounding depth. It is designed for producers who want a modern vocal reverb that dials in quickly — bright white interface, premium gold controls, and a reverb character that moves from tight studio gloss to wide cinematic atmosphere.

**Supported Formats:** VST3, AU (macOS), Standalone  
**Compatible DAWs:** Ableton Live, Logic Pro, FL Studio, Reaper, Cubase, Studio One, and all VST3/AU hosts

---

## Installation

### Windows
1. Run `Nova Space-Setup.exe`
2. Follow the installer prompts
3. VST3 will be installed to `C:\Program Files\Common Files\VST3\`
4. Rescan plugins in your DAW

### macOS
1. Open `Nova Space-macOS.pkg` and follow the prompts
2. VST3 installs to `/Library/Audio/Plug-Ins/VST3/`
3. AU installs to `/Library/Audio/Plug-Ins/Components/`
4. Rescan plugins in your DAW

### Linux
1. Copy `Nova Space.vst3` to `~/.vst3/` or `/usr/lib/vst3/`
2. Copy `Nova Space.lv2` to `~/.lv2/` or `/usr/lib/lv2/`
3. Rescan plugins in your DAW

---

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Space** | 0–100% | 45% | Room size, decay growth, and reflection spread. Low = compact studio room; High = cinematic environment. |
| **Air** | 0–100% | 60% | Reverb tail brightness. Low = darker, warmer ambience; High = brighter modern vocal sheen. |
| **Depth** | 0–100% | 40% | Front-to-back placement via pre-delay and tail presence. Low = source stays forward; High = pushes deeper into the field. |
| **Width** | 0–100% | 80% | Stereo spread. Low = focused mono ambience; High = wide cinematic image. |
| **Mix** | 0–100% | 18% | Wet/dry blend. Works on insert or in parallel. |
| **Nova Mode** | Studio / Arena / Dream / Vintage | Studio | Core reverb personality (see below). |

---

## Nova Mode

Nova Mode switches the algorithm personality without requiring you to understand reverb topology.

| Mode | Character |
|------|-----------|
| **Studio** | Short and clean — tight vocal-forward ambience, preserves lyric clarity |
| **Arena** | Large, modern, wide hall — big energy without muddiness |
| **Dream** | Long, airy, diffuse — soft ambient wash for pads and backgrounds |
| **Vintage** | Warm, plate-inspired — analog character with classic vocal density |

---

## Workflow Tips

- **Vocal insert:** Start with `Nova Mode: Studio`, `Space: 40%`, `Mix: 15–20%`
- **Vocal send:** Turn `Mix` to 100% and use your DAW's send level for blend control
- **Pads/ambient:** Try `Nova Mode: Dream`, `Space: 70%`, `Width: 80%`, `Air: 65%`
- **Vintage vibe:** `Nova Mode: Vintage`, `Air: 30%`, `Depth: 50%` for a classic plate feel

---

## Credits

Built with Audio Plugin Coder (APC) — https://noizefield.com

**© 2026 Noizefield**
