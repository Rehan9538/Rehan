/**
 * @file event_queue.h
 * @brief Message queue for offline event buffering
 * 
 * Bounded FIFO queue for storing diagnostic events when offline,
 * with automatic cleanup and memory protection.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "bms_diagnostics.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Queued event (typically a sensor state for MQTT publication)
 */
typedef struct {
    uint64_t event_id;              /*!< Unique event identifier */
    uint64_t timestamp;             /*!< Event timestamp (milliseconds) */
    sensor_state_t sensor_state;    /*!< Associated sensor state */
} queued_event_t;

/**
 * @brief Event queue handle (opaque structure)
 */
typedef void* event_queue_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Create an event queue
 * 
 * Allocates a bounded FIFO queue with specified max capacity.
 *
 * @param[in] max_events Maximum number of events to buffer
 * @param[in] max_memory_bytes Maximum memory for queue (protection against overflow)
 * @return Handle to event queue, NULL on error
 */
event_queue_t event_queue_create(uint32_t max_events, uint32_t max_memory_bytes);

/**
 * @brief Destroy event queue
 * 
 * Frees all allocated memory and pending events.
 *
 * @param[in] queue Queue handle
 * @return 0 on success, negative on error
 */
int event_queue_destroy(event_queue_t queue);

/**
 * @brief Enqueue an event (FIFO)
 * 
 * Adds event to rear of queue. Returns error if queue full.
 *
 * @param[in] queue Queue handle
 * @param[in] event Event to enqueue (copied internally)
 * @return 0 on success, negative if queue full
 */
int event_queue_enqueue(event_queue_t queue, const queued_event_t *event);

/**
 * @brief Dequeue an event (FIFO)
 * 
 * Removes and returns oldest event from queue.
 *
 * @param[in] queue Queue handle
 * @param[out] event Pointer to queued_event_t (caller allocated)
 * @return 0 on success, negative if queue empty
 */
int event_queue_dequeue(event_queue_t queue, queued_event_t *event);

/**
 * @brief Peek at next event without removing
 * 
 * @param[in] queue Queue handle
 * @param[out] event Pointer to queued_event_t (caller allocated)
 * @return 0 on success, negative if queue empty
 */
int event_queue_peek(event_queue_t queue, queued_event_t *event);

/**
 * @brief Get current queue size
 * 
 * @param[in] queue Queue handle
 * @return Number of events currently queued
 */
uint32_t event_queue_get_size(event_queue_t queue);

/**
 * @brief Get queue capacity
 * 
 * @param[in] queue Queue handle
 * @return Maximum queue size
 */
uint32_t event_queue_get_capacity(event_queue_t queue);

/**
 * @brief Check if queue is empty
 * 
 * @param[in] queue Queue handle
 * @return true if empty, false otherwise
 */
bool event_queue_is_empty(event_queue_t queue);

/**
 * @brief Check if queue is full
 * 
 * @param[in] queue Queue handle
 * @return true if full, false otherwise
 */
bool event_queue_is_full(event_queue_t queue);

/**
 * @brief Clear all events from queue
 * 
 * @param[in] queue Queue handle
 * @return 0 on success
 */
int event_queue_clear(event_queue_t queue);

/**
 * @brief Get memory usage of queue
 * 
 * @param[in] queue Queue handle
 * @return Current memory usage in bytes
 */
uint32_t event_queue_get_memory_usage(event_queue_t queue);

#endif /* EVENT_QUEUE_H */
