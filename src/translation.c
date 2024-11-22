/**
 * @file translation.c
 * Translation entry point
 */

#include "translation.h"

#include "tlb.h"
#include "page_table.h"

uintptr_t translate(address_context_t *a_ctx, ptw_sim_context_t *ctx) {

  bool must_update_tlb = false;

  // Try the TLB
  // Eviction (if necessary) is handled inside this call
  // This call tells us which TLBs to update as well - via output params
  //bool update_fourk_tlb, update_twom_tlb, udpate_oneg_tlb;
  uintptr_t translated_addr = check_tlb(a_ctx, ctx);

  if (translated_addr == SIXTY_FOUR_BIT_MASK) {
    must_update_tlb = true;
  }

  // Walk the page table
  translated_addr = walk(a_ctx, ctx);

  // If it's still invalid, then we fault. This would result in going to the OS
  // and having the OS swap pages around For now, it's just a fault and is out
  // of scope of this project.
  if (translated_addr == SIXTY_FOUR_BIT_MASK) {
    return SIXTY_FOUR_BIT_MASK;
  }

  // Publish the found address into the TLB
  // An entry is guaranteed to be free here since we already did the eviction
  if (must_update_tlb) {
    // Update all TLBs, for now
    // TODO: pass back which TLBs to update and only update those.
    update_tlbs(true, true, true, ctx, a_ctx);
  }

  return translated_addr;
}