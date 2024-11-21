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

	uintptr_t pa = SIXTY_FOUR_BIT_MASK;

	uint64_t va = a_ctx->va;
	// Pointer to the page table base. That is a block of 512 SDP entries
	pte_t * page_table_base = ctx->page_table_pointers[pid];
	uint32_p pid = a_ctx->pid;

	// Top 9 bits of VA specify SPDP pointer
	// No page size maps to a real page at this level
	uint16_t sdp_offset = GET_SDP_ENTRY_IDX(va);
	pte_t * sdp = page_table_base[sdp_offset];

	// If the VA doesn't match the VPN, we have a malformed SDP
	if(GET_SDP_BITS(va) != GET_SDP_BITS(sdp->vpn)){
		return -EFAULT;
	}

	// If valid bit not set, then we have TNV (Translation Not Valid)
	// Alert the OS and make them fix it or whatever
	if (!sdp->page_metadata.valid == 0x1){
		return -EINVAL
	}

	// Check permissions - Need at least read.
	// Don't check against requested permissions since the page doesn't map here.
	permissions_t permissions = {0};
	permissions.read = 1;

	if (!check_permissions(permissions, sdp->page_metadata.permissions)){
		return -EUNAUTHORIZED;
	}

	// Don't check noncacheable, user_supervisor, dirty, global until we find the right page size for a match

	// Next 9 bits of VA specify PDP pointer
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