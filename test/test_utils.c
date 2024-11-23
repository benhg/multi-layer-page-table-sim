/**
 * @file test_utils.c
 * 
 * Utility functions for testing
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hw_structures.h"
#include "config.h"
#include "page_table.h"
#include "util.h"
#include "tlb.h"
#include "page_table_api.h"
#include "test_utils.h"

uintptr_t allocate_physical_frame(uintptr_t vpn) {
    static uintptr_t next_frame = 0x100000; // Example start address for physical memory
    uintptr_t frame = next_frame;
    next_frame += FOUR_K; // Increment by 4KB for each allocation
    return frame;
}

void populate_address_context(address_context_t *a_ctx, uint64_t va, permissions_t permissions, uint8_t user_supervisor, uint32_t pid) {
    if (a_ctx == NULL) {
        return; // Handle null pointer gracefully.
    }

    a_ctx->va = va;
    a_ctx->permissions = permissions;
    a_ctx->user_supervisor = user_supervisor & 0x1; // Ensure only 1 bit is used.
    a_ctx->pid = pid;
}


/**
 * @brief Helper function to initialize a TLB.
 *
 * @param tlb Pointer to the TLB structure to initialize.
 */
void initialize_tlb(tlb_t *tlb) {

    tlb = (tlb_t *)malloc(sizeof(tlb_t));
    
    memset(tlb, 0, sizeof(tlb_t));

    // Optionally initialize each TLB entry to a default invalid state
    for (size_t i = 0; i < TLB_ENTRY_COUNT; i++) {
        tlb->arr[i].valid = 0; // Mark all entries as invalid initially
    }
}

page_table_entry_t *allocate_page_table() {
    page_table_entry_t *table = (page_table_entry_t *)malloc(sizeof(page_table_entry_t) * 512);
    if (table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for page table.\n");
        exit(EXIT_FAILURE); // Handle critical allocation failure
    }

    // Initialize each entry in the page table
    for (size_t i = 0; i < 512; i++) {
        table[i].phys_frame.oneg_pte_index = 0;
        table[i].phys_frame.twom_pte_index = 0;
        table[i].phys_frame.fourk_pte_index = 0;
        table[i].vpn = 0;
        table[i].page_metadata.valid = 0; // Mark as invalid initially
        table[i].page_metadata.permissions.raw = 0;
        table[i].page_metadata.page_size = 0;
    }
    return table;
}

void populate_sim_context(ptw_sim_context_t *ctx, size_t max_pid) {
    if (ctx == NULL) {
        return; // Handle null pointer gracefully.
    }

    // Initialize the TLBs
    initialize_tlb(ctx->oneg_tlb);  // 1GB page TLB
    initialize_tlb(ctx->twom_tlb);  // 2MB page TLB
    initialize_tlb(ctx->fourk_tlb); // 4KB page TLB

    // Initialize the page table pointers for each PID
    for (size_t pid = 0; pid < max_pid && pid < MAX_PID; pid++) {
        // Allocate memory for the SDP table (1st level)
        ctx->page_table_pointers[pid] = allocate_page_table();

        // Populate the SDP table
        for (size_t sdp_idx = 0; sdp_idx < 512; sdp_idx++) {
            page_table_entry_t *sdp_entry = &ctx->page_table_pointers[pid][sdp_idx];

            // Allocate memory for PDP table (2nd level)
            sdp_entry->phys_frame.oneg_pte_index = (uintptr_t)allocate_page_table();
            sdp_entry->page_metadata.valid = 1;
            sdp_entry->page_metadata.page_size = PG_SIZE_MAX; // Set to SDP level size
            sdp_entry->vpn = (pid << 20) | sdp_idx; // Example VPN

            // Populate the PDP table
            page_table_entry_t *pdp_table = (page_table_entry_t *)sdp_entry->phys_frame.oneg_pte_index;
            for (size_t pdp_idx = 0; pdp_idx < 512; pdp_idx++) {
                page_table_entry_t *pdp_entry = &pdp_table[pdp_idx];

                // Allocate memory for PDE table (3rd level)
                pdp_entry->phys_frame.twom_pte_index = (uintptr_t)allocate_page_table();
                pdp_entry->page_metadata.valid = 1;
                pdp_entry->page_metadata.page_size = ONE_G; // Set to PDP level size
                pdp_entry->vpn = (sdp_idx << 9) | pdp_idx; // Example VPN

                // Populate the PDE table
                page_table_entry_t *pde_table = (page_table_entry_t *)pdp_entry->phys_frame.twom_pte_index;
                for (size_t pde_idx = 0; pde_idx < 512; pde_idx++) {
                    page_table_entry_t *pde_entry = &pde_table[pde_idx];

                    // Allocate memory for PTE table (4th level)
                    pde_entry->phys_frame.fourk_pte_index = (uintptr_t)allocate_page_table();
                    pde_entry->page_metadata.valid = 1;
                    pde_entry->page_metadata.page_size = TWO_M; // Set to PDE level size
                    pde_entry->vpn = (pdp_idx << 9) | pde_idx; // Example VPN

                    // Populate the PTE table
                    page_table_entry_t *pte_table = (page_table_entry_t *)pde_entry->phys_frame.fourk_pte_index;
                    for (size_t pte_idx = 0; pte_idx < 512; pte_idx++) {
                        page_table_entry_t *pte_entry = &pte_table[pte_idx];

                        // Set up leaf-level PTE
                        pte_entry->page_metadata.valid = 1;
                        pte_entry->page_metadata.page_size = FOUR_K; // Leaf size
                        pte_entry->vpn = (pde_idx << 9) | pte_idx; // Example VPN
                        pte_entry->phys_frame.fourk_pte_index = allocate_physical_frame(pte_entry->vpn); // Allocate physical frame
                        pte_entry->page_metadata.permissions.val.read = 0;
                        pte_entry->page_metadata.permissions.val.write = 0;
                        pte_entry->page_metadata.permissions.val.execute = 0;

                    }
                }
            }
        }
    }

    // Initialize any additional backend management structures here
    // e.g., page fault handlers, statistics tracking, etc.
}


