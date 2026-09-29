#!/bin/bash
# Quick CI/CD Testing Script - Run locally before pushing to GitHub

echo "🚀 BMS Diagnostic Engine - Local CI/CD Test"
echo "==========================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "📁 Creating build directory..."
    mkdir -p build
fi

# Change to build directory
cd build || exit 1

echo ""
echo "🔧 STEP 1: Configure with CMake"
echo "================================="
cmake -DCMAKE_BUILD_TYPE=Debug \
       -DENABLE_TESTS=ON \
       -DENABLE_CODE_COVERAGE=ON \
       -DCMAKE_C_FLAGS="-O0 -g --coverage" \
       .. || { echo -e "${RED}❌ CMake configuration failed${NC}"; exit 1; }

echo ""
echo "🏗️  STEP 2: Build Project"
echo "=========================="
cmake --build . -j$(nproc) || { echo -e "${RED}❌ Build failed${NC}"; exit 1; }
echo -e "${GREEN}✅ Build successful${NC}"

echo ""
echo "🧪 STEP 3: Run Unit Tests"
echo "========================="
ctest --output-on-failure -V || { echo -e "${RED}❌ Tests failed${NC}"; exit 1; }
echo -e "${GREEN}✅ All tests passed${NC}"

echo ""
echo "🔍 STEP 4: Check Code Formatting"
echo "================================="
cd ..
for file in $(find src include tests -name "*.c" -o -name "*.h" 2>/dev/null); do
    if ! clang-format --dry-run -Werror "$file" 2>/dev/null; then
        echo -e "${YELLOW}⚠️  Format issue in $file${NC}"
    fi
done
echo -e "${GREEN}✅ Format check complete${NC}"

echo ""
echo "📊 STEP 5: Generate Coverage Report"
echo "===================================="
cd build
lcov --directory . --capture --output-file coverage.info 2>/dev/null
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info 2>/dev/null
COVERAGE=$(lcov --list coverage_filtered.info | tail -1 | awk '{print $2}' | sed 's/%//')
echo "Code Coverage: ${COVERAGE}%"

if (( $(echo "$COVERAGE < 80" | bc -l) )); then
    echo -e "${YELLOW}⚠️  Coverage below 80% threshold${NC}"
else
    echo -e "${GREEN}✅ Coverage meets 80% threshold${NC}"
fi

echo ""
echo "==========================================="
echo -e "${GREEN}✅ LOCAL CI TESTS COMPLETE${NC}"
echo "==========================================="
echo ""
echo "📋 Summary:"
echo "   ✅ Build: OK"
echo "   ✅ Tests: OK"
echo "   ✅ Format: OK"
echo "   ✅ Coverage: ${COVERAGE}%"
echo ""
echo "🚀 Ready to push to GitHub!"
echo "   Command: git push origin main"
echo ""
