/**
 * @file drift_detector.h
 * @brief Out-of-specification (OOS) drift detection for sensor calibration
 * 
 * Detects when a sensor reading deviates significantly from neighbor baselines,
 * indicating possible calibration drift or sensor malfunction.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef DRIFT_DETECTOR_H
#define DRIFT_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Drift detection result
 */
typedef struct {
    bool is_oos;                    /*!< True if out-of-specification detected */
    double deviation_from_baseline; /*!< Measured deviation in sensor units */
    double baseline_value;          /*!< Calculated baseline from neighbors */
    double confidence;              /*!< Confidence of OOS detection (0-100%) */
    uint32_t neighbor_count;        /*!< Number of valid neighbors used */
    uint64_t persistence_time_ms;   /*!< Duration of OOS condition */
} drift_result_t;

/**
 * @brief Drift detector handle (opaque structure)
 */
typedef void* drift_detector_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create a drift detector
 * 
 * @param[in] tolerance_celsius Maximum acceptable deviation in °C
 * @param[in] persistence_ms Minimum persistence before flagging OOS
 * @param[in] min_neighbors Minimum valid neighbors required for baseline
 * @return Handle to drift detector, NULL on error
 */
drift_detector_t drift_detector_create(double tolerance_celsius, 
                                       uint32_t persistence_ms, 
                                       uint32_t min_neighbors);

/**
 * @brief Destroy a drift detector
 * 
 * @param[in] detector Detector handle
 * @return 0 on success, negative on error
 */
int drift_detector_destroy(drift_detector_t detector);

/**
 * @brief Analyze sensor for drift using neighbor baseline
 * 
 * Compares sensor value against median/mean of valid neighbor readings.
 *
 * @param[in] detector Detector handle
 * @param[in] sensor_value Current sensor reading
 * @param[in] neighbor_values Array of neighbor sensor values
 * @param[in] neighbor_count Number of neighbor values in array
 * @param[in] timestamp_ms Current timestamp in milliseconds
 * @param[out] result Pointer to drift_result_t (caller allocated)
 * @return 0 on success, negative on error
 */
int drift_detector_analyze(drift_detector_t detector, double sensor_value, 
                          const double *neighbor_values, 
                          uint32_t neighbor_count, uint64_t timestamp_ms,
                          drift_result_t *result);

/**
 * @brief Reset drift detector state
 * 
 * Clears persistence counters and history.
 *
 * @param[in] detector Detector handle
 * @return 0 on success, negative on error
 */
int drift_detector_reset(drift_detector_t detector);

/**
 * @brief Set spatial gradient tolerance for multi-zone systems
 * 
 * Allows expected gradients (e.g., 0.2°C between zones) to be excluded
 * from drift detection.
 *
 * @param[in] detector Detector handle
 * @param[in] gradient_tolerance_celsius Maximum expected gradient
 * @return 0 on success, negative on error
 */
int drift_detector_set_gradient_tolerance(drift_detector_t detector, 
                                          double gradient_tolerance_celsius);

/**
 * @brief Mark a neighbor sensor as invalid
 * 
 * Excludes a neighbor from baseline calculations (e.g., if faulty).
 *
 * @param[in] detector Detector handle
 * @param[in] neighbor_index Index in neighbor array
 * @return 0 on success, negative on error
 */
int drift_detector_invalidate_neighbor(drift_detector_t detector, 
                                       uint32_t neighbor_index);

#endif /* DRIFT_DETECTOR_H */
