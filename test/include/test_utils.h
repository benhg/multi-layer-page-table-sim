/**
 * test_utils.h
 * 
 * Utility functions for testing the page table sim
 */


/**
 * @brief Populates an address_context_t structure with the given parameters.
 *
 * @param a_ctx Pointer to the address_context_t structure to populate.
 * @param va Virtual address to assign to the context.
 * @param permissions Permissions for the context (e.g., read, write, execute).
 * @param user_supervisor Specifies whether the context is user mode (1) or supervisor mode (0).
 * @param pid Process ID to associate with the context.
 */
void populate_address_context(address_context_t *a_ctx, uint64_t va, permissions_t permissions, uint8_t user_supervisor, uint32_t pid);