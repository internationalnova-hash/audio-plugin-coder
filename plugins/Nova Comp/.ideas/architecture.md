# DSP Architecture Specification

## Core Concept
**Nova Comp** should feel like a real optical leveler: 
- smooth
- forgiving
- musical
- fast to dial in
- slightly analog

It is inspired by the workflow and behavior of an LA-2A-style compressor, but modernized with:
- `Mix`
- `Tone`
- `Magic`
- `Smooth / Punch / Limit` mode switching

The user experience must stay simple while the backend does the premium work.

## Core Components

- **Input conditioning stage**
  - gently stabilizes the incoming signal before compression
  - keeps the front end smooth and musical
  - feeds both the detector and the tonal shaping path

- **Tone tilt stage**
  - subtle tilt EQ around the compression stage
  - lower settings feel warmer and smoother
  - higher settings feel brighter and more present
  - should influence how the compressor reacts, not just the final output EQ

- **Optical detector envelope**
  - smooth level-tracking block inspired by optical compression
  - should avoid jumpy, hard-edged gain reduction behavior

- **Gain computer**
  - converts detector level into gain reduction amount
  - driven primarily by the `level` macro control
  - includes threshold, knee, and ratio behavior per mode

- **Program-dependent release engine**
  - one of the key premium behaviors
  - small peaks recover faster
  - bigger sustained gain reduction recovers more slowly and musically

- **Mode voicing layer**
  - `Smooth` = classic optical leveling
  - `Punch` = tighter, more modern control
  - `Limit` = stronger peak management

- **Harmonic sweetening stage**
  - applies subtle warmth as compression increases
  - should make the compressor feel expensive, not saturated or obvious

- **Parallel mix stage**
  - modern Nova upgrade
  - dry/wet blend after full compression and tone shaping

- **Makeup gain stage**
  - final output compensation via `gain`
  - supports preset balancing and A/B loudness matching

- **Metering layer**
  - supports `GR` and `OUT`
  - should move more slowly and smoothly than Nova Heat’s meter

- **Magic polish layer**
  - adds subtle vocal focus, release refinement, top smoothing, stereo polish, and auto trim
  - must behave like enhancement, not a hidden preset recall

## Processing Chain

```text
Input → Light Tone Shaping → Optical Detector → Gain Computer → Program-Dependent Attack/Release Smoothing → Harmonic Sweetening → Makeup Gain → Parallel Mix → Meter Tap (GR / OUT) → Output
```

## Main Control Mapping

Nova Comp should present itself as a **3-knob signature compressor**:
- `COMPRESSION`
- `TONE SHAPING`
- `OUTPUT`

Hidden complexity lives in the modes and backend behavior, not in extra front-panel controls.

### `level` / `COMPRESSION`
This is the **Peak Reduction / Input / Threshold / Ratio-feel** equivalent, combined into one Nova-style macro.

- Range: `0.0 → 10.0`
- Feel:
  - low = light control (`~1–2 dB GR`)
  - mid = classic vocal compression (`~3–6 dB GR`)
  - high = heavy control (`~6–10 dB GR`)

When the user turns `COMPRESSION`, they are effectively:
- pushing signal harder into the compressor behavior
- lowering the effective threshold
- increasing compression amount
- slightly increasing intensity depending on the selected mode

Suggested mapping:
```text
compressionNorm = compression / 10.0
thresholdDb = -10.0 - compressionNorm * 22.0
```

So:
- `0.0 → -10 dB`
- `10.0 → -32 dB`

Optional ratio bias by mode:
- `Smooth`: `ratio = 2.5 + compressionNorm * 1.0`
- `Punch`: `ratio = 3.5 + compressionNorm * 1.0`
- `Limit`: `ratio = 6.0 + compressionNorm * 2.0`

> For **v1**, do **not** add a separate `Input` knob. `COMPRESSION` already provides the smarter Nova workflow.

