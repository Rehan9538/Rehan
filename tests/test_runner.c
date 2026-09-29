/**
 * @file test_runner.c
 * @brief Main test runner for all BMS diagnostic unit tests
 * 
 * Registers all test suites and runs them using CUnit framework.
 *
 * @date 2026-09-29
 * @version 1.0.0
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>

/* Forward declarations of test suite registration functions */
extern CU_SuiteInfo freeze_suite;
extern CU_SuiteInfo drift_suite;
extern CU_SuiteInfo outlier_suite;
extern CU_SuiteInfo state_suite;
extern CU_SuiteInfo edge_suite;
extern CU_SuiteInfo config_suite;
extern CU_SuiteInfo reset_suite;
extern CU_SuiteInfo mqtt_suite;

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   BMS Diagnostic Engine - Unit Test Suite                     ║\n");
    printf("║   16 Test Cases across 7 Categories                           ║\n");
    printf("║   2026-09-29                                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Initialize CUnit test registry */
    if (CU_initialize_registry() != CUE_SUCCESS) {
        fprintf(stderr, "[ERROR] CUnit initialization failed\n");
        return CU_get_error();
    }
    
    /* Add test suites to registry */
    printf("[INFO] Registering test suites...\n");
    
    int added = 0;
    
    if (CU_add_suite(&freeze_suite) != NULL) {
        printf("[OK] Freeze Detection Tests (4 tests)\n");
        added++;
    }
    if (CU_add_suite(&drift_suite) != NULL) {
        printf("[OK] Drift Detection Tests (3 tests)\n");
        added++;
    }
    if (CU_add_suite(&outlier_suite) != NULL) {
        printf("[OK] Outlier Filter Tests (1 test)\n");
        added++;
    }
    if (CU_add_suite(&state_suite) != NULL) {
        printf("[OK] State Machine Tests (1 test)\n");
        added++;
    }
    if (CU_add_suite(&edge_suite) != NULL) {
        printf("[OK] Edge Case Tests (3 tests)\n");
        added++;
    }
    if (CU_add_suite(&config_suite) != NULL) {
        printf("[OK] Configuration Tests (1 test)\n");
        added++;
    }
    if (CU_add_suite(&reset_suite) != NULL) {
        printf("[OK] Reset/Recovery Tests (1 test)\n");
        added++;
    }
    if (CU_add_suite(&mqtt_suite) != NULL) {
        printf("[OK] MQTT Reporting Tests (2 tests)\n");
        added++;
    }
    
    printf("\n[INFO] %d test suites registered\n\n", added);
    
    /* Run all tests */
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Running Tests...                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    CU_basic_run_tests();
    
    /* Print summary */
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Test Summary                                                ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    CU_RunSummary *summary = CU_get_run_summary();
    
    printf("Total Tests Run:        %u\n", summary->nTestsRun);
    printf("Tests Passed:           %u\n", summary->nTestsRun - summary->nTestsFailed);
    printf("Tests Failed:           %u\n", summary->nTestsFailed);
    printf("Assertions Failed:      %u\n", summary->nAssertsFailed);
    
    if (summary->nTestsFailed == 0 && summary->nAssertsFailed == 0) {
        printf("\n✅ ALL TESTS PASSED\n");
    } else {
        printf("\n❌ SOME TESTS FAILED - See details above\n");
    }
    
    printf("\n");
    
    /* Clean up */
    CU_cleanup_registry();
    
    return (summary->nTestsFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
