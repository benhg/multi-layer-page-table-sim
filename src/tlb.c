/**
 * @file tlb.c
 * 
 * TLB translation related functions
 */

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

		return address;
	}


	tlbe_t * tlb =  ctx->twom_tlb;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = oneg_tlb[i];

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

		// If we get here, we found our match. Return the address. The VPN gets replaced by the physical frame, and the offset is identical
		uintptr_t address = 0
		address |= (tlbe.phys_frame & VPN_MASK_2MB);
		address |= (OFFSET_MASK_2MB & va);

		return address;
	}

	tlbe_t * tlb =  ctx->fourk_tlb;
	for (int i=0; i<TLB_ENTRY_COUNT; i++){

		tlbe_t tlbe = oneg_tlb[i];

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

		// If we get here, we found our match. Return the address. The VPN gets replaced by the physical frame, and the offset is identical
		uintptr_t address = 0
		address |= (tlbe.phys_frame & VPN_MASK_4KB);
		address |= (OFFSET_MASK_4KB & va);

		return address;
	}

	// If we get here, it is a TLB miss.
	return SIXTY_FOUR_BIT_MASK;
}