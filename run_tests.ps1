# run_tests.ps1 - PowerShell script to run CI tests locally

Write-Host "🚀 BMS Diagnostic Engine - Local Test Script" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Colors
$GREEN = "`e[32m"
$RED = "`e[31m"
$YELLOW = "`e[33m"
$RESET = "`e[0m"

# Check if build directory exists
if (-not (Test-Path "build")) {
    Write-Host "📁 Creating build directory..." -ForegroundColor Cyan
    New-Item -ItemType Directory -Path "build" -Force | Out-Null
}

# Change to build directory
Set-Location "build"

Write-Host ""
Write-Host "🔧 STEP 1: Configure with CMake" -ForegroundColor Cyan
Write-Host "=================================" -ForegroundColor Cyan
cmake -DCMAKE_BUILD_TYPE=Debug `
       -DENABLE_TESTS=ON `
       -DENABLE_CODE_COVERAGE=ON `
       -DCMAKE_C_FLAGS="-O0 -g --coverage" `
       -G "Unix Makefiles" `
       .. 

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake configuration failed" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "🏗️  STEP 2: Build Project" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
cmake --build . -j4

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Build failed" -ForegroundColor Red
    exit 1
}
Write-Host "✅ Build successful" -ForegroundColor Green

Write-Host ""
Write-Host "🧪 STEP 3: Run Unit Tests" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
ctest --output-on-failure -V

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Tests failed" -ForegroundColor Red
    exit 1
}
Write-Host "✅ All tests passed" -ForegroundColor Green

Write-Host ""
Write-Host "=============================================" -ForegroundColor Green
Write-Host "✅ LOCAL CI TESTS COMPLETE" -ForegroundColor Green
Write-Host "=============================================" -ForegroundColor Green
Write-Host ""
Write-Host "📋 Summary:" -ForegroundColor Cyan
Write-Host "   ✅ Build: OK" -ForegroundColor Green
Write-Host "   ✅ Tests: OK" -ForegroundColor Green
Write-Host ""
Write-Host "🚀 Ready to push to GitHub!" -ForegroundColor Cyan
Write-Host "   Command: git push origin main" -ForegroundColor Cyan
Write-Host ""
