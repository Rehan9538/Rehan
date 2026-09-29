# Software Design and Requirements Document (SWDD)
## BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine

**Document Version:** 1.0  
**Date:** 2026-09-28  
**Project:** Battery Management System (BMS) Diagnostic Service  
**Implementation Language:** C  
**Status:** Draft

---

## 1. Executive Summary

This document specifies the software requirements and design for the BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine. The system is a continuous diagnostic service designed to run on BMS edge controllers to detect sensor anomalies (frozen readings and calibration drift) by cross-referencing adjacent, compatible sensors. The service maintains auditable health states and communicates faults to cloud infrastructure via AWS IoT Core.

---

## 2. Scope

### 2.1 In Scope
- Real-time sensor anomaly detection (freeze and drift)
- Cross-sensor validation using configurable topology
- Per-sensor health state machine with hysteresis
- Configurable diagnostic thresholds and windows
- JSON-based diagnostic reporting
- Integration with AWS IoT Core and DynamoDB
- Unit test framework with mock data generators
- CI/CD automation with security scanning
- Infrastructure-as-Code (Terraform) for AWS deployment

### 2.2 Out of Scope
- Sensor hardware drivers (user-provided interface)
- Real-time operating system (RTOS) integration details
- Wireless communication protocols beyond MQTT
- Visual/UI dashboarding
- Historical data analysis or machine learning

---

## 3. Design Goals & Constraints

### 3.1 Design Goals
1. **Reliability** — Continuously operate with minimal false positives/negatives
2. **Auditability** — Document all diagnostic decisions with supporting evidence
3. **Configurability** — Support diverse sensor topologies and operating conditions
4. **Performance** — Minimal latency and memory footprint for embedded environments
5. **Maintainability** — Modular architecture with clear separation of concerns
6. **Security** — Secure MQTT communication; no hardcoded credentials

### 3.2 Constraints
- **Memory:** Embedded edge device with limited RAM (buffer size must be configurable)
- **CPU:** Low-power processor; real-time diagnostic computation required
- **Connectivity:** Intermittent or limited internet; offline queuing needed
- **Sampling Rate:** Variable sensor input frequency (device-dependent)
- **Latency:** Diagnostic decisions within 110ms for OOS detection

---

## 4. Functional Requirements

### 4.1 Sensor Data Ingestion (FR-1)
**Requirement:** The system shall ingest real-time sensor readings with timestamps.

**Details:**
- Accept readings from multiple BMS sensors (temperature, voltage, current)
- Maintain readings with microsecond-resolution timestamps
- Support asynchronous and synchronous data acquisition
- Handle missing or stale data gracefully
- **Configuration:** Sampling cadence, sensor IDs, measurement units

**Acceptance Criteria:**
- System processes sensor inputs without data loss (buffering)
- Timestamps are maintained throughout processing pipeline
- Stale data is flagged and excluded from baseline calculations

---

### 4.2 Physical Topology Configuration (FR-2)
**Requirement:** The system shall use a configured physical topology to identify compatible sensor neighbors.

**Details:**
- Define sensor relationships (adjacent, zone-based, circuit-based)
- Specify compatibility rules (measurement type, operating conditions)
- Support hierarchical sensor grouping (e.g., battery pack → module → cell)
- Topology is externally configurable (JSON or configuration file)
- Only compatible neighbors are compared for anomaly detection

**Acceptance Criteria:**
- Configuration file successfully loads without parsing errors
- Incompatible sensors are correctly excluded from neighbor baseline
- Topology changes are reflected in diagnostics without service restart

---

### 4.3 Freeze Detection (FR-3)
**Requirement:** The system shall detect "frozen" sensor readings that remain unchanged despite environmental changes.

**Details:**
- A sensor is flagged as frozen if:
  1. Its standard deviation over a configurable window < 0.05°C
  2. At least one compatible neighbor shows significant variation (>0.5°C)
  3. Both conditions persist for a configurable duration
- Transient freezes are not flagged (hysteresis applied)
- A static value by itself is insufficient; context (neighbor activity) is required

