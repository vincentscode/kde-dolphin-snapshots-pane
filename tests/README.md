# Test Suite for KDE Dolphin Snapshots Pane

This directory contains comprehensive tests for the KDE Dolphin Snapshots Pane plugin.

## Test Coverage

### 1. Snapshot Discovery Tests (`SnapshotDiscoveryTest.cpp`)

Tests the snapshot discovery functionality:
- **Snapshot Directory Discovery**: Tests detection of various snapshot directory types (`.snap`, `.snapshot`, `.snapshots`, `.zfs/snapshot`)
- **Priority Handling**: Verifies correct priority when multiple snapshot directories exist
- **Edge Cases**: Tests behavior with no snapshots, empty directories, etc.
- **Multiple Snapshots**: Tests handling of multiple snapshot entries
- **Timestamp Parsing**: Validates snapshot name parsing and ordering

### 2. Localization Tests (`LocalizationTest.cpp`)

Tests the internationalization and localization:
- **Translation Files**: Verifies existence and validity of `.po` files (German and English)
- **Plural Forms**: Tests proper handling of singular/plural translations
- **Context Translations**: Validates context-based translation strings
- **Column Headers**: Tests translation of UI elements (Name, Date)
- **Locale Formatting**: Tests date/time formatting with different locales

## Building and Running Tests

### Prerequisites

You need the following dependencies installed:
- CMake >= 3.16
- Qt6 >= 6.8.2 (Core, Widgets, Test)
- KDE Frameworks 6 >= 6.11.0 (CoreAddons, I18n, XmlGui, KIO)
- extra-cmake-modules (ECM)

### Build Commands

```bash
# From the repository root
mkdir build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the tests
cmake --build . --target SnapshotDiscoveryTest
cmake --build . --target LocalizationTest

# Or build all tests
cmake --build .
```

### Running Tests

```bash
# From the build directory

# Run all tests
ctest --output-on-failure

# Run individual tests
./tests/SnapshotDiscoveryTest
./tests/LocalizationTest

# Run with verbose output
ctest -V

# Run specific test
ctest -R SnapshotDiscoveryTest
```

## Test Structure

Each test uses Qt Test framework (`QTest`) and follows these patterns:

- `initTestCase()`: Setup run once before all tests
- `cleanupTestCase()`: Cleanup run once after all tests
- `init()`: Setup run before each test case
- `cleanup()`: Cleanup run after each test case
- Test methods: Individual test cases following naming convention `test*`

## Adding New Tests

To add a new test:

1. Create a new `.cpp` file in this directory
2. Include necessary headers and QTest framework
3. Create a test class inheriting from `QObject`
4. Add test methods as private slots
5. Use `QTEST_MAIN(YourTestClass)` macro
6. Update `CMakeLists.txt` to include your test:
   ```cmake
   add_snapshot_test(YourNewTest)
   ```

## Continuous Integration

These tests are designed to run in CI environments. They:
- Use temporary directories for file system tests
- Don't require system-wide installations
- Clean up after themselves
- Provide detailed failure messages
- Have reasonable timeouts (30 seconds per test)

## Test Output

Tests use standard Qt Test output format:
- `PASS`: Test passed successfully
- `FAIL`: Test failed with details
- `XFAIL`: Expected failure (known issue)
- `SKIP`: Test skipped due to missing requirements
