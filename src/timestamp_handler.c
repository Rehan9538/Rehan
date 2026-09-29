/**
 * @file timestamp_handler.c
 * @brief Timestamp-based data alignment implementation
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include "timestamp_handler.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Internal Structures
 * ============================================================================ */

typedef struct {
    timestamped_reading_t *readings;
    uint32_t max_readings;
    uint32_t current_size;
    uint32_t time_tolerance_ms;
    bool is_sorted;
} timestamp_buffer_internal_t;

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

static int compare_readings(const void *a, const void *b) {
    const timestamped_reading_t *ra = (const timestamped_reading_t *)a;
    const timestamped_reading_t *rb = (const timestamped_reading_t *)b;
    
    if (ra->timestamp_ms < rb->timestamp_ms) return -1;
    if (ra->timestamp_ms > rb->timestamp_ms) return 1;
    return 0;
}

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

timestamp_buffer_t timestamp_buffer_create(uint32_t max_readings,
                                          uint32_t time_tolerance_ms) {
    if (max_readings == 0) {
        return NULL;
    }
    
    timestamp_buffer_internal_t *buffer = 
        (timestamp_buffer_internal_t *)malloc(sizeof(timestamp_buffer_internal_t));
    if (!buffer) {
        return NULL;
    }
    
    buffer->readings = (timestamped_reading_t *)calloc(max_readings,
                                                        sizeof(timestamped_reading_t));
    if (!buffer->readings) {
        free(buffer);
        return NULL;
    }
    
    buffer->max_readings = max_readings;
    buffer->current_size = 0;
    buffer->time_tolerance_ms = time_tolerance_ms;
    buffer->is_sorted = true;
    
    return (timestamp_buffer_t)buffer;
}

int timestamp_buffer_destroy(timestamp_buffer_t buffer) {
    if (!buffer) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    free(b->readings);
    free(b);
    
    return 0;
}

int timestamp_buffer_add(timestamp_buffer_t buffer,
                         const timestamped_reading_t *reading) {
    if (!buffer || !reading) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    
    if (b->current_size >= b->max_readings) {
        return -1;  /* Buffer full */
    }
    
    b->readings[b->current_size] = *reading;
    b->current_size++;
    b->is_sorted = false;  /* Buffer is no longer sorted */
    
    return 0;
}

int timestamp_buffer_sort(timestamp_buffer_t buffer) {
    if (!buffer) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    
    if (b->current_size <= 1) {
        b->is_sorted = true;
        return 0;
    }
    
    /* Sort by timestamp */
    qsort(b->readings, b->current_size, sizeof(timestamped_reading_t),
          compare_readings);
    
    b->is_sorted = true;
    return 0;
}

int timestamp_buffer_get_at_index(timestamp_buffer_t buffer, uint32_t index,
                                  timestamped_reading_t *reading) {
    if (!buffer || !reading) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    
    if (index >= b->current_size) {
        return -1;  /* Index out of bounds */
    }
    
    *reading = b->readings[index];
    return 0;
}

int timestamp_buffer_get_all(timestamp_buffer_t buffer,
                            timestamped_reading_t *readings,
                            uint32_t max_readings, uint32_t *num_readings) {
    if (!buffer || !readings || !num_readings) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    
    uint32_t count = b->current_size < max_readings ? 
                     b->current_size : max_readings;
    
    memcpy(readings, b->readings, count * sizeof(timestamped_reading_t));
    *num_readings = count;
    
    return 0;
}

uint32_t timestamp_buffer_get_size(timestamp_buffer_t buffer) {
    if (!buffer) {
        return 0;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    return b->current_size;
}

bool timestamp_buffer_is_sorted(timestamp_buffer_t buffer) {
    if (!buffer) {
        return false;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    return b->is_sorted;
}

int timestamp_buffer_clear(timestamp_buffer_t buffer) {
    if (!buffer) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    b->current_size = 0;
    b->is_sorted = true;
    
    return 0;
}

int timestamp_buffer_get_time_range(timestamp_buffer_t buffer,
                                   uint64_t *min_timestamp_ms,
                                   uint64_t *max_timestamp_ms) {
    if (!buffer) {
        return -1;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    
    if (b->current_size == 0) {
        return -1;  /* Empty buffer */
    }
    
    if (min_timestamp_ms) {
        *min_timestamp_ms = b->readings[0].timestamp_ms;
    }
    if (max_timestamp_ms) {
        *max_timestamp_ms = b->readings[b->current_size - 1].timestamp_ms;
    }
    
    return 0;
}

uint32_t timestamp_buffer_mark_stale(timestamp_buffer_t buffer,
                                    uint64_t current_time_ms,
                                    uint32_t stale_threshold_ms) {
    if (!buffer) {
        return 0;
    }
    
    timestamp_buffer_internal_t *b = (timestamp_buffer_internal_t *)buffer;
    uint32_t stale_count = 0;
    
    for (uint32_t i = 0; i < b->current_size; i++) {
        if ((current_time_ms - b->readings[i].timestamp_ms) > stale_threshold_ms) {
            b->readings[i].is_valid = false;
            stale_count++;
        }
    }
    
    return stale_count;
}