**Configuration Parameters:**
- `freeze_stdev_threshold` — Std. dev. below which sensor is considered static (default: 0.05°C)
- `neighbor_variation_threshold` — Minimum neighbor variation to confirm freeze (default: 0.5°C)
- `freeze_window_duration` — Time window for rolling std. dev. calculation (default: 1 hour)
- `freeze_persistence_duration` — Duration before flagging freeze (default: configurable, >110ms)

**Acceptance Criteria:**
- Frozen sensor with changing neighbors is flagged within persistence window
- Healthy sensor with normal variation is not falsely flagged
- Transient glitches do not trigger false freeze alarms

---

### 4.4 Drift Detection (FR-4)
**Requirement:** The system shall detect calibration drift (persistent deviation from baseline).

**Details:**
- A sensor is flagged as "Out of Spec" (OOS) if:
  1. Its reading deviates from the median of compatible neighbors by >1.5°C
  2. Deviation persists for >110ms
  3. At least 2 compatible neighbors are available for baseline
- Sensor bias/gradient accounting: Expected spatial gradients are configurable

**Configuration Parameters:**
- `drift_tolerance_celsius` — Maximum allowed deviation (default: 1.5°C)
- `drift_persistence_time_ms` — Duration before flagging OOS (default: 110ms)
- `min_neighbors_for_baseline` — Minimum valid neighbors required (default: 2)
- `spatial_gradient_tolerance` — Acceptable gradient across zones (default: 0.2°C)

**Acceptance Criteria:**
- Drifted sensor is flagged as OOS within 110ms persistence window
- Transient outliers do not trigger OOS flags
- Sensor with expected gradient is not flagged
- Degraded neighbor (drifting) does not contaminate baseline

---

### 4.5 Outlier Filtering (FR-5)
**Requirement:** The system shall filter transient outliers and handle edge cases.

**Details:**
- Outliers are detected using:
  - Interquartile range (IQR) method or median absolute deviation
  - Configurable sensitivity (e.g., 1.5×IQR)
- Transient outliers (single sample spikes) are marked but not counted toward baseline
- Edge cases handled:
  - Startup phase (insufficient historical data)
  - Missing readings or gaps (stale data flagged)
  - Asynchronous sampling (timestamps used to align readings)
  - Insufficient neighbors (confidence reduced; alarm suppressed)

**Acceptance Criteria:**
- Single-spike outlier does not falsely trigger freeze/OOS
- Confidence metric reflects data completeness
- System operates correctly during startup (no false alarms)

---

### 4.6 Sensor Health State Machine (FR-6)
**Requirement:** The system shall maintain per-sensor health state with confidence and hysteresis.

**Details:**
- State Machine:
  ```
  [HEALTHY] → (evidence accumulates) → [SUSPECT] → (evidence persists) → [FAULTY]
  [FAULTY] → (repair confirmed, manual reset) → [HEALTHY]
  ```
- Hysteresis: Once flagged as SUSPECT, sensor must show sustained improvement to return to HEALTHY
- Confidence metric: 0-100%, updated with each new reading
  - Increases when sensor aligns with neighbors
  - Decreases when deviation detected
- Per-sensor tracking:
  - Current state (HEALTHY/SUSPECT/FAULTY)
  - Timestamp of last state transition
  - Confidence value
  - Fault type (FREEZE, OOS, UNKNOWN)
  - Evidence summary (median neighbor value, observed deviation, duration)

**Acceptance Criteria:**
- State transitions are logged with timestamps
- False alarms do not cause flapping (hysteresis prevents oscillation)
- Confidence correctly reflects diagnostic certainty
- State recovers correctly after confirmed repair

---

### 4.7 Calibration Offset Reset (FR-7)
**Requirement:** The system shall expose an API to reset calibration offsets upon repair confirmation.

**Details:**
- Reset function clears long-term calibration offsets back to zero
- Reset is triggered via:
  - External API call (HTTP/MQTT command from cloud)
  - Manual reset command from local terminal
