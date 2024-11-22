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

uintptr_t walk(address_context_t *a_ctx, ptw_sim_context_t *ctx);

#endif 
