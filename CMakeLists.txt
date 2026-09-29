cmake_minimum_required(VERSION 3.20)
project(BMS_Diagnostics C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type defaults to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Project version
set(PROJECT_VERSION 1.0.0)
set(PROJECT_DESCRIPTION "BMS Sensor Plausibility & Cross-Calibration Diagnostic Engine")

# Options
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_CODE_COVERAGE "Enable code coverage reporting" OFF)

# Add compile flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Coverage flags
if(ENABLE_CODE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/bms_diagnostics.c
    src/rolling_window.c
    src/drift_detector.c
    src/outlier_filter.c
    src/state_machine.c
    src/config_loader.c
    src/mqtt_reporter.c
    src/event_queue.c
)

# Create main library
add_library(bms_diagnostics STATIC ${SOURCES})
target_include_directories(bms_diagnostics PUBLIC include)

# Unit tests
if(ENABLE_TESTS)
    enable_testing()
    
    # Find CUnit using pkg-config (more reliable on Linux)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(CUNIT cunit)
    endif()
    
    # Fallback: manual library detection
    if(NOT CUNIT_FOUND)
        find_library(CUNIT_LIBRARY cunit REQUIRED)
        find_path(CUNIT_INCLUDE_DIR CUnit/CUnit.h REQUIRED)
        set(CUNIT_LIBRARIES ${CUNIT_LIBRARY})
        set(CUNIT_INCLUDE_DIRS ${CUNIT_INCLUDE_DIR})
    endif()
    
    set(TEST_SOURCES
        tests/test_freeze_detection.c
        tests/test_drift_detection.c
        tests/test_outlier_filter.c
        tests/test_state_machine.c
        tests/test_edge_cases.c
        tests/test_config.c
        tests/test_reset.c
        tests/test_mqtt.c
        tests/test_runner.c
    )
    
    # Create test executable
    add_executable(bms_diagnostic_tests ${TEST_SOURCES})
    target_link_libraries(bms_diagnostic_tests PRIVATE bms_diagnostics ${CUNIT_LIBRARIES})
    target_include_directories(bms_diagnostic_tests PRIVATE include ${CUNIT_INCLUDE_DIRS})
    
    # Add tests
    add_test(NAME FREEZE_UT_001 COMMAND bms_diagnostic_tests freeze_001)
    add_test(NAME FREEZE_UT_002 COMMAND bms_diagnostic_tests freeze_002)
    add_test(NAME FREEZE_UT_003 COMMAND bms_diagnostic_tests freeze_003)
    add_test(NAME FREEZE_UT_004 COMMAND bms_diagnostic_tests freeze_004)
    
    add_test(NAME DRIFT_UT_001 COMMAND bms_diagnostic_tests drift_001)
    add_test(NAME DRIFT_UT_002 COMMAND bms_diagnostic_tests drift_002)
    add_test(NAME DRIFT_UT_003 COMMAND bms_diagnostic_tests drift_003)
    
    add_test(NAME OUTLIER_UT_001 COMMAND bms_diagnostic_tests outlier_001)
    
    add_test(NAME STATE_UT_001 COMMAND bms_diagnostic_tests state_001)
    
    add_test(NAME EDGE_UT_001 COMMAND bms_diagnostic_tests edge_001)
    add_test(NAME EDGE_UT_002 COMMAND bms_diagnostic_tests edge_002)
    add_test(NAME EDGE_UT_003 COMMAND bms_diagnostic_tests edge_003)
    
    add_test(NAME CONFIG_UT_001 COMMAND bms_diagnostic_tests config_001)
    
    add_test(NAME RESET_UT_001 COMMAND bms_diagnostic_tests reset_001)
    
    add_test(NAME MQTT_UT_001 COMMAND bms_diagnostic_tests mqtt_001)
    add_test(NAME MQTT_UT_002 COMMAND bms_diagnostic_tests mqtt_002)
    
    # Set test timeout to 300 seconds
    set_tests_properties(
        FREEZE_UT_001 FREEZE_UT_002 FREEZE_UT_003 FREEZE_UT_004
        DRIFT_UT_001 DRIFT_UT_002 DRIFT_UT_003
        OUTLIER_UT_001
        STATE_UT_001
        EDGE_UT_001 EDGE_UT_002 EDGE_UT_003
        CONFIG_UT_001
        RESET_UT_001
        MQTT_UT_001 MQTT_UT_002
        PROPERTIES TIMEOUT 300
    )
    
    # Add test target
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -V
        DEPENDS bms_diagnostic_tests
        COMMENT "Running unit tests..."
    )
endif()

# Create executable (optional main program)
add_executable(bms_service src/main.c)
target_link_libraries(bms_service PRIVATE bms_diagnostics)
target_include_directories(bms_service PRIVATE include)

# Install targets
install(TARGETS bms_diagnostics bms_service
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Print configuration summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "BMS Diagnostics Engine - Build Summary")
message(STATUS "========================================")
message(STATUS "Project Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Unit Tests Enabled: ${ENABLE_TESTS}")
message(STATUS "Code Coverage Enabled: ${ENABLE_CODE_COVERAGE}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "========================================")
message(STATUS "")
