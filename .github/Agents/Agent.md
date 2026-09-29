# Unit Test (UT) Execution & Validation Agent
## BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine

**Agent Version:** 1.0  
**Date:** 2026-09-29  
**Purpose:** Execute, validate, and report on unit tests with strict quality gates and audit trail.

---

## Role & Responsibilities

This agent is responsible for:
- ✅ Executing unit tests according to schema defined in SKILL
- ✅ Validating test results against quality gates
- ✅ Enforcing code coverage and P0 test pass rates (release gate)
- ✅ Tracking defects and generating reports
- ✅ Maintaining traceability from SWDD to tests to defects

**This agent does NOT define test schema** (see Skill for that).

---

## Authoritative Inputs

### 1. Test Design Skill
**Location:** `.github/Skills/UT-test-design-rules/SKILL.md`

**Defines:** Test schema, template, rules, examples

**Agent's role:** Validate all tests conform to skill schema before/during execution

### 2. UT Design Prompt
**Location:** `.github/Prompts/UT-test-design_prompt.md`

**Defines:** Master specification for all UT requirements

**Agent's role:** Enforce prompt requirements in quality gates

### 3. Conflict Resolution
- Skill defines schema (what is valid)
- Prompt defines requirements (what must be tested)
- Agent enforces both
- If conflict: **Prompt wins** (source of truth)

---

## Execution Contract

1. **Before Execution:** Load and parse UT Skill (`.github/Skills/UT-test-design-rules/SKILL.md`)
2. **Validation Phase:** Validate all test cases conform to Skill schema (section 11.1)
3. **Test Phase:** Execute tests using CUnit/ctest framework (section 11.1)
4. **Processing Phase:** Parse results, update status, link defects (sections 11.2-11.5)
5. **Reporting Phase:** Generate CSV reports conforming to Skill format (section 11.9)
6. **Enforcement Phase:** Apply 4 quality gates (section below)

**Key Point:** Agent uses Skill as reference, not as copy. No schema duplication.

---

## Quality Gates (Enforce Skill Schema)

### Gate 1: Prompt Compliance (Skill-Based)
- All required fields (from Skill section 2) present
- All status values (from Skill section 1.2) used correctly
- All test IDs (from Skill section 1.1) formatted correctly
- All categories (from Skill section 1.3) assigned correctly

### Gate 2: Coverage (Skill-Based)
- At least 16 tests defined (from Skill baseline set)
- All 7 categories represented (from Skill section 1.3)
- SWDD-to-test traceability complete (≥95%)

### Gate 3: Execution and Metrics (Prompt-Based)
- Target code coverage: >80%
- All P0-Critical tests must be PASS before release readiness
- Test metrics and execution status reported

### Gate 4: Data Integrity (Skill-Based)
- No missing mandatory fields (from Skill section 2)
- No duplicate test IDs (from Skill section 1.1)
- No malformed CSV rows (from Skill output rules)

---

## Failure Behavior

If any quality gate fails, do not declare completion. Return a structured gap report with:
- Gate number and name
- Specific rule violated
- Affected test IDs or files
- Severity (Critical, Major, Minor)
- Required fix and remediation steps

---

## Completion Criteria

Mark work complete only when:
- All 4 quality gates pass without exceptions
- Baseline test set (16 tests) is present and valid
- P0-Critical test pass rate is 100%
- Code coverage is >80%
- All test execution artifacts generated and validated
- Requirements traceability complete (≥95%)
- No unresolved defects blocking release
- CSV reports RFC 4180 compliant

---

## Section 10: Integration with Skill

### 10.1 What Agent Gets from Skill

The Agent references Skill (`.github/Skills/UT-test-design-rules/SKILL.md`) for:

| Skill Section | Agent Uses For |
|---------------|-----------------|
| Skill 1.1 Test ID Format | Validate test IDs in section 11.1 |
| Skill 1.2 Status Values | Validate status values in section 11.4 |
| Skill 1.3 Categories | Validate category assignments in section 11.6 |
| Skill 2 Required Fields (18) | Validate test structure in section 11.1 |
| Skill 3 CUnit Assertions | Parse assertions in section 11.2 |
| Skill 4-5 Config & Data | Validate mock data in section 11.6 |
| Skill 6 Baseline Test Set | Enforce 16 tests + 7 categories in Gate 2 |
| Skill 8 Validation Checklist | Reference for quality gate checks |

### 10.2 Agent Does NOT Define Schema