- Offset reset must be logged with timestamp and justification
- State transitions to HEALTHY after successful reset
- Resetting does not affect historical fault records

**Acceptance Criteria:**
- Offset reset is callable and executes without errors
- State transitions to HEALTHY with reset timestamp
- Historical fault data is preserved for audit trail
- Unauthorized resets are rejected (if authentication implemented)

---

### 4.8 Continuous Operation & Bounded Diagnostics (FR-8)
**Requirement:** The system shall maintain bounded resource usage and prevent faulty sensors from contaminating baselines.

**Details:**
- Circular buffers for rolling windows (fixed memory footprint)
- Faulty sensor data is excluded from:
  - Neighbor baseline calculations
  - Median/mean computations
  - Variance/std. dev. calculations
- Diagnostic computation time is bounded (deterministic)
- Logging and buffer management do not cause memory leaks
- System continues operating even if a sensor fails (graceful degradation)

**Acceptance Criteria:**
- Memory usage remains constant regardless of runtime duration
- No deadlocks or race conditions
- Faulty sensor is automatically excluded from neighbor baselines
- System operates correctly with 1, 2, or 3+ neighbors

---

### 4.9 Auditability & Evidence Tracking (FR-9)
**Requirement:** All diagnostic decisions shall be auditable with documented assumptions and supporting evidence.

**Details:**
- For each fault flag, the system logs:
  - Timestamp
  - Sensor ID and measurement
  - Fault type (FREEZE/OOS)
  - Neighbor readings and median value
  - Deviation amount and persistence duration
  - Confidence level
  - Configuration parameters used (window size, threshold, etc.)
- JSON format for structured audit records
- Evidence structure:
  ```json
  {
    "timestamp": "ISO8601",
    "sensor_id": "string",
    "fault_type": "FREEZE|OOS",
    "sensor_reading_celsius": 45.2,
    "neighbor_median_celsius": 42.1,
    "deviation_celsius": 3.1,
    "persistence_ms": 150,
    "confidence_percent": 85,
    "neighbors_used": ["TEMP_A2", "TEMP_A3"],
    "config_drift_threshold": 1.5,
    "config_window_ms": 3600000
  }
  ```

**Acceptance Criteria:**
- Every fault decision is logged with complete evidence chain
- Configuration parameters are captured with audit record
- Logs can be analyzed to understand decision-making
- All evidence is machine-readable (JSON) and human-interpretable

---

## 5. Configuration & Parameterization

### 5.1 Configuration File Structure (JSON)
```json
{
  "service_name": "bms_diagnostics",
  "version": "1.0",
  "sensors": [
    {
      "id": "TEMP_A1",
      "type": "temperature",
      "unit": "celsius",
      "min_value": -20,
      "max_value": 80,
      "neighbors": ["TEMP_A2", "TEMP_A3"],
      "operating_zone": "zone_a"
    }
  ],
  "diagnostics": {
    "freeze_detection": {
      "enabled": true,
      "stdev_threshold_celsius": 0.05,
      "neighbor_variation_threshold_celsius": 0.5,
      "window_duration_ms": 3600000,
      "persistence_duration_ms": 150
    },
    "drift_detection": {
      "enabled": true,
      "tolerance_celsius": 1.5,
      "persistence_duration_ms": 110,
      "min_neighbors": 2,
      "spatial_gradient_tolerance_celsius": 0.2
    },
    "outlier_filter": {
      "enabled": true,
      "method": "iqr",
      "sensitivity": 1.5
    }
  },
  "reporting": {
    "mqtt": {
      "broker": "a1234567890xyz-ats.iot.us-east-1.amazonaws.com",
      "port": 8883,
      "topic_diagnostics": "bms/diagnostics/faults",
      "topic_health": "bms/diagnostics/health",
      "qos": 1
    },
    "audit_log_path": "/var/log/bms_diagnostics.log"
  }
}
```

