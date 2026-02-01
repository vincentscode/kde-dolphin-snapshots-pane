cmake_minimum_required(VERSION 3.16)

set (RELEASE_SERVICE_VERSION_MAJOR "1")
set (RELEASE_SERVICE_VERSION_MINOR "0")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kde-dolphin-snapshots-pane VERSION ${RELEASE_SERVICE_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(QT_MIN_VERSION "6.8.2")
set(KF_MIN_VERSION "6.11.0")

set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build" CACHE PATH "Build directory" FORCE)

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(QtVersionOption)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
include(ECMOptionalAddSubdirectory)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Core5Compat
    Widgets
    Network
    DBus
)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    XmlGui
    I18n
    KIO
    TextWidgets
    Config
    CoreAddons
    WidgetsAddons
    Solid
)

add_library(
    kde-dolphin-snapshots-pane
    SHARED
    src/SnapshotFinder.h
    src/SnapshotFinder.cpp
    src/SnapshotsPane.h
    src/SnapshotsPane.cpp
    src/SnapshotsPanePlugin.h
    src/SnapshotsPanePlugin.cpp
)

target_link_libraries(kde-dolphin-snapshots-pane KF6::CoreAddons KF6::XmlGui KF6::I18n)
target_link_libraries(kde-dolphin-snapshots-pane KF6::KIOCore KF6::KIOFileWidgets KF6::KIOWidgets)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${KF6_INCLUDE_DIRS}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

install(TARGETS kde-dolphin-snapshots-pane DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/propertiesdialog/)
install(FILES kde-dolphin-snapshots-pane.json DESTINATION  ${KDE_INSTALL_PLUGINDIR}/kf6/propertiesdialog/)

ki18n_install(po)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()