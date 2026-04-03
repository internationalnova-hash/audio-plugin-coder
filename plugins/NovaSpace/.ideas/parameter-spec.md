# Nova Space Parameter Spec

| ID | Name | Type | Range | Default | Unit |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `space` | Space | Float | 0.00 - 1.00 | 0.45 | normalized |
| `air` | Air | Float | 0.00 - 1.00 | 0.60 | normalized |
| `depth` | Depth | Float | 0.00 - 1.00 | 0.40 | normalized |
| `width` | Width | Float | 0.00 - 1.00 | 0.80 | normalized |
| `mix` | Mix | Float | 0.00 - 1.00 | 0.18 | ratio |
| `nova_mode` | Nova Mode | Choice | Studio, Arena, Dream, Vintage | Studio | mode |

## Control Intent

### `space`
Macro control for room size, decay growth, and reflection spread. Lower values feel like a compact studio room; higher values move toward larger cinematic environments.

### `air`
Controls the brightness and shimmer of the reverb tail. Lower values produce darker, warmer ambience; higher values produce brighter modern vocal sheen.

### `depth`
Macro control for front-to-back placement by shaping pre-delay feel, wet emphasis, and tail presence. Lower values keep the source forward; higher values push it deeper into the field.

### `width`
Controls stereo spread and mid/side emphasis in the wet field. Lower values collapse toward mono; higher values create a wider, more immersive image.

### `mix`
Standard wet/dry blend for insert or parallel use.

### `nova_mode`
Selects the core reverb personality:

- `Studio`: short and clean for vocal clarity.
- `Arena`: bigger, wider, modern hall energy.
- `Dream`: long, airy, diffuse ambience.
- `Vintage`: warmer, plate-like analog tone.