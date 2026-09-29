# Unit Test (UT) Design & Definition Skill
## BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine

**Skill Version:** 1.0  
**Date:** 2026-09-29  
**Purpose:** Define what constitutes a valid, well-formed unit test case for BMS diagnostics.

---

## Overview

This skill provides the **schema, rules, and templates** for unit test design in the BMS diagnostic engine project. It is the single source of truth for:
- Test structure and required fields
- Naming conventions and ID formats
- Status values and test categories
- CUnit assertion patterns
- Configuration parameter rules

**This skill is reusable by:**
- ✅ Test designers writing new tests
- ✅ Code reviewers validating test PRs
- ✅ Linters checking test format compliance
- ✅ Test generators auto-creating test stubs
- ✅ Documentation generators referencing templates
- ✅ Agents validating tests during execution

---

## Authoritative Source

This skill is based on: `.github/Prompts/UT-test-design_prompt.md`

**In case of conflict:** The prompt file overrides this skill.

---

## 1. Test Schema Rules

### 1.1 Test ID Format

**Rule:** Must be `MODULE_UT_SEQUENCE_VARIANT`

**Format Breakdown:**
- `MODULE`: Test category prefix (FREEZE, DRIFT, OUTLIER, STATE, EDGE, CONFIG, RESET, MQTT)
- `UT`: Literal "UT" (means "Unit Test")
- `SEQUENCE`: 3-digit number (001-999)
- `VARIANT`: Short description of variant (e.g., BASIC_DETECTION, HYSTERESIS_RECOVERY)

**Examples (VALID):**
- `FREEZE_UT_001_BASIC_DETECTION` ✅
- `DRIFT_UT_001_OOS_DETECTION` ✅
- `STATE_UT_001_TRANSITIONS` ✅
- `EDGE_UT_002_MISSING_DATA` ✅

**Examples (INVALID):**
- `TC_001` ❌ (wrong format)
- `FREEZE_001_DETECTION` ❌ (missing UT)
- `FREEZE_UT_001` ❌ (missing variant description)
- `Freeze_UT_001_Detection` ❌ (wrong case)

### 1.2 Test Status Values

**Allowed Values (EXACT):**
- `PASS` - Test executed successfully, all assertions passed
- `FAIL` - Test executed but one or more assertions failed
- `NOT_APPLICABLE` - Test cannot be run (missing dependency, platform not supported, etc.)
- `NOT_STARTED` - Test has not been executed yet
- `BLOCKED` - Test cannot run due to external blocker (dependent test failed, environment issue, etc.)

**Disallowed Forms (REJECT):**
- ❌ `Passed`, `Failed` (lowercase)
- ❌ `PASSED`, `FAILED` (alternate form)
- ❌ `Skipped` (not in schema)
- ❌ `Not Run`, `NotRun` (use NOT_STARTED instead)
- ❌ `SUCCESS`, `ERROR` (use PASS/FAIL)

### 1.3 Test Categories (7 Types)

All tests must be assigned to exactly one category:

| Category | Prefix | Description | Example |
|----------|--------|-------------|---------|
| Freeze Detection | FREEZE_UT_* | Sensor freeze detection logic | FREEZE_UT_001_BASIC_DETECTION |
| Drift Detection | DRIFT_UT_* | Out-of-specification drift detection | DRIFT_UT_001_OOS_DETECTION |
| Outlier Filter | OUTLIER_UT_* | Transient spike/outlier filtering | OUTLIER_UT_001_TRANSIENT_SPIKE |
| State Machine | STATE_UT_* | State transition logic | STATE_UT_001_TRANSITIONS |
| Edge Cases | EDGE_UT_* | Error handling and edge cases | EDGE_UT_001_STARTUP_PHASE |
| Configuration & Reset | CONFIG_UT_*, RESET_UT_* | Config loading and reset operations | CONFIG_UT_001_LOAD_VALIDATE |
| MQTT/Reporting | MQTT_UT_* | MQTT publishing and offline queue | MQTT_UT_001_FAULT_PUBLISH |

