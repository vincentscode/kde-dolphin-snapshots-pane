# Test configuration
enable_testing()

# Find Qt Test module
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)

# Include directories for tests
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${Qt6Test_INCLUDE_DIRS}
)

# Helper function to create a test
function(add_snapshot_test TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/SnapshotsPane.cpp)
    target_link_libraries(${TEST_NAME}
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        KF6::CoreAddons
        KF6::I18n
        KF6::KIOCore
        KF6::KIOFileWidgets
        KF6::KIOWidgets
        KF6::XmlGui
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 30)
endfunction()

# Add snapshot discovery tests
add_snapshot_test(SnapshotDiscoveryTest)

# Add localization tests
add_snapshot_test(LocalizationTest)
