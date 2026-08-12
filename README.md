# SampleRealm: Sub-bass

A monophonic sub-bass synthesizer

## Features

### Oscillators
- 2 sine oscillators with independent tuning
- Coarse (±24 semitones) and fine (±50 cents) pitch per oscillator
- Continuous crossfade between the two oscillators

### Voice
- Monophonic with a held-note stack — the most recent note sounds
- Legato retriggering: the envelope restarts only on the first note of a phrase, so overlapping notes change pitch without a new attack
- Sample-accurate note timing, MIDI events split within the block

### Envelope
- ADSR with attack, decay, and release from 1 ms to 5 seconds
- Skewed ranges for finer control at short times

### Saturation
- tanh soft-clip drive, applied post-envelope
- Gain-normalized, so raising drive adds harmonics without raising level

### Output
- Master gain, −60 to +6 dB


Adding a parameter means touching `Parameters.h`, `Parameters.cpp`, and one
`addControl()` line in `PluginEditor.cpp`.

## Build Requirements

- CMake 3.25+
- A C++23-capable compiler
- Git
- macOS development environment for AU/Standalone/VST3 builds

## Building

**Debug Build:**
```bash
cmake --preset debug
cmake --build --preset debug
```

**Release Build:**
```bash
cmake --preset release
cmake --build --preset release
```

Run the standalone:

```bash
open build-debug/SubBass_artefacts/Debug/Standalone/SubBass.app
```


## Debugging in Xcode

### 1. Generate Xcode Project

```bash
cmake -B build-xcode -G Xcode
open build-xcode/SubBass.xcodeproj
```

### 2. Configure Debugging

1. Select your plugin target from the scheme dropdown
2. Go to **Product → Scheme → Edit Scheme**
3. Click **Run** on the left sidebar
4. Under **Executable**, choose **Other** and navigate to executable.

### 3. Build and Run

1. Press **Cmd+B** to build the plugin
2. Press **Cmd+R** to run with AudioPluginHost
3. Load your plugin in AudioPluginHost
