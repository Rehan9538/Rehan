# Copilot Instructions

## Project: Battery Management System (BMS) Diagnostic Service

**Implementation Language:** C

Implement a continuous battery-management-system (BMS) diagnostic service that detects frozen sensor readings and calibration drift by cross-referencing physically adjacent sensors.

## Requirements
- Use configured physical topology; compare only sensors with compatible measurement types and comparable operating conditions.
- Flag a possible freeze only when a sensor remains effectively unchanged over a configurable window while relevant neighboring sensors or operating conditions change. A static value by itself is not a fault.
- Flag possible calibration drift when a persistent deviation from the compatible-neighbor baseline exceeds configurable tolerances, allowing for noise and expected spatial gradients.
- Filter transient outliers and account for startup, sampling cadence, timestamps, stale or missing readings, and insufficient neighbor data. Do not infer a fault when evidence is inadequate.
- Maintain per-sensor health state and confidence, with timestamps and supporting evidence. Use persistence and hysteresis to avoid alert flapping and support recovery.
- Keep diagnosis bounded for continuous operation, and prevent a suspected faulty sensor from contaminating neighboring sensors' baselines.
- Keep topology, thresholds, windows, and tolerances configurable. Separate data acquisition, diagnosis, and reporting where practical.
- Test healthy variation and gradients, freeze during changing conditions, gradual drift, transient outliers, missing or asynchronous data, insufficient neighbors, and recovery.
- Make diagnostic decisions auditable by documenting assumptions and exposing the evidence used.