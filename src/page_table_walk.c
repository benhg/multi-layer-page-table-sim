/**
 * @file page_table_walk.c
 * 
 * The actual business logic of the page table walk
 */

#include <stdint.h>
#include <stdbool.h>

#include "page_table_api.h"



uintptr_t walk(address_context_t * a_ctx, ptw_sim_context_t *ctx){
	/**
	 * First, use the PID to look up the pointer to the directory table
	 * 
	 * Use the top (VA_SIZE - (9*3)) bits of the address as the index in the directory pointer table. The physical frame that it points at is the base of a page directory.
	 * 
	 * That points to a directory page table entry. Use the next 9 bits as an index into the directory.
	 * 
	 * That points us to a page table entry. Use that page table entry (in whatever size it is labelled as)
	 * 
	 * Assume, for now, that within a PID, an address has only one match
	 */

	uint64_t address = a_ctx->va;

	// Top 9 bits of VA specify PDP pointer
	// If PDP pointer is marked 1G page, return immediately with that frame


	// Otherwise, use that to look up the PDE
	// If PDE page is marked as a 2M page, then return immediately with that frame

	// Otherwise use that to look up the leaf-level PTE
	// If that PTE is invalid or not matching, return fault and the OS will need to make page entries.


}


uintptr_t translate(address_context_t * a_ctx){

	bool must_update_tlb = false;

	// Try the TLB
	// Eviction (if necessary) is handled inside this call
	// This call tells us which TLBs to update as well - via output params
	bool update_fourk_tlb, update_twom_tlb, udpate_oneg_tlb;
	uintptr_t translated_addr = check_tlb();

	if (translated_addr == SIXTY_FOUR_BIT_MASK){
		must_update_tlb = true;
	}

	// Walk the page table
	translated_addr = walk(a_ctx);

	// If it's still invalid, then we fault. This would result in going to the OS and having the OS swap pages around
	// For now, it's just a fault and is out of scope of this project.
	if (translated_addr == SIXTY_FOUR_BIT_MASK){
		return SIXTY_FOUR_BIT_MASK;
	}

	// Publish the found address into the TLB
	// An entry is guaranteed to be free here since we already did the eviction
	if (must_update_tlb){
		tlb_update(a_ctx, translated_addr);
	}


}