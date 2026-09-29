/**
 * @file timestamp_handler.h
 * @brief Asynchronous timestamp handling and time-based data alignment
 * 
 * Handles out-of-order sensor readings by sorting based on timestamps
 * rather than arrival order. Critical for systems with network delays.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef TIMESTAMP_HANDLER_H
#define TIMESTAMP_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Timestamped sensor reading
 */
typedef struct {
    char sensor_id[32];     /*!< Sensor identifier */
    double value;           /*!< Sensor reading value */
    uint64_t timestamp_ms;  /*!< Timestamp in milliseconds */
    bool is_valid;          /*!< True if reading is valid */
} timestamped_reading_t;

/**
 * @brief Timestamp handler buffer handle (opaque structure)
 */
typedef void* timestamp_buffer_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create timestamp buffer
 * 
 * Allocates a buffer that can sort readings by timestamp.
 *
 * @param[in] max_readings Maximum number of readings to buffer
 * @param[in] time_tolerance_ms Time window tolerance for alignment
 * @return Handle to timestamp buffer, NULL on error
 */
timestamp_buffer_t timestamp_buffer_create(uint32_t max_readings,
                                          uint32_t time_tolerance_ms);

/**
 * @brief Destroy timestamp buffer
 * 
 * @param[in] buffer Buffer handle
 * @return 0 on success, negative on error
 */
int timestamp_buffer_destroy(timestamp_buffer_t buffer);

/**
 * @brief Add reading to buffer
 * 
 * Buffers a reading which may arrive out of order. Readings are NOT
 * sorted until sort() is called.
 *
 * @param[in] buffer Buffer handle
 * @param[in] reading Pointer to timestamped_reading_t
 * @return 0 on success, negative on error
 */
int timestamp_buffer_add(timestamp_buffer_t buffer, 
                         const timestamped_reading_t *reading);

/**
 * @brief Sort buffer by timestamp
 * 
 * Arranges all buffered readings in chronological order (ascending timestamps).
 * This should be called after all readings have been added before processing.
 *
 * @param[in] buffer Buffer handle
 * @return 0 on success, negative on error
 */
int timestamp_buffer_sort(timestamp_buffer_t buffer);

/**
 * @brief Get reading at index (after sorting)
 * 
 * @param[in] buffer Buffer handle
 * @param[in] index Index into sorted buffer (0-based)
 * @param[out] reading Pointer to timestamped_reading_t (caller allocated)
 * @return 0 on success, negative if index out of bounds
 */
int timestamp_buffer_get_at_index(timestamp_buffer_t buffer, uint32_t index,
                                  timestamped_reading_t *reading);

/**
 * @brief Get all sorted readings
 * 
 * Returns array of readings in sorted (chronological) order.
 *
 * @param[in] buffer Buffer handle
 * @param[out] readings Array of timestamped_reading_t (caller allocated)
 * @param[in] max_readings Max elements in output array
 * @param[out] num_readings Number of readings returned
 * @return 0 on success, negative on error
 */
int timestamp_buffer_get_all(timestamp_buffer_t buffer,
                            timestamped_reading_t *readings,
                            uint32_t max_readings, uint32_t *num_readings);

/**
 * @brief Get number of readings in buffer
 * 
 * @param[in] buffer Buffer handle
 * @return Number of readings currently buffered
 */
uint32_t timestamp_buffer_get_size(timestamp_buffer_t buffer);

/**
 * @brief Check if buffer is sorted
 * 
 * @param[in] buffer Buffer handle
 * @return true if sorted, false if needs sorting
 */
bool timestamp_buffer_is_sorted(timestamp_buffer_t buffer);

/**
 * @brief Clear buffer
 * 
 * Removes all readings from buffer without deallocating memory.
 *
 * @param[in] buffer Buffer handle
 * @return 0 on success, negative on error
 */
int timestamp_buffer_clear(timestamp_buffer_t buffer);

/**
 * @brief Get time range of buffered readings
 * 
 * @param[in] buffer Buffer handle
 * @param[out] min_timestamp_ms Oldest timestamp (may be NULL)
 * @param[out] max_timestamp_ms Newest timestamp (may be NULL)
 * @return 0 on success, negative if buffer empty
 */
int timestamp_buffer_get_time_range(timestamp_buffer_t buffer,
                                   uint64_t *min_timestamp_ms,
                                   uint64_t *max_timestamp_ms);

/**
 * @brief Detect stale readings
 * 
 * Marks readings older than current_time - stale_threshold as invalid.
 *
 * @param[in] buffer Buffer handle
 * @param[in] current_time_ms Current time in milliseconds
 * @param[in] stale_threshold_ms Threshold for considering data stale
 * @return Number of readings marked stale
 */
uint32_t timestamp_buffer_mark_stale(timestamp_buffer_t buffer,
                                    uint64_t current_time_ms,
                                    uint32_t stale_threshold_ms);

#endif /* TIMESTAMP_HANDLER_H */
