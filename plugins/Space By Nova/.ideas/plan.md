# Implementation Plan — Space By Nova

## Complexity Score: 3 / 5

## Framework Decision
**Selected UI framework:** `webview`

### Rationale
Space By Nova is a premium, brand-forward plugin with strong emphasis on:
- luxury white-and-gold styling,
- smooth preset motion,
- polished knob animation,
- and a minimalist boutique hardware feel.

Those requirements are best served by a **JUCE WebView UI**, while the DSP remains native C++ in the processor.

## Implementation Strategy
Because the plugin is moderate-to-advanced complexity, it should be built in phases.

### Phase 4.1 — Core Processing
- [ ] Create APVTS parameter layout for visible and advanced controls
- [ ] Implement stereo vocal reverb signal path
- [ ] Add pre-delay, early reflections, damping, width, and wet/dry mix
- [ ] Tune the default sound around the `Studio` profile

### Phase 4.2 — Macro Mapping + Nova Modes
- [ ] Map `space`, `air`, `depth`, `mix`, and `width` to internal DSP targets
- [ ] Implement `Studio`, `Arena`, `Dream`, and `Vintage` mode targets
- [ ] Ramp mode changes over **100–300 ms** using smoothed parameter transitions
- [ ] Ensure host automation and preset recall remain stable

### Phase 4.3 — WebView Front End
- [ ] Build the main UI with premium white/gold styling
- [ ] Add the five macro controls and `Nova Mode` selector
- [ ] Create a hidden advanced panel for `pre_delay_ms`, `decay`, `damping`, and `early_reflections`
- [ ] Synchronize knob animation timing with DSP smoothing for a premium feel

### Phase 4.4 — Polish and Verification
- [ ] Check mono compatibility and stereo width behavior
- [ ] Verify CPU stays reasonable for multiple instances
- [ ] Validate the four mode personalities with real vocal material
- [ ] Confirm that transitions remain smooth and free of zipper noise

## Dependencies

### Required JUCE Modules
- `juce_audio_basics`
- `juce_audio_processors`
- `juce_dsp`
- `juce_gui_basics`
- `juce_gui_extra`

### Recommended Repo References
- `plugins/CloudWash/Source/PluginEditor.h` — WebView integration pattern
- `plugins/CloudWash/Source/PluginProcessor.cpp` — parameter organization and preset ideas
- `plugins/CloudWash/Source/dsp/clouds/dsp/fx/reverb.h` — reverb topology reference
- `docs/webview-framework.md` — framework constraints and expectations

## Risk Assessment

### High Risk
- Dialing in a reverb tail that feels premium on vocals instead of metallic or washy
- Preserving clarity while still giving `Dream` and `Arena` enough size

### Medium Risk
- Getting the macro mappings to feel intuitive over the full range
- Synchronizing UI motion and DSP smoothing during `Nova Mode` changes
- Avoiding phase issues or weak mono compatibility at extreme `width` values

### Low Risk
- Preset table structure
- Hidden advanced parameter panel
- Wet/dry balance control

## Validation Goals
- `Studio` should remain tight and intelligible
- `Arena` should feel wide and polished
- `Dream` should feel cinematic and floaty
- `Vintage` should feel warm and smooth
- Mode changes should feel expensive, not abrupt

## Next Phase
After planning is complete, the correct next step is the **DESIGN** phase to create the approved UI specification before DSP implementation begins.
