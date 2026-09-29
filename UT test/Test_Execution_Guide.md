# UT Test Execution Guide
## Quick Start for Running Tests

**Date:** 2026-09-29  
**Framework:** CUnit  
**Build System:** CMake  
**Total Tests:** 16  

---

## Quick Commands

```bash
# Build the project with tests enabled
cd /path/to/project
cmake -B build -DENABLE_TESTS=ON
cmake --build build

# Run all tests with verbose output
ctest --output-on-failure -V

# Run tests for specific category
ctest -R "FREEZE" --output-on-failure
ctest -R "DRIFT" --output-on-failure
ctest -R "OUTLIER" --output-on-failure
ctest -R "STATE" --output-on-failure
ctest -R "EDGE" --output-on-failure
ctest -R "CONFIG" --output-on-failure
ctest -R "RESET" --output-on-failure
ctest -R "MQTT" --output-on-failure

# Generate code coverage report
cd build
ctest --coverage
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_report
```

---

## Expected Test Output Format

### Successful Test Run
```
Test project /path/to/build
    Start  1: FREEZE_UT_001
1/16 Test #1: FREEZE_UT_001 ....................... Passed  2.34 sec
    Start  2: FREEZE_UT_002
2/16 Test #2: FREEZE_UT_002 ....................... Passed  1.89 sec
    ...
16/16 Test #16: MQTT_UT_002 ....................... Passed  1.45 sec

100% tests passed, 0 tests failed out of 16

Total Test time (real) = 28.45 sec
```

### Test Failure Output
```
Test #1: FREEZE_UT_001 ....... Passed  2.34 sec
Test #3: FREEZE_UT_003 ....... FAILED  0.56 sec

The following tests FAILED:
    3 - FREEZE_UT_003 (SEGFAULT)

CTest error code: 1
```

---

## Test Categories & Individual Runs

### Freeze Detection Tests (4 tests)
```bash
ctest -R "FREEZE" --output-on-failure -V
# Expected: 4 tests, all should PASS or be NOT_APPLICABLE
```

**Tests included:**
- FREEZE_UT_001 - Frozen Sensor Basic Detection (P0-Critical)
- FREEZE_UT_002 - Healthy Not Falsely Flagged (P0-Critical)
- FREEZE_UT_003 - Transient Ignored (P1-High)
- FREEZE_UT_004 - Recovery with Hysteresis (P1-High)

### Drift Detection Tests (3 tests)
```bash
ctest -R "DRIFT" --output-on-failure -V
# Expected: 3 tests, all should PASS or be NOT_APPLICABLE
```

**Tests included:**
- DRIFT_UT_001 - OOS Detection (P0-Critical)
- DRIFT_UT_002 - Gradient Not Flagged (P0-Critical)
- DRIFT_UT_003 - Insufficient Neighbors (P1-High)

### Outlier Filter Tests (1 test)
```bash
ctest -R "OUTLIER" --output-on-failure -V
# Expected: 1 test, should PASS or be NOT_APPLICABLE
```

**Test included:**
- OUTLIER_UT_001 - Transient Spike Filtered (P1-High)

### State Machine Tests (1 test)
```bash
ctest -R "STATE" --output-on-failure -V
# Expected: 1 test, should PASS or be NOT_APPLICABLE
```

**Test included:**
- STATE_UT_001 - State Transitions (P1-High)

### Edge Case Tests (3 tests)
```bash
ctest -R "EDGE" --output-on-failure -V
# Expected: 3 tests, all should PASS or be NOT_APPLICABLE
```

**Tests included:**
- EDGE_UT_001 - Startup Phase (P1-High)
- EDGE_UT_002 - Missing/Stale Data (P1-High)
- EDGE_UT_003 - Async Timestamps (P2-Medium)

### Configuration Tests (1 test)
```bash
ctest -R "CONFIG" --output-on-failure -V
# Expected: 1 test, should PASS or be NOT_APPLICABLE
```

**Test included:**
- CONFIG_UT_001 - Config Load & Validate (P1-High)

### Reset/Recovery Tests (1 test)
```bash
ctest -R "RESET" --output-on-failure -V
# Expected: 1 test, should PASS or be NOT_APPLICABLE
```

**Test included:**
- RESET_UT_001 - Calibration Offset Reset (P1-High)

### MQTT/Reporting Tests (2 tests)
```bash
ctest -R "MQTT" --output-on-failure -V
# Expected: 2 tests, all should PASS or be NOT_APPLICABLE
```

**Tests included:**
- MQTT_UT_001 - Fault Event Publishing (P1-High)
- MQTT_UT_002 - Offline Queuing & Retry (P1-High)

---

## Critical P0 Tests (Release Gate)

**Must all PASS for release:**
```bash
ctest -R "FREEZE_UT_001|FREEZE_UT_002|DRIFT_UT_001|DRIFT_UT_002" --output-on-failure -V
```

