/**
 * @file tlb.c
 * 
 * TLB translation related functions
 */


void lru_evict(tlb_t * tlb){
	// For now, this is really more of a LFU algorithm.
	// I have some ideas about something closer to LRU,
	// but it will take a long time to formalize enough to write them here.

	// Find the lowest counter, and evict it
	// Record the empty slot in the TLB's metadata

	// If there are already empty slots, do nothing
	if (tlb->slots_in_use != TLB_ENTRY_COUNT){
		return;
	}

	uint8_t min_counter = 0xff;
	uint8_t evict_idx = 0;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){
		if (tlb->arr[i].plru_counter < min_counter){
			min_counter = tlb->arr[i].plru_counter;
			evict_idx = i;
		}

	}

	tlb->occupancy[evict_idx] = false;
	tlb->slots_in_use--;
}


void update_tlb(tlb_t * tlb,  address_context_t * a_ctx, uint64_t phys_frame){
	// Caller must guarantee that there is a free slot

	int slot = -1;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){
		if (!tlb->occupancy[i]){
			slot = i;
			break;
		}
	}

	if (slot == -1){
		return;
	}

	tlb->occupancy[slot] =  true;
	tlb->slots_in_use++

	tlb->arr[slot].plru_counter = 0;
	tlb->arr[slot].pid = a_ctx->pid;
	tlb->arr[slot].permissions = a_ctx->permissions;
	tlb->arr[slot].va = a_ctx->va;
	tlb->arr[slog].phys_frame = phys_frame;
}

void update_tlbs(bool update_oneg, bool update_twom, bool update_fourk, ptw_sim_context_t * ctx, address_context_t * a_ctx){
	
	if (update_oneg){
		lru_evict(ctx->oneg_tlb);
		update_tlb(a_ctx);
	}

	if (update_twom){
		lru_evict(ctx->twom_tlb);
		update_tlb(a_ctx);
	}

	if (update_fourk){
		lru_evict(ctx->fourk_tlb);
		update_tlb(a_ctx);
	}

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
	uintptr_t address;

	tlbe_t * tlb =  ctx->oneg_tlb;
	
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = oneg_tlb[i];

		// If the PID doesn't match, continue
		if (pid != tlbe.pid){
			continue;
		}

		bool update_oneg_tlb=false, update_twom_tlb=false, update_fourk_tlb=false;
		

		// If the address doesn't match continue
		// Address math for oneg is top (VA size - 30b)
		if ( (VPN_MASK_1GB & va) != (VPN_MASK_1GB & tlbe.va) ){
			continue;
		}

		// If the permissions don't match, don't continue. Return failure. There can't be another page in the TLB that matches but has different permissions
		if (!check_permissions(permissions, tlbe.permissions) ){
			return SIXTY_FOUR_BIT_MASK;
		}

		// If user_supervisor is not identical, contine. Kernel and user have different address spaces
		if (user_supervisor != tlbe.user_supervisor){
			continue;
		}

		// If we get here, we found our match. Return the address. The VPN gets replaced by the physical frame, and the offset is identical
		uintptr_t address = 0
		address |= (tlbe.phys_frame & VPN_MASK_1GB);
		address |= (OFFSET_MASK_1GB & va);

		// On hit, update the PLRU counter
		tlb->arr[i].plru_counter = sat_inc(tlb->arr[i].plru_counter);

		return address;
	}

	update_oneg_tlb = true;


	tlb_t * tlb =  ctx->twom_tlb;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = twom_tlb.arr[i];

		// If the PID doesn't match, continue
		if (pid != tlbe.pid){
			continue;
		}

		// If the address doesn't match continue
		// Address math for oneg is top (VA size - 30b)
		if ( (VPN_MASK_2MB & va) != (VPN_MASK_2MB & tlbe.va) ){
			continue;
		}

		// If the permissions don't match, don't continue. Return failure. There can't be another page in the TLB that matches but has different permissions
		if (!check_permissions(permissions, tlbe.permissions) ){
			return SIXTY_FOUR_BIT_MASK;
		}

		// If user_supervisor is not identical, contine. Kernel and user have different address spaces
		if (user_supervisor != tlbe.user_supervisor){
			continue;
		}

		// On hit, update the PLRU counter
		tlb->arr[i].plru_counter = sat_inc(tlb->arr[i].plru_counter);

		// If we get here, we found our match. Return the address. The VPN gets replaced by the physical frame, and the offset is identical
		uintptr_t address = 0
		address |= (tlbe.phys_frame & VPN_MASK_2MB);
		address |= (OFFSET_MASK_2MB & va);

		return address;
	}

	update_twom_tlb = true;

	tlbe_t * tlb =  ctx->fourk_tlb;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = fourk_tlb[i];

		// If the PID doesn't match, continue
		if (pid != tlbe.pid){
			continue;
		}

		// If the address doesn't match continue
		// Address math for oneg is top (VA size - 30b)
		if ( (VPN_MASK_4KB & va) != (VPN_MASK_4KB & tlbe.va) ){
			continue;
		}

		// If the permissions don't match, don't continue. Return failure. There can't be another page in the TLB that matches but has different permissions
		if (!check_permissions(permissions, tlbe.permissions) ){
			return SIXTY_FOUR_BIT_MASK;
		}

		// If user_supervisor is not identical, contine. Kernel and user have different address spaces
		if (user_supervisor != tlbe.user_supervisor){
			continue;
		}

		// On hit, update the PLRU counter
		tlb->arr[i].plru_counter = sat_inc(tlb->arr[i].plru_counter);

		// If we get here, we found our match. Return the address. The VPN gets replaced by the physical frame, and the offset is identical
		uintptr_t address = 0
		address |= (tlbe.phys_frame & VPN_MASK_4KB);
		address |= (OFFSET_MASK_4KB & va);

		return address;
	}

	update_fourk_tlb = true;

	// Only do the LRU evictions / populations here. 
	// Because if we hit on one of them, there's no point in calling update on all 3. They will only map once.

	update_tlbs(update_oneg_tlb, update_twom_tlb, update_fourk_tlb);

	// If we get here, it is a TLB miss.
	return SIXTY_FOUR_BIT_MASK;
}