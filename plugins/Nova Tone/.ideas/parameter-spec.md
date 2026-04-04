# Nova Tone — Parameter Specification

## Core Parameters

| ID | Name | Type | Range / Options | Default | Unit |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `low_freq` | Low Frequency Selector | Choice | 20, 30, 60, 100 | 60 | Hz |
| `low_boost` | Low Boost | Float | 0.0 - 10.0 | 0.0 | step |
| `low_attenuation` | Low Attenuation | Float | 0.0 - 10.0 | 0.0 | step |
| `high_boost_freq` | High Boost Frequency | Choice | 3000, 4000, 5000, 8000, 10000, 12000, 16000 | 8000 | Hz |
| `high_boost` | High Boost | Float | 0.0 - 10.0 | 0.0 | step |
| `bandwidth` | Bandwidth | Float | 1.0 - 10.0 | 5.0 | width |
| `high_attenuation_freq` | High Attenuation Frequency | Choice | 5000, 10000, 20000 | 10000 | Hz |
| `high_attenuation` | High Attenuation | Float | 0.0 - 10.0 | 0.0 | step |
| `output_gain` | Output Gain | Float | -10.0 - 10.0 | 0.0 | dB |
| `mode_preset` | Mode Preset | Choice | Neutral, Vocal, Bass, Air, Master | Neutral | mode |

## Behavioral Notes
- `low_boost` and `low_attenuation` must be allowed to operate **simultaneously at the same selected low frequency**.
- Curves should be broad and musical rather than narrow or surgical.
- The plugin should remain neutral when all gain-based parameters are at `0.0` and mode is `Neutral`.
- A subtle analog-style saturation stage is acceptable if it supports the premium hardware feel.

## Initial Preset Targets

| Mode | Low Freq | Low Boost | Low Atten | High Boost Freq | High Boost | Bandwidth | High Atten Freq | High Atten | Output |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| Vocal | 100 Hz | 2.0 | 1.5 | 10 kHz | 5.5 | 4.0 | 5 kHz | 2.0 | 0.0 dB |
| Bass | 60 Hz | 6.0 | 4.5 | 3 kHz | 1.5 | 3.0 | 5 kHz | 0.0 | 0.0 dB |
| Air | 30 Hz | 0.0 | 0.0 | 12 kHz | 7.0 | 6.0 | 10 kHz | 2.5 | 0.0 dB |
| Master | 30 Hz | 3.0 | 2.0 | 10 kHz | 4.0 | 5.0 | 20 kHz | 1.5 | 0.0 dB |