**Tests (4 total):**
- FREEZE_UT_001 - Frozen Sensor Basic Detection
- FREEZE_UT_002 - Healthy Not Falsely Flagged
- DRIFT_UT_001 - Out-of-Specification Detection
- DRIFT_UT_002 - Gradient Not Falsely Flagged

**Expected Result:** 4/4 PASSED

---

## Code Coverage Analysis

After running tests with coverage:

```bash
cd build
genhtml coverage_filtered.info --output-directory coverage_report
open coverage_report/index.html  # Open in browser
```

**Target Coverage:** >80%

**Coverage by Module (Expected):**
- `src/bms_diagnostics.c` - >90% (core freeze/drift logic)
- `src/rolling_window.c` - >85% (window calculations)
- `src/drift_detector.c` - >90% (drift logic)
- `src/outlier_filter.c` - >80% (statistical filtering)
- `src/state_machine.c` - >85% (state transitions)
- `src/config_loader.c` - >80% (JSON parsing)
- `src/mqtt_reporter.c` - >75% (depends on AWS SDK)

---

## Troubleshooting

### Test Not Found
```
Error: no tests found matching "FREEZE_UT_001"
```
**Solution:** Verify test is registered in CMakeLists.txt using add_test()

### Segmentation Fault
```
Test #1: FREEZE_UT_001 ....... FAILED (SEGFAULT)
```
**Solution:** Run with debugger:
```bash
gdb ./build/bms_diagnostic_tests
(gdb) run
(gdb) bt  # Print backtrace
```

### Timeout
```
Test #5: CONFIG_UT_001 ....... FAILED (Timeout)
```
**Solution:** Increase timeout or check for infinite loops:
```bash
ctest --timeout 10 --output-on-failure
```

### Coverage Report Not Generated
```
Couldn't find 'coverage' executable.
```
**Solution:** Install lcov:
```bash
sudo apt-get install lcov  # Ubuntu/Debian
brew install lcov          # macOS
choco install lcov         # Windows (chocolatey)
```

---

## Continuous Integration (GitHub Actions)

Example workflow file (.github/workflows/test.yml):

```yaml
name: Unit Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Install dependencies
        run: sudo apt-get install cmake cunit lcov
      
      - name: Build
        run: |
          cmake -B build -DENABLE_TESTS=ON
          cmake --build build
      
      - name: Run tests
        run: |
          cd build
          ctest --output-on-failure -V
      
      - name: Generate coverage
        run: |
          cd build
          ctest --coverage
          lcov --directory . --capture --output-file coverage.info
          lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
      
      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          files: ./build/coverage_filtered.info
```

---

## Test Status Update Procedure

After running tests:

1. **Record Results**
   ```
   Test ID: FREEZE_UT_001
   Status: PASS
   Last Run: 2026-09-29T14:35:22Z
   Coverage: 87%
   Comments: All assertions passed; no issues
   ```

2. **Update CSV**
   - Edit Test_Status_Tracking.csv
   - Update Status column for each test
   - Record Last Run timestamp
   - Add Coverage % for that test
   - Update Comments with any notes

3. **Link Defects**
   - If FAIL: Create issue in GitHub/Jira
   - Link issue ID to Defects Found field
   - Update test status to FAILED with defect link

4. **Generate Report**
   - Count: X passed, Y failed, Z blocked
   - Calculate pass rate: X/(X+Y) × 100%
   - Note: P0 tests status (release gate)
   - Summarize coverage by module

---

## Metrics Summary Format

After each test run, record:

```
Test Execution Summary
Date: 2026-09-29T14:35:22Z
Total Tests: 16
Passed: [X]
Failed: [Y]
Blocked: [Z]
Not Started: [W]
Pass Rate: [X/(X+Y) × 100]%

Critical (P0) Tests:
- FREEZE_UT_001: [PASS/FAIL]
- FREEZE_UT_002: [PASS/FAIL]
- DRIFT_UT_001: [PASS/FAIL]
- DRIFT_UT_002: [PASS/FAIL]
P0 Pass Rate: [count passed / 4]

Code Coverage:
- Overall: [X]%
- Target: >80%
- Status: [MET/NOT MET]

Test Execution Time: [X seconds]
- Target: <30s
- Status: [MET/NOT MET]

Known Issues / Defects:
- [Issue ID]: [Description]
- [Issue ID]: [Description]
```

---

## Reference Files

- **Test Definitions:** Test_Cases_Design.csv
- **Status Tracking:** Test_Status_Tracking.csv
- **Design Document:** UT_Design_Document.md
- **Skill Reference:** /.github/Skills/UT-test-design-rules/SKILL.md
- **UT Prompt:** /.github/Prompts/UT-test-design_prompt.md
- **SWDD:** /Docs/SWDD.md

---

**Version:** 1.0  
**Date:** 2026-09-29  
**Ready for Execution**

