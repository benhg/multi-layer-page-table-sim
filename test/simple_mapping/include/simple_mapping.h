/**
 * File with test functions for simple mapping test
 */

#ifndef SIMPLE_MAPPING_H
#define SIMPLE_MAPPING_H

/**
 * @brief Runs a simple mapping test to validate the simulator's translation
 * logic.
 *
 * This function sets up a basic page table structure, populates mappings for a
 * few virtual addresses, and attempts to translate them using the simulator. It
 * verifies that the resulting physical addresses match expected values and
 * reports success or failure.
 *
 * @param ctx Pointer to the pre-allocated and initialized simulator context.
 *
 * @return
 * - 0 on success.
 * - Non-zero on failure.
 */
int run_simple_mapping_test(ptw_sim_context_t *ctx);

#endif