#!/bin/bash
# Test Suite Validation Script
# This script validates the test suite structure without requiring full KDE build environment

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "========================================"
echo "KDE Dolphin Snapshots Pane - Test Suite Validation"
echo "========================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

passed=0
failed=0

check_pass() {
    echo -e "${GREEN}✓${NC} $1"
    passed=$((passed + 1))
}

check_fail() {
    echo -e "${RED}✗${NC} $1"
    failed=$((failed + 1))
}

check_warn() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# Check test files exist
echo "Checking test files..."
echo "----------------------------------------"

if [ -f "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp" ]; then
    check_pass "SnapshotDiscoveryTest.cpp exists"
else
    check_fail "SnapshotDiscoveryTest.cpp missing"
fi

if [ -f "$SCRIPT_DIR/LocalizationTest.cpp" ]; then
    check_pass "LocalizationTest.cpp exists"
else
    check_fail "LocalizationTest.cpp missing"
fi

if [ -f "$SCRIPT_DIR/CMakeLists.txt" ]; then
    check_pass "tests/CMakeLists.txt exists"
else
    check_fail "tests/CMakeLists.txt missing"
fi

if [ -f "$SCRIPT_DIR/README.md" ]; then
    check_pass "tests/README.md exists"
else
    check_fail "tests/README.md missing"
fi

echo ""

# Check test file structure
echo "Validating test structure..."
echo "----------------------------------------"

# Check SnapshotDiscoveryTest.cpp
if grep -q "class SnapshotDiscoveryTest : public QObject" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp"; then
    check_pass "SnapshotDiscoveryTest class properly defined"
else
    check_fail "SnapshotDiscoveryTest class not found"
fi

if grep -q "QTEST_MAIN(SnapshotDiscoveryTest)" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp"; then
    check_pass "SnapshotDiscoveryTest has QTEST_MAIN"
else
    check_fail "SnapshotDiscoveryTest missing QTEST_MAIN"
fi

# Count test methods in SnapshotDiscoveryTest
snapshot_test_count=$(grep -c "void test" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp" || echo 0)
if [ "$snapshot_test_count" -ge 5 ]; then
    check_pass "SnapshotDiscoveryTest has $snapshot_test_count test methods"
else
    check_fail "SnapshotDiscoveryTest has only $snapshot_test_count test methods (expected >= 5)"
fi

# Check LocalizationTest.cpp
if grep -q "class LocalizationTest : public QObject" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "LocalizationTest class properly defined"
else
    check_fail "LocalizationTest class not found"
fi

if grep -q "QTEST_MAIN(LocalizationTest)" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "LocalizationTest has QTEST_MAIN"
else
    check_fail "LocalizationTest missing QTEST_MAIN"
fi

# Count test methods in LocalizationTest
localization_test_count=$(grep -c "void test" "$SCRIPT_DIR/LocalizationTest.cpp" || echo 0)
if [ "$localization_test_count" -ge 5 ]; then
    check_pass "LocalizationTest has $localization_test_count test methods"
else
    check_fail "LocalizationTest has only $localization_test_count test methods (expected >= 5)"
fi

echo ""

# Check CMakeLists.txt configuration
echo "Validating CMake configuration..."
echo "----------------------------------------"

if grep -q "enable_testing()" "$SCRIPT_DIR/CMakeLists.txt"; then
    check_pass "enable_testing() found in tests/CMakeLists.txt"
else
    check_fail "enable_testing() not found"
fi

if grep -q "Qt6::Test" "$SCRIPT_DIR/CMakeLists.txt"; then
    check_pass "Qt6::Test dependency configured"
else
    check_fail "Qt6::Test dependency missing"
fi

if grep -q "add_snapshot_test(SnapshotDiscoveryTest)" "$SCRIPT_DIR/CMakeLists.txt"; then
    check_pass "SnapshotDiscoveryTest added to build"
else
    check_fail "SnapshotDiscoveryTest not added to build"
fi

if grep -q "add_snapshot_test(LocalizationTest)" "$SCRIPT_DIR/CMakeLists.txt"; then
    check_pass "LocalizationTest added to build"
else
    check_fail "LocalizationTest not added to build"
fi

# Check main CMakeLists.txt
if grep -q "add_subdirectory(tests)" "$REPO_ROOT/CMakeLists.txt"; then
    check_pass "Tests subdirectory added to main CMakeLists.txt"
else
    check_fail "Tests not integrated into main build"
fi

echo ""

# Check test coverage areas
echo "Checking test coverage areas..."
echo "----------------------------------------"

# Snapshot discovery coverage
if grep -q "testFindSnapshotDirectory" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp"; then
    check_pass "Snapshot directory discovery tests present"
else
    check_fail "Missing snapshot directory discovery tests"
fi

if grep -q "\.snap" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp" && \
   grep -q "\.snapshot" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp" && \
   grep -q "\.snapshots" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp" && \
   grep -q "\.zfs/snapshot" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp"; then
    check_pass "All snapshot directory types tested (.snap, .snapshot, .snapshots, .zfs/snapshot)"
else
    check_fail "Not all snapshot directory types are tested"
fi

if grep -q "QTemporaryDir" "$SCRIPT_DIR/SnapshotDiscoveryTest.cpp"; then
    check_pass "Tests use temporary directories (good practice)"
else
    check_warn "Tests might not use temporary directories"
fi

# Localization coverage
if grep -q "testPoFilesExist" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "Translation file existence tests present"
else
    check_fail "Missing translation file tests"
fi

if grep -q "testPluralForm" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "Plural form translation tests present"
else
    check_fail "Missing plural form tests"
fi

if grep -q "testContext" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "Context-based translation tests present"
else
    check_fail "Missing context translation tests"
fi

if grep -q "de.po" "$SCRIPT_DIR/LocalizationTest.cpp" && \
   grep -q "en_US.po" "$SCRIPT_DIR/LocalizationTest.cpp"; then
    check_pass "Tests cover both German and English translations"
else
    check_fail "Not all translation files are tested"
fi

echo ""
echo "========================================"
echo "Validation Summary"
echo "========================================"
echo -e "${GREEN}Passed:${NC} $passed"
if [ $failed -gt 0 ]; then
    echo -e "${RED}Failed:${NC} $failed"
else
    echo -e "${GREEN}Failed:${NC} $failed"
fi
echo ""

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}✓ All validation checks passed!${NC}"
    echo ""
    echo "The test suite is properly structured and ready to build."
    echo "To build and run tests, you need:"
    echo "  - CMake >= 3.16"
    echo "  - Qt6 >= 6.8.2 (with Test module)"
    echo "  - KDE Frameworks 6 >= 6.11.0"
    echo "  - extra-cmake-modules (ECM)"
    echo ""
    echo "Build commands:"
    echo "  mkdir build && cd build"
    echo "  cmake .."
    echo "  cmake --build ."
    echo "  ctest --output-on-failure"
    exit 0
else
    echo -e "${RED}✗ Some validation checks failed!${NC}"
    echo "Please review and fix the issues above."
    exit 1
fi