### `gain` / `OUTPUT`
- Range: `-12 dB → +12 dB`
- Purpose: makeup gain after compression
- lets the user level-match and finish the sound quickly

```text
outputGainLin = dB_to_linear(gainDb)
```

### `mix`
- Range: `0 → 100`
- Parallel compression blend

```text
mixNorm = mix / 100.0
final = dry * (1.0 - mixNorm) + wet * mixNorm
```

### `tone` / `TONE SHAPING`
- Range: `0.0 → 10.0`
- `0.0` = darker / smoother
- `5.0` = neutral
- `10.0` = brighter / more present

This is **not just EQ**. It should control:
- tonal balance
- what frequencies trigger compression more strongly
- post-compression polish and vocal clarity

Suggested mappings:
```text
toneNorm = tone / 10.0
tiltDb = -3.0 + toneNorm * 6.0
sidechainTiltDb = -2.0 + toneNorm * 4.0
```

Interpretation:
- lower `tone` = more body / low sensitivity
- higher `tone` = more upper-mid / vocal sensitivity

Important: `tone` should interact with the compression stage itself, not just sit as a final output EQ.

## Mode Behavior Targets

| Mode | Ratio | Attack | Release Fast | Release Slow | Knee | Character |
|------|------:|-------:|-------------:|-------------:|-----:|-----------|
| `Smooth` | ~2.5:1 to 3.5:1 | 10 ms | 90 ms | 1300 ms | 8.0 dB | classic optical leveling |
| `Punch` | ~3.5:1 to 4.5:1 | 5 ms | 60 ms | 500 ms | 5.0 dB | tighter and more modern |
| `Limit` | ~6:1 to 8:1 | 2.5 ms | 80 ms | 700 ms | 2.5 dB | strong peak control |

### `Smooth`
- least aggressive
- soft knee
- subtle harmonic warmth
- ideal for vocals, bass, acoustic instruments, and mix bus glue

### `Punch`
- more transient control
- more forward and energetic
- ideal for drums, guitars, synths, and modern vocal shaping

### `Limit`
- firmer leveling and stronger GR authority
- best for peak control, aggressive vocals, buses, and creative compression

## Optical Release Behavior
This is one of the most important parts of Nova Comp.

A real optical-style compressor should not use one fixed release. It should release:
- faster after short peaks
- slower after heavy sustained compression

Suggested implementation:
```text
grNorm = currentGainReduction / maxGainReduction
effectiveReleaseMs = releaseFastMs + grNorm * (releaseSlowMs - releaseFastMs)
```

This gives:
- light GR = quicker recovery
- heavy GR = smoother long recovery

## Gain Reduction Computation

```text
detectorLevelDb = envelope(input)
overDb = detectorLevelDb - thresholdDb

if overDb <= 0:
    targetGrDb = 0
else:
    targetGrDb = compressionCurve(overDb, ratio, kneeDb)
```

Then smooth the gain reduction using attack and the effective release time.

```text
compressed = input * dB_to_linear(-gainReductionDb)
```

## Harmonic Enhancement
Nova Comp should add slight analog richness as compression increases.

Suggested hidden rule:
```text
warmthAmount = gainReductionDb * 0.03
wet = tanh(compressed * (1.0 + warmthAmount))
```

This must stay restrained and premium.

## Magic Mode
`Magic` should act like a finishing layer, not a different compressor.

In **Nova Comp**, `Magic` means:
> automatic leveling + polish + control

When ON:
- vocals become more consistent
- peaks get controlled more smoothly
- compression feels more musical
- tone gets slightly more polished
- output stays more balanced

### Under-the-hood behaviors
1. **Intelligent release**
   - faster release for small peaks
   - slower release for sustained compression
2. **Auto vocal leveling**
   - slightly helps quieter phrases stay forward
   - gently controls louder phrases more
3. **Soft transient control**
   - especially useful in `Punch` and `Limit`
   - keeps punch while removing spikes
4. **Tone-aware compression**
   - avoids harshness when bright
   - avoids muddiness when dark
