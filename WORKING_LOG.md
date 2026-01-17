# AEC Verification Experiment - Working Log

## Goal

Verify that AEC (Acoustic Echo Cancellation) is working on the XMOS chip by:
1. Playing pre-recorded speech through the speaker
2. Speaking simultaneously
3. Capturing the AGC output (post-AEC) and streaming to laptop
4. Analyzing in Audacity to confirm echo is cancelled

## Background

From analyzing the XMOS firmware (`voice-kit-xmos-firmware`), we discovered:

- The audio pipeline is **sequential**: `Mic → AEC → IC → NS → AGC`
- **All stages always run** - the stage configuration only selects which OUTPUT to tap
- Default `channel_0_stage: AGC` means we get **fully processed audio including AEC**
- Speaker reference signal comes from I2S (XMOS sees speaker output and uses it for AEC)

## Expected Results

| Scenario | What You Should Hear in Audacity |
|----------|----------------------------------|
| AEC working | Your voice clear, speaker echo mostly cancelled |
| AEC not working | Both your voice AND the played speech mixed |

---

## Implementation Plan

### Component 1: UDP Audio Streamer

Create a custom ESPHome component that:
- Taps into `i2s_mics` microphone via `add_data_callback()`
- Streams raw audio bytes over UDP to laptop (no header, just raw PCM)
- Can be started/stopped via a switch

**Files to create:**
```
esphome/components/udp_audio_stream/
├── __init__.py              # Python config schema
├── udp_audio_stream.h       # C++ header
└── udp_audio_stream.cpp     # C++ implementation
```

**Audio Format:**
- Sample Rate: 16000 Hz
- Bits per Sample: 32-bit
- Channels: 2 (stereo)
- Encoding: Signed integer, little-endian

### Component 2: Receiver (Simplified)

Use `nc` (netcat) to receive raw audio:
```bash
# On laptop - open firewall port 5555 first
nc -u -l 5555 > captured.raw
```

Then import directly into Audacity:
- **File > Import > Raw Data**
- Encoding: **Signed 32-bit PCM**
- Byte order: **Little-endian**
- Channels: **2 (Stereo)**
- Sample rate: **16000 Hz**

### Component 3: YAML Configuration

Add to `home-assistant-voice.yaml`:
- External component reference
- UDP streamer instance with target IP and port 5555
- Control switch to start/stop streaming

---

## Files to Create/Modify

| File | Action | Description |
|------|--------|-------------|
| `esphome/components/udp_audio_stream/__init__.py` | Create | Python config schema |
| `esphome/components/udp_audio_stream/udp_audio_stream.h` | Create | C++ header |
| `esphome/components/udp_audio_stream/udp_audio_stream.cpp` | Create | C++ implementation |
| `home-assistant-voice.yaml` | Modify | Add component config |

---

## Experiment Procedure

1. **Setup:**
   - Create the UDP audio stream component
   - Modify YAML to include the component (update `target_ip` to laptop's IP)
   - Flash firmware to Voice PE device
   - Open firewall port 5555 on laptop
   - Start receiver: `nc -u -l 5555 > captured.raw`

2. **Run Experiment:**
   - Turn on "Stream Audio to Laptop" switch via Home Assistant
   - Play test speech via HTTP URL (user provides)
   - Speak clearly while speech is playing
   - Stop streaming after 10+ seconds
   - Press Ctrl+C on nc to stop receiving

3. **Analyze in Audacity:**
   - File > Import > Raw Data
   - Settings: Signed 32-bit PCM, Little-endian, 2 channels, 16000 Hz
   - Compare levels of your voice vs played speech
   - If AEC works: played speech should be significantly attenuated

---

## User Inputs Needed

- [ ] Laptop IP address for UDP target
- [ ] HTTP URL to speech recording for test playback

---

## Progress Log

- [x] Create UDP audio stream component
  - Created `esphome/components/udp_audio_stream/__init__.py`
  - Created `esphome/components/udp_audio_stream/udp_audio_stream.h`
  - Created `esphome/components/udp_audio_stream/udp_audio_stream.cpp`
- [x] Update home-assistant-voice.yaml
  - Added local external_components reference
  - Added udp_audio_stream configuration (target_ip needs updating)
  - Added "Stream Audio to Laptop" switch
  - **Modified single-click** to toggle streaming (original behavior commented out)
  - Visual feedback: LED ring turns solid red when streaming
- [ ] Build and flash firmware
- [ ] Run experiment
- [ ] Analyze results