void initialize_page_table_entry(page_table_entry_t *entry, uint32_t pid, size_t index) {
    if (entry == NULL) {
        return;
    }

    entry->page_metadata.valid = 0;                // Mark as invalid initially
    entry->vpn = (pid << 20) | index; // Assign a VPN based on PID and index (example logic)
    entry->phys_frame.fourk_pte_index = 0; // Physical frame starts as 0 (not mapped)
    entry->phys_frame.twom_pte_index = 0; // Physical frame starts as 0 (not mapped)
    entry->phys_frame.oneg_pte_index = 0; // Physical frame starts as 0 (not mapped)
    entry->page_metadata.permissions.val.read = 0;
    entry->page_metadata.permissions.val.write = 0;
    entry->page_metadata.permissions.val.execute = 0;
    entry->page_metadata.page_size = FOUR_K; // Default to 4KB pages
    entry->page_metadata.valid = 0;  // Initially invalid
}

void teardown_sim_context(ptw_sim_context_t *ctx, size_t max_pid) {
    if (!ctx) return;

    // Iterate over all PIDs to free page table memory
    for (size_t pid = 0; pid < max_pid; pid++) {
        page_table_entry_t *sdp_base = ctx->page_table_pointers[pid];

        if (!sdp_base) continue;

        // Free each level of page table entries
        for (size_t sdp_idx = 0; sdp_idx < NUM_ENTRIES_PER_PAGE; sdp_idx++) {
            page_table_entry_t *pdp_base =  (page_table_entry_t *) sdp_base[sdp_idx].phys_frame.oneg_pte_index;
            if (!pdp_base) continue;

            for (size_t pdp_idx = 0; pdp_idx < NUM_ENTRIES_PER_PAGE; pdp_idx++) {
                page_table_entry_t *pde_base = (page_table_entry_t *)  pdp_base[pdp_idx].phys_frame.twom_pte_index;
                if (!pde_base) continue;

                for (size_t pde_idx = 0; pde_idx < NUM_ENTRIES_PER_PAGE; pde_idx++) {
                    page_table_entry_t *pte_base = (page_table_entry_t *)  pde_base[pde_idx].phys_frame.fourk_pte_index;
                    if (pte_base) {
                        free(pte_base); // Free PTE base
                    }
                }

                free(pde_base); // Free PDE base
            }

            free(pdp_base); // Free PDP base
        }

        free(sdp_base); // Free SDP base
        ctx->page_table_pointers[pid] = NULL;
    }

    // Clear the TLBs (optional depending on simulator design)
    clear_tlb(ctx->oneg_tlb);
    clear_tlb(ctx->twom_tlb);
    clear_tlb(ctx->fourk_tlb);

}

void clear_tlb(tlb_t *tlb) {
    if (!tlb) return;

    // Clear all TLB entries
    memset(tlb->arr, 0, sizeof(tlb_entry_t) * TLB_ENTRY_COUNT);

    // Reset metadata (optional, depending on your implementation)
    tlb->slots_in_use = 0;
    for (int i =0; i<TLB_ENTRY_COUNT; i++){
        tlb->arr[i].plru_counter = 0; // Reset any tracking of LRU or similar
    }
}
