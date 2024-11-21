/**
 * @file page_table_walk.c
 * 
 * The actual business logic of the page table walk
 */

#include <stdint.h>
#include <stdbool.h>


void tlb_update(){

}

/**
 * Check if a page's permissions are more or less strict than the permissions requested
 * 
 * Return true if the request would be permitted. EG. read request to a R/W page is allowed, but W to an RO page isn't
 */
static bool check_permissions(permissions_t permissions_req, permissions_t permissions_page){
	bool read_check = true;
	bool write_check = true;
	bool execute_check = true;

	// Only set to false if the permissions request read
	if (permissions_req.read == 1){
		if (permissions_page.read != 1){
			read_check = false;

		}
	}

	if (permissions_req.write == 1){
		if (permissions_page.write != 1){
			write_check = false;

		}
	}

	if (permissions_req.execute == 1){
		if (permissions_page.execute != 1){
			execute_check = false;

		}
	}

	return read_check && write_check && execute_check;
}

uintptr_t check_tlb(uintptr_t va, uint32_t pid, uint8_t user_supervisor, permissions_t permissions, ptw_sim_context_t * ctx){
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

	/**
	 * Also, we'll walk in all sizes.
	 * 
	 * In hardware, that'd be all 3 in parallel and then returning the biggest matching page
	 */

	tlbe_t * tlb =  oneg_tlb;
	
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = oneg_tlb[i];

		// If the address doesn't match continue
		// Address math for oneg is top (VA size - 30b)
		if ( (VPN_MASK_1GB & va) == (VPN_MASK_1GB & tlbe.va) ){

		}

		// If the permissions don't match, don't continue. Return failure. There can't be another page in the TLB that matches but has different permissions
		if (check_permissions() ){

		}

		// If user_supervisor is not identical, contine. Kernel and user have different address spaces
		if (user_supervisor != ){

		}




	}



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