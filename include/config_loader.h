/**
 * @file config_loader.h
 * @brief JSON configuration loading and validation
 * 
 * Loads diagnostic parameters from JSON configuration file with
 * schema validation and error reporting.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Diagnostic configuration parameters
 */
typedef struct {
    /* Freeze detection parameters */
    double freeze_stdev_threshold;      /*!< Std dev threshold for frozen sensor (°C) */
    double neighbor_variation_threshold; /*!< Min neighbor variation (°C) */
    uint32_t freeze_window_duration_ms; /*!< Time window for analysis (ms) */
    uint32_t freeze_persistence_duration_ms; /*!< Hysteresis for freeze detection (ms) */

    /* Drift detection parameters */
    double drift_tolerance_celsius;     /*!< Max deviation from baseline (°C) */
    uint32_t drift_persistence_ms;      /*!< Persistence window for OOS (ms) */
    double spatial_gradient_tolerance_celsius; /*!< Expected gradient between zones (°C) */
    uint32_t min_neighbors_for_baseline; /*!< Min valid neighbors for baseline */

    /* Outlier filtering parameters */
    double outlier_threshold_sigma;     /*!< Z-score threshold for outliers */
    uint32_t min_baseline_samples;      /*!< Min samples before filtering active */

    /* State machine parameters */
    uint32_t suspect_threshold_ms;      /*!< Time to transition to SUSPECT */
    uint32_t faulty_threshold_ms;       /*!< Time to transition to FAULTY */
    uint32_t recovery_threshold_ms;     /*!< Time to recover to HEALTHY */

    /* Startup parameters */
    uint32_t startup_phase_duration_ms; /*!< Duration of startup phase (ms) */

    /* Data handling */
    uint32_t stale_threshold_ms;        /*!< Time before data considered stale */
    uint32_t max_gap_tolerance_ms;      /*!< Max gap before flagged */
    uint32_t buffer_size;               /*!< Sample buffer size */

    /* MQTT parameters */
    char mqtt_broker_endpoint[256];     /*!< AWS IoT endpoint */
    char mqtt_topic[128];               /*!< Publication topic */
    uint32_t queue_max_size;            /*!< Max offline queue size */

    /* Sensor configuration */
    uint32_t num_sensors;               /*!< Number of monitored sensors */
} diagnostic_config_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Load configuration from JSON file
 * 
 * Parses JSON configuration file and populates diagnostic_config_t structure
 * with validation of all required fields.
 *
 * @param[in] config_file Path to JSON configuration file
 * @param[out] config Pointer to diagnostic_config_t (caller allocated)
 * @return 0 on success, negative on error
 */
int config_load(const char *config_file, diagnostic_config_t *config);

/**
 * @brief Validate configuration parameters
 * 
 * Checks all parameters for reasonable ranges and mutual compatibility.
 *
 * @param[in] config Pointer to diagnostic_config_t to validate
 * @return 0 if valid, negative if invalid with error message via stderr
 */
int config_validate(const diagnostic_config_t *config);

/**
 * @brief Get validation error message from last load/validate
 * 
 * @return Error message string, empty if no error
 */
const char* config_get_error_message(void);

/**
 * @brief Get default configuration
 * 
 * Returns hardcoded default configuration (useful for testing).
 *
 * @param[out] config Pointer to diagnostic_config_t (caller allocated)
 * @return 0 on success
 */
int config_get_defaults(diagnostic_config_t *config);

/**
 * @brief Print configuration for debugging
 * 
 * Outputs all configuration parameters to stdout.
 *
 * @param[in] config Pointer to diagnostic_config_t to print
 * @return 0 on success
 */
int config_print(const diagnostic_config_t *config);

/**
 * @brief Load configuration from string buffer
 * 
 * Parses JSON configuration from memory buffer instead of file.
 * Useful for testing with predefined configurations.
 *
 * @param[in] json_string JSON configuration as string
 * @param[in] json_length Length of string in bytes
 * @param[out] config Pointer to diagnostic_config_t (caller allocated)
 * @return 0 on success, negative on error
 */
int config_load_from_string(const char *json_string, size_t json_length, 
                           diagnostic_config_t *config);

#endif /* CONFIG_LOADER_H */
