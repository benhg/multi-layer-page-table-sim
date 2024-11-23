/**
 * The functions to run the simple mapping test
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "translation.h"
#include "test_utils.h"

int run_simple_mapping_test(ptw_sim_context_t *ctx) {
    // Initialize test variables
    uintptr_t test_va_4k = 0x12345000;   // A virtual address to map (4K page)
    uintptr_t test_va_2m = 0x45678000;   // A virtual address to map (2M page)
    uintptr_t test_va_1g = 0x789A0000;   // A virtual address to map (1G page)
    uintptr_t expected_pa_4k = 0xABC45000; // Expected physical address (4K)
    uintptr_t expected_pa_2m = 0xDEF78000; // Expected physical address (2M)
    uintptr_t expected_pa_1g = 0x123A0000; // Expected physical address (1G)

    // Define the PID and permissions for the mappings
    uint32_t test_pid = 1; 
    permissions_t perms = {0};
    perms.val.read = 1;
    perms.val.write = 1;
    perms.val.execute = 1;

    // Set up mappings in the page tables using the helper function
    if (setup_mapping(ctx, test_pid, test_va_4k, expected_pa_4k, FOUR_K, perms) != 0) {
        fprintf(stderr, "Failed to set up 4K mapping.\n");
        return -1;
    }

    if (setup_mapping(ctx, test_pid, test_va_2m, expected_pa_2m, TWO_M, perms) != 0) {
        fprintf(stderr, "Failed to set up 2M mapping.\n");
        return -1;
    }

    if (setup_mapping(ctx, test_pid, test_va_1g, expected_pa_1g, ONE_G, perms) != 0) {
        fprintf(stderr, "Failed to set up 1G mapping.\n");
        return -1;
    }

    // Prepare the address context for translation
    address_context_t a_ctx = { .va = test_va_4k, .pid = test_pid, .permissions = perms };

    // Test 4K page translation
    uintptr_t result_pa = translate(&a_ctx, ctx);
    assert(result_pa == expected_pa_4k && "4K translation failed.");

    // Test 2M page translation
    a_ctx.va = test_va_2m;
    result_pa = translate(&a_ctx, ctx);
    assert(result_pa == expected_pa_2m && "2M translation failed.");

    // Test 1G page translation
    a_ctx.va = test_va_1g;
    result_pa = translate(&a_ctx, ctx);
    assert(result_pa == expected_pa_1g && "1G translation failed.");

    // If all tests pass
    printf("All translations passed!\n");
    return 0;
}