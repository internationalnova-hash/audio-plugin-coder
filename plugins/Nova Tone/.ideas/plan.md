# Implementation Plan

## Complexity Score: 3 / 5

## Implementation Strategy

**Approach:** `phased`

Nova Tone should be implemented in clearly separated passes so the analog EQ behavior stays musical and the UI feels premium from the start.

---

## Phase 2.1.1 — Core Tone Engine
- [ ] Set up parameter definitions and defaults
- [ ] Implement low frequency selector logic
- [ ] Build independent low boost and low attenuation stages
- [ ] Implement high boost stage with frequency selection and bandwidth mapping
- [ ] Implement high attenuation stage with musical smoothing
- [ ] Add output gain stage

### Goal
Get the plugin to a **neutral but working EQ** that already behaves broadly and musically.

---

## Phase 2.1.2 — Analog Character & Presets
- [ ] Add smoothing to all continuous controls
- [ ] Tune filter responses for rounded, non-harsh tone
- [ ] Add subtle analog-style saturation / softening
- [ ] Implement mode buttons: `Vocal`, `Bass`, `Air`, `Master`
- [ ] Ensure preset transitions feel stable and musical

### Goal
Move from a functional EQ to a **characterful tone shaper** that feels expensive and flattering.

---

## Phase 2.1.3 — UI & Final Polish
- [ ] Build a one-screen WebView interface
- [ ] Use warm champagne / ivory metal panel styling
- [ ] Group controls into clear hardware-like sections
- [ ] Add family resemblance to `Space By Nova` without copying it exactly
- [ ] Ensure labels, dividers, and knob behavior feel like real outboard gear

### Goal
Deliver a UI that feels like **studio hardware**, not generic plugin software.

---

## DSP Design Notes

### Low Section
The low section should preserve the Pultec-inspired behavior of boosting and attenuating at the same selected point. The result should feel like a musical contour rather than a simple shelf.

### High Boost Section
The high boost should emphasize smoothness and gloss. The `bandwidth` control should map from broad/open to more focused presence without ever becoming harsh or clinical.

### High Attenuation Section
This should act like a graceful softening curve for the top end, useful for taming brittleness while keeping air.

---

## Required JUCE Modules

### Required
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_audio_utils`
- `juce_dsp`
- `juce_gui_basics`
- `juce_gui_extra`

### WebView UI Support
- `juce_gui_extra` with WebBrowser / WebView enabled

---

## UI Framework Selection

**Decision:** `webview`

**Rationale:**
Nova Tone needs a premium hardware-style faceplate with warm metal tones, tactile gold controls, clear section engraving, and a refined boutique look. WebView is the best fit for that visual direction and keeps it aligned with the `Space By Nova` family design language.

---

## Risk Assessment

### High Risk
- tuning the low boost + attenuation interaction so it feels musical instead of exaggerated
- matching the analog-inspired behavior without creating unstable or overly resonant curves
- maintaining premium UX while keeping the control count clear and simple

### Medium Risk
- mapping `bandwidth` in a way that feels intuitive
- balancing subtle saturation so it enhances tone without obvious distortion
- preset values needing iteration for different source material

### Low Risk
- section-based control layout
- output gain implementation
- preset button routing and parameter assignment

---

## Recommended Next Step
Proceed to the **design phase** using a warm ivory-and-gold hardware faceplate with:
- a champagne body (`#F2E8DC` range)
- a slightly deeper inner panel (`#E4D6C3`)
- warm dark labels (`#4A4036`)
- tactile gold knobs and mode buttons