### 5.2 Tunable Parameters
| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| `freeze_stdev_threshold` | 0.05°C | 0.01–1.0°C | Sensor static threshold |
| `drift_tolerance` | 1.5°C | 0.5–5.0°C | Max allowed deviation |
| `drift_persistence_ms` | 110ms | 50–1000ms | OOS detection window |
| `freeze_window_duration` | 1 hour | 10min–24hr | Rolling window for variance |
| `min_neighbors` | 2 | 1–5 | Minimum valid neighbors |
| `hysteresis_window_ms` | 500ms | 100–2000ms | State transition hysteresis |

---

## 6. Non-Functional Requirements

### 6.1 Performance (NFR-1)
- **Latency:** Diagnostic decisions within 110ms of measurement
- **Throughput:** Support up to 100 sensors at 100Hz sampling rate
- **Memory:** Circular buffers use <10 MB for 1-hour window at 100Hz
- **CPU:** <5% CPU utilization on typical BMS edge processor (ARM Cortex-M4)

### 6.2 Reliability (NFR-2)
- **Availability:** >99.5% uptime (continuous operation)
- **MTBF:** No memory leaks or hangs after 1-month continuous operation
- **Data Integrity:** No loss of sensor readings in transit

### 6.3 Maintainability (NFR-3)
- **Modularity:** Clear separation: data acquisition, diagnosis, reporting
- **Code Quality:** 
  - Static analysis with zero critical defects
  - Unit test coverage >80%
  - Documented API and design patterns
- **Debugging:** Trace-level logging available (can be disabled in production)

### 6.4 Security (NFR-4)
- **MQTT Security:** TLS 1.2+ with certificate validation
- **Authentication:** AWS IoT device certificates (auto-provisioned)
- **Secrets:** No hardcoded credentials in source; environment or config-based
- **Vulnerability Scanning:** Regular scans (Trivy, OWASP Dependency Check)

### 6.5 Scalability (NFR-5)
- **Horizontal:** Multiple edge devices independently operate
- **Vertical:** Configuration supports 1–100+ sensors per device
- **Cloud:** DynamoDB auto-scales; SNS handles unlimited subscribers

### 6.6 Portability (NFR-6)
- **Platform:** Linux, embedded Linux, real-time OS (FreeRTOS)
- **Architecture:** x86, ARM (32/64-bit)
- **Compiler:** GCC, Clang
- **Dependencies:** Minimal external libs (C standard library, mbedTLS for MQTT)

---

## 7. System Architecture

### 7.1 Module Decomposition
```
┌─────────────────────────────────────────────────┐
│         BMS Diagnostic Engine (C)                │
├─────────────────────────────────────────────────┤
│                                                  │
│  ┌─────────────────┐  ┌──────────────────┐     │
│  │  Data Acquisition│  │ Sensor Config     │     │
│  │  Module          │  │ Topology          │     │
│  └────────┬─────────┘  └──────────────────┘     │
│           │                                      │
│           ▼                                      │
│  ┌─────────────────────────────────────────┐   │
│  │   Rolling Window Buffer                  │   │
│  │   (Circular Buffer per Sensor)           │   │
│  └────────┬──────────────────────────────────┘   │
│           │                                      │
│           ▼                                      │
│  ┌──────────────────────────────────────────┐  │
│  │  Diagnostic Engine Core                   │  │
│  │  ├─ Freeze Detection                      │  │
│  │  ├─ Drift Detection                       │  │
│  │  ├─ Outlier Filter                        │  │
│  │  └─ State Machine (per sensor)            │  │
│  └────────┬───────────────────────────────────┘  │
│           │                                      │
│           ▼                                      │
│  ┌──────────────────────────────────────────┐  │
│  │  Reporting & Audit                        │  │
│  │  ├─ JSON Serialization                    │  │
│  │  ├─ MQTT Publisher                        │  │
│  │  └─ Local Audit Log                       │  │
│  └──────────────────────────────────────────┘  │
│                                                  │
└─────────────────────────────────────────────────┘
           │
           │ MQTT (TLS)
           ▼
     ┌──────────────┐
     │ AWS IoT Core │
     │  Topic Rules │
     └──────┬───────┘
            │
     ┌──────┴──────────┐
     ▼                 ▼
  DynamoDB         CloudWatch
  (Faults)         (Alarms)
```

