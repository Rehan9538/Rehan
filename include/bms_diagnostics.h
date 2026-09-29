/**
 * @file bms_diagnostics.h
 * @brief Main diagnostic engine for BMS sensor plausibility and cross-calibration
 * 
 * This is the primary API for the BMS Sensor Plausibility & Cross-Calibration
 * Diagnostic Engine. It provides functions for initializing the diagnostic system,
 * ingesting sensor data, and retrieving sensor health states.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef BMS_DIAGNOSTICS_H
#define BMS_DIAGNOSTICS_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Sensor health states
 */
typedef enum {
    HEALTH_UNKNOWN = -1,   /*!< Unknown state (initialization) */
    HEALTH_HEALTHY = 0,    /*!< Sensor operating normally */
    HEALTH_SUSPECT = 1,    /*!< Anomaly detected, confidence increasing */
    HEALTH_FAULTY = 2,     /*!< Confirmed fault, requires maintenance */
    HEALTH_IN_STARTUP = 3  /*!< System startup phase, no confidence */
} health_status_t;

/**
 * @brief Diagnostic fault types
 */
typedef enum {
    FAULT_NONE = 0,        /*!< No fault detected */
    FAULT_FREEZE = 1,      /*!< Sensor reading frozen/static */
    FAULT_OOS = 2,         /*!< Out-of-specification (drift) detected */
    FAULT_UNKNOWN = 3      /*!< Unknown/unclassified fault */
} fault_type_t;

/**
 * @brief Sensor health state structure
 * 
 * Contains comprehensive diagnostic information for a single sensor,
 * including health status, fault type, confidence metrics, and evidence.
 */
typedef struct {
    char sensor_id[32];              /*!< Unique sensor identifier */
    health_status_t health;          /*!< Current health status */
    fault_type_t fault_type;         /*!< Type of fault (if any) */
    double confidence;               /*!< Confidence level (0-100%) */
    double deviation_celsius;        /*!< Measured deviation from baseline */
    double standard_deviation;       /*!< Computed standard deviation */
    uint64_t last_update_timestamp;  /*!< Last update time (milliseconds) */
    char evidence[256];              /*!< Description of supporting evidence */
    uint32_t persistent_count;       /*!< Persistence counter for hysteresis */
} sensor_state_t;

/**
 * @brief Audit log entry for compliance and debugging
 */
typedef struct {
    uint64_t timestamp;              /*!< Event timestamp (milliseconds) */
    char sensor_id[32];              /*!< Affected sensor */
    char event_type[32];             /*!< Type of event (e.g., STATE_CHANGE) */
    char details[256];               /*!< Event details */
} audit_log_entry_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Initialize the diagnostic engine
 * 
 * Sets up the diagnostic system with provided configuration parameters.
 * Must be called once before any sensor data can be ingested.
 *
 * @param[in] config_file Path to JSON configuration file
 * @return 0 on success, negative value on error
 */
int bms_diagnostics_init(const char *config_file);

/**
 * @brief Shut down the diagnostic engine
 * 
 * Cleans up resources and flushes any pending audit logs.
 *
 * @return 0 on success, negative value on error
 */
int bms_diagnostics_shutdown(void);

/**
 * @brief Ingest a sensor reading
 * 
 * Process a new sensor reading with timestamp. Data is added to rolling
 * window buffers and analyzed for anomalies.
 *
 * @param[in] sensor_id Unique sensor identifier
 * @param[in] value Sensor reading value (temperature in °C)
 * @param[in] timestamp_ms Timestamp in milliseconds (or 0 for current time)
 * @return 0 on success, negative value on error
 */
int bms_ingest_reading(const char *sensor_id, double value, uint64_t timestamp_ms);

/**
 * @brief Perform diagnostic analysis for all sensors
 * 
 * Runs freeze detection, drift detection, outlier filtering, and state
 * machine updates. Should be called periodically (e.g., every 20ms).
 *
 * @return 0 on success, negative value on error
 */
int bms_diagnose(void);

/**
 * @brief Get current health state of a sensor
 * 
 * @param[in] sensor_id Unique sensor identifier
 * @param[out] state Pointer to sensor_state_t structure (caller allocated)
 * @return 0 on success, negative value on error
 */
int bms_get_sensor_state(const char *sensor_id, sensor_state_t *state);

/**
 * @brief Get health states for all sensors
 * 
 * @param[out] states Array of sensor_state_t structures (caller allocated)
 * @param[in] max_sensors Maximum number of sensors to return
 * @param[out] num_sensors Actual number of sensors returned
 * @return 0 on success, negative value on error
 */
int bms_get_all_sensor_states(sensor_state_t *states, uint32_t max_sensors, 
                              uint32_t *num_sensors);

/**
 * @brief Get audit log entries
 * 
 * Retrieve recent audit log entries for compliance and debugging.
 *
 * @param[out] entries Array of audit_log_entry_t structures (caller allocated)
 * @param[in] max_entries Maximum number of entries to return
 * @param[out] num_entries Actual number of entries returned
 * @return 0 on success, negative value on error
 */
int bms_get_audit_log(audit_log_entry_t *entries, uint32_t max_entries, 
                      uint32_t *num_entries);

/**
 * @brief Get overall system health status
 * 
 * Returns aggregate health considering all monitored sensors.
 *
 * @return Overall health status (HEALTHY, SUSPECT, or FAULTY)
 */
health_status_t bms_get_system_health(void);

/**
 * @brief Get number of active sensors
 * 
 * @return Number of sensors currently being monitored
 */
uint32_t bms_get_sensor_count(void);

/**
 * @brief Clear all sensor states and restart diagnostics
 * 
 * Useful for testing or system reset. Does not reload configuration.
 *
 * @return 0 on success, negative value on error
 */
int bms_reset_states(void);

#endif /* BMS_DIAGNOSTICS_H */
