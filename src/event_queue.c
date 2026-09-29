/**
 * @file event_queue.c
 * @brief Event queue implementation for offline buffering
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include "event_queue.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Internal Structures
 * ============================================================================ */

typedef struct {
    queued_event_t *events;
    uint32_t max_events;
    uint32_t max_memory_bytes;
    uint32_t current_size;
    uint32_t head_index;
    uint32_t tail_index;
    uint32_t next_event_id;
} event_queue_internal_t;

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

event_queue_t event_queue_create(uint32_t max_events, uint32_t max_memory_bytes) {
    if (max_events == 0) {
        return NULL;
    }
    
    event_queue_internal_t *queue = 
        (event_queue_internal_t *)malloc(sizeof(event_queue_internal_t));
    if (!queue) {
        return NULL;
    }
    
    queue->events = (queued_event_t *)calloc(max_events, sizeof(queued_event_t));
    if (!queue->events) {
        free(queue);
        return NULL;
    }
    
    queue->max_events = max_events;
    queue->max_memory_bytes = max_memory_bytes;
    queue->current_size = 0;
    queue->head_index = 0;
    queue->tail_index = 0;
    queue->next_event_id = 1;
    
    return (event_queue_t)queue;
}

int event_queue_destroy(event_queue_t queue) {
    if (!queue) {
        return -1;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    free(q->events);
    free(q);
    
    return 0;
}

int event_queue_enqueue(event_queue_t queue, const queued_event_t *event) {
    if (!queue || !event) {
        return -1;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    
    if (q->current_size >= q->max_events) {
        return -1;  /* Queue full */
    }
    
    queued_event_t *slot = &q->events[q->tail_index];
    *slot = *event;
    slot->event_id = q->next_event_id++;
    
    q->tail_index = (q->tail_index + 1) % q->max_events;
    q->current_size++;
    
    return 0;
}

int event_queue_dequeue(event_queue_t queue, queued_event_t *event) {
    if (!queue || !event) {
        return -1;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    
    if (q->current_size == 0) {
        return -1;  /* Empty queue */
    }
    
    *event = q->events[q->head_index];
    q->head_index = (q->head_index + 1) % q->max_events;
    q->current_size--;
    
    return 0;
}

int event_queue_peek(event_queue_t queue, queued_event_t *event) {
    if (!queue || !event) {
        return -1;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    
    if (q->current_size == 0) {
        return -1;  /* Empty queue */
    }
    
    *event = q->events[q->head_index];
    return 0;
}

uint32_t event_queue_get_size(event_queue_t queue) {
    if (!queue) {
        return 0;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    return q->current_size;
}

uint32_t event_queue_get_capacity(event_queue_t queue) {
    if (!queue) {
        return 0;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    return q->max_events;
}

bool event_queue_is_empty(event_queue_t queue) {
    if (!queue) {
        return true;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    return q->current_size == 0;
}

bool event_queue_is_full(event_queue_t queue) {
    if (!queue) {
        return true;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    return q->current_size >= q->max_events;
}

int event_queue_clear(event_queue_t queue) {
    if (!queue) {
        return -1;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    q->current_size = 0;
    q->head_index = 0;
    q->tail_index = 0;
    
    return 0;
}

uint32_t event_queue_get_memory_usage(event_queue_t queue) {
    if (!queue) {
        return 0;
    }
    
    event_queue_internal_t *q = (event_queue_internal_t *)queue;
    return sizeof(event_queue_internal_t) + 
           (q->current_size * sizeof(queued_event_t));
}