### 1.4 Priority Levels

All tests must have exactly one priority:

| Priority | Level | Meaning |
|----------|-------|---------|
| P0-Critical | MUST PASS | Core functionality, blocks release if failed |
| P1-High | SHOULD PASS | Important feature, may defer to next release |
| P2-Medium | NICE TO PASS | Enhancement, lower urgency |
| P3-Low | OPTIONAL | Polish, deferred by default |

**Release Readiness Rule:** All P0-Critical tests must be `PASS` before release.

---

## 2. Required Test Template Fields (18 Fields)

Every test case MUST include all 18 fields. No field can be empty or missing.

| # | Field | Type | Required | Description |
|----|-------|------|----------|-------------|
| 1 | Test ID | Text | ✅ YES | Format: MODULE_UT_SEQUENCE_VARIANT |
| 2 | Test Name | Text | ✅ YES | Descriptive name of what is being tested |
| 3 | Category | Enum | ✅ YES | One of 7 categories above |
| 4 | Priority | Enum | ✅ YES | P0-Critical, P1-High, P2-Medium, P3-Low |
| 5 | Module(s) Under Test | Text | ✅ YES | File paths (e.g., src/bms_diagnostics.h/c) |
| 6 | Test Description | Text | ✅ YES | Clear description of what is being tested |
| 7 | Prerequisites | List | ✅ YES | Setup requirements, config params, dependencies |
| 8 | Test Configuration Parameters | Table | ✅ YES | Config table with Parameter, Value, Notes columns |
| 9 | Test Data / Mock Inputs | Code | ✅ YES | C code block with mock sensor data |
| 10 | Test Steps | List | ✅ YES | Numbered steps: setup, action, verify, cleanup |
| 11 | Expected Results | List | ✅ YES | What should happen if test passes |
| 12 | Test Assertions (CUnit Code) | Code | ✅ YES | CUnit assert macros validating results |
| 13 | Test Status | Enum | ✅ YES | PASS, FAIL, NOT_APPLICABLE, NOT_STARTED, BLOCKED |
| 14 | Status Justification | Text | ✅ YES | Why test has current status (date, reason, etc.) |
| 15 | Defects Found | List | ✅ YES | Defect IDs if FAIL (or "None" if PASS) |
| 16 | Comments | Text | ✅ YES | Edge cases, limitations, follow-ups |
| 17 | Last Updated | Text | ✅ YES | Date and author name (e.g., 2026-09-29, John Doe) |
| 18 | Related Test Cases | List | ✅ YES | Cross-references to related tests |

---

## 3. CUnit Assertion Rules

All test assertions must use CUnit format. No raw assertions or printf statements allowed.

### 3.1 Supported CUnit Assertion Macros

| Macro | Purpose | Example |
|-------|---------|---------|
| `CU_ASSERT(condition)` | Assert boolean condition true | `CU_ASSERT(result != NULL)` |
| `CU_ASSERT_EQUAL(actual, expected)` | Assert equality | `CU_ASSERT_EQUAL(state.health, HEALTH_FAULTY)` |
| `CU_ASSERT_NOT_EQUAL(actual, expected)` | Assert inequality | `CU_ASSERT_NOT_EQUAL(count, 0)` |
| `CU_ASSERT_DOUBLE_EQUAL(actual, expected, tolerance)` | Assert double with tolerance | `CU_ASSERT_DOUBLE_EQUAL(temp, 45.2, 0.01)` |
| `CU_ASSERT_PTR_NOT_NULL(ptr)` | Assert pointer not NULL | `CU_ASSERT_PTR_NOT_NULL(result)` |
| `CU_ASSERT_PTR_NULL(ptr)` | Assert pointer is NULL | `CU_ASSERT_PTR_NULL(error)` |
| `CU_ASSERT_STRING_EQUAL(actual, expected)` | Assert strings equal | `CU_ASSERT_STRING_EQUAL(fault_type, "FREEZE")` |
| `CU_ASSERT_STRING_NOT_EQUAL(actual, expected)` | Assert strings not equal | `CU_ASSERT_STRING_NOT_EQUAL(status, "OK")` |

