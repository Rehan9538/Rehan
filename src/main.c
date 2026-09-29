/**
 * @file main.c
 * @brief Main entry point for BMS diagnostic service
 *
 * Demonstrates initialization and basic usage of the diagnostic engine.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include "bms_diagnostics.h"
#include "config_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ============================================================================
 * Main Program
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   BMS Sensor Plausibility & Cross-Calibration Diagnostic      ║\n");
    printf("║   Engine v1.0.0                                               ║\n");
    printf("║   2026-09-29                                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Get configuration file path from command line or use default */
    const char *config_file = (argc > 1) ? argv[1] : "config.json";
    
    printf("[INFO] Configuration file: %s\n", config_file);
    printf("[INFO] Initializing diagnostic engine...\n");
    
    /* Initialize diagnostic engine */
    if (bms_diagnostics_init(config_file) != 0) {
        printf("[ERROR] Failed to initialize diagnostic engine\n");
        printf("[ERROR] Ensure %s exists and is valid\n", config_file);
        return EXIT_FAILURE;
    }
    
    printf("[INFO] Diagnostic engine initialized successfully\n");
    printf("[INFO] Active sensors: %u\n", bms_get_sensor_count());
    
    /* Display system health */
    health_status_t health = bms_get_system_health();
    printf("[INFO] System health: ");
    switch (health) {
        case HEALTH_HEALTHY:
            printf("HEALTHY\n");
            break;
        case HEALTH_SUSPECT:
            printf("SUSPECT\n");
            break;
        case HEALTH_FAULTY:
            printf("FAULTY\n");
            break;
        case HEALTH_IN_STARTUP:
            printf("IN_STARTUP\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Ready for sensor data ingestion                             ║\n");
    printf("║   Call bms_ingest_reading() to add sensor data                ║\n");
    printf("║   Call bms_diagnose() periodically for analysis               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* ====================================================================
     * TODO: MAIN APPLICATION LOOP
     * ====================================================================
     * 
     * Example usage pattern:
     * 
     *   while (system_running) {
     *       // Receive sensor readings
     *       for each sensor reading:
     *           timestamp_ms = get_current_timestamp_ms();
     *           value = read_sensor_adc();
     *           bms_ingest_reading(sensor_id, value, timestamp_ms);
     *       
     *       // Run periodic diagnostic analysis
     *       if (time_since_last_diagnose >= 20ms):
     *           bms_diagnose();
     *           
     *           // Check for faults
     *           sensor_state_t state;
     *           bms_get_sensor_state(sensor_id, &state);
     *           
     *           if (state.health != HEALTH_HEALTHY):
     *               mqtt_publish_fault(state);
     *       
     *       // Sleep to avoid busy-loop
     *       sleep(10ms);
     *   }
     * 
     * ====================================================================
     */
    
    printf("[INFO] Diagnostic engine is ready for operation\n");
    printf("[INFO] For production deployment:\n");
    printf("       1. Configure sensors and topology in config.json\n");
    printf("       2. Integrate with MQTT reporter for cloud connectivity\n");
    printf("       3. Implement sensor data acquisition loop\n");
    printf("       4. Add periodic bms_diagnose() calls\n");
    printf("       5. Deploy on BMS edge controller\n");
    printf("\n");
    
    /* Clean up and shutdown */
    printf("[INFO] Shutting down diagnostic engine...\n");
    
    if (bms_diagnostics_shutdown() != 0) {
        printf("[ERROR] Error during shutdown\n");
        return EXIT_FAILURE;
    }
    
    printf("[INFO] Diagnostic engine shut down successfully\n");
    printf("\n");
    
    return EXIT_SUCCESS;
}
