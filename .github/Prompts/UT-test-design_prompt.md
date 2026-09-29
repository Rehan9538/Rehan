# Unit Test (UT) Design Specification & Prompt
## BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine

**Document Version:** 1.0  
**Date:** 2026-09-29  
**Project:** Battery Management System (BMS) Diagnostic Service  
**Implementation Language:** C  
**Test Framework:** CUnit  
**Status:** Active

---

## 1. Overview

This document serves as the **master prompt and specification** for all Unit Tests in the BMS Diagnostic Engine project. It provides:
- Structured test case templates
- Test categorization by module
- Configuration parameters for test execution
- Status tracking (PASS/FAIL/NOT_APPLICABLE)
- Evidence and comment fields for audit trail

### 1.1 Test Coverage Target
- **Goal:** >80% code coverage
- **Focus:** Critical path functionality (freeze detection, drift detection, state machine)
- **Framework:** CUnit (lightweight, C-native)
- **Execution:** `ctest` or `make test`

### 1.2 Test Execution Environment
```bash
# Build and run tests
$ cd /path/to/project
$ cmake -B build
$ cmake --build build
$ ctest --output-on-failure -V

# With code coverage
$ ctest --coverage
$ lcov --directory . --capture --output-file coverage.info
$ genhtml coverage.info --output-directory coverage_report
```

---

## 2. Test Case Template Structure

Each test case follows this standardized format for consistency and traceability:

```markdown
### Test ID: [MODULE]_UT_[SEQUENCE]_[VARIANT]

**Test Name:** [Descriptive Name]

**Category:** [Freeze Detection | Drift Detection | Outlier Filter | State Machine | Edge Cases | Configuration | MQTT/Reporting]

**Priority:** [P0-Critical | P1-High | P2-Medium | P3-Low]

**Module(s) Under Test:** [src/module.h/c]

**Test Description:**
[Clear description of what is being tested, including purpose and scope]

**Prerequisites:**
- [Dependency 1]
- [Dependency 2]
- Configuration file loaded with specified parameters

**Test Configuration Parameters:**
| Parameter | Value | Notes |
|-----------|-------|-------|
| `freeze_stdev_threshold` | 0.05°C | ... |
| `drift_tolerance` | 1.5°C | ... |

**Test Data / Mock Inputs:**
\`\`\`c
// Mock sensor readings to be ingested
SensorReading inputs[] = {
  {"TEMP_A1", 45.0, 0, 100},      // timestamp_us=0, quality=100
  {"TEMP_A1", 45.0, 1000000, 100}, // 1 second later
  ...
};
\`\`\`

**Test Steps:**
1. [Setup step: Initialize mock data, configure system]
2. [Action step: Call function under test]
3. [Verification step: Assert expected outcome]
4. [Cleanup step: Free resources]

**Expected Results:**
- [Assertion 1: Condition that must be true]
- [Assertion 2: State or value expected]
- [Assertion 3: No side effects or errors]

**Test Assertions (CUnit Code):**
\`\`\`c
// Example CUnit assertions
CU_ASSERT_EQUAL(state.health, HEALTH_FAULTY);
CU_ASSERT_DOUBLE_EQUAL(sensor_value, 45.2, 0.01);
CU_ASSERT_PTR_NOT_NULL(result);
CU_ASSERT_STRING_EQUAL(fault_type, "FREEZE");
\`\`\`

**Test Status:** [PASS | FAIL | NOT_APPLICABLE | NOT_STARTED | BLOCKED]

**Status Justification:**
[If PASS: date passed, environment details]
[If FAIL: failure reason, steps to reproduce]
[If NOT_APPLICABLE: reason why test is not applicable]
[If BLOCKED: blocker details and dependent test ID]

**Defects Found:**
- [If applicable: defect ID, description, linked issue]

**Comments:**
[Additional notes, edge cases considered, known limitations, follow-up investigations]

**Last Updated:** [Date, Author Name]

**Related Test Cases:**
- [Reference to related tests that share setup or data]

---
\`\`\`

---

## 3. Test Categories & Comprehensive Test Specifications

### 3.1 FREEZE DETECTION TESTS (FREEZE_UT_XXX)

#### Test ID: FREEZE_UT_001_BASIC_DETECTION

**Test Name:** Frozen Sensor Basic Detection

**Category:** Freeze Detection

**Priority:** P0-Critical

**Module(s) Under Test:** src/bms_diagnostics.h/c, src/rolling_window.h/c

**Test Description:**
Verify that a sensor whose reading remains static (standard deviation <0.05°C) while at least one compatible neighbor shows significant variation (>0.5°C) is flagged as frozen within the configured persistence window.

**Prerequisites:**
- System initialized with 3 temperature sensors in same zone (TEMP_A1, TEMP_A2, TEMP_A3)
- Sensors configured as neighbors
- Configuration: freeze_stdev_threshold=0.05°C, neighbor_variation=0.5°C, persistence=150ms

**Test Configuration Parameters:**
| Parameter | Value | Notes |
|-----------|-------|-------|
| freeze_stdev_threshold | 0.05°C | Default threshold |
| neighbor_variation_threshold | 0.5°C | Minimum neighbor variation |
| freeze_window_duration | 1000ms | Reduced for UT (1-hour in production) |
| freeze_persistence_duration | 150ms | Detection window |
| num_sensors | 3 | TEMP_A1 (frozen), TEMP_A2, TEMP_A3 (varying) |
| num_samples | 50 | Over 1000ms window |

**Test Data / Mock Inputs:**
- TEMP_A1: Frozen at 45.0°C (stddev = 0.001°C)
- TEMP_A2, TEMP_A3: Varying between 44.0-46.0°C (stddev = 0.8°C)

**Test Steps:**
1. Initialize diagnostic engine with mock config
2. Ingest test data in chronological order
3. After 150ms, call bms_diagnose() for TEMP_A1
4. Retrieve sensor health state
5. Verify state transitions to SUSPECT/FAULTY with FREEZE fault type
6. Check evidence: deviation recorded, confidence metric appropriate

**Expected Results:**
- state.health == HEALTH_SUSPECT or HEALTH_FAULTY
- state.fault_type == "FREEZE"
- state.confidence >= 70 (high confidence)
- Audit log contains complete evidence
- Neighboring sensors remain HEALTHY

**Test Status:** NOT_STARTED

**Defects Found:** None

**Comments:**
- Uses reduced window duration for speed (1000ms vs 1 hour)
- Hysteresis logic not tested in basic case (see FREEZE_UT_004)

**Last Updated:** 2026-09-29, Design Team

---

#### Test ID: FREEZE_UT_002_HEALTHY_NOT_FLAGGED

**Test Name:** Healthy Sensor Not Falsely Flagged as Frozen

**Category:** Freeze Detection

**Priority:** P0-Critical

**Module(s) Under Test:** src/bms_diagnostics.h/c

**Test Description:**
Verify that a sensor with normal variation (stddev >0.05°C) is NOT flagged as frozen. Prevents false positives.

**Prerequisites:**
- System initialized with 3 temperature sensors
- All sensors configured as neighbors
- Configuration: freeze_stdev_threshold=0.05°C

**Test Status:** NOT_STARTED

---

#### Test ID: FREEZE_UT_003_TRANSIENT_IGNORED

**Test Name:** Transient Freeze Ignored (No Alert)

**Category:** Freeze Detection

**Priority:** P1-High

**Test Description:**
Verify that brief stasis periods (<150ms) do NOT trigger false freeze alerts. Hysteresis required.

**Test Status:** NOT_STARTED

---

#### Test ID: FREEZE_UT_004_HYSTERESIS_RECOVERY

**Test Name:** Frozen Sensor Recovery with Hysteresis

**Category:** Freeze Detection

**Priority:** P1-High

**Test Description:**
Verify that flagged frozen sensor requires sustained recovery (not just one good reading) before transitioning back to HEALTHY.

**Test Status:** NOT_STARTED

---

### 3.2 DRIFT DETECTION TESTS (DRIFT_UT_XXX)

#### Test ID: DRIFT_UT_001_OOS_DETECTION

**Test Name:** Out-of-Specification (OOS) Drift Detection

**Category:** Drift Detection

**Priority:** P0-Critical

**Test Description:**
Verify that sensor reading deviating >1.5°C from neighbor median for >110ms is flagged as OOS.

**Prerequisites:**
- System initialized with 3 temperature sensors
- Sensors configured as neighbors
- Configuration: drift_tolerance=1.5°C, drift_persistence=110ms

**Test Status:** NOT_STARTED

---

#### Test ID: DRIFT_UT_002_GRADIENT_NOT_FLAGGED

**Test Name:** Healthy Sensor with Gradient Not Falsely Flagged

**Category:** Drift Detection

**Priority:** P0-Critical

**Test Description:**
Verify that expected spatial gradient (e.g., 0.2°C between zones) does NOT trigger OOS alert.

**Test Status:** NOT_STARTED

---

#### Test ID: DRIFT_UT_003_INSUFFICIENT_NEIGHBORS

**Test Name:** Insufficient Neighbors - Confidence Reduced, No Alert

**Category:** Drift Detection

**Priority:** P1-High

**Test Description:**
Verify that with <2 valid neighbors, NO OOS alert is triggered (insufficient evidence). Confidence reduced.

**Test Status:** NOT_STARTED

---

### 3.3 OUTLIER FILTER TESTS (OUTLIER_UT_XXX)

#### Test ID: OUTLIER_UT_001_TRANSIENT_SPIKE

**Test Name:** Transient Spike Filtered - Not Counted as Fault

**Category:** Outlier Filter

**Priority:** P1-High

**Test Description:**
Verify that isolated spike (>3σ from mean) is detected as outlier, excluded from baseline, and does NOT trigger alert.

**Test Status:** NOT_STARTED

---

### 3.4 STATE MACHINE TESTS (STATE_UT_XXX)

#### Test ID: STATE_UT_001_TRANSITIONS

**Test Name:** State Machine Transitions (HEALTHY → SUSPECT → FAULTY)

**Category:** State Machine

**Priority:** P1-High

**Test Description:**
Verify correct state progression through HEALTHY → SUSPECT → FAULTY → HEALTHY.

**Test Status:** NOT_STARTED

---

### 3.5 EDGE CASES & ERROR HANDLING (EDGE_UT_XXX)

#### Test ID: EDGE_UT_001_STARTUP_PHASE

**Test Name:** Startup Phase - No False Alarms with Insufficient Data

**Category:** Edge Cases

**Priority:** P1-High

**Test Description:**
Verify that during startup (<1 minute data), NO false freeze/drift alarms triggered. Confidence remains low.

**Test Status:** NOT_STARTED

---

#### Test ID: EDGE_UT_002_MISSING_STALE_DATA

**Test Name:** Missing or Stale Data - Handled Gracefully

**Category:** Edge Cases

**Priority:** P1-High

**Test Description:**
Verify correct handling of missing readings, stale timestamps, and data gaps without crashes or false diagnostics.

**Test Status:** NOT_STARTED

---

#### Test ID: EDGE_UT_003_ASYNC_TIMESTAMPS

**Test Name:** Asynchronous Timestamps - Correct Alignment

**Category:** Edge Cases

**Priority:** P2-Medium

**Test Description:**
Verify out-of-order readings are correctly aligned by timestamp (not arrival order).

**Test Status:** NOT_STARTED

---

### 3.6 CONFIGURATION & RESET TESTS

#### Test ID: CONFIG_UT_001_LOAD_VALIDATE

**Test Name:** Configuration Load and Validation

**Category:** Configuration

**Priority:** P1-High

**Test Description:**
Verify JSON config correctly loaded, parsed, validated. Invalid configs fail fast with clear errors.

**Test Status:** NOT_STARTED

---

#### Test ID: RESET_UT_001_CALIBRATION_RESET

**Test Name:** Calibration Offset Reset - State Recovery

**Category:** Reset/Recovery

**Priority:** P1-High

**Test Description:**
Verify reset API successfully clears offsets, transitions FAULTY→HEALTHY, and logs audit trail.

**Test Status:** NOT_STARTED

---

### 3.7 MQTT & REPORTING TESTS

#### Test ID: MQTT_UT_001_FAULT_PUBLISH

**Test Name:** MQTT - Fault Event Publishing

**Category:** MQTT/Reporting

**Priority:** P1-High

**Test Description:**
Verify diagnostic faults serialized to JSON and published to AWS IoT Core topic with correct schema.

**Test Status:** NOT_STARTED

---

#### Test ID: MQTT_UT_002_OFFLINE_QUEUE

**Test Name:** MQTT - Offline Queuing and Retry

**Category:** MQTT/Reporting

**Priority:** P1-High

**Test Description:**
Verify faults queued locally when offline; published on reconnect (bounded queue size).

**Test Status:** NOT_STARTED

---

## 4. Test Execution & Reporting

### 4.1 Test Execution Command
\`\`\`bash
# Run all unit tests
$ cd /path/to/project
$ cmake -B build && cd build
$ ctest --output-on-failure -V

# Run tests for specific module
$ ctest -R "FREEZE" --output-on-failure
$ ctest -R "DRIFT" --output-on-failure
\`\`\`

### 4.2 Test Status Tracking Table

| Test ID | Test Name | Status | Last Run | Coverage | Comments |
|---------|-----------|--------|----------|----------|----------|
| FREEZE_UT_001 | Frozen Sensor Detection | NOT_STARTED | — | — | — |
| FREEZE_UT_002 | Healthy Not Flagged | NOT_STARTED | — | — | — |
| FREEZE_UT_003 | Transient Ignored | NOT_STARTED | — | — | — |
| FREEZE_UT_004 | Hysteresis Recovery | NOT_STARTED | — | — | — |
| DRIFT_UT_001 | OOS Detection | NOT_STARTED | — | — | — |
| DRIFT_UT_002 | Gradient Not Flagged | NOT_STARTED | — | — | — |
| DRIFT_UT_003 | Insufficient Neighbors | NOT_STARTED | — | — | — |
| OUTLIER_UT_001 | Transient Spike | NOT_STARTED | — | — | — |
| STATE_UT_001 | State Transitions | NOT_STARTED | — | — | — |
| EDGE_UT_001 | Startup Phase | NOT_STARTED | — | — | — |
| EDGE_UT_002 | Missing Data | NOT_STARTED | — | — | — |
| EDGE_UT_003 | Async Timestamps | NOT_STARTED | — | — | — |
| CONFIG_UT_001 | Config Load | NOT_STARTED | — | — | — |
| RESET_UT_001 | Calibration Reset | NOT_STARTED | — | — | — |
| MQTT_UT_001 | Fault Publishing | NOT_STARTED | — | — | — |
| MQTT_UT_002 | Offline Queue | NOT_STARTED | — | — | — |

---

## 5. Acceptance Criteria for Test Suite

- [ ] 16+ test cases defined (all categories covered)
- [ ] Each test has clear pass/fail criteria
- [ ] Test data properly generated (mock sensors with realistic variation)
- [ ] Code coverage >80% achieved
- [ ] All CRITICAL (P0) tests passing
- [ ] Documentation complete
- [ ] CI/CD integration ready (ctest in GitHub Actions)
- [ ] Defect tracking system in place

---

## 6. Test Metrics Target

| Metric | Target | Note |
|--------|--------|------|
| Code Coverage | >80% | Lines of code executed |
| Test Pass Rate | 100% | All tests must pass for release |
| Critical Test Pass | 100% | P0 tests non-negotiable |
| Test Execution Time | <30s | Total suite runtime |

---

**Version:** 1.0  
**Date:** 2026-09-29  
**Owner:** Design Team  
**Status:** Active

For each test case, use the template above to ensure:
✓ Test ID properly formatted (MODULE_UT_SEQUENCE_VARIANT)
✓ Clear test name and category
✓ Configuration parameters documented
✓ Test data and steps well-defined
✓ Expected results and assertions specific
✓ Status tracking (PASS/FAIL/NOT_APPLICABLE/NOT_STARTED/BLOCKED)
✓ Comments and related test links maintained
✓ Evidence preserved for audit trail
