find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)

add_executable(SnapshotFinderTest
    SnapshotFinderTest.cpp
    ${CMAKE_SOURCE_DIR}/src/SnapshotFinder.cpp
)

set_target_properties(SnapshotFinderTest PROPERTIES AUTOMOC ON)
target_include_directories(SnapshotFinderTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(SnapshotFinderTest
    Qt6::Core
    Qt6::Test
)

add_test(NAME SnapshotFinderTest COMMAND SnapshotFinderTest)
