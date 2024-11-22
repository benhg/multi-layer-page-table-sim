/**
 * @file page_table.h
 * 
 * Header for page table-specific APIs and stuff
 */

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

/**
 * Bit arithmetic macros for decoding pieces of addresses
 * 
 * TODO: Check for off-by-one errors in the bit maps.
 */

#define N_SDP_BITS_COMPLEMENT 9
#define SDP_STARTING_BIT 39

#define N_PDP_BITS_COMPLEMENT 9
#define PDP_STARTING_BIT 30

#define N_BITS_PDE_COMPLEMENT 9
#define PDE_STARTING_BIT 21

#define N_BITS_PTE_COMPLEMENT 9
#define PTE_STARTING_BIT 12


// SDP bits are the top 9 ([39:47])
#define SDP_BIT_MASK (((1ULL << (va_size - N_SDP_BITS_COMPLEMENT)) - 1) << PDP_STARTING_BIT)
#define GET_SDP_BITS(va) ((va & (SDP_BIT_MASK)))

// PDP bits are the next 9 ([30:38])
#define PDP_BIT_MASK (((1ULL << (va_size -  (N_SDP_BITS_COMPLEMENT +  N_PDP_BITS_COMPLEMENT) ) - 1) << PDP_STARTING_BIT)
#define GET_PDP_BITS(va) ((va & (PDP_BIT_MASK)))
// Offset is the bottom bits while the mask is the top bits
#define GET_PDP_OFFSET(va) (va & ~PDP_BIT_MASK)

// PDE bits are the middle 9 ([21:29])
#define PDE_BIT_MASK (((1ULL << (va_size - (N_PDP_BITS_COMPLEMENT + N_PDP_BITS_COMPLEMENT + N_BITS_PDE_COMPLEMENT))) - 1) << PDE_STARTING_BIT)
#define GET_PDE_BITS(va) ((va & (PDE_BIT_MASK)))
#define GET_PDE_OFFSET(va) (va & ~PDE_BIT_MASK)

// PTE bits are the middle 9 below PDE ([12:20])
#define PTE_BIT_MASK (((1ULL << (va_size - (N_PDP_BITS_COMPLEMENT + N_PDP_BITS_COMPLEMENT + N_BITS_PDE_COMPLEMENT + N_BITS_PTE_COMPLEMENT))) - 1) << PTE_STARTING_BIT)
#define GET_PDP_BITS(va) ((va & (PTE_BIT_MASK)))
#define GET_PTE_OFFSET(va) (va & ~PTE_BIT_MASK)

// Shift the bits back to get the index
#define GET_SDP_ENTRY_IDX(VA) ((GET_SDP_BITS(VA) >> SDP_STARTING_BIT))
#define GET_PDP_ENTRY_IDX(VA) ((GET_PDP_BITS(VA) >> PDP_STARTING_BIT))
#define GET_PDE_ENTRY_IDX(VA) ((GET_PDE_BITS(VA) >> PDE_STARTING_BIT))
#define GET_PTE_ENTRY_IDX(VA) ((GET_PTE_BITS(VA) >> PTE_STARTING_BIT))


/**
 * Function declarations for page tables
 */

/**
 * @brief Performs a page table walk to translate a virtual address to a physical address.
 *
 * This function translates a given virtual address (VA) to its corresponding 
 * physical address (PA) by traversing a multi-level page table structure. The 
 * traversal is based on the address context and the page table walker simulation context.
 *
 * @param a_ctx A pointer to the address context, which contains details such as 
 * the virtual address (VA), process ID (PID), permissions, and user/supervisor mode.
 * @param ctx A pointer to the page table walker simulation context, which contains 
 * information like page table pointers and system-wide configuration.
 *
 * @return The physical address corresponding to the given virtual address, or an 
 * appropriate error code if the translation fails:
 * - `-EFAULT`: Malformed entry in the page table.
 * - `-EINVAL`: Invalid or non-present page table entry (translation not valid).
 * - `-EUNAUTHORIZED`: Insufficient permissions to access the page.
 * - `-EACCESS`: User/supervisor mode mismatch.
 *
 * @details
 * The function performs the following steps:
 * - Extracts the PID to locate the base of the page directory pointer table.
 * - Uses the top bits of the VA to index into the directory pointer table, then 
 * retrieves the physical frame of the page directory.
 * - Iteratively descends through the page directory pointer table (SDP), page 
 * directory (PDP), and page table entry (PTE) levels:
 *   - Validates the virtual address matches the expected entry.
 *   - Checks permissions and user/supervisor mode at each level.
 * - If a valid page table entry with a matching page size is found, computes 
 * the physical address by combining the physical frame and VA offset.
 * - If the page size is 1GB, 2MB, or 4KB, returns the PA for the corresponding 
 * page size directly.
 * - If no valid translation is found, returns an error code indicating a page fault.
 *
 * This function assumes that each VA maps to a single PA within the context of a PID.
 */
uintptr_t walk(address_context_t *a_ctx, ptw_sim_context_t *ctx);

#endif 
