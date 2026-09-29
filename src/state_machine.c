/**
 * @file state_machine.c
 * @brief Sensor health state machine implementation with hysteresis
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include "state_machine.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Internal Structures
 * ============================================================================ */

#define STATE_HISTORY_SIZE 20

typedef struct {
    char sensor_id[32];
    health_status_t current_state;
    fault_type_t fault_type;
    state_machine_config_t config;
    uint64_t state_change_time;
    uint64_t persistence_timer;
    uint32_t persistence_counter;
    double confidence;
    char evidence[256];
    
    char history[STATE_HISTORY_SIZE][64];
    uint32_t history_index;
} state_machine_internal_t;

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

state_machine_t state_machine_create(const char *sensor_id,
                                    const state_machine_config_t *config) {
    if (!sensor_id || !config) {
        return NULL;
    }
    
    state_machine_internal_t *sm = 
        (state_machine_internal_t *)malloc(sizeof(state_machine_internal_t));
    if (!sm) {
        return NULL;
    }
    
    strncpy(sm->sensor_id, sensor_id, sizeof(sm->sensor_id) - 1);
    sm->current_state = HEALTH_HEALTHY;
    sm->fault_type = FAULT_NONE;
    sm->config = *config;
    sm->state_change_time = 0;
    sm->persistence_timer = 0;
    sm->persistence_counter = 0;
    sm->confidence = 0.0;
    sm->evidence[0] = '\0';
    sm->history_index = 0;
    
    return (state_machine_t)sm;
}

int state_machine_destroy(state_machine_t state_machine) {
    if (!state_machine) {
        return -1;
    }
    
    free((state_machine_internal_t *)state_machine);
    return 0;
}

int state_machine_process_event(state_machine_t state_machine,
                                state_event_t event, uint64_t timestamp_ms) {
    if (!state_machine) {
        return -1;
    }
    
    (void)event;
    (void)timestamp_ms;
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    (void)sm;
    
    /* TODO: Implement state transitions with hysteresis */
    /* HEALTHY + FAULT_DETECTED -> SUSPECT (after persistence_ms) */
    /* SUSPECT + FAULT_DETECTED -> FAULTY (after persistence_ms) */
    /* FAULTY + FAULT_CLEARED -> SUSPECT (after recovery_ms) */
    /* SUSPECT + FAULT_CLEARED -> HEALTHY (after recovery_ms) */
    /* * + EVENT_RESET -> HEALTHY immediately */
    
    return 0;
}

int state_machine_get_state(state_machine_t state_machine,
                           sensor_state_t *state) {
    if (!state_machine || !state) {
        return -1;
    }
    
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    
    strncpy(state->sensor_id, sm->sensor_id, sizeof(state->sensor_id) - 1);
    state->sensor_id[sizeof(state->sensor_id) - 1] = '\0';  /* Ensure null termination */
    state->health = sm->current_state;
    state->fault_type = sm->fault_type;
    state->confidence = sm->confidence;
    state->persistent_count = sm->persistence_counter;
    state->last_update_timestamp = 0;  /* TODO: Track timestamp */
    strncpy(state->evidence, sm->evidence, sizeof(state->evidence) - 1);
    state->evidence[sizeof(state->evidence) - 1] = '\0';  /* Ensure null termination */
    
    return 0;
}

int state_machine_set_evidence(state_machine_t state_machine,
                               const char *evidence, double confidence,
                               fault_type_t fault_type) {
    if (!state_machine || !evidence) {
        return -1;
    }
    
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    
    strncpy(sm->evidence, evidence, sizeof(sm->evidence) - 1);
    sm->confidence = confidence;
    sm->fault_type = fault_type;
    
    return 0;
}

int state_machine_reset(state_machine_t state_machine, uint64_t timestamp_ms) {
    if (!state_machine) {
        return -1;
    }
    
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    
    sm->current_state = HEALTH_HEALTHY;
    sm->fault_type = FAULT_NONE;
    sm->persistence_counter = 0;
    sm->persistence_timer = 0;
    sm->confidence = 0.0;
    sm->state_change_time = timestamp_ms;
    sm->evidence[0] = '\0';
    
    return 0;
}

int state_machine_get_history(state_machine_t state_machine,
                              char history[][64], uint32_t max_history,
                              uint32_t *num_history) {
    if (!state_machine || !history || !num_history) {
        return -1;
    }
    
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    
    uint32_t count = sm->history_index < max_history ? 
                     sm->history_index : max_history;
    
    for (uint32_t i = 0; i < count; i++) {
        strncpy(history[i], sm->history[i], 63);
    }
    
    *num_history = count;
    return 0;
}

uint32_t state_machine_get_persistence_counter(state_machine_t state_machine) {
    if (!state_machine) {
        return 0;
    }
    
    state_machine_internal_t *sm = (state_machine_internal_t *)state_machine;
    return sm->persistence_counter;
}
