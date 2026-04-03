# Implementation Plan

## Complexity Score: 3/5

## Implementation Strategy

### Phased Implementation

#### Phase 2.1.1: Core Processing
- [ ] Create APVTS parameter layout for all five macro controls plus `nova_mode`
- [ ] Build pre-delay, shared reverb core, wet tone shaping, and wet/dry mix
- [ ] Implement `Studio` mode as baseline vocal profile
- [ ] Add smoothing for all continuous controls

#### Phase 2.1.2: Mode Expansion
- [ ] Add `Arena`, `Dream`, and `Vintage` mode profiles
- [ ] Smooth mode changes by ramping target coefficients instead of hard resets
- [ ] Tune width behavior and wet-field stereo processing per mode

#### Phase 2.1.3: Polish
- [ ] Refine macro curves for vocals and ambient use
- [ ] Add edge-case handling for automation extremes and sample-rate changes
- [ ] Optimize CPU behavior and verify session-state recall

## Dependencies

### Required JUCE Modules
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_audio_utils`
- `juce_dsp`
- `juce_gui_extra`

### Optional Modules
- `juce_audio_formats`
- `juce_gui_basics`

## UI Framework

**Decision:** `webview`

**Rationale:**
NovaSpace needs a premium white-and-gold interface, a centered hero control, and clear mode-state presentation. Those are easier to iterate and refine in WebView than native C++ widgets, and the repo already has working WebView patterns for parameter relays, embedded resources, and JUCE bridge integration.

**Implementation Strategy:** `phased`

## Risk Assessment

### High Risk
- Smooth mode transitions without audible jumps or unstable tail behavior.
- Macro mapping that feels musical on vocals rather than technical or over-coupled.
- Cross-platform WebView setup, especially member ordering and resource loading.

### Medium Risk
- Pre-delay and wet-balance interaction creating phasey or detached vocal placement.
- Wet stereo widening becoming too hollow at extreme settings.
- Reverb damping curves feeling too dark or too brittle across all four modes.

### Low Risk
- APVTS state management.
- Fixed-size WebView layout.
- Basic wet/dry mixing and output protection.