/**
 * @file page_table_walk.c
 *
 * The actual business logic of the page table walk
 */

#include <stdbool.h>
#include <stdint.h>

#include "page_table.h"
#include "page_table_api.h"
#include "util.h"

uintptr_t walk(address_context_t *a_ctx, ptw_sim_context_t *ctx) {
  /**
   * First, use the PID to look up the pointer to the directory table
   *
   * Use the top (VA_SIZE - (9*3)) bits of the address as the index in the
   * directory pointer table. The physical frame that it points at is the base
   * of a page directory.
   *
   * That points to a directory page table entry. Use the next 9 bits as an
   * index into the directory.
   *
   * That points us to a page table entry. Use that page table entry (in
   * whatever size it is labelled as)
   *
   * Assume, for now, that within a PID, an address has only one match
   */

  uintptr_t pa = 0;

  uint64_t va = a_ctx->va;
  // Pointer to the page table base. That is a block of 512 SDP entries
  uint32_t pid = a_ctx->pid;
  pte_t *page_table_base = ctx->page_table_pointers[pid];

  // Top 9 bits of VA specify SPDP pointer
  // No page size maps to a real page at this level
  uint16_t sdp_offset = GET_SDP_ENTRY_IDX(va);
  pte_t *sdp = &page_table_base[sdp_offset];

  // If the VA doesn't match the VPN, we have a malformed SDP
  if (GET_SDP_BITS(va) != GET_SDP_BITS(sdp->vpn)) {
    return -EFAULT;
  }

  // If valid bit not set, then we have TNV (Translation Not Valid)
  // Alert the OS and make them fix it or whatever
  if ((sdp->page_metadata.valid != 0x1)) {
    return -EINVAL;
  }

  // Check permissions - Need at least read.
  // Don't check against requested permissions since the page doesn't map here.
  permissions_t r_permissions = {0};
  r_permissions.val.read = 1;

  if (!check_permissions(r_permissions, sdp->page_metadata.permissions)) {
    return -EUNAUTHORIZED;
  }

  // Don't check noncacheable, user_supervisor, dirty, global until we find the
  // right page size for a match

  // The phys_frame is overloaded. In this case, it points at a 4k page.
  // This address should always be 4k-aligned
  pte_t *pdp_base = (pte_t *)sdp->phys_frame.oneg_pte_index;
  // Next 9 bits of VA specify PDP pointer
  // If PDP pointer is marked 1G page, return immediately with that frame
  uint16_t pdp_offset = GET_PDP_ENTRY_IDX(va);
  pte_t *pdp = &pdp_base[pdp_offset];

  // If the VA doesn't match the VPN, we have a malformed PDP
  if (GET_PDP_BITS(va) != GET_PDP_BITS(pdp->vpn)) {
    return -EFAULT;
  }

  // If not valid, return TNV
  if (!pdp->page_metadata.valid) {
    return -EINVAL;
  }

  // If the page is labelled as a 1G page, that means we found our page and we
  // should do the translation
  if (pdp->page_metadata.page_size == ONE_G) {

    // Check for permissions
    if (!check_permissions(a_ctx->permissions,
                           pdp->page_metadata.permissions)) {
      return -EUNAUTHORIZED;
    }

    // user_supervisor must be the same
    if (a_ctx->user_supervisor != pdp->page_metadata.user_supervisor) {
      return -EACCESS;
    }

    // Ignore noncacheable and dirty until swap and caches exist, respectively

    pa |= GET_PDP_BITS(pdp->phys_frame.oneg_pte_index);
    pa |= GET_PDP_OFFSET(va);

    return pa;
  }

  // Otherwise, only check read_permissions and continue walking
  if (!check_permissions(r_permissions, pdp->page_metadata.permissions)) {
    return -EUNAUTHORIZED;
  }

  pte_t *pde_base = (pte_t *)pdp->phys_frame.twom_pte_index;
  uint16_t pde_offset = GET_PDP_ENTRY_IDX(va);
  pte_t *pde = &pde_base[pde_offset];

  // Otherwise, use that to look up the PDE
  // If PDE page is marked as a 2M page, then return immediately with that frame

  // If the VA doesn't match the VPN, we have a malformed PDP
  if (GET_PDE_BITS(va) != GET_PDE_BITS(pde->vpn)) {
    return -EFAULT;
  }

  // If not valid, return TNV
  if (!pde->page_metadata.valid) {
    return -EINVAL;
  }

  // If the page is labelled as a 1G page, that means we found our page and we
  // should do the translation
  if (pde->page_metadata.page_size == TWO_M) {

    // Check for permissions
    if (!check_permissions(a_ctx->permissions,
                           pde->page_metadata.permissions)) {
      return -EUNAUTHORIZED;
    }

    // user_supervisor must be the same
    if (a_ctx->user_supervisor != pde->page_metadata.user_supervisor) {
      return -EACCESS;
    }

    // Ignore noncacheable and dirty until swap and caches exist, respectively

    pa |= GET_PDE_BITS(pde->phys_frame.twom_pte_index);
    pa |= GET_PDE_OFFSET(va);

    return pa;
  }

  // ONE_G page can also be mapped here
  if (pde->page_metadata.page_size == ONE_G) {

    pa |= GET_PDP_BITS(pde->phys_frame.oneg_pte_index);
    pa |= GET_PDP_OFFSET(va);

    return pa;
  }

  // Otherwise, only check read_permissions and continue walking
  if (!check_permissions(r_permissions, pdp->page_metadata.permissions)) {
    return -EUNAUTHORIZED;
  }

  // Otherwise use that to look up the leaf-level PTE
  // If that PTE is invalid or not matching, return fault and the OS will need
  // to make page entries.
  pte_t *pte_base = (pte_t *)pde->phys_frame.fourk_pte_index;
  uint16_t pte_offset = GET_PTE_ENTRY_IDX(va);
  pte_t *pte = &pte_base[pte_offset];

  // Otherwise, use that to look up the PDE
  // If PDE page is marked as a 2M page, then return immediately with that frame

  // If the VA doesn't match the VPN, we have a malformed PDP
  if (GET_PTE_BITS(va) != GET_PTE_BITS(pte->vpn)) {
    return -EFAULT;
  }

  // If not valid, return TNV
  if (!pte->page_metadata.valid) {
    return -EINVAL;
  }

  // Check for permissions
  if (!check_permissions(a_ctx->permissions, pde->page_metadata.permissions)) {
    return -EUNAUTHORIZED;
  }

  // user_supervisor must be the same
  if (a_ctx->user_supervisor != pde->page_metadata.user_supervisor) {
    return -EACCESS;
  }

  // Ignore noncacheable and dirty until swap and caches exist, respectively

  // At this point, all 3 page sizes are valid, the only difference is what
  // macro we use. So we can pull out the checks and run them up front.
  if (pte->page_metadata.page_size == ONE_G) {

    pa |= GET_PDP_BITS(pte->phys_frame.oneg_pte_index);
    pa |= GET_PDP_OFFSET(va);

    return pa;
  }

  if (pte->page_metadata.page_size == TWO_M) {

    pa |= GET_PDE_BITS(pte->phys_frame.twom_pte_index);
    pa |= GET_PDE_OFFSET(va);

    return pa;
  }

  if (pte->page_metadata.page_size == TWO_M) {

    pa |= GET_PTE_BITS(pte->phys_frame.fourk_pte_index);
    pa |= GET_PTE_OFFSET(va);

    return pa;
  }

  // If we get here, the page was not found and this is a page fault.
  // When paging exists, the OS can fix it
  // Otherwise, return fault to the process
  return -EFAULT;
}