### 3.2 CUnit Assertion Rules

**Required:**
- ✅ Each assertion must use one of supported macros above
- ✅ Each assertion must have exactly one condition/comparison
- ✅ Each assertion must be specific (not generic boolean checks)
- ✅ Each assertion must be callable C function

**Forbidden:**
- ❌ Raw `assert()` from C stdlib
- ❌ `printf()` statements for validation
- ❌ Comments claiming test passed (use assertions)
- ❌ Multiple conditions in single assertion (use separate assertions)

### 3.3 Examples

**GOOD Assertions:**
```c
CU_ASSERT_EQUAL(state.health, HEALTH_FAULTY);
CU_ASSERT_DOUBLE_EQUAL(sensor_value, 45.2, 0.01);
CU_ASSERT_PTR_NOT_NULL(result);
CU_ASSERT_STRING_EQUAL(fault_type, "FREEZE");
CU_ASSERT_NOT_EQUAL(confidence, 0);
```

**BAD Assertions:**
```c
assert(state.health == HEALTH_FAULTY);  // ❌ Uses stdlib assert
printf("Test passed\n");  // ❌ No actual assertion
if (state.health != HEALTH_FAULTY) error_count++;  // ❌ Not using CUnit
CU_ASSERT(x == 5 && y == 10);  // ❌ Multiple conditions
```

---

## 4. Test Configuration Parameters Rule

Test configuration must be documented in a table format:

**Required Table Columns:**
- `Parameter`: Name of config parameter (e.g., `freeze_stdev_threshold`)
- `Value`: Specific value for this test (e.g., `0.05°C`)
- `Notes`: Explanation or reference (e.g., `Default threshold` or `Reduced for UT (1-hour in production)`)

**Example:**
```markdown
| Parameter | Value | Notes |
|-----------|-------|-------|
| freeze_stdev_threshold | 0.05°C | Default threshold |
| neighbor_variation_threshold | 0.5°C | Minimum neighbor variation |
| freeze_window_duration | 1000ms | Reduced for UT (1-hour in production) |
| num_sensors | 3 | TEMP_A1 (frozen), TEMP_A2, TEMP_A3 (varying) |
```

---

## 5. Test Data (Mock Inputs) Rule

Test data must be provided as valid C code:

**Required:**
- ✅ Must be syntactically correct C code
- ✅ Must define mock sensor data in applicable format
- ✅ Must include comments explaining data generation
- ✅ Must follow configuration parameters defined in section above
- ✅ Must include all necessary fields (sensor ID, value, timestamp, quality)

**Example:**
```c
// Mock sensor readings to be ingested
SensorReading inputs[] = {
  {"TEMP_A1", 45.0, 0, 100},              // frozen at 45.0°C
  {"TEMP_A1", 45.0, 1000000, 100},       // 1 second later, still frozen
  {"TEMP_A2", 44.5, 0, 100},             // varying neighbor
  {"TEMP_A2", 45.5, 1000000, 100},       // varies by 1°C
  {"TEMP_A3", 45.0, 500000, 100},        // other neighbor
};
```

**Rules:**
- Data must be realistic (not all zeros or extreme values)
- Data must cover the test scenario fully
- Timestamps must be in chronological order
- Sensor IDs must match prerequisites

---

## 6. Baseline Test Set (16 Required Tests)

All 16 tests listed below are REQUIRED in every test suite. New tests can be added, but these must be present:

