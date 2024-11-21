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

#define N_PDP_BITS_COMPLEMENT 30
#define PDP_STARTING_BIT 30

#define N_BITS_PDE_COMPLEMENT 9
#define PDE_STARTING_BIT 21

#define N_BITS_PTE_COMPLEMENT 9
#define PTE_STARTING_BIT 12


// PDP bits are the top 18 ([30:47])
#define PDP_BIT_MASK (((1ULL << (va_size - N_PDP_BITS)) - 1) << PDP_STARTING_BIT)
#define GET_PDP_BITS(va) ((va & (PDP_BIT_MASK)))

// PDE bits are the middle 9 ([21:30])
#define PDE_BIT_MASK (((1ULL << (va_size - (N_PDP_BITS_COMPLEMENT + N_BITS_PDE_COMPLEMENT))) - 1) << PDE_STARTING_BIT)
#define GET_PDE_BITS(va) ((va & (PDE_BIT_MASK)))

// PTE bits are the middle 9 below PDE ([12:20])
#define PTE_BIT_MASK (((1ULL << (va_size - (N_PDP_BITS_COMPLEMENT + N_BITS_PDE_COMPLEMENT + N_BITS_PTE_COMPLEMENT))) - 1) << PTE_STARTING_BIT)
#define GET_PDP_BITS(va) ((va & (PTE_BIT_MASK)))

// Shift the bits back to get the index
#define GET_PDP_ENTRY_IDX (GET_PDP_BITS(VA) >> PDP_STARTING_BIT)
#define GET_PDE_ENTRY_IDX (GET_PDP_BITS(VA) >> PDP_STARTING_BIT)

#endif 
