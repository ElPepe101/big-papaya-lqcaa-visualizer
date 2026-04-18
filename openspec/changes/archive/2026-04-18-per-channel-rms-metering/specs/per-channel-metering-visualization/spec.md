## MODIFIED Requirements

### Requirement: Per-channel level metrics

The system SHALL compute a **root-mean-square (RMS)** level independently for each of the 16 input channels over the samples present in each processed audio snapshot (the buffer made available to the main thread from the audio input path). For each channel, RMS SHALL be the square root of the mean of squared sample values across all frames in that snapshot for that channel.

The value used for **on-screen metering** SHALL be a **smoothed** representation of per-channel RMS (for example an exponential moving average or separate attack and release smoothing) computed on the main thread so that motion reflects perceived level without excessive flicker from single-sample spikes.

#### Scenario: All channels updated together

- **WHEN** a new complete input buffer snapshot is available for processing
- **THEN** the system SHALL produce 16 scalar RMS-based level values, one per channel index 0–15, before smoothing

#### Scenario: Smoothing applied for display

- **WHEN** RMS values are updated for the 16 channels
- **THEN** the levels used for drawing the meters SHALL incorporate smoothing such that brief transients do not fully dominate bar height compared to the underlying RMS