### 7.2 Module Specifications

#### 7.2.1 Data Acquisition Module (`src/data_acquisition.h/c`)
- **Responsibility:** Ingest sensor readings with timestamps
- **Interface:**
  ```c
  typedef struct {
    char* sensor_id;
    double value;
    uint64_t timestamp_us;
    uint8_t sample_quality;  // 0-100 confidence
  } SensorReading;
  
  int bms_ingest_reading(const SensorReading* reading);
  ```
- **Behavior:** Store in circular buffer; handle missing/late arrivals

#### 7.2.2 Rolling Window Buffer (`src/rolling_window.h/c`)
- **Responsibility:** Maintain fixed-size circular buffer of readings per sensor
- **Interface:**
  ```c
  typedef struct RollingWindow RollingWindow;
  
  RollingWindow* rolling_window_create(size_t max_samples);
  void rolling_window_push(RollingWindow* win, double value);
  double rolling_window_stdev(RollingWindow* win);
  double rolling_window_median(RollingWindow* win);
  void rolling_window_free(RollingWindow* win);
  ```
- **Behavior:** O(1) insertion; compute variance/median on-demand

#### 7.2.3 Diagnostic Engine Core (`src/bms_diagnostics.h/c`)
- **Responsibility:** Perform freeze & drift detection; manage state machine
- **Interface:**
  ```c
  typedef enum {
    HEALTH_HEALTHY = 0,
    HEALTH_SUSPECT = 1,
    HEALTH_FAULTY = 2
  } SensorHealth;
  
  typedef struct {
    SensorHealth state;
    uint8_t confidence;
    char* fault_type;
    uint64_t last_transition_us;
  } SensorHealthState;
  
  int bms_diagnose(const SensorReading* reading, SensorHealthState* state_out);
  int bms_reset_calibration(const char* sensor_id);
  ```
- **Behavior:** Compare sensor against neighbors; apply state machine logic

#### 7.2.4 Configuration Module (`src/config.h/c`)
- **Responsibility:** Parse and validate configuration file (JSON)
- **Interface:**
  ```c
  typedef struct Config Config;
  
  Config* config_load_json(const char* filepath);
  const char** config_get_neighbors(Config* cfg, const char* sensor_id);
  double config_get_drift_threshold(Config* cfg);
  void config_free(Config* cfg);
  ```
- **Behavior:** Validate on load; fail fast on errors

#### 7.2.5 MQTT & Reporting Module (`src/mqtt_reporter.h/c`)
- **Responsibility:** Serialize faults to JSON; publish via MQTT; log audit trail
- **Interface:**
  ```c
  typedef struct MQTTReporter MQTTReporter;
  
  MQTTReporter* mqtt_reporter_create(const char* config_path);
  int mqtt_publish_fault(MQTTReporter* reporter, const SensorHealthState* fault);
  int mqtt_publish_health(MQTTReporter* reporter, const char* sensor_id);
  void mqtt_reporter_free(MQTTReporter* reporter);
  ```
- **Behavior:** Queue offline; retry on connection loss; log all events

---

## 8. Testing Requirements

### 8.1 Unit Tests (CUnit Framework)
**Test Coverage Target:** >80%

#### Test Categories:
1. **Freeze Detection Tests**
   - `test_frozen_sensor_detected()` — Static sensor, changing neighbors
   - `test_healthy_sensor_not_flagged()` — Normal variation, no alert
   - `test_transient_freeze_ignored()` — Brief stasis, no alert

2. **Drift Detection Tests**
   - `test_drifted_sensor_flagged()` — Deviation >1.5°C for >110ms
   - `test_healthy_offset_ignored()` — Expected gradient, no alert
   - `test_insufficient_neighbors()` — Only 1 neighbor, no alert

