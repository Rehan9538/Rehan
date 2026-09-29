/**
 * @file mqtt_reporter.h
 * @brief MQTT reporting for diagnostic events to AWS IoT Core
 * 
 * Publishes sensor diagnostic faults to AWS IoT Core topic with JSON serialization.
 * Integrates with event_queue for offline capability.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#ifndef MQTT_REPORTER_H
#define MQTT_REPORTER_H

#include "bms_diagnostics.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief MQTT publication result
 */
typedef enum {
    MQTT_SUCCESS = 0,           /*!< Message published successfully */
    MQTT_QUEUED = 1,            /*!< Message queued (offline) */
    MQTT_QUEUE_FULL = 2,        /*!< Queue full, message dropped */
    MQTT_ERROR = -1             /*!< Publishing error */
} mqtt_result_t;

/**
 * @brief MQTT reporter handle (opaque structure)
 */
typedef void* mqtt_reporter_t;

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/**
 * @brief Initialize MQTT reporter
 * 
 * Connects to AWS IoT Core broker using provided credentials.
 *
 * @param[in] endpoint AWS IoT endpoint URL
 * @param[in] topic MQTT topic for publishing
 * @param[in] cert_path Path to client certificate file
 * @param[in] key_path Path to private key file
 * @param[in] ca_path Path to CA certificate file
 * @return Handle to MQTT reporter, NULL on error
 */
mqtt_reporter_t mqtt_reporter_init(const char *endpoint, const char *topic,
                                   const char *cert_path, const char *key_path,
                                   const char *ca_path);

/**
 * @brief Shutdown MQTT reporter
 * 
 * Closes connection and frees resources.
 *
 * @param[in] reporter Reporter handle
 * @return 0 on success, negative on error
 */
int mqtt_reporter_shutdown(mqtt_reporter_t reporter);

/**
 * @brief Publish a diagnostic event
 * 
 * Serializes sensor state to JSON and publishes to IoT Core.
 * Queues message if offline.
 *
 * @param[in] reporter Reporter handle
 * @param[in] state Sensor state to publish
 * @return MQTT result code (MQTT_SUCCESS, MQTT_QUEUED, or error)
 */
mqtt_result_t mqtt_reporter_publish_event(mqtt_reporter_t reporter, 
                                          const sensor_state_t *state);

/**
 * @brief Publish multiple events
 * 
 * @param[in] reporter Reporter handle
 * @param[in] states Array of sensor states
 * @param[in] num_states Number of states in array
 * @return Number of successfully published events
 */
uint32_t mqtt_reporter_publish_events(mqtt_reporter_t reporter,
                                      const sensor_state_t *states,
                                      uint32_t num_states);

/**
 * @brief Check if connected to broker
 * 
 * @param[in] reporter Reporter handle
 * @return true if connected, false if offline
 */
bool mqtt_reporter_is_connected(mqtt_reporter_t reporter);

/**
 * @brief Reconnect to broker
 * 
 * Attempts to restore connection after temporary loss.
 *
 * @param[in] reporter Reporter handle
 * @return 0 on success, negative on error
 */
int mqtt_reporter_reconnect(mqtt_reporter_t reporter);

/**
 * @brief Get queued message count
 * 
 * Returns number of messages waiting in offline queue.
 *
 * @param[in] reporter Reporter handle
 * @return Number of queued messages
 */
uint32_t mqtt_reporter_get_queue_size(mqtt_reporter_t reporter);

/**
 * @brief Get queue capacity
 * 
 * @param[in] reporter Reporter handle
 * @return Maximum queue size in messages
 */
uint32_t mqtt_reporter_get_queue_capacity(mqtt_reporter_t reporter);

/**
 * @brief Flush queued messages to broker
 * 
 * Attempts to publish all queued messages (requires connection).
 *
 * @param[in] reporter Reporter handle
 * @return Number of messages published
 */
uint32_t mqtt_reporter_flush_queue(mqtt_reporter_t reporter);

#endif /* MQTT_REPORTER_H */
