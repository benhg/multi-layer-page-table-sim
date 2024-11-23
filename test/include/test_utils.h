/**
 * test_utils.h
 * 
 * Utility functions for testing the page table sim
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "hw_structures.h"
#include "config.h"
#include "page_table.h"
#include "util.h"
#include "tlb.h"
#include "page_table_api.h"

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

/**
 * @brief Populates a ptw_sim_context_t structure with default or provided values.
 *
 * @param ctx Pointer to the ptw_sim_context_t structure to populate.
 * @param max_pid Maximum number of PIDs supported (defines page table array size).
 */
void populate_sim_context(ptw_sim_context_t *ctx, size_t max_pid);

/**
 * @brief Helper function to initialize a TLB.
 *
 * @param tlb Pointer to the TLB structure to initialize.
 * @param page_size The page size associated with the TLB (e.g., 4KB, 2MB, 1GB).
 */
void initialize_tlb(tlb_t *tlb);

/**
 * @brief Helper function to initialize a page table entry.
 *
 * @param entry Pointer to the page table entry to initialize.
 * @param pid Process ID associated with this page table entry.
 * @param index Index of the entry within the page table.
 */
void initialize_page_table_entry(page_table_entry_t *entry, uint32_t pid, size_t index);

/**
 * @brief Frees all memory and resources allocated during simulation context initialization.
 * 
 * This function undoes all allocations and setups performed by `populate_sim_context`.
 * It iterates through all PIDs and releases memory for each layer of page tables (SDP, PDP, PDE, and PTE),
 * as well as any other dynamically allocated resources within the simulation context.
 * 
 * @param ctx Pointer to the simulation context to be torn down.
 * @param max_pid The maximum number of PIDs for which resources were allocated.
 * 
 * @note After calling this function, `ctx` should not be used unless reinitialized.
 */
void teardown_sim_context(ptw_sim_context_t *ctx, size_t max_pid);

/**
 * @brief Resets and clears all entries in a given Translation Lookaside Buffer (TLB).
 * 
 * This function zeroes out all entries in the specified TLB and resets its metadata,
 * such as the number of valid entries and any LRU-related indices. It ensures the TLB
 * is in a clean state, ready for new translations.
 * 
 * @param tlb Pointer to the TLB to be cleared.
 * 
 * @note If the TLB pointer is `NULL`, the function does nothing.
 */
void clear_tlb(tlb_t *tlb);

/**
 * @brief Allocates and initializes a new page table.
 *
 * This function dynamically allocates memory for a page table and initializes 
 * its entries to default values. Each page table contains 512 entries, as per 
 * the x86-64 paging model. The entries are initially invalid until populated 
 * with specific mappings.
 *
 * The allocated memory is 4KB aligned, ensuring compatibility with hardware 
 * paging requirements. The caller is responsible for deallocating the memory 
 * using an appropriate teardown function to avoid memory leaks.
 *
 * @return A pointer to the newly allocated page table on success.
 *         Returns NULL if the memory allocation fails.
 *
 * Example:
 * @code
 * page_table_entry_t *page_table = allocate_page_table();
 * if (!page_table) {
 *     fprintf(stderr, "Failed to allocate page table\n");
 *     return -1;
 * }
 * // Populate the page table here...
 * @endcode
 */
page_table_entry_t *allocate_page_table();

/**
 * @brief Sets up a mapping in the page table hierarchy.
 *
 * This function configures a virtual-to-physical address mapping in the
 * hierarchical page tables of the simulator. It navigates through the levels
 * (SDP, PDP, PDE, PTE) to set up the mapping, creating intermediate levels
 * if they do not already exist.
 *
 * @param ctx Pointer to the page table walker simulator context.
 * @param pid Process ID for which the mapping should be set.
 * @param va Virtual address to be mapped.
 * @param pa Physical address to map the virtual address to.
 * @param page_size Size of the page (e.g., 1G, 2M, or 4K).
 * @param perms Permissions for the mapping (e.g., read, write, execute).
 * @return 0 on success, -1 on failure (e.g., invalid inputs or memory allocation errors).
 */
int setup_mapping(ptw_sim_context_t *ctx, uint32_t pid, uintptr_t va, uintptr_t pa, page_size_t page_size, permissions_t perms);

#endif
