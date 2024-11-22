/**
 * @file tlb.h
 * Header for TLB APIs
 */

#ifndef TLB_H
#define TLB_H

void lru_evict(tlb_t *tlb);

void update_tlb(tlb_t *tlb, address_context_t *a_ctx, uint64_t phys_frame);

uintptr_t check_tlb(uintptr_t va, uint32_t pid, uint8_t user_supervisor,
                    permissions_t permissions, ptw_sim_context_t *ctx);

void update_tlbs(bool update_oneg, bool update_twom, bool update_fourk,
                 ptw_sim_context_t *ctx, address_context_t *a_ctx);

#endif
