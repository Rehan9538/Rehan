/**
 * @file calibration.h
 * @brief Calibration offset management and reset
 * 
 * Manages per-sensor calibration offsets for correction of known biases.
 * Supports offset reset for field maintenance and recovery procedures.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Calibration offset record
 */
typedef struct {
    char sensor_id[32];         /*!< Sensor identifier */
    double offset_celsius;      /*!< Calibration offset in °C */
    uint64_t last_calibration;  /*!< Timestamp of last calibration (ms) */
    uint64_t last_reset;        /*!< Timestamp of last reset (ms) */
    bool is_valid;              /*!< True if offset is valid */
} calibration_offset_t;

/**
 * @brief Calibration manager handle (opaque structure)
 */
typedef void* calibration_mgr_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create calibration manager
 * 
 * Initializes calibration offset storage.
 *
 * @param[in] max_sensors Maximum number of sensors to manage
 * @return Handle to calibration manager, NULL on error
 */
calibration_mgr_t calibration_create(uint32_t max_sensors);

/**
 * @brief Destroy calibration manager
 * 
 * @param[in] mgr Manager handle
 * @return 0 on success, negative on error
 */
int calibration_destroy(calibration_mgr_t mgr);

/**
 * @brief Set calibration offset for a sensor
 * 
 * Registers a known calibration offset that can be applied to readings.
 *
 * @param[in] mgr Manager handle
 * @param[in] sensor_id Sensor identifier
 * @param[in] offset_celsius Calibration offset in °C
 * @return 0 on success, negative on error
 */
int calibration_set_offset(calibration_mgr_t mgr, const char *sensor_id,
                          double offset_celsius);

/**
 * @brief Get calibration offset for a sensor
 * 
 * Retrieves current calibration offset.
 *
 * @param[in] mgr Manager handle
 * @param[in] sensor_id Sensor identifier
 * @param[out] offset_celsius Pointer to store offset value
 * @return 0 on success, negative if sensor not found
 */
int calibration_get_offset(calibration_mgr_t mgr, const char *sensor_id,
                          double *offset_celsius);

/**
 * @brief Reset calibration offset to zero
 * 
 * Clears offset for a sensor (field maintenance/recovery procedure).
 * Logs timestamp for audit trail.
 *
 * @param[in] mgr Manager handle
 * @param[in] sensor_id Sensor identifier
 * @return 0 on success, negative if sensor not found
 */
int calibration_reset_offset(calibration_mgr_t mgr, const char *sensor_id);

/**
 * @brief Apply calibration offset to reading
 * 
 * Corrects a raw sensor reading using stored calibration offset.
 * Returns corrected value: reading + offset.
 *
 * @param[in] mgr Manager handle
 * @param[in] sensor_id Sensor identifier
 * @param[in] raw_value Raw sensor reading
 * @param[out] corrected_value Pointer to store corrected value
 * @return 0 on success, negative if sensor not found
 */
int calibration_apply_offset(calibration_mgr_t mgr, const char *sensor_id,
                            double raw_value, double *corrected_value);

/**
 * @brief Get calibration record for audit trail
 * 
 * @param[in] mgr Manager handle
 * @param[in] sensor_id Sensor identifier
 * @param[out] record Pointer to calibration_offset_t (caller allocated)
 * @return 0 on success, negative if sensor not found
 */
int calibration_get_record(calibration_mgr_t mgr, const char *sensor_id,
                          calibration_offset_t *record);

/**
 * @brief Save calibration state to file
 * 
 * Persists all calibration offsets to file for recovery after power loss.
 *
 * @param[in] mgr Manager handle
 * @param[in] filepath Path to save file
 * @return 0 on success, negative on error
 */
int calibration_save_to_file(calibration_mgr_t mgr, const char *filepath);

/**
 * @brief Load calibration state from file
 * 
 * Restores all calibration offsets from previously saved file.
 *
 * @param[in] mgr Manager handle
 * @param[in] filepath Path to load file
 * @return 0 on success, negative on error
 */
int calibration_load_from_file(calibration_mgr_t mgr, const char *filepath);

#endif /* CALIBRATION_H */
