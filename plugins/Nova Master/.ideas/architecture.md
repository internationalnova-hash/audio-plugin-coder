# Nova Master — DSP Architecture Specification

## Core Components
Nova Master should combine several restrained mix-bus processes into one finishing chain.

### Core DSP philosophy
This plugin must prioritize:
- refinement over hype
- invisible glue over obvious compression
- controlled air over sharp brightness
- anchored low end over exaggerated bass
- depth and polish over obvious widening

It should feel like the last 5% that makes a mix feel finished, not like an aggressive loudness enhancer.

- Input trim and safety stage
- Tonal tilt EQ stage
- Low-end contour / foundation enhancer
- Glue compression stage
- Harmonic finish / sweetening stage
- Stereo polish / width enhancement stage
- Output trim and ceiling management
- Metering for output / loudness view

## Processing Chain

```text
Input
→ Tone Tilt
→ Weight Contour
→ Air Shelf
→ Glue Compression
→ Subtle Harmonic Finish
→ Width Processing
→ Finish Layer
→ Output Trim / Ceiling
→ Output
```

## Parameter Mapping

| Parameter | DSP Component | Function | Range |
|---|---|---|---|
| `tone` | Tonal tilt EQ | Tilts the overall spectral balance darker or brighter | `-3 dB` to `+3 dB` effective tilt |
| `glue` | Compressor + density stage | Changes threshold, ratio, recovery feel, and mix-bus density | `0.0 - 10.0` |
| `weight` | Low contour stage | Adds focused low-end body around the foundation zone | `0.0 - 10.0` |
| `air` | High shelf / polish stage | Adds open top-end lift and mastering sheen | `0.0 - 10.0` |
| `width` | Stereo side stage | Enhances upper stereo width while protecting low-end mono stability | `0.0 - 10.0` |
| `finish_mode` | Final polish layer | Adds subtle smoothing, harmonic finish, stereo refinement, and level balancing | `Off / On` |
| `mode_preset` | Macro state recall | Recalls musical voicings for Clean / Warm / Wide / Loud | enum |
| `output_gain` | Final trim | Matches or offsets final level | `-6 dB` to `+6 dB` |

## Suggested Internal Behavior

### Tonal Tilt
```text
toneNorm = tone / 10.0
tiltDb = -3.0 + toneNorm * 6.0
```
- lower settings = darker / smoother
- higher settings = brighter / more present
- pivot target: about `1.8 kHz` to `2.2 kHz`

### Glue Compression
The `glue` control should act like a mastering-safe bus compressor macro.

It should be **slower, smoother, and less obvious** than the punch-forward behavior common in more aggressive finalizer plugins.

```text
glueNorm = glue / 10.0
thresholdDb = -6.0 - glueNorm * 12.0
ratio = 1.5 + glueNorm * 2.0
attackMs ≈ 30
releaseMs = 120 + glueNorm * 230
knee = soft
```

Target behavior:
- low = barely touching
- middle = connected and polished
- high = denser, more cohesive, still mix-bus safe

### Weight Stage
The `weight` control should add low-end body without mud.

```text
weightNorm = weight / 10.0
lowShelfDb = -1.5 + weightNorm * 3.5
```
- main contour area: about `80 Hz`
- optional tiny tightening around `180–250 Hz`
- slightly increase low-end control as `Glue` rises

### Air Stage
The `air` control should add polish and openness while staying smooth.

The top end should feel **expensive, not boosted**.
It must avoid the “brighter = better” trap that quickly becomes harsh.

```text
airNorm = air / 10.0
highShelfDb = -1.0 + airNorm * 4.0
```
- main shelf area: about `10–14 kHz`
- add slight top smoothing as `Air` rises to avoid brittleness

### Width Stage
The `width` control should widen mostly the upper band, not the low end.

The goal is **depth and space**, not exaggerated stereo hype.

```text
widthNorm = width / 10.0
sideGainDb = widthNorm * 3.5
decorrelationMs = widthNorm * 6.0
```
- keep low frequencies mostly centered below roughly `180–250 Hz`
- widen mainly upper mids / highs
- keep the center image anchored

### Finish Mode
When `finish_mode` is enabled, add a premium polish layer:

```text
extraGlue = +8% to +12%
harmonicDrive = 1.03 to 1.08
sideBoost = +4% to +8%
autoTrim = -0.3 dB to -0.8 dB
```

Suggested additions:
- micro dynamic smoothing
- subtle tanh-style harmonic finish
- slight top smoothing
- tiny stereo polish
- level-managed auto trim

The effect should be heard as:
> “the mix got more expensive”

## Hidden Rules

1. As `Glue` increases, slightly tighten the low end.
2. As `Air` increases, slightly smooth the top.
3. As `Width` increases, keep the center anchored and the low end mostly mono.
4. `Finish` should not create a huge loudness jump.
5. Avoid any brickwall-limiter feel or aggressive transient flattening.
6. Harmonic finish must stay in the micro-harmonic zone, not audible distortion.

### What to avoid
- harsh high-frequency hype
- over-compression
- aggressive transient smashing
- obvious loudness jumps
- bloated low end

## Output Protection / Ceiling
Use a gentle final protection stage even if no dedicated ceiling knob is exposed.

```text
ceiling = -0.5 dBFS
```

A very light peak-catching limiter is acceptable as long as it stays transparent.

## Reference Pseudocode

```text
toneNorm = tone / 10.0
glueNorm = glue / 10.0
weightNorm = weight / 10.0
airNorm = air / 10.0
widthNorm = width / 10.0

tiltDb = -3.0 + toneNorm * 6.0
lowShelfDb = -1.5 + weightNorm * 3.5
highShelfDb = -1.0 + airNorm * 4.0
sideGainDb = widthNorm * 3.5

signal1 = applyTiltEQ(input, tiltDb)
signal2 = applyLowShelf(signal1, 80Hz, lowShelfDb)
signal3 = applyHighShelf(signal2, 12kHz, highShelfDb)

thresholdDb = -6.0 - glueNorm * 12.0
ratio = 1.5 + glueNorm * 2.0

glued = busCompress(signal3, thresholdDb, ratio, attack=30ms, release=auto, knee=soft)

harmonicDrive = 1.0 + glueNorm * 0.03
if finishOn:
    harmonicDrive += 0.06

finished = tanh(glued * harmonicDrive)
widened = stereoWidthHighBandOnly(finished, sideGainDb)

if finishOn:
    widened = applyDynamicSmoothing(widened)
    widened = applyTopSmoothing(widened, amount=small)
    widened = applyExtraStereoPolish(widened, amount=small)
    widened = applyTrim(widened, -0.5dB)

output = applyCeiling(widened, -0.5dBFS)
```

## Modes

### `Clean`
- most transparent
- safest top end
- lightest glue
- minimal harmonic thickening

### `Warm`
- more low-mid richness
- slightly darker tilt bias
- more harmonic finish
- more body than openness

### `Wide`
- more stereo polish and openness
- more air and side enhancement
- still keep low end stable

### `Loud`
- stronger glue and density
- firmer loudness control
- slightly more aggressive finishing feel

## Complexity Assessment
**Score: 3 / 5**

### Rationale
Nova Master is more advanced than a simple EQ or saturator because it combines multiple subtle mastering processes in one chain:
- tonal shaping
- controlled compression
- harmonic finishing
- stereo management
- output management

However, it is still practical for a phased JUCE implementation and does not require highly experimental DSP research.
