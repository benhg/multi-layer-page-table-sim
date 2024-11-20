/**
 * @file page_table_walk.c
 * 
 * The actual business logic of the page table walk
 */

#include <stdint.h>
#include <stdbool.h>


void tlb_update(){

}

uintptr_t check_tlb(uintptr_t va, uint32_t pid, uint8_t user_supervisor, uint8_t permissions){
	/**
	 * First, check the TLB
	 * If hit, return;
	 * 
	 * If miss:
	 * 	  pseudo-lru_evict()
	 * 	  wait for walk
	 *    add adress translation that we just ran to TLB
	 */

	/**
	 * Our TLB check will just walk through the TLB linearly
	 * Since we would implement the TLB as a set-associative structure (or CAM if we need it to be single-cycle),
	 * we can handle that in cycle counting later.
	 */

}


uintptr_t translate(uintptr_t va, uint32_t pid, uint8_t user_supervisor, uint8_t permisions){
	/**
	 * First, use the PID to look up the pointer to the directory table
	 * 
	 * Use the top 8 bits of the address as the index in the directory pointer table. The physical frame that it points at is the base of a page directory.
	 * 
	 * That points to a directory page table entry. Use the next 9 bits as an index into the directory.
	 * 
	 * That points us to a page table entry. Use that page table entry (in whatever size it is labelled as)
	 * 
	 * Assume, for now, that within a PID, an address has only one match
	 */

	bool must_update_tlb = false;

	// Try the TLB
	// Eviction (if necessary) is handled inside this call
	uintptr_t translated_addr = check_tlb(va, pid, user_supervisor, permissions);

	if (translated_addr == SIXTY_FOUR_BIT_MASK){
		must_update_tlb = true;
	}


	// Publish the found address into the TLB
	// An entry is guaranteed to be free here since we already did the eviction
	tlb_update(va, translated_addr, pid, user_supervisor, permissions);


}