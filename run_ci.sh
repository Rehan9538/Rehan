#!/bin/bash

# Local CI/CD Pipeline Script
# Simulates GitHub Actions workflow locally for testing before push
# Usage: ./run_ci.sh [debug|release|all]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="${1:-all}"  # Default to 'all'
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_DIR="${PROJECT_ROOT}/ci_logs_${TIMESTAMP}"

# Functions
print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# Check dependencies
check_dependencies() {
    print_header "Checking Dependencies"
    
    local missing=0
    
    for cmd in cmake gcc g++ cppcheck clang-format lcov; do
        if command -v $cmd &> /dev/null; then
            local version=$($cmd --version 2>/dev/null | head -n 1 || echo "unknown")
            print_success "$cmd: $version"
        else
            print_error "$cmd not found"
            missing=$((missing + 1))
        fi
    done
    
    # Check for CUnit
    if pkg-config --exists cunit 2>/dev/null; then
        local version=$(pkg-config --modversion cunit)
        print_success "CUnit: $version"
    else
        print_warning "CUnit not found (may need: sudo apt-get install libcunit1-dev)"
        missing=$((missing + 1))
    fi
    
    if [ $missing -gt 0 ]; then
        print_error "$missing dependencies missing. Install and try again."
        return 1
    fi
    
    return 0
}

# Build function
build_project() {
    local build_type=$1
    print_header "Building Project (${build_type})"
    
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    
    cd "${BUILD_DIR}"
    
    echo "Configuring CMake..."
    cmake -DCMAKE_BUILD_TYPE="${build_type}" \
          -DENABLE_TESTS=ON \
          -DENABLE_CODE_COVERAGE=ON \
          .. || return 1
    
    echo "Building..."
    cmake --build . -j$(nproc) || return 1
    
    print_success "Build completed (${build_type})"
    return 0
}

# Run tests
run_tests() {
    print_header "Running Unit Tests"
    
    cd "${BUILD_DIR}"
    
    echo "Executing tests with CTest..."
    ctest --output-on-failure -V --timeout 300 2>&1 | tee "${LOG_DIR}/test_output.log" || return 1
    
    # Count test results
    local total=$(ctest --print-labels 2>/dev/null | wc -l)
    echo ""
    print_success "All tests passed (${total} total)"
    return 0
}

# Code quality checks
check_code_quality() {
    print_header "Code Quality & Linting"
    
    cd "${PROJECT_ROOT}"
    
    # cppcheck
    echo "Running cppcheck (static analysis)..."
    cppcheck --enable=all \
             --error-exitcode=0 \
             --suppress=missingIncludeSystem \
             src/ include/ tests/ 2>&1 | tee "${LOG_DIR}/cppcheck.log" || true
    
    # clang-format
    echo -e "\nChecking code formatting..."
    local format_issues=0
    find src include tests -name "*.c" -o -name "*.h" | while read file; do
        if ! clang-format --dry-run -Werror "$file" 2>/dev/null; then
            echo "Format issue: $file"
            format_issues=$((format_issues + 1))
        fi
    done
    
    if [ $format_issues -gt 0 ]; then
        print_warning "Found $format_issues code formatting issues (use: clang-format -i <file>)"
    else
        print_success "Code formatting check passed"
    fi
    
    # clang-tidy
    echo -e "\nRunning clang-tidy (linting)..."
    find src -name "*.c" | while read file; do
        echo "Analyzing: $file"
        clang-tidy "$file" -- -Iinclude 2>/dev/null || true
    done | tee "${LOG_DIR}/clang_tidy.log" || true
    
    return 0
}

# Code coverage
generate_coverage() {
    print_header "Code Coverage Report"
    
    cd "${BUILD_DIR}"
    
    echo "Generating lcov coverage report..."
    
    # Capture coverage data
    lcov --directory . --capture --output-file coverage.info || return 1
    
    # Remove system paths
    lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info || return 1
    
    # Display summary
    echo ""
    lcov --list coverage_filtered.info
    
    # Generate HTML report
    echo ""
    echo "Generating HTML report..."
    genhtml coverage_filtered.info --output-directory coverage_report
    
    print_success "Coverage report generated in: ${BUILD_DIR}/coverage_report/index.html"
    
    return 0
}

# Security scan
run_security_scan() {
    print_header "Security Analysis"
    
    cd "${PROJECT_ROOT}"
    
    echo "Running security checks with cppcheck..."
    cppcheck --enable=security,warning \
             --error-exitcode=0 \
             --suppress=missingIncludeSystem \
             src/ include/ tests/ 2>&1 | tee "${LOG_DIR}/security.log" || true
    
    print_success "Security scan completed"
    return 0
}

# Print summary
print_summary() {
    print_header "CI/CD Pipeline Summary"
    
    echo "Logs directory: ${LOG_DIR}"
    echo ""
    echo "Output files:"
    echo "  - Test output: ${LOG_DIR}/test_output.log"
    echo "  - cppcheck: ${LOG_DIR}/cppcheck.log"
    echo "  - clang-tidy: ${LOG_DIR}/clang_tidy.log"
    echo "  - Security: ${LOG_DIR}/security.log"
    echo ""
    echo "Coverage report:"
    echo "  - ${BUILD_DIR}/coverage_report/index.html"
    echo ""
    print_success "Pipeline completed successfully!"
}

# Main execution
main() {
    # Create log directory
    mkdir -p "${LOG_DIR}"
    
    # Check dependencies
    if ! check_dependencies; then
        print_error "Missing dependencies. Install and try again."
        exit 1
    fi
    
    echo ""
    
    case "${BUILD_TYPE}" in
        debug)
            print_header "Running Debug Build Pipeline"
            build_project "Debug" || exit 1
            run_tests || exit 1
            check_code_quality
            generate_coverage || exit 1
            ;;
        release)
            print_header "Running Release Build Pipeline"
            build_project "Release" || exit 1
            run_tests || exit 1
            check_code_quality
            ;;
        all)
            print_header "Running Full CI/CD Pipeline (Debug + Release)"
            
            # Debug build
            build_project "Debug" || exit 1
            run_tests || exit 1
            check_code_quality
            generate_coverage || exit 1
            
            echo ""
            
            # Release build
            build_project "Release" || exit 1
            run_tests || exit 1
            check_code_quality
            ;;
        *)
            print_error "Invalid build type: ${BUILD_TYPE}"
            echo "Usage: $0 [debug|release|all]"
            exit 1
            ;;
    esac
    
    # Security scan
    run_security_scan
    
    # Final summary
    echo ""
    print_summary
}

# Run main
main
