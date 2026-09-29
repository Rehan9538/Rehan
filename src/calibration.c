/**
 * @file calibration.c
 * @brief Calibration offset management implementation
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include "calibration.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Internal Structures
 * ============================================================================ */

typedef struct {
    calibration_offset_t *offsets;
    uint32_t max_sensors;
    uint32_t num_sensors;
} calibration_mgr_internal_t;

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

calibration_mgr_t calibration_create(uint32_t max_sensors) {
    if (max_sensors == 0) {
        return NULL;
    }
    
    calibration_mgr_internal_t *mgr = 
        (calibration_mgr_internal_t *)malloc(sizeof(calibration_mgr_internal_t));
    if (!mgr) {
        return NULL;
    }
    
    mgr->offsets = (calibration_offset_t *)calloc(max_sensors, 
                                                   sizeof(calibration_offset_t));
    if (!mgr->offsets) {
        free(mgr);
        return NULL;
    }
    
    mgr->max_sensors = max_sensors;
    mgr->num_sensors = 0;
    
    return (calibration_mgr_t)mgr;
}

int calibration_destroy(calibration_mgr_t mgr) {
    if (!mgr) {
        return -1;
    }
    
    calibration_mgr_internal_t *m = (calibration_mgr_internal_t *)mgr;
    free(m->offsets);
    free(m);
    
    return 0;
}

int calibration_set_offset(calibration_mgr_t mgr, const char *sensor_id,
                          double offset_celsius) {
    if (!mgr || !sensor_id) {
        return -1;
    }
    
    calibration_mgr_internal_t *m = (calibration_mgr_internal_t *)mgr;
    
    /* Find existing offset entry */
    for (uint32_t i = 0; i < m->num_sensors; i++) {
        if (strcmp(m->offsets[i].sensor_id, sensor_id) == 0) {
            m->offsets[i].offset_celsius = offset_celsius;
            m->offsets[i].is_valid = true;
            return 0;
        }
    }
    
    /* Create new entry if space available */
    if (m->num_sensors >= m->max_sensors) {
        return -1;  /* No space */
    }
    
    strncpy(m->offsets[m->num_sensors].sensor_id, sensor_id, 31);
    m->offsets[m->num_sensors].offset_celsius = offset_celsius;
    m->offsets[m->num_sensors].is_valid = true;
    m->offsets[m->num_sensors].last_calibration = 0;
    m->offsets[m->num_sensors].last_reset = 0;
    
    m->num_sensors++;
    return 0;
}

int calibration_get_offset(calibration_mgr_t mgr, const char *sensor_id,
                          double *offset_celsius) {
    if (!mgr || !sensor_id || !offset_celsius) {
        return -1;
    }
    
    calibration_mgr_internal_t *m = (calibration_mgr_internal_t *)mgr;
    
    for (uint32_t i = 0; i < m->num_sensors; i++) {
        if (strcmp(m->offsets[i].sensor_id, sensor_id) == 0) {
            *offset_celsius = m->offsets[i].offset_celsius;
            return 0;
        }
    }
    
    return -1;  /* Not found */
}

int calibration_reset_offset(calibration_mgr_t mgr, const char *sensor_id) {
    if (!mgr || !sensor_id) {
        return -1;
    }
    
    calibration_mgr_internal_t *m = (calibration_mgr_internal_t *)mgr;
    
    for (uint32_t i = 0; i < m->num_sensors; i++) {
        if (strcmp(m->offsets[i].sensor_id, sensor_id) == 0) {
            m->offsets[i].offset_celsius = 0.0;
            m->offsets[i].last_reset = 0;  /* TODO: Current timestamp */
            return 0;
        }
    }
    
    return -1;  /* Not found */
}

int calibration_apply_offset(calibration_mgr_t mgr, const char *sensor_id,
                            double raw_value, double *corrected_value) {
    if (!mgr || !sensor_id || !corrected_value) {
        return -1;
    }
    
    double offset = 0.0;
    if (calibration_get_offset(mgr, sensor_id, &offset) != 0) {
        offset = 0.0;  /* Default to no offset if not found */
    }
    
    *corrected_value = raw_value + offset;
    return 0;
}

int calibration_get_record(calibration_mgr_t mgr, const char *sensor_id,
                          calibration_offset_t *record) {
    if (!mgr || !sensor_id || !record) {
        return -1;
    }
    
    calibration_mgr_internal_t *m = (calibration_mgr_internal_t *)mgr;
    
    for (uint32_t i = 0; i < m->num_sensors; i++) {
        if (strcmp(m->offsets[i].sensor_id, sensor_id) == 0) {
            *record = m->offsets[i];
            return 0;
        }
    }
    
    return -1;  /* Not found */
}

int calibration_save_to_file(calibration_mgr_t mgr, const char *filepath) {
    if (!mgr || !filepath) {
        return -1;
    }
    
    /* TODO: Serialize offsets to file */
    
    return 0;
}

int calibration_load_from_file(calibration_mgr_t mgr, const char *filepath) {
    if (!mgr || !filepath) {
        return -1;
    }
    
    /* TODO: Deserialize offsets from file */
    
    return 0;
}
