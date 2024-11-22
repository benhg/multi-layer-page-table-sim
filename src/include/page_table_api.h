#ifndef PAGE_TABLE_API_H
#define PAGE_TABLE_API_H

#include <stdint.h>

#include "config.h"

/**
 * @file page_table_api.h
 * @brief this file maintains external API and data structures
 * We use structs to model hardware structures like TLBs, page table walkers, etc.
 */

/**
 * Utility Structs
 */

typedef enum page_size {
	FOUR_K = 0;
	TWO_M = 1;
	ONE_G = 3;
	SIZE_MAX = 4;
} page_size_t;


/**
 * Structs modeling hardware structures
 */



/**
 * Address context struct
 * Use this to move around address and metadata (PID, etc.)
 * This keeps from "parameter explosion"
 */
typedef struct address_context {
	uint64_t va;
	permissions_t permissions;
	uint8_t:1 user_supervisor;
	uint32_t pid;
} address_context_t;


/**
 * Context struct
 * 
 * This struct stores the context of a run of this program
 * Basically, it's a struct of pointers to the data structures you need to run the program
 */
typedef struct ptw_sim_context {
	/**
	 * Three TLBs
	 */
	tlb_t oneg_tlb;
	tlb_t twom_tlb;
	tlb_t fourk_tlb;
	/**
	 * Array of pointers to page tables
	 * In sim, these are indexes into the PT array
	 * In hardware, these are registers that point at a single SDP entry
	 * 
	 * Each of these pointes must point at 512 valid SDP pages.
	 * 512 SDP entries, each of which is 64b consumes 1 page
	 * In real hardware, we'd shave this to be 64b exactly.
	 * In simulator, add extra metadata to make life easy.
	 */
	page_table_entry_t * page_table_pointers[MAX_PID];

	// TODO: add backend management pointers here for easy programming of falid page tables

	/**
	 * For completeness, we can put the PDE and PTE blocks as well
	 */
	pte_t pde_page_base[MAX_PID][NUM_PDE_ENTRY];

} ptw_sim_context_t;



/**
 * Translate function
 * 
 * This is the entry point for our memory translation. Basically, what has happened to get here is a cache miss (assuming our cache is virtually addressed) and we need to go to memory
 * 
 * Later, we may implement an actual cache into the simulator but for now, this is the entry point
 */
uintptr_t translate(uintptr_t va, uint32_t pid, uint8_t user_supervisor, uint8_t permisions);


/**
 * This is mostly going to be used internally
 */
int invalidate_tlb_by_page(uintptr_t va, page_size_t page_size);

/**
 * Use pseudo-LRU algorithm to evict an address from the TLB
 */
int tlb_evict(page_size_t page_size);


/**
 * This models the x86 instruction `invlpg`
 * 
 * static inline void __native_flush_tlb_single(unsigned long addr) {
       asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
 *	}
 */
void inval_page(uintptr_t addr){

}

#endif