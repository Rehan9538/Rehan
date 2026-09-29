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
    (void)argc;
    (void)argv;

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
    
    int added = 8;
    if (CU_register_nsuites(8, &freeze_suite, &drift_suite, &outlier_suite,
                            &state_suite, &edge_suite, &config_suite,
                            &reset_suite, &mqtt_suite) != CUE_SUCCESS) {
        fprintf(stderr, "[ERROR] Failed to register test suites: %s\n",
                CU_get_error_msg());
        CU_cleanup_registry();
        return EXIT_FAILURE;
    }

    printf("[OK] Registered all %d test suites\n", added);
    
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