### Freeze Detection (4 tests)
```
FREEZE_UT_001_BASIC_DETECTION (P0-Critical)
FREEZE_UT_002_HEALTHY_NOT_FLAGGED (P0-Critical)
FREEZE_UT_003_TRANSIENT_IGNORED (P1-High)
FREEZE_UT_004_HYSTERESIS_RECOVERY (P1-High)
```

### Drift Detection (3 tests)
```
DRIFT_UT_001_OOS_DETECTION (P0-Critical)
DRIFT_UT_002_GRADIENT_NOT_FLAGGED (P0-Critical)
DRIFT_UT_003_INSUFFICIENT_NEIGHBORS (P1-High)
```

### Outlier Filter (1 test)
```
OUTLIER_UT_001_TRANSIENT_SPIKE (P1-High)
```

### State Machine (1 test)
```
STATE_UT_001_TRANSITIONS (P1-High)
```

### Edge Cases (3 tests)
```
EDGE_UT_001_STARTUP_PHASE (P1-High)
EDGE_UT_002_MISSING_DATA (P1-High)
EDGE_UT_003_ASYNC_TIMESTAMPS (P2-Medium)
```

### Configuration & Reset (2 tests)
```
CONFIG_UT_001_LOAD_VALIDATE (P1-High)
RESET_UT_001_CALIBRATION_RESET (P1-High)
```

### MQTT/Reporting (2 tests)
```
MQTT_UT_001_FAULT_PUBLISH (P1-High)
MQTT_UT_002_OFFLINE_QUEUE (P1-High)
```

**Total: 16 tests minimum**

---

## 7. Complete Test Case Template

Use this template for all new test cases. All 18 fields must be completed:

```markdown
### Test ID: [MODULE]_UT_[SEQUENCE]_[VARIANT]

**Test Name:** [Descriptive Name]

**Category:** [One of 7 categories]

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
| param1 | value1 | explanation |
| param2 | value2 | explanation |

**Test Data / Mock Inputs:**
```c
// Mock sensor readings
SensorReading inputs[] = {
  {"SENSOR_ID", value, timestamp, quality},
  ...
};
```

**Test Steps:**
1. [Setup step]
2. [Action step]
3. [Verification step]
4. [Cleanup step]

**Expected Results:**
- [Assertion 1: Condition that must be true]
- [Assertion 2: State or value expected]
- [Assertion 3: No side effects or errors]

**Test Assertions (CUnit Code):**
```c
CU_ASSERT_EQUAL(actual, expected);
CU_ASSERT_DOUBLE_EQUAL(value, 45.2, 0.01);
CU_ASSERT_PTR_NOT_NULL(result);
CU_ASSERT_STRING_EQUAL(fault_type, "FREEZE");
```

**Test Status:** [PASS | FAIL | NOT_APPLICABLE | NOT_STARTED | BLOCKED]

**Status Justification:**
[Explain current status - date passed, failure reason, why not applicable, blocker details]

**Defects Found:**
- [If FAIL: DEFECT_ID, description, link]
- [If PASS: None]

**Comments:**
[Additional notes, edge cases, known limitations, follow-up investigations]

**Last Updated:** [Date, Author Name]

**Related Test Cases:**
- [Reference to related tests]
```

---

## 8. Validation Rules Checklist

Use this checklist to validate that a test case conforms to this skill:

### Structural Validation
- [ ] Test ID format is MODULE_UT_SEQUENCE_VARIANT
- [ ] Test Name is descriptive and clear
- [ ] Category is one of 7 types
- [ ] Priority is P0/P1/P2/P3
- [ ] All 18 fields are present
- [ ] No empty or placeholder text in mandatory fields

### Schema Validation
- [ ] Test Status is exactly PASS, FAIL, NOT_APPLICABLE, NOT_STARTED, or BLOCKED
- [ ] Configuration Parameters table has 3 columns: Parameter, Value, Notes
- [ ] CUnit assertions use only supported macros
- [ ] Mock data is syntactically correct C code

