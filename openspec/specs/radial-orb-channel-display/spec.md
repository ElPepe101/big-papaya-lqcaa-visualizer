# radial-orb-channel-display Specification

## Purpose

TBD — update after stabilizing orb UX. Describes the 16-channel donut orb and opacity-driven sector fill.

## Requirements

### Requirement: Donut orb layout with fixed clockwise sectors

The system SHALL render the 16 channel levels on a **two-dimensional donut** (annulus): an outer boundary, an **inner hole** with strictly positive inner radius, and **16 contiguous angular sectors** of equal angle spanning the full circle. Channel index **0** (ch1) SHALL occupy the sector starting at the **top** of the circle (12 o’clock), and channels **1…15** SHALL follow in **clockwise** order around the orb.

#### Scenario: All sixteen sectors visible

- **WHEN** the application window is showing the orb view
- **THEN** the user SHALL see sixteen distinct sector regions corresponding to channels 1–16 in clockwise order starting from the top

### Requirement: Opacity-only mapping on fixed donut sectors

The system SHALL render each channel’s smoothed level as **opacity** (or equivalent alpha) on a **fixed** annulus sector: the **inner** and **outer** radii of the colored region SHALL NOT vary with level; only the **fill opacity** SHALL vary with level. Level SHALL be derived from the same smoothed per-channel RMS pipeline used elsewhere (including consistent mapping and shaping such as `ofMap` into a normalized range and `sqrt` for display, as implemented).

#### Scenario: Silence vs loud channel

- **WHEN** one channel’s smoothed level is near zero and another channel’s level is high
- **THEN** the quiet channel’s sector SHALL appear visually weaker (lower opacity) and the loud channel’s sector SHALL appear stronger (higher opacity), with both sectors covering the same annular wedge geometry

#### Scenario: No alternate geometry modes

- **WHEN** the application is running
- **THEN** the orb SHALL NOT offer radial-extent-only fills or inner-radius-thickness wedge modes as selectable behaviors
