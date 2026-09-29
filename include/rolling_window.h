/**
 * @file rolling_window.h
 * @brief Rolling window statistics for sensor data analysis
 * 
 * Maintains a sliding window buffer of recent sensor readings and computes
 * real-time statistics (mean, standard deviation, min, max) for anomaly detection.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef ROLLING_WINDOW_H
#define ROLLING_WINDOW_H

#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Rolling window statistics structure
 */
typedef struct {
    double mean;                /*!< Mean of current window */
    double std_deviation;       /*!< Standard deviation of current window */
    double min_value;           /*!< Minimum value in window */
    double max_value;           /*!< Maximum value in window */
    uint32_t sample_count;      /*!< Number of samples in window */
    uint64_t window_start_time; /*!< Timestamp of oldest sample (ms) */
    uint64_t window_end_time;   /*!< Timestamp of newest sample (ms) */
} window_stats_t;

/**
 * @brief Rolling window handle (opaque structure)
 */
typedef void* rolling_window_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create a new rolling window
 * 
 * Allocates a rolling window buffer that maintains statistics over
 * a specified duration or sample count.
 *
 * @param[in] max_samples Maximum number of samples to buffer
 * @param[in] window_duration_ms Maximum window duration in milliseconds
 * @return Handle to rolling window, NULL on error
 */
rolling_window_t rolling_window_create(uint32_t max_samples, 
                                       uint32_t window_duration_ms);

/**
 * @brief Destroy a rolling window
 * 
 * Frees all allocated memory associated with the window.
 *
 * @param[in] window Window handle to destroy
 * @return 0 on success, negative on error
 */
int rolling_window_destroy(rolling_window_t window);

/**
 * @brief Add a sample to the rolling window
 * 
 * Inserts a new sample and automatically removes old samples outside
 * the time window.
 *
 * @param[in] window Window handle
 * @param[in] value Sample value to add
 * @param[in] timestamp_ms Timestamp of sample in milliseconds
 * @return 0 on success, negative on error
 */
int rolling_window_add_sample(rolling_window_t window, double value, 
                              uint64_t timestamp_ms);

/**
 * @brief Get current window statistics
 * 
 * Computes and returns statistics for current window contents.
 *
 * @param[in] window Window handle
 * @param[out] stats Pointer to window_stats_t (caller allocated)
 * @return 0 on success, negative on error
 */
int rolling_window_get_stats(rolling_window_t window, window_stats_t *stats);

/**
 * @brief Clear all samples from window
 * 
 * Resets the window to empty state without deallocating memory.
 *
 * @param[in] window Window handle
 * @return 0 on success, negative on error
 */
int rolling_window_clear(rolling_window_t window);

/**
 * @brief Get number of samples in window
 * 
 * @param[in] window Window handle
 * @return Number of samples currently in window
 */
uint32_t rolling_window_get_sample_count(rolling_window_t window);

/**
 * @brief Get oldest sample value in window
 * 
 * @param[in] window Window handle
 * @param[out] value Pointer to store oldest sample value
 * @param[out] timestamp_ms Pointer to store timestamp (may be NULL)
 * @return 0 on success, negative on error (e.g., empty window)
 */
int rolling_window_get_oldest_sample(rolling_window_t window, double *value, 
                                     uint64_t *timestamp_ms);

/**
 * @brief Get newest sample value in window
 * 
 * @param[in] window Window handle
 * @param[out] value Pointer to store newest sample value
 * @param[out] timestamp_ms Pointer to store timestamp (may be NULL)
 * @return 0 on success, negative on error (e.g., empty window)
 */
int rolling_window_get_newest_sample(rolling_window_t window, double *value, 
                                     uint64_t *timestamp_ms);

#endif /* ROLLING_WINDOW_H */
