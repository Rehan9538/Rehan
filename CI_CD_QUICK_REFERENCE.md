# 🚀 CI/CD Pipeline - Quick Start Guide

**Last Updated:** 2026-09-29  
**Status:** ✅ **FULLY CONFIGURED**

---

## 🎯 What Your Pipeline Does

Your `.github/workflows/ci.yml` automatically:

| Task | Tools | Time |
|------|-------|------|
| **Build** | GCC (Debug + Release) | ~2 min |
| **Test** | CUnit (16 tests) | ~1 min |
| **Quality** | cppcheck, clang-format, clang-tidy | ~2 min |
| **Coverage** | lcov + Codecov.io (threshold: 80%) | ~2 min |
| **Security** | Static analysis | ~1 min |

---

## 📋 Quick Steps

### **Step 1: Push Your Code**
```bash
git add .
git commit -m "Your changes"
git push origin main
```

### **Step 2: Check Results**
1. Go to **GitHub Actions** tab
2. Click **CI/CD Pipeline - BMS Diagnostic Engine**
3. Watch it run in real-time ✅

### **Step 3: Download Artifacts**
After completion, download:
- 📊 **coverage-report/** — HTML code coverage
- 📋 **test-report-gcc-Debug/** — Test results
- 🔒 **security-report** — Security findings

---

## ✅ Success Indicators

Your build **PASSES** when you see:

```
✅ Build & Test: PASSED
✅ Code Quality: PASSED  
✅ Code Coverage: PASSED (>80%)
✅ Security Scan: PASSED
```

---

## ⚙️ Configuration

### **Coverage Threshold** (Currently 80%)
To change, edit `.github/workflows/ci.yml`:
```yaml
env:
  COVERAGE_THRESHOLD: 80  # ← Adjust here
```

### **Test Timeout** (Currently 300s)
To change test timeout, edit this line in `ci.yml`:
```yaml
--timeout 300  # ← Change to different value
```

---

## 🛠️ Local Testing (Before Push)

Test locally to catch issues early:

```bash
# Configure build
cmake -B build -DENABLE_TESTS=ON -DENABLE_CODE_COVERAGE=ON

# Build
cmake --build build -j$(nproc)

# Run tests
cd build && ctest --output-on-failure -V

# Check code quality
clang-format --dry-run src/*.c include/*.h
cppcheck src/ include/

# Generate coverage
lcov --directory . --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
# Open coverage_report/index.html in browser
```

---

## ❌ Troubleshooting

| Problem | Solution |
|---------|----------|
| Tests fail | Check **Run Unit Tests** logs in Actions |
| Coverage < 80% | Implement more code or add tests |
| Format issues | Run `clang-format -i src/*.c include/*.h` |
| Build fails | Check **Configure CMake** and **Build** logs |
| Linting warnings | Review **Code Quality** job logs |

---

## 📊 Artifacts Explanation

After each run, artifacts are available for 30 days:

### **coverage-report/**
Interactive HTML showing which lines are tested
- Green = covered
- Red = uncovered
- Shows coverage % per file

### **test-report-gcc-Debug/**
Detailed test execution log with:
- Test names
- Pass/fail status
- Execution time
- Error messages

### **test-report-gcc-Release/**
Same as Debug but for Release build

### **security-report**
Security analysis findings:
- Potential bugs
- Unsafe functions
- Memory issues

---

## 🎯 Next Actions

1. ✅ **Push code:**
   ```bash
   git push origin main
   ```

2. ✅ **Watch it run:**
   - GitHub → Actions tab
   - Should show green checkmarks

3. ✅ **Check results:**
   - All jobs passing?
   - Coverage > 80%?
   - No critical errors?

4. ✅ **Download artifacts:**
   - Especially coverage report
   - Review which areas need testing

---

## 💡 Pro Tips

**Tip 1:** Auto-fix formatting before pushing
```bash
clang-format -i src/*.c include/*.h
git add . && git commit -m "Fix formatting" && git push
```

**Tip 2:** Test locally before pushing
```bash
cd build && ctest -VV  # See detailed output
# Only push if all tests pass!
```

**Tip 3:** Monitor coverage trends
- Keep each commit's coverage moving up
- Aim for 85%+ coverage over time

**Tip 4:** Review security findings
- Download security report after each run
- Fix any critical issues immediately

---

## 📱 GitHub Mobile

You can monitor builds from your phone:
1. GitHub app → Your repo
2. Actions tab → Latest workflow
3. See status and download artifacts

---

## 🔗 Useful Links

- **Workflow File:** `.github/workflows/ci.yml`
- **CMake Config:** `CMakeLists.txt`
- **Code Style:** `.clang-format`
- **Linting Rules:** `.clang-tidy`
- **Main Documentation:** `IMPLEMENTATION_INDEX.md`

---

## ✨ You're Ready!

Just push your code and the CI pipeline handles the rest! 🎉

Questions? Check the detailed logs in GitHub Actions.


### 3. Monitor Workflow

Go to your GitHub repository:
```
GitHub → Actions → ci → Latest run
```

---

## 📊 Files Created

| File | Purpose | Location |
|------|---------|----------|
| **ci.yml** | Main GitHub Actions workflow | `.github/workflows/` |
| **CMakeLists.txt** | Build configuration | Project root |
| **.clang-format** | Code style rules | Project root |
| **.clang-tidy** | Linting rules | Project root |
| **CI_CD_SETUP.md** | Complete documentation | Project root |
| **run_ci.sh** | Local testing script | Project root |
| **CI_CD_QUICK_REFERENCE.md** | This file | Project root |

---

## 🛠️ Local Testing

### Run Full Pipeline (Before Push)
```bash
./run_ci.sh all
```

### Run Debug Build Only
```bash
./run_ci.sh debug
```

### Run Release Build Only
```bash
./run_ci.sh release
```

### Manual Steps

```bash
# Build
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DENABLE_CODE_COVERAGE=ON ..
cmake --build . -j$(nproc)

# Test
ctest --output-on-failure -V

# Check format
clang-format --dry-run src/*.c

# Static analysis
cppcheck --enable=all src/ include/ tests/

# Coverage
ctest --coverage
lcov --directory . --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

---

## 🔄 GitHub Actions Workflow

### Trigger Points
- ✅ Push to `main` or `develop`
- ✅ Pull request to `main` or `develop`

### Jobs (Run in Parallel)

1. **Build & Test** (2 configs)
   - GCC Debug
   - GCC Release
   - Runs all 16 unit tests
   - Reports test failures immediately

2. **Code Quality**
   - cppcheck (static analysis)
   - clang-format (formatting check)
   - clang-tidy (linting)
   - Continues on warnings (informational)

3. **Code Coverage**
   - Builds with coverage instrumentation
   - Generates lcov report
   - Uploads to Codecov.io
   - Target: >80% coverage

4. **Security Scan**
   - cppcheck security checks
   - Vulnerability analysis

5. **Summary**
   - Overall status report
   - Fails if any critical job failed

---

## ✅ Status Checks

### Critical (Must Pass)
- ✅ Build compilation
- ✅ Unit tests (all 16)

### Recommended to Pass
- ✅ Code coverage (>80%)
- ✅ Linting checks

### Informational
- ✅ Security warnings
- ✅ Code style issues

---

## 📈 Test Results

### Expected Output
```
Test project ...
16 tests total
Passed: 16
Failed: 0
Pass Rate: 100%

P0-Critical Tests:
✅ FREEZE_UT_001
✅ FREEZE_UT_002
✅ DRIFT_UT_001
✅ DRIFT_UT_002
```

### If Test Fails
1. Check GitHub Actions logs
2. Download test report artifact
3. Run locally: `./run_ci.sh debug`
4. Fix issue in code
5. Commit and push again

---

## 📊 Coverage Reports

### Access Coverage Report
1. Go to GitHub Actions workflow run
2. Download `coverage-report` artifact
3. Extract and open `index.html`
4. View line-by-line coverage

### Codecov Badge
Add to README.md:
```markdown
[![Code Coverage](https://codecov.io/gh/YOUR_ORG/bms-diagnostics/branch/main/graph/badge.svg)](https://codecov.io/gh/YOUR_ORG/bms-diagnostics)
```

---

## 🛡️ Code Quality

### Enforce Standards

1. **Branch Protection** (Recommended)
   - Go to Settings → Branches
   - Add rule for `main`
   - ✅ Require status checks
   - ✅ Require code reviews
   - ✅ Dismiss stale reviews

2. **Commit Hooks** (Local)
   ```bash
   # Before commit, run linting
   ./run_ci.sh debug
   ```

---

## 📝 Configuration Files

### .clang-format
Code formatting rules (100-char line, 4-space indent, LLVM style)

To apply formatting:
```bash
clang-format -i src/*.c include/*.h
```

### .clang-tidy
Linting and static analysis rules

To check a file:
```bash
clang-tidy src/file.c -- -Iinclude
```

### CMakeLists.txt
Build configuration with test setup

Options:
- `-DENABLE_TESTS=ON` - Enable unit tests
- `-DENABLE_CODE_COVERAGE=ON` - Enable coverage
- `-DCMAKE_BUILD_TYPE=Debug|Release` - Build type

---

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| Workflow not running | Push to `main`/`develop`, check branch filters |
| CUnit not found | `sudo apt-get install libcunit1-dev` |
| Tests timeout | Increase timeout in ci.yml (default 300s) |
| Coverage missing | Ensure `-DENABLE_CODE_COVERAGE=ON` |
| Format warnings | Run: `clang-format -i <file>` |
| Build fails locally | Run: `./run_ci.sh debug` for detailed error |

---

## 📚 Documentation Files

| Document | Purpose |
|----------|---------|
| **CI_CD_SETUP.md** | Comprehensive setup guide (600+ lines) |
| **CI_CD_QUICK_REFERENCE.md** | This quick reference guide |
| **UT_test/UT_Design_Document.md** | Unit test specifications |
| **.github/workflows/ci.yml** | Actual workflow configuration |

---

## 🔗 Integration Points

### With UT Test Design
- Automatically runs 16 baseline tests from `UT test/`
- Maps to P0-Critical, P1-High, P2-Medium priorities
- Tracks coverage per test
- Links defects to test failures

### With GitHub
- Status badges on PR
- Coverage badges on README
- Codecov integration
- Artifact storage (90 days)
- Failure notifications

---

## 🎯 Next Steps

1. ✅ Test locally: `./run_ci.sh all`
2. ✅ Push to GitHub
3. ✅ Monitor Actions tab
4. ✅ Configure branch protection
5. ✅ Set up Codecov.io account
6. ✅ Add badges to README
7. ✅ Share with team

---

## 📞 Quick Commands

```bash
# Build only
mkdir -p build && cd build && cmake .. && cmake --build .

# Test only
cd build && ctest --output-on-failure -V

# Coverage
cd build && ctest --coverage && lcov -d . -c -o coverage.info && genhtml coverage.info -o coverage_report

# Format check
clang-format --dry-run src/*.c

# Full pipeline locally
./run_ci.sh all

# Lint specific file
clang-tidy src/file.c -- -Iinclude

# Static analysis
cppcheck --enable=all src/
```

---

## 📊 Artifacts Generated

**Per Workflow Run:**
- Test reports (stdout)
- Coverage HTML report
- Security scan report
- Build logs

**Stored for:** 90 days  
**Access via:** Actions tab → Workflow run → Artifacts

---

## ✨ Features

✅ **Parallelized** - Jobs run concurrently (faster feedback)  
✅ **Comprehensive** - Compilation, tests, quality, coverage, security  
✅ **Automated** - Runs on push/PR automatically  
✅ **Informative** - Detailed logs and reports  
✅ **Safe** - Quality gates prevent bad code  
✅ **Scalable** - Easy to add more checks  
✅ **Local Mirror** - run_ci.sh replicates GitHub workflow  

---

**Ready to use!** Push to GitHub and watch your tests run automatically. 🚀

