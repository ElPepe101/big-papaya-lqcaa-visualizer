## ADDED Requirements

### Requirement: Per-channel level metrics

The system SHALL compute a level metric (peak or RMS as chosen in implementation) independently for each of the 16 input channels on each processed audio frame available to the render thread.

#### Scenario: All channels updated together

- **WHEN** a new complete input buffer snapshot is available for processing
- **THEN** the system SHALL produce 16 scalar level values, one per channel index 0–15

### Requirement: Simultaneous visualization

The system SHALL render a visual representation of all 16 channel levels during the same frame without requiring the user to switch views to see individual channels.

#### Scenario: All meters visible

- **WHEN** the application window is visible
- **THEN** the user SHALL see 16 distinct meter regions (for example bars) corresponding to channels 1–16 in order