5. **Auto output balance**
   - prevents louder settings from always seeming better
6. **Subtle wet-path width**
   - optional and mono-safe

Suggested boosts:
```text
releaseSmoothing += 12%
levelingBias += 10%
topSmooth += 10%
wetStereoBoost += 5%
autoTrim += -0.4 dB
```

Suggested per-mode scaling:
- `Smooth = 1.0x`
- `Punch = 0.8x`
- `Limit = 0.65x`

Why lower in `Limit`: too much polish would weaken the strong control feel.

## Meter Behavior
The meter should support:
- `GR`
- `OUT`

### `GR` mode
- show gain reduction in dB
- practical display range around `0 to -10 dB`
- smooth and readable, not jumpy

### `OUT` mode
- show output level
- slightly slower, more analog-style movement

Recommended default: `GR`

## Final Preset Targets

| Preset | Mode | Compression | Tone Shaping | Output | Mix | Magic | Expected GR |
|------|------|------------:|-------------:|-------:|----:|------|-------------|
| `VOCAL` | `Smooth` | 4.2 | 5.8 | +1.8 dB | 72% | ON | ~2–5 dB |
| `DRUMS` | `Punch` | 5.8 | 5.0 | +1.2 dB | 58% | OFF | ~4–7 dB |
| `BASS` | `Smooth` | 5.3 | 4.0 | +2.3 dB | 80% | OFF | ~3–6 dB |
| `MASTER` | `Smooth` | 2.6 | 5.0 | 0.0 dB | 45% | ON | ~1–2 dB |

## Hidden Premium Rules
1. As gain reduction rises, slightly soften highs:
   ```text
   topSmoothAmount = gainReductionDb * small amount
   ```
2. `Tone` should partially affect sidechain sensitivity in the upper mids.
3. `Mix` must happen after the full compression and tone-shaping path.
4. Add subtle auto trim so more compression does not just sound better because it is louder:
   ```text
   autoTrimDb = -(gainReductionDb * 0.15)
   ```

## Simple Reference Pseudocode

```text
compressionNorm = compression / 10.0
toneNorm = tone / 10.0
mixNorm = mix / 100.0

thresholdDb = -10.0 - compressionNorm * 22.0
tiltDb = -3.0 + toneNorm * 6.0
sidechainTiltDb = -2.0 + toneNorm * 4.0

preShaped = applyTiltEQ(input, tiltDb)
detectorInput = applyTiltEQ(preShaped, sidechainTiltDb)

detectorLevelDb = envelope(detectorInput)
overDb = detectorLevelDb - thresholdDb

ratio = modeDependentRatio(mode, compressionNorm)
kneeDb = modeDependentKnee(mode)

targetGrDb = computeSoftKneeGR(overDb, ratio, kneeDb)

grNorm = clamp(targetGrDb / 10.0, 0.0, 1.0)
effectiveReleaseMs = releaseFastMs + grNorm * (releaseSlowMs - releaseFastMs)

gainReductionDb = smoothGR(targetGrDb, attackMs, effectiveReleaseMs)

compressed = preShaped * dB_to_linear(-gainReductionDb)

warmthAmount = gainReductionDb * 0.025
colored = tanh(compressed * (1.0 + warmthAmount))

postPolished = applyTonePolish(colored, toneNorm)
wet = postPolished
mixed = input * (1.0 - mixNorm) + wet * mixNorm

if magicOn:
    mixed = applyMagicPolish(mixed, mode)

autoTrimDb = -(gainReductionDb * 0.15)
final = mixed * dB_to_linear(outputDb + autoTrimDb)
```

## Complexity Assessment
**Score: 3 / 5**

### Rationale
Nova Comp keeps a very simple front panel, but the backend still requires:
- optical-style detector behavior
- program-dependent release
- three meaningful compression personalities
- subtle harmonic enhancement
- dual-view analog-style metering

That makes it a moderate, polished dynamics processor rather than a basic utility comp.