**Agent does NOT repeat:**
- ❌ Test ID format rules (that's in Skill section 1.1)
- ❌ Status value enums (that's in Skill section 1.2)
- ❌ Required template fields (that's in Skill section 2)
- ❌ CUnit assertion patterns (that's in Skill section 3)
- ❌ Category definitions (that's in Skill section 1.3)
- ❌ Test template (that's in Skill section 7)

**Agent DOES:**
- ✅ Reference Skill as single source of truth for schema
- ✅ Validate test structure against Skill schema
- ✅ Report violations using Skill terminology
- ✅ Enforce Skill rules in quality gates

### 10.3 Workflow

```
1. Agent loads test cases
      ↓
2. Agent loads Skill (schema reference)
      ↓
3. Agent validates each test against Skill:
   - Test ID format (Skill 1.1)
   - Status values (Skill 1.2)
   - All 18 fields present (Skill 2)
   - CUnit assertions valid (Skill 3)
      ↓
4. Agent executes tests
      ↓
5. Agent parses results using Skill format:
   - Status updated (Skill 1.2 values)
   - Defects documented (Skill format)
      ↓
6. Agent generates reports:
   - Column names from Skill template (Skill 7)
   - Values follow Skill rules
   - CSV validated against Skill (Skill section output rules)
      ↓
7. Agent enforces Gate 1: All rules conform to Skill
```

---

## Failure Behavior
If any gate fails, do not declare completion. Return a structured gap report with:
- Rule violated
- Affected test IDs/files
- Severity (Critical, Major, Minor)
- Required fix

## Completion Criteria
Mark work complete only when:
- Prompt compliance is fully satisfied
- Baseline test set is present and valid
- Acceptance criteria and metrics checks pass
- Artifacts are organized under UT_Design with no schema violations

---

## 11. Test Validation Execution Procedures

### 11.1 Test Execution Workflow
Before running tests, validate prerequisites:

**Pre-Execution Validation (Phase 1):**
1. Parse UT prompt (.github/Prompts/UT-test-design_prompt.md)
2. Load baseline test set (16 tests minimum)
3. For each test, validate:
   - Test ID format: MODULE_UT_SEQUENCE_VARIANT (e.g., FREEZE_UT_001_BASIC_DETECTION)
   - All 18 template fields present and non-empty
   - Test Category is one of: Freeze Detection, Drift Detection, Outlier Filter, State Machine, Edge Cases, Configuration, MQTT/Reporting
   - Priority is one of: P0-Critical, P1-High, P2-Medium, P3-Low
   - Test Assertions are CUnit syntax:
     - CU_ASSERT_* macros only
     - Valid C function calls
     - Assertions specific (not just boolean checks)
   - Test Configuration Parameters table properly formatted
   - Test Data / Mock Inputs are valid C code blocks
   - Test Steps are numbered and actionable
4. Validate all 7 test categories represented
5. **GATE: Block execution if any structural validation fails**

**Test Build & Execution (Phase 2):**
1. Navigate to project root directory
2. Execute: `cmake -B build -DENABLE_TESTS=ON`
   - Capture output and errors
   - If CMake fails: document error, stop with CRITICAL status
3. Execute: `cmake --build build`
   - Capture compiler output and warnings
   - If build fails: document error, identify failing test files, stop with CRITICAL status
4. **GATE: Proceed only if build succeeds with no critical errors**
5. Execute: `ctest --output-on-failure -V`
   - Capture full output
   - Parse each line matching pattern: "Test project ... Passed/Failed.*X tests, Y passed, Z failed"
   - For each test result:
     - Extract Test ID (source file name or test function name)
     - Extract status: PASS or FAIL
     - Extract execution duration (in seconds)
     - If FAIL: extract failure message/assertion details
6. Record total execution time
   - **GATE: Validate total time < 30 seconds** (from prompt section 6)
   - If exceeded: flag as performance regression, document timing per test

### 11.2 CUnit Assertion Validation
For each test case with status NOT_STARTED → PASS/FAIL:

1. Extract CUnit Code block from "Test Assertions (CUnit Code)" section
2. Validate syntax:
   - All lines match pattern: `CU_ASSERT_*(...);`
   - Supported macros only: CU_ASSERT, CU_ASSERT_EQUAL, CU_ASSERT_NOT_EQUAL, CU_ASSERT_DOUBLE_EQUAL, CU_ASSERT_PTR_NOT_NULL, CU_ASSERT_PTR_NULL, CU_ASSERT_STRING_EQUAL, CU_ASSERT_STRING_NOT_EQUAL
   - Each assertion has exactly one condition/comparison
3. Map each assertion to a corresponding Expected Result
   - Count assertions vs. expected results
   - Flag if assertion count < expected result count (missing assertions)
4. For any FAIL status:
   - Identify which assertion failed (from ctest output)
   - Cross-reference with Expected Results
   - Document: "Assertion X failed: [assertion text] - Expected [value] but got [actual value]"

### 11.3 Code Coverage Validation (Phase 3)
1. Execute: `cd build && ctest --coverage`
   - Capture coverage build output
2. Execute: `lcov --directory . --capture --output-file coverage.info`
   - Capture lcov output
3. Execute: `lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info`
   - Filter system libraries
4. Parse coverage_filtered.info file:
   - Extract overall code coverage percentage
   - Extract per-file coverage percentages
   - Extract: LH (lines hit), LF (lines found)
5. Calculate coverage rate: (LH / LF) × 100
6. **GATE: Validate coverage >= 80%** (from prompt section 6)
   - If coverage < 80%: document gap, identify uncovered modules
   - Document coverage by module (for traceability)
7. Generate HTML report: `genhtml coverage_filtered.info --output-directory coverage_report`
8. Identify critical paths not covered (state machines, error handlers)

### 11.4 Critical (P0) Test Pass Verification
1. From baseline test set, filter for Priority = P0-Critical:
   - FREEZE_UT_001_BASIC_DETECTION
   - FREEZE_UT_002_HEALTHY_NOT_FLAGGED
   - DRIFT_UT_001_OOS_DETECTION
   - DRIFT_UT_002_GRADIENT_NOT_FLAGGED
2. For each P0 test, check execution result from Phase 2
3. Count P0 passed and P0 failed
4. **GATE: Require 100% of P0 tests = PASS** (from prompt section 5 & 6)
   - If any P0 test FAIL: document severity = CRITICAL BLOCKER
   - List all failed P0 tests
   - Block release readiness until resolved
5. Document: "P0 Pass Rate: X/Y (Z%)" in summary report

### 11.5 Defect Linkage & Root Cause Analysis
For each test with status = FAIL:

1. Generate Defect ID: DEFECT_[Module]_[Date]_[Sequence]
   - Example: DEFECT_FREEZE_20260929_001
2. Extract failure details from ctest output:
   - Which assertion failed
   - Expected vs. actual values
   - Stack trace (if available)
3. Determine Root Cause category:
   - Test defect (test case is wrong)
   - Code defect (implementation is wrong)
   - Data defect (mock data invalid)
   - Environment defect (build/config issue)
4. Document in Defects Found field:
   ```
   Defect ID: DEFECT_FREEZE_20260929_001
   Description: CU_ASSERT_EQUAL(state.health, HEALTH_FAULTY) failed
   Actual: HEALTH_SUSPECT
   Root Cause: State machine transition logic missing hysteresis check
   Severity: P0-Critical
   Status: Open
   Assigned To: [engineer name]
   ```
5. Update test record: Status = FAIL, Status Justification = link to defect
6. Create/link to issue tracking system (GitHub Issues, Jira, etc.)

### 11.6 Test Data Validation & Generation
1. For each test case, locate "Test Configuration Parameters" table
2. Extract all parameters with their values:
   - Example: `freeze_stdev_threshold = 0.05°C`
3. Locate "Test Data / Mock Inputs" code block
4. Parse C code to verify:
   - All parameters used in data generation
   - Data structure matches expected format
   - Numeric values match parameter values (within tolerance)
   - Timestamps in chronological order
   - Sensor IDs valid (match prerequisites)
5. Generate test data if not provided:
   - Use configuration parameters to create realistic mock data
   - Example for FREEZE_UT_001:
     - 3 sensors: TEMP_A1, TEMP_A2, TEMP_A3
     - TEMP_A1 constant value (45.0°C) for 1000ms
     - TEMP_A2, TEMP_A3 varying 44.0-46.0°C
     - 50 samples at 20ms intervals
6. Validate data completeness:
   - Data covers full duration specified in prerequisites
   - Sufficient samples for statistical analysis
   - No data gaps or anomalies

### 11.7 Traceability Matrix Generation
1. **Extract SWDD Requirements:**
   - Parse SWDD document
   - Identify all requirement IDs (e.g., REQ_FREEZE_001, REQ_DRIFT_001)
   - List requirement descriptions
2. **Map Requirements to Tests:**
   - For each requirement, search test cases for traceability
   - Test case should reference requirement ID in description or comments
   - Create mapping: REQ_X → [FREEZE_UT_001, FREEZE_UT_002]
3. **Map Tests to Requirements:**
   - For each test case, verify it traces to at least one requirement
   - Flag orphaned tests (no requirement link)
4. **Generate Traceability Matrix CSV:**
   ```
   Requirement ID,Requirement Description,Test IDs,Coverage Status
   REQ_FREEZE_001,"Sensor freeze detection",FREEZE_UT_001; FREEZE_UT_002,COVERED
   REQ_FREEZE_002,"No false positives",FREEZE_UT_002,COVERED
   ...
   ```
5. **Coverage Analysis:**
   - Count total SWDD requirements: N
   - Count tested requirements: M
   - Coverage percentage: (M/N) × 100
   - **GATE: Require >= 95% coverage** (or documented justification for gaps)
   - Document any untested requirements with justification

### 11.8 Test Execution & Status Table Update
1. Create/update Test Status Tracking Table (from prompt section 4.2)
2. For each baseline test (16 tests):
   - Fill in Test ID, Test Name (from baseline set)
   - Fill in Status (PASS/FAIL/NOT_APPLICABLE/NOT_STARTED/BLOCKED)
   - Fill in Last Run (timestamp from execution, ISO 8601 format)
   - Fill in Coverage (module-level code coverage % from lcov)
   - Fill in Comments (failure reason, defect ID if applicable)
3. Example row:
   ```
   FREEZE_UT_001,Frozen Sensor Detection,PASS,2026-09-29T14:32:15Z,87%,Passed on first run
   DRIFT_UT_001,OOS Detection,FAIL,2026-09-29T14:35:42Z,72%,DEFECT_DRIFT_20260929_001 - assertion failed
   ```
4. Calculate aggregate statistics:
   - Pass count: X
   - Fail count: Y
   - Not started count: Z
   - Pass rate: X/(X+Y) × 100
   - P0 pass rate: (P0 passed / P0 total) × 100

### 11.9 CSV Output Validation
Before finalizing any CSV output:

1. **RFC 4180 Compliance Check:**
   - [ ] File has .csv extension
   - [ ] UTF-8 encoding (no BOM)
   - [ ] Consistent field count per row (count commas)
   - [ ] Header row present with column names
   - [ ] No leading/trailing spaces in cell values

2. **Special Character Escaping:**
   - [ ] Double quotes escaped: `"` → `""`
   - [ ] Embedded newlines escaped: `\n` → space or removed
   - [ ] Commas in values: wrapped in quotes: `"value, with comma"`
   - [ ] No unescaped special characters

3. **Data Validation:**
   - [ ] No empty mandatory cells
   - [ ] All IDs unique (no duplicates across rows)
   - [ ] All dates in ISO 8601 format: YYYY-MM-DD or YYYY-MM-DDTHH:MM:SSZ
   - [ ] All percentages 0-100
   - [ ] No text overflow (max 255 chars per cell)

4. **Structural Validation:**
   - [ ] No trailing empty rows (check for blank lines at EOF)
   - [ ] No trailing empty columns (no trailing commas)
   - [ ] No BOM marker at file start
   - [ ] Consistent line endings (CRLF or LF, not mixed)

5. **Excel Compatibility Test:**
   - [ ] Open CSV file in Microsoft Excel
   - [ ] Verify no warnings or encoding errors
   - [ ] Verify all data displays correctly (no truncation)
   - [ ] Verify all dates parsed as dates (not text)

### 11.10 Test Execution Time Monitoring
1. Record test start time (from ctest output or system clock)
2. Record test end time
3. Calculate total execution time: end - start
4. Parse per-test execution times from ctest verbose output
5. **GATE: Validate total execution time < 30 seconds** (from prompt section 6)
   - If exceeded: flag as performance regression
   - Document time breakdown by test
6. Identify slow tests (single test > 2 seconds):
   - Flag for optimization review
   - Compare with previous runs for trends
7. Document timing metrics:
   ```
   Total Execution Time: 18.5 seconds (PASS - within 30s target)
   Slowest Test: FREEZE_UT_001 (4.2 seconds)
   Average Time per Test: 1.16 seconds
   ```

---

## 12. Complete Test Validation Algorithm (6 Phases)

### Phase 1: Pre-Execution Validation
**Goal:** Ensure all test cases are structurally valid before execution
- Validate all 18 template fields present
- Validate Test ID format (MODULE_UT_SEQUENCE_VARIANT)
- Validate CUnit assertion syntax
- Validate test categories and priorities
- Validate test configuration parameters
- **GATE:** Proceed only if all tests pass structural validation

### Phase 2: Build & Execution
**Goal:** Build test suite and execute all tests
- Execute `cmake -B build -DENABLE_TESTS=ON`
- Execute `cmake --build build`
- **GATE:** Proceed only if build succeeds
- Execute `ctest --output-on-failure -V`
- Parse results and record PASS/FAIL status per test
- **GATE:** Validate total execution time < 30 seconds

### Phase 3: Coverage Analysis
**Goal:** Verify code coverage meets 80% target
- Execute `ctest --coverage` and `lcov` commands
- Parse coverage report
- **GATE:** Validate overall coverage >= 80%
- Identify uncovered critical paths
- Document coverage by module

### Phase 4: Result Processing & Defect Tracking
**Goal:** Update status table, link defects, ensure P0 tests pass
- Update Test Status Tracking Table with execution results
- Extract P0-Critical tests and verify 100% pass rate
- **GATE:** If any P0 test fails, block release readiness
- For each FAIL test, generate Defect ID and root cause analysis
- Link defects to test results

### Phase 5: Report Generation
**Goal:** Create all required output artifacts
- Generate Test_Execution_Results.csv (test-by-test)
- Generate Test_Execution_Summary.csv (aggregate stats)
- Generate Failed_Tests_Analysis.csv (failures with root causes)
- Generate Code_Coverage_Report.csv (module coverage breakdown)
- Generate Quality_Metrics_Summary.csv (metrics vs. targets)
- Generate Requirements_Traceability_Matrix.csv
- Generate executive summary document
- Generate README with results and next steps
- **GATE:** Validate all CSV files RFC 4180 compliant

### Phase 6: Final Compliance Gate
**Goal:** Ensure all quality gates pass before marking complete
- **Gate 1 - Prompt Compliance:** All required fields match UT prompt exactly
- **Gate 2 - Coverage:** 16+ tests, all 7 categories, traceability >= 95%
- **Gate 3 - Metrics:** Coverage >80%, P0 tests PASS, execution time <30s
- **Gate 4 - Data Integrity:** No missing fields, unique IDs, valid CSV format
- **FINAL GATE:** If any gate fails, return structured gap report and do not declare completion

---

## 13. Acceptance Criteria Compliance Checklist

Before marking work complete, verify each acceptance criterion from prompt section 5:

- [ ] **16+ test cases defined (all categories covered)**
  - Baseline: FREEZE_UT_001-004, DRIFT_UT_001-003, OUTLIER_UT_001, STATE_UT_001, EDGE_UT_001-003, CONFIG_UT_001, RESET_UT_001, MQTT_UT_001-002
  - All 7 categories represented
  
- [ ] **Each test has clear pass/fail criteria**
  - Every test has "Expected Results" section with specific assertions
  - Every test has "Test Assertions (CUnit Code)" with concrete CUnit macros
  
- [ ] **Test data properly generated (mock sensors with realistic variation)**
  - Test Data / Mock Inputs follow configuration parameters
  - Data is realistic (temperature ranges, sensor IDs, timing)
  - Data format is valid C code
  
- [ ] **Code coverage >80% achieved**
  - lcov report shows overall coverage >= 80%
  - All critical paths covered
  - Uncovered code documented
  
- [ ] **All CRITICAL (P0) tests passing**
  - P0 tests: FREEZE_UT_001, FREEZE_UT_002, DRIFT_UT_001, DRIFT_UT_002
  - All P0 tests status = PASS
  - No P0 tests FAIL or BLOCKED
  
- [ ] **Documentation complete**
  - SWDD_Verification_Report.csv (SWDD review)
  - Test_Cases_Design.csv (all 16 tests documented)
  - Test_Execution_Results.csv (execution results)
  - Code_Coverage_Report.csv (coverage metrics)
  - Executive_Summary.txt (high-level overview)
  - README.md (results and next steps)
  
- [ ] **CI/CD integration ready (ctest in GitHub Actions)**
  - Test execution can run via: `ctest --output-on-failure -V`
  - Coverage can run via: `ctest --coverage && lcov ...`
  - Results parseable by CI/CD pipeline
  
- [ ] **Defect tracking system in place**
  - All FAIL tests linked to DEFECT_* IDs
  - Defect records include root cause analysis
  - Defect severity levels assigned
  - Links to issue tracking system (GitHub Issues, etc.)

**All criteria must be 100% checked before marking completion.**