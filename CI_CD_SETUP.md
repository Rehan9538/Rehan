# GitHub Actions CI/CD Pipeline Setup
## BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine

**Date:** 2026-09-29  
**Status:** Complete  
**Pipeline:** Fully Automated CI/CD

---

## Overview

This project includes a comprehensive GitHub Actions CI/CD pipeline that automatically:
- ✅ Compiles the project (GCC Debug & Release)
- ✅ Runs all 16 unit tests (CUnit framework)
- ✅ Performs code quality checks (cppcheck, clang-format, clang-tidy)
- ✅ Generates code coverage reports (lcov)
- ✅ Runs security analysis
- ✅ Uploads artifacts to GitHub

---

## Files Included

### `.github/workflows/ci.yml`
**Main GitHub Actions workflow file**
- Triggers on push to `main` and `develop` branches
- Triggers on all pull requests to `main` and `develop`
- Runs 5 jobs in parallel:
  1. Build & Test (GCC Debug + Release)
  2. Code Quality (cppcheck, clang-format, clang-tidy)
  3. Code Coverage (lcov + codecov)
  4. Security Scan (cppcheck security checks)
  5. Summary report

### `CMakeLists.txt`
**Build configuration**
- Defines build options: `ENABLE_TESTS`, `ENABLE_CODE_COVERAGE`
- Sets C11 standard with strict compiler flags
- Configures all 16 unit tests with CTest
- Supports both Debug and Release builds
- Enables coverage instrumentation when requested

### `.clang-format`
**Code formatting rules**
- Enforces consistent C code style
- 100-character line limit
- 4-space indentation
- LLVM-based formatting style

### `.clang-tidy`
**Static analysis rules**
- Enables bugprone, clang-analyzer, performance, security checks
- Defines identifier naming conventions (snake_case, UPPER_CASE)
- Treats warnings as errors for critical checks
- Customizable check options

---

## Workflow Jobs

### Job 1: Build and Test

**Runs on:** Ubuntu Latest  
**Strategy:** Matrix with 2 configurations (GCC Debug + Release)

#### Steps:
1. ✅ Checkout code
2. ✅ Install dependencies (cmake, libcunit1, cppcheck, clang-format, lcov)
3. ✅ Configure CMake with build flags
4. ✅ Build project
5. ✅ Run all 16 unit tests
6. ✅ Generate test report
7. ✅ Upload test artifacts

**Artifacts:**
- `test-report-gcc-Debug`
- `test-report-gcc-Release`

**Success Criteria:**
- All 16 unit tests PASS
- Build completes without errors
- No warnings (with -Werror)

---

### Job 2: Code Quality & Linting

**Runs on:** Ubuntu Latest

#### Tools Used:
- **cppcheck** - Static analysis tool
- **clang-format** - Code formatting checker
- **clang-tidy** - Comprehensive linting

#### Steps:
1. ✅ Run cppcheck (all checks enabled)
   - Enable all checks
   - Suppress system includes
   - Suppress unused functions
   - Checks: `src/`, `include/`, `tests/`

2. ✅ Check clang-format compliance
   - Scans all `.c` and `.h` files
   - Generates formatted version if needed
   - Reports differences

3. ✅ Run clang-tidy linting
   - Analyzes all source files
   - Reports style violations
   - Suggests improvements

**Note:** These jobs continue on error (warnings don't block) to provide full feedback

---

### Job 3: Code Coverage Report

**Runs on:** Ubuntu Latest  
**Build Type:** Debug (for complete coverage)

#### Steps:
1. ✅ Install coverage tools (lcov, gcc with coverage flags)
2. ✅ Configure CMake with coverage enabled
3. ✅ Build project with coverage instrumentation
4. ✅ Run tests with coverage tracking
5. ✅ Generate lcov report
6. ✅ Upload to Codecov.io
7. ✅ Store HTML report as artifact

**Artifacts:**
- `coverage-report/` - Interactive HTML coverage report
- `coverage.info` - Raw coverage data

**Coverage Target:** >80%

**Codecov Integration:**
- Automatic upload of coverage reports
- Badge/status on pull requests
- Historical tracking

---

### Job 4: Security Scan

**Runs on:** Ubuntu Latest

#### Steps:
1. ✅ Run cppcheck with security focus
   - Enable security and warning checks
   - Scan for common vulnerabilities
   - Report potential issues

**Artifacts:**
- `security-report.txt` - Detailed security findings

---

### Job 5: Build Summary

**Runs on:** Ubuntu Latest  
**Depends on:** All previous jobs

#### Steps:
1. ✅ Print summary of all job results
2. ✅ Fail if build or tests failed (critical gate)
3. ✅ Success notification if all checks pass

---

## Local Usage

### Prerequisites
```bash
sudo apt-get install -y \
  build-essential \
  cmake \
  libcunit1-dev \
  cppcheck \
  clang-format \
  clang-tidy \
  lcov
```

### Build and Test
```bash
# Create build directory
mkdir -p build
cd build

# Configure with tests enabled
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_TESTS=ON \
      -DENABLE_CODE_COVERAGE=ON \
      ..

# Build
cmake --build . -j$(nproc)

# Run tests
ctest --output-on-failure -V

# Or use custom test target
cmake --build . --target run_tests
```

### Check Code Quality
```bash
# Format check
clang-format --dry-run -Werror src/*.c include/*.h tests/*.c

# Static analysis
cppcheck --enable=all src/ include/ tests/

# Linting
clang-tidy src/*.c -- -Iinclude
```

### Generate Coverage Report
```bash
cd build
ctest --coverage
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_report
open coverage_report/index.html
```

---

## GitHub Status Checks

### Branch Protection Rules (Recommended)

To ensure code quality, set up these branch protections on `main` and `develop`:

```
Require status checks to pass before merging:
✅ build-and-test (gcc-Debug)
✅ build-and-test (gcc-Release)
✅ code-quality
✅ code-coverage
✅ security-scan

Require code reviews: 1
Require up-to-date branches before merging: Yes
Dismiss stale reviews when new commits pushed: Yes
```

### Status Badges

Add these badges to your README.md:

```markdown
![CI/CD Pipeline](https://github.com/YOUR_ORG/bms-diagnostics/actions/workflows/ci.yml/badge.svg?branch=main)
![Code Coverage](https://codecov.io/gh/YOUR_ORG/bms-diagnostics/branch/main/graph/badge.svg)
```

---

## Workflow Configuration

### Triggers

**On Push:**
```yaml
on:
  push:
    branches: [ main, develop ]
```
Runs on every commit to `main` or `develop`

**On Pull Request:**
```yaml
on:
  pull_request:
    branches: [ main, develop ]
```
Runs on all pull requests targeting `main` or `develop`

### Environment Variables
```yaml
CMAKE_VERSION: "3.27"           # CMake version to use
CTEST_OUTPUT_ON_FAILURE: ON    # Show test failure details
CTEST_PARALLEL_LEVEL: 2        # Run 2 tests in parallel
```

---

## Test Matrix Strategy

The workflow tests with multiple configurations:

```
Configuration 1: GCC Debug
- Compiler: gcc
- Build Type: Debug (-g -O0)
- Purpose: Development, debugging
- Coverage: Full coverage possible

Configuration 2: GCC Release
- Compiler: gcc
- Build Type: Release (-O3)
- Purpose: Production-like testing
- Coverage: Optimized code
```

Both configurations must pass for the build to succeed.

---

## Artifacts

GitHub Actions stores the following artifacts for 90 days:

| Artifact | Description | Retention |
|----------|-------------|-----------|
| `test-report-gcc-Debug` | Test execution log | 90 days |
| `test-report-gcc-Release` | Test execution log | 90 days |
| `coverage-report` | HTML coverage report | 90 days |
| `security-report` | Security findings | 90 days |

### Accessing Artifacts

1. Go to GitHub Actions tab
2. Click on the workflow run
3. Scroll down to "Artifacts" section
4. Download desired artifact
5. For coverage: Extract and open `index.html` in browser

---

## Troubleshooting

### Build Fails: "CUnit not found"
**Solution:** Ensure libcunit1-dev is installed
```bash
sudo apt-get install libcunit1-dev
```

### Tests Timeout
**Solution:** Timeout is set to 300 seconds per test. For slower systems:
1. Edit `.github/workflows/ci.yml`
2. Increase timeout value
3. Or run tests serially by changing `CTEST_PARALLEL_LEVEL`

### Coverage Report Missing
**Solution:** Ensure code was built with coverage flags:
```bash
cmake ... -DENABLE_CODE_COVERAGE=ON
```

### Linting False Positives
**Solution:** Adjust `.clang-tidy` configuration:
- Add suppression comments in code: `// NOLINT(rule-name)`
- Disable specific checks in `.clang-tidy`
- Use pragmas: `#pragma clang diagnostic ignored "..."`

### Workflow Not Triggering
**Solution:** Verify:
1. Workflow file is in `.github/workflows/` directory
2. YAML syntax is correct (`yamllint` can help)
3. Branch is configured (check push/PR filters)
4. Actions are enabled in repository settings

---

## Performance Optimization

### Parallel Execution
```yaml
CTEST_PARALLEL_LEVEL: 2  # Run multiple tests simultaneously
```
Adjust based on runner capabilities.

### Build Caching
Currently not enabled. To enable:
```yaml
- uses: actions/cache@v3
  with:
    path: build
    key: build-${{ runner.os }}-${{ hashFiles('**CMakeLists.txt') }}
```

### Matrix Strategy
Tests are run with 2 compiler configurations in parallel, not sequential.

---

## Example Workflow Runs

### Successful Run
```
✅ Build and Test (GCC Debug) - PASSED
✅ Build and Test (GCC Release) - PASSED
✅ Code Quality & Linting - PASSED
✅ Code Coverage Report - PASSED (coverage: 85%)
✅ Security Scan - PASSED
✅ Build Summary - PASSED

All checks passed! Ready to merge.
```

### Failed Run (Test Failure)
```
✅ Build and Test (GCC Debug) - FAILED
   └─ FREEZE_UT_001: Assertion failed (expected 45.0, got 44.9)
⏭️ Build and Test (GCC Release) - CANCELLED
⏭️ Code Quality & Linting - SKIPPED
⏭️ Code Coverage Report - SKIPPED
⏭️ Security Scan - SKIPPED
❌ Build Summary - FAILED

Fix test FREEZE_UT_001 and push again.
```

---

## Integration with UT Test Design

The workflow automatically runs the 16 baseline tests from the UT Test Design:

**P0-Critical Tests (must all PASS):**
- FREEZE_UT_001 - Frozen Sensor Basic Detection
- FREEZE_UT_002 - Healthy Not Falsely Flagged
- DRIFT_UT_001 - Out-of-Spec Detection
- DRIFT_UT_002 - Gradient Not Falsely Flagged

**P1-High Tests (should all PASS):**
- FREEZE_UT_003, FREEZE_UT_004
- DRIFT_UT_003
- OUTLIER_UT_001
- STATE_UT_001
- EDGE_UT_001, EDGE_UT_002, EDGE_UT_003
- CONFIG_UT_001
- RESET_UT_001
- MQTT_UT_001, MQTT_UT_002

**Release Gate:**
All 4 P0-Critical tests must PASS before allowing merge to `main`.

---

## Next Steps

1. **Push to GitHub:**
   ```bash
   git add .
   git commit -m "Add GitHub Actions CI/CD workflow"
   git push origin main
   ```

2. **Monitor Workflow:**
   - Go to GitHub Actions tab
   - Watch workflow execution
   - Check logs if any job fails

3. **Configure Branch Protection:**
   - Go to Settings → Branches
   - Add rule for `main` branch
   - Require status checks to pass

4. **Set Up Codecov:**
   - Visit https://codecov.io
   - Add repository
   - Enable badge on README

5. **Link Tests to Issues:**
   - Use test defect fields to track issues
   - Link GitHub Issues/PRs to test failures
   - Update status after fixes

---

## Additional Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [CUnit Documentation](http://cunit.sourceforge.net/)
- [CMake Documentation](https://cmake.org/documentation/)
- [clang-format Documentation](https://clang.llvm.org/docs/ClangFormat/)
- [clang-tidy Documentation](https://clang.llvm.org/extra/clang-tidy/)
- [lcov Documentation](http://ltp.sourceforge.net/coverage/lcov.php)

---

## Support & Maintenance

**Workflow Maintenance:**
- Review job configurations quarterly
- Update dependencies as new versions available
- Adjust timeout values based on performance data

**Adding New Tests:**
1. Create test in `tests/test_*.c`
2. Add to CMakeLists.txt `add_test()` call
3. Workflow will automatically include it in next run

**Modifying Linting Rules:**
1. Edit `.clang-tidy` or `.clang-format`
2. Commit changes
3. Next workflow run uses new rules

---

**Version:** 1.0  
**Date:** 2026-09-29  
**Status:** Ready for Production