### Content Validation
- [ ] Test Description is specific and testable
- [ ] Prerequisites list all setup requirements
- [ ] Test Steps are numbered and actionable
- [ ] Expected Results match Test Assertions
- [ ] Test Assertions are specific (not generic)
- [ ] Status Justification explains why current status

### Consistency Validation
- [ ] Test ID matches Test Name category
- [ ] Category matches Test ID prefix
- [ ] Configuration parameters used in mock data
- [ ] All CUnit macros match expected results
- [ ] Date format is consistent (YYYY-MM-DD)

---

## 9. Quick Reference: Test Field Summary

| Field | What It Answers | Max Length | Required |
|-------|-----------------|------------|----------|
| Test ID | What is unique identifier? | 50 chars | ✅ YES |
| Test Name | What is human-readable description? | 100 chars | ✅ YES |
| Category | What type of test? | Enum (7) | ✅ YES |
| Priority | How urgent/critical? | Enum (4) | ✅ YES |
| Module(s) | What code files are tested? | 200 chars | ✅ YES |
| Description | What is purpose? | 500 chars | ✅ YES |
| Prerequisites | What is needed before? | Bullet list | ✅ YES |
| Config Params | What values for this test? | Table format | ✅ YES |
| Mock Data | What input data? | C code block | ✅ YES |
| Steps | What actions to take? | Numbered list | ✅ YES |
| Expected Results | What should happen? | Bullet list | ✅ YES |
| Assertions | How to verify? | CUnit code | ✅ YES |
| Status | Did it pass/fail? | Enum (5) | ✅ YES |
| Justification | Why this status? | 200 chars | ✅ YES |
| Defects | What broke? | Bullet list | ✅ YES |
| Comments | What else matters? | 300 chars | ✅ YES |
| Last Updated | When/by whom? | Date + name | ✅ YES |
| Related Tests | What tests are similar? | Bullet list | ✅ YES |

---

## 10. Common Mistakes to Avoid

| Mistake | Wrong | Correct |
|---------|-------|---------|
| Status value case | `Pass`, `PASSED` | `PASS` |
| Test ID format | `TC_001` | `FREEZE_UT_001_BASIC_DETECTION` |
| Missing variant | `FREEZE_UT_001` | `FREEZE_UT_001_BASIC_DETECTION` |
| Wrong assertion | `assert(x == 5)` | `CU_ASSERT_EQUAL(x, 5)` |
| Empty field | `Test Name: TBD` | `Test Name: [descriptive name]` |
| Wrong category | `Debugging` | One of 7 defined categories |
| Multiple conditions | `CU_ASSERT(x && y)` | Separate assertions per condition |
| Alternate status | `Not Run`, `Skipped` | `NOT_STARTED`, `BLOCKED` |
| Missing CUnit code | Description only | Code block with CUnit macros |
| Invalid config table | No columns | 3-column table: Parameter, Value, Notes |

---

## 11. Integration with Agent

**How This Skill is Used:**

- **Agent Section 11.1:** References this skill for Test Execution Workflow validation
- **Agent Section 11.2:** Uses CUnit assertion rules from section 3 above
- **Agent Section 11.6:** Uses Test Data rules from section 4-5 above
- **Agent Quality Gate 1:** Validates all schema rules in this skill

**Where to Find Usage:**
- Agent validates test structure against skill definitions
- Agent rejects tests that violate skill schema
- Agent documents violations as compliance failures

---

## References

- **Master Prompt:** `.github/Prompts/UT-test-design_prompt.md`
- **Execution Agent:** `.github/Agents/Agent.md`
- **Architecture Guide:** `.github/Docs/Agent-Skill-Architecture-Analysis.md`
- **Test Validation Workflow:** `.github/Docs/Skill-Agent-Differences.md`

**Version:** 1.0  
**Last Updated:** 2026-09-29  
**Owner:** Design Team  
**Status:** Active
