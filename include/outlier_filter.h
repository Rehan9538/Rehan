/**
 * @file outlier_filter.h
 * @brief Statistical outlier detection and filtering
 * 
 * Identifies and excludes isolated spike/drop outliers (>3σ from mean)
 * from baseline calculations to improve robustness to transient noise.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef OUTLIER_FILTER_H
#define OUTLIER_FILTER_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Outlier detection result
 */
typedef struct {
    bool is_outlier;              /*!< True if value is outlier (>3σ) */
    double z_score;               /*!< Z-score of value (std devs from mean) */
    double threshold_sigma;       /*!< Sigma threshold used (typically 3.0) */
    double outlier_threshold_value; /*!< Computed threshold value */
} outlier_result_t;

/**
 * @brief Outlier filter handle (opaque structure)
 */
typedef void* outlier_filter_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create an outlier filter
 * 
 * @param[in] threshold_sigma Number of standard deviations (typically 3.0)
 * @param[in] min_baseline_samples Minimum samples before filtering active
 * @return Handle to outlier filter, NULL on error
 */
outlier_filter_t outlier_filter_create(double threshold_sigma, 
                                       uint32_t min_baseline_samples);

/**
 * @brief Destroy an outlier filter
 * 
 * @param[in] filter Filter handle
 * @return 0 on success, negative on error
 */
int outlier_filter_destroy(outlier_filter_t filter);

/**
 * @brief Add a sample and update baseline statistics
 * 
 * Incorporates sample into baseline (mean/stddev) calculations.
 * Outlier samples should still be added but flagged separately.
 *
 * @param[in] filter Filter handle
 * @param[in] value Sample value
 * @param[in] is_excluded True if sample should be excluded from baseline
 * @return 0 on success, negative on error
 */
int outlier_filter_add_sample(outlier_filter_t filter, double value, 
                              bool is_excluded);

/**
 * @brief Detect if a value is an outlier
 * 
 * Compares value against current baseline (mean ± 3σ).
 *
 * @param[in] filter Filter handle
 * @param[in] value Value to test
 * @param[out] result Pointer to outlier_result_t (caller allocated)
 * @return 0 on success, negative on error
 */
int outlier_filter_detect(outlier_filter_t filter, double value, 
                          outlier_result_t *result);

/**
 * @brief Get current baseline statistics
 * 
 * @param[in] filter Filter handle
 * @param[out] mean Pointer to store mean value
 * @param[out] std_dev Pointer to store standard deviation
 * @param[out] sample_count Pointer to store number of samples used
 * @return 0 on success, negative on error
 */
int outlier_filter_get_baseline(outlier_filter_t filter, double *mean, 
                                double *std_dev, uint32_t *sample_count);

/**
 * @brief Reset filter and clear all samples
 * 
 * @param[in] filter Filter handle
 * @return 0 on success, negative on error
 */
int outlier_filter_reset(outlier_filter_t filter);

/**
 * @brief Get number of samples in baseline
 * 
 * @param[in] filter Filter handle
 * @return Number of samples (including excluded outliers)
 */
uint32_t outlier_filter_get_sample_count(outlier_filter_t filter);

#endif /* OUTLIER_FILTER_H */
