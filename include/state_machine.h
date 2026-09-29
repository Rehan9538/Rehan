/**
 * @file state_machine.h
 * @brief Sensor health state machine with hysteresis
 * 
 * Implements state transitions for sensor health (HEALTHY → SUSPECT → FAULTY),
 * with configurable hysteresis to prevent oscillation and ensure stable decisions.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "bms_diagnostics.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief State machine transition event
 */
typedef enum {
    EVENT_FAULT_DETECTED = 0,    /*!< Anomaly detected */
    EVENT_FAULT_CLEARED = 1,     /*!< Anomaly evidence removed */
    EVENT_RESET = 2              /*!< Manual state reset */
} state_event_t;

/**
 * @brief State machine configuration
 */
typedef struct {
    uint32_t suspect_threshold_ms;  /*!< Time to transition HEALTHY→SUSPECT */
    uint32_t faulty_threshold_ms;   /*!< Time to transition SUSPECT→FAULTY */
    uint32_t recovery_threshold_ms; /*!< Time to transition FAULTY→HEALTHY */
} state_machine_config_t;

/**
 * @brief State machine handle (opaque structure)
 */
typedef void* state_machine_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create a state machine for a sensor
 * 
 * @param[in] sensor_id Unique sensor identifier
 * @param[in] config Pointer to state machine configuration
 * @return Handle to state machine, NULL on error
 */
state_machine_t state_machine_create(const char *sensor_id, 
                                    const state_machine_config_t *config);

/**
 * @brief Destroy a state machine
 * 
 * @param[in] state_machine State machine handle
 * @return 0 on success, negative on error
 */
int state_machine_destroy(state_machine_t state_machine);

/**
 * @brief Process an event and update state
 * 
 * Advances state machine based on event and current hysteresis counters.
 * Implements state transitions with persistence thresholds.
 *
 * @param[in] state_machine State machine handle
 * @param[in] event Event to process
 * @param[in] timestamp_ms Current timestamp in milliseconds
 * @return 0 on success, negative on error
 */
int state_machine_process_event(state_machine_t state_machine, 
                                state_event_t event, uint64_t timestamp_ms);

/**
 * @brief Get current sensor state
 * 
 * @param[in] state_machine State machine handle
 * @param[out] state Pointer to sensor_state_t (caller allocated)
 * @return 0 on success, negative on error
 */
int state_machine_get_state(state_machine_t state_machine, sensor_state_t *state);

/**
 * @brief Set state evidence/description
 * 
 * Updates the evidence field with current fault information.
 *
 * @param[in] state_machine State machine handle
 * @param[in] evidence Description of evidence (max 256 chars)
 * @param[in] confidence Confidence level (0-100%)
 * @param[in] fault_type Type of fault detected
 * @return 0 on success, negative on error
 */
int state_machine_set_evidence(state_machine_t state_machine, 
                               const char *evidence, double confidence,
                               fault_type_t fault_type);

/**
 * @brief Reset state machine to HEALTHY
 * 
 * Clears all hysteresis counters and returns sensor to HEALTHY state.
 *
 * @param[in] state_machine State machine handle
 * @param[in] timestamp_ms Current timestamp in milliseconds
 * @return 0 on success, negative on error
 */
int state_machine_reset(state_machine_t state_machine, uint64_t timestamp_ms);

/**
 * @brief Get state transition history
 * 
 * Returns list of recent state transitions for audit trail.
 *
 * @param[in] state_machine State machine handle
 * @param[out] history Array of state strings (caller allocated)
 * @param[in] max_history Maximum history entries
 * @param[out] num_history Actual number of entries returned
 * @return 0 on success, negative on error
 */
int state_machine_get_history(state_machine_t state_machine, 
                              char history[][64], uint32_t max_history, 
                              uint32_t *num_history);

/**
 * @brief Get hysteresis persistence counter
 * 
 * Returns current persistence counter for diagnostics/debugging.
 *
 * @param[in] state_machine State machine handle
 * @return Current persistence counter value
 */
uint32_t state_machine_get_persistence_counter(state_machine_t state_machine);

#endif /* STATE_MACHINE_H */