3. **Outlier Filter Tests**
   - `test_transient_spike_filtered()` — Single spike, no fault
   - `test_sustained_outlier_flagged()` — Persistent deviation, alert

4. **State Machine Tests**
   - `test_healthy_to_suspect()` — Evidence accumulation
   - `test_suspect_to_faulty()` — Persistence
   - `test_faulty_to_healthy_on_reset()` — Calibration reset

5. **Edge Cases**
   - `test_startup_phase()` — Insufficient data, no false alarms
   - `test_missing_neighbors()` — Confidence reduced
   - `test_asynchronous_timestamps()` — Correct alignment

### 8.2 Integration Tests
- Mock sensor data with realistic variations
- Verify end-to-end: ingestion → diagnosis → MQTT publish
- Test offline queuing and retry logic

### 8.3 Test Data Scenarios
| Scenario | Description | Expected Result |
|----------|-------------|-----------------|
| Frozen | Sensor static; neighbors vary | FREEZE flag |
| Drifting | Sensor offset; neighbors stable | OOS flag |
| Healthy | All sensors in variation band | HEALTHY |
| Transient Spike | Single outlier | Filtered; no fault |
| Missing Data | Stale/late readings | Reduced confidence |
| Startup | <1min of data | No alarms; confidence low |
| Recovery | Faulty sensor reset | Transition to HEALTHY |

---

## 9. Deployment & Operations

### 9.1 Build & Package
- **Build System:** CMake
- **Compiler:** GCC/Clang with `-Wall -Werror` (no warnings)
- **Artifact:** `libdiagnostics.a` (static library) or `.so` (shared)
- **Docker:** Multi-stage Dockerfile for slim image (<50MB)

### 9.2 Configuration Management
- Configuration file: `/etc/bms_diagnostics/config.json` (or env-specified)
- Certificates: AWS IoT Core certificates in `/etc/bms_diagnostics/certs/`
- Logs: `/var/log/bms_diagnostics.log` or syslog

### 9.3 AWS Deployment
**Terraform Infrastructure:**
- AWS IoT Core topic rule: `bms/diagnostics/faults` → DynamoDB
- DynamoDB table: `BmsSensorFaults` (partition key: sensor_id, sort key: timestamp)
- CloudWatch Alarms: High/Critical faults → SNS topic
- SNS: Subscribers (email, SMS, Slack, etc.)

### 9.4 Monitoring & Alerting
- **Metrics:** Fault count, false positive rate, MQTT connection status
- **Alarms:** 
  - Device offline >5 minutes
  - >5 FREEZE flags in 1 hour
  - >10 OOS flags in 1 hour

---

## 10. Interface Specifications

### 10.1 Data Ingestion Interface (User-provided)
Sensors must call:
```c
int bms_ingest_reading(const SensorReading* reading);
```
Where `SensorReading` includes sensor_id, value, timestamp, quality flag.

### 10.2 Reset/Control Interface (External)
```bash
# HTTP/MQTT command to reset calibration
POST /api/diagnostics/reset?sensor_id=TEMP_A1
# or via MQTT:
mosquitto_pub -t "bms/diagnostics/commands/reset" -m '{"sensor_id":"TEMP_A1"}'
```

### 10.3 Query Interface (Diagnostic Status)
```c
int bms_get_sensor_health(const char* sensor_id, SensorHealthState* state_out);
int bms_get_all_sensor_health(SensorHealthState** states_out, size_t* count_out);
```

---

## 11. Quality Assurance

### 11.1 Code Review Checklist
- [ ] Module responsibilities clearly separated
- [ ] No memory leaks (valgrind/ASAN)
- [ ] No race conditions (thread-safe if concurrent)
- [ ] Configuration parsing robust (edge cases tested)
- [ ] Error handling comprehensive (return codes, logging)

### 11.2 Static Analysis
- **cppcheck:** No high-severity issues
- **clang-analyzer:** No defects
- **clang-format:** Code style conformance

### 11.3 Security Review
- [ ] MQTT TLS certificates validated
- [ ] No hardcoded credentials
- [ ] Input validation (config file, sensor data)
- [ ] Buffer overflow protection
- [ ] Integer overflow checks

### 11.4 Performance Profiling
- **Memory:** Peak usage <10MB (1-hour window, 100Hz, 100 sensors)
- **CPU:** <5% on ARM Cortex-M4 (100 sensors, 100Hz)
- **Latency:** Diagnostic decision <110ms from reading

---

## 12. Documentation Requirements

### 12.1 Code Documentation
- Function headers with purpose, params, return values
- Struct member descriptions
- Algorithm explanations (variance, median, state machine)

### 12.2 User Documentation
- **Installation Guide:** Build, deploy, configure
- **Configuration Reference:** All parameters explained
- **API Documentation:** Public function signatures
- **Troubleshooting Guide:** Common issues and solutions

### 12.3 Design Documentation
- **Architecture Diagram:** Module interactions
- **State Machine Diagram:** HEALTHY ↔ SUSPECT ↔ FAULTY
- **Data Flow Diagram:** Ingestion → Diagnosis → Reporting
- **Algorithm Pseudo-code:** Freeze/drift detection logic

---

## 13. Risk & Mitigation

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| False positive freeze flags | High | Medium | Hysteresis, neighbor confirmation required |
| Faulty neighbor contaminates baseline | High | High | Exclude faulty sensors from median calc |
| Memory exhaustion over time | Medium | High | Fixed circular buffers, no dynamic alloc in hot path |
| MQTT connection loss | High | Medium | Offline queue with local storage, retry logic |
| Configuration error causes all sensors to alarm | Medium | High | Validate config on load, dry-run mode |
| Real-time deadline miss (>110ms latency) | Low | High | Profile & optimize; use fixed-time algorithms |

---

## 14. Acceptance Criteria

### 14.1 Functional Acceptance
- [ ] Freeze detection correctly flags static sensors (true positive)
- [ ] Drift detection correctly flags out-of-spec sensors (true positive)
- [ ] Healthy sensors are not falsely alarmed (false negative)
- [ ] Transient outliers are filtered (no false positive)
- [ ] State machine transitions with correct hysteresis
- [ ] Calibration reset restores HEALTHY state
- [ ] Configuration is loaded and applied correctly
- [ ] MQTT faults are published with complete evidence

### 14.2 Non-Functional Acceptance
- [ ] Memory usage stable (no growth over 1 month runtime)
- [ ] CPU usage <5% on target hardware
- [ ] Diagnostic latency <110ms (95th percentile)
- [ ] Unit test coverage >80%
- [ ] Static analysis zero critical defects
- [ ] Trivy scan: no High/Critical vulnerabilities

### 14.3 Deployment Acceptance
- [ ] Terraform successfully deploys AWS infrastructure
- [ ] Device connects to IoT Core and authenticates
- [ ] Faults appear in DynamoDB within 10 seconds
- [ ] CloudWatch alarms trigger on configured thresholds
- [ ] Logs are auditable and contain all required evidence

---

## 15. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-09-28 | Design Team | Initial SWDD |

---

## Appendix A: Glossary

- **BMS:** Battery Management System
- **OOS:** Out of Specification
- **Freeze:** Sensor returning static reading despite environmental changes
- **Drift:** Persistent deviation from neighbor baseline
- **Hysteresis:** State transition requires sustained evidence, not single spike
- **Topology:** Configured sensor relationships and compatibility rules
- **Circular Buffer:** Fixed-size ring buffer for efficient memory use
- **MQTT:** Message Queuing Telemetry Transport (IoT protocol)
- **TLS:** Transport Layer Security (encryption)

---

## Appendix B: References

1. BMS_Diagnostic_Copilot_Training_Use_Cases.pdf — Use Case 4
2. AWS IoT Core Documentation — https://docs.aws.amazon.com/iot-core/
3. MQTT 3.1.1 Specification — https://mqtt.org/
4. OWASP Secure Coding Practices — https://owasp.org/

