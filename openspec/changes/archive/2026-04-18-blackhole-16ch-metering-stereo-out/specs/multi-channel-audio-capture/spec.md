## ADDED Requirements

### Requirement: Single multi-channel input stream

The application SHALL open exactly one audio input stream configured for **16 input channels** when operating in the BlackHole capture mode described in the project proposal.

#### Scenario: Successful open with 16 channels

- **WHEN** the application starts and the configured input device supports at least 16 input channels
- **THEN** the audio subsystem SHALL deliver interleaved or API-documented channel-ordered buffers consistent with 16 inputs without reducing channel count to stereo unless explicitly configured otherwise

#### Scenario: Device unsuitable

- **WHEN** the selected device cannot provide 16 input channels
- **THEN** the application SHALL report the failure in a developer-visible way (for example log output) and SHALL NOT silently fall back to two channels without explicit user configuration

### Requirement: Sample rate consistency

The application SHALL use a single sample rate for the duplex stream that matches the user’s session configuration as documented in `design.md` (typically matching Ableton and BlackHole).

#### Scenario: Rate known at startup

- **WHEN** audio starts
- **THEN** the application SHALL log the active sample rate and buffer size
