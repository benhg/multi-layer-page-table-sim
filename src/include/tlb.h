/**
 * @file tlb.h
 * Header for TLB APIs
 */

#ifndef TLB_H
#define TLB_H

/**
 * @brief Evicts an entry from the TLB using a modified LFU algorithm with decay.
 *
 * This function identifies the entry with the lowest counter value and evicts it 
 * to make room for a new entry. If there are empty slots, no eviction is performed.
 *
 * @param tlb Pointer to the TLB structure.
 */
void lru_evict(tlb_t *tlb);

/**
 * @brief Updates the TLB with a new translation entry.
 *
 * This function finds a free slot in the TLB and populates it with the provided
 * virtual-to-physical mapping. The caller must ensure there is a free slot.
 *
 * @param tlb Pointer to the TLB structure.
 * @param a_ctx Pointer to the address context structure containing the translation information.
 * @param phys_frame Physical frame address to be mapped.
 */
void update_tlb(tlb_t *tlb, address_context_t *a_ctx, uint64_t phys_frame);


/**
 * @brief Updates multiple TLBs based on the specified flags.
 *
 * Performs eviction and population of one or more TLBs based on the page size being updated.
 *
 * @param update_oneg Update the 1 GiB page TLB if true.
 * @param update_twom Update the 2 MiB page TLB if true.
 * @param update_fourk Update the 4 KiB page TLB if true.
 * @param ctx Pointer to the page table walk simulation context.
 * @param a_ctx Pointer to the address context structure containing the translation information.
 */
void update_tlbs(bool update_oneg, bool update_twom, bool update_fourk,
                 ptw_sim_context_t *ctx, address_context_t *a_ctx);


/**
 * @brief Checks for a TLB hit and handles a TLB miss if necessary.
 *
 * Searches the TLBs for a matching virtual-to-physical translation. On a hit, returns
 * the physical address. On a miss, evicts entries if necessary, walks the page tables,
 * and updates the TLBs with the new translation.
 *
 * @param a_ctx Pointer to the address context structure containing the translation information.
 * @param ctx Pointer to the page table walk simulation context.
 * @return Physical address on a TLB hit, or a special value indicating a miss.
 */
uintptr_t check_tlb(address_context_t *a_ctx, ptw_sim_context_t *ctx);



#endif
