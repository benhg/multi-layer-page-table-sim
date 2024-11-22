/**
 * @file test_utils.c
 * 
 * Utility functions for testing
 */

#include <stdlib.h>

void populate_address_context(address_context_t *a_ctx, uint64_t va, permissions_t permissions, uint8_t user_supervisor, uint32_t pid) {
    if (a_ctx == NULL) {
        return; // Handle null pointer gracefully.
    }

    a_ctx->va = va;
    a_ctx->permissions = permissions;
    a_ctx->user_supervisor = user_supervisor & 0x1; // Ensure only 1 bit is used.
    a_ctx->pid = pid;
}


void populate_sim_context(ptw_sim_context_t *ctx, size_t max_pid) {
    if (ctx == NULL) {
        return; // Handle null pointer gracefully.
    }

    // Initialize the TLBs
    initialize_tlb(&ctx->oneg_tlb, ONE_G_PAGE_SIZE);  // 1GB page TLB
    initialize_tlb(&ctx->twom_tlb, TWO_M_PAGE_SIZE);  // 2MB page TLB
    initialize_tlb(&ctx->fourk_tlb, FOUR_K_PAGE_SIZE); // 4KB page TLB

    // Initialize the page table pointers for each PID
    for (size_t pid = 0; pid < max_pid && pid < MAX_PID; pid++) {
        // Allocate memory for the SDP table (1st level)
        ctx->page_table_pointers[pid] = allocate_page_table();

        // Populate the SDP table
        for (size_t sdp_idx = 0; sdp_idx < 512; sdp_idx++) {
            page_table_entry_t *sdp_entry = &ctx->page_table_pointers[pid][sdp_idx];

            // Allocate memory for PDP table (2nd level)
            sdp_entry->phys_frame = (uintptr_t)allocate_page_table();
            sdp_entry->page_metadata.valid = 1;
            sdp_entry->page_metadata.page_size = SDP_PAGE_SIZE; // Set to SDP level size
            sdp_entry->vpn = (pid << 20) | sdp_idx; // Example VPN

            // Populate the PDP table
            page_table_entry_t *pdp_table = (page_table_entry_t *)sdp_entry->phys_frame;
            for (size_t pdp_idx = 0; pdp_idx < 512; pdp_idx++) {
                page_table_entry_t *pdp_entry = &pdp_table[pdp_idx];

                // Allocate memory for PDE table (3rd level)
                pdp_entry->phys_frame = (uintptr_t)allocate_page_table();
                pdp_entry->page_metadata.valid = 1;
                pdp_entry->page_metadata.page_size = PDP_PAGE_SIZE; // Set to PDP level size
                pdp_entry->vpn = (sdp_idx << 9) | pdp_idx; // Example VPN

                // Populate the PDE table
                page_table_entry_t *pde_table = (page_table_entry_t *)pdp_entry->phys_frame;
                for (size_t pde_idx = 0; pde_idx < 512; pde_idx++) {
                    page_table_entry_t *pde_entry = &pde_table[pde_idx];

                    // Allocate memory for PTE table (4th level)
                    pde_entry->phys_frame = (uintptr_t)allocate_page_table();
                    pde_entry->page_metadata.valid = 1;
                    pde_entry->page_metadata.page_size = PDE_PAGE_SIZE; // Set to PDE level size
                    pde_entry->vpn = (pdp_idx << 9) | pde_idx; // Example VPN

                    // Populate the PTE table
                    page_table_entry_t *pte_table = (page_table_entry_t *)pde_entry->phys_frame;
                    for (size_t pte_idx = 0; pte_idx < 512; pte_idx++) {
                        page_table_entry_t *pte_entry = &pte_table[pte_idx];

                        // Set up leaf-level PTE
                        pte_entry->page_metadata.valid = 1;
                        pte_entry->page_metadata.page_size = FOUR_K_PAGE_SIZE; // Leaf size
                        pte_entry->vpn = (pde_idx << 9) | pte_idx; // Example VPN
                        pte_entry->phys_frame = allocate_physical_frame(pte_entry->vpn); // Allocate physical frame
                        pte_entry->page_metadata.permissions = DEFAULT_PERMISSIONS; // Default permissions
                    }
                }
            }
        }
    }

    // Initialize any additional backend management structures here
    // e.g., page fault handlers, statistics tracking, etc.
}


page_table_entry_t *allocate_page_table() {
    page_table_entry_t *table = (page_table_entry_t *)malloc(sizeof(page_table_entry_t) * 512);
    if (table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for page table.\n");
        exit(EXIT_FAILURE); // Handle critical allocation failure
    }

    // Initialize each entry in the page table
    for (size_t i = 0; i < 512; i++) {
        table[i].phys_frame = 0;
        table[i].vpn = 0;
        table[i].page_metadata.valid = 0; // Mark as invalid initially
        table[i].page_metadata.permissions = 0;
        table[i].page_metadata.page_size = 0;
    }
    return table;
}


uintptr_t allocate_physical_frame(uintptr_t vpn) {
    static uintptr_t next_frame = 0x100000; // Example start address for physical memory
    uintptr_t frame = next_frame;
    next_frame += FOUR_K_PAGE_SIZE; // Increment by 4KB for each allocation
    return frame;
}


/**
 * @brief Helper function to initialize a TLB.
 *
 * @param tlb Pointer to the TLB structure to initialize.
 * @param page_size The page size associated with the TLB (e.g., 4KB, 2MB, 1GB).
 */
void initialize_tlb(tlb_t *tlb, size_t page_size) {
    if (tlb == NULL) {
        return;
    }

    tlb->page_size = page_size;
    tlb->entries = (tlb_entry_t *)malloc(MAX_TLB_ENTRIES * sizeof(tlb_entry_t));
    if (tlb->entries == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for TLB entries.\n");
        return;
    }

    // Optionally initialize each TLB entry to a default invalid state
    for (size_t i = 0; i < MAX_TLB_ENTRIES; i++) {
        tlb->entries[i].valid = 0; // Mark all entries as invalid initially
    }
}


void initialize_page_table_entry(page_table_entry_t *entry, uint32_t pid, size_t index) {
    if (entry == NULL) {
        return;
    }

    entry->valid = 0;                // Mark as invalid initially
    entry->vpn = (pid << 20) | index; // Assign a VPN based on PID and index (example logic)
    entry->phys_frame = 0;           // Physical frame starts as 0 (not mapped)
    entry->page_metadata.permissions = DEFAULT_PERMISSIONS; // Set default permissions
    entry->page_metadata.page_size = FOUR_K_PAGE_SIZE; // Default to 4KB pages
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
            page_table_entry_t *pdp_base = sdp_base[sdp_idx].phys_frame;
            if (!pdp_base) continue;

            for (size_t pdp_idx = 0; pdp_idx < NUM_ENTRIES_PER_PAGE; pdp_idx++) {
                page_table_entry_t *pde_base = pdp_base[pdp_idx].phys_frame;
                if (!pde_base) continue;

                for (size_t pde_idx = 0; pde_idx < NUM_ENTRIES_PER_PAGE; pde_idx++) {
                    page_table_entry_t *pte_base = pde_base[pde_idx].phys_frame;
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
    clear_tlb(&ctx->oneg_tlb);
    clear_tlb(&ctx->twom_tlb);
    clear_tlb(&ctx->fourk_tlb);

    // Clear backend metadata (if dynamically allocated)
    if (ctx->backend_metadata) {
        free(ctx->backend_metadata);
        ctx->backend_metadata = NULL;
    }
}

void clear_tlb(tlb_t *tlb) {
    if (!tlb) return;

    // Clear all TLB entries
    memset(tlb->entries, 0, sizeof(tlb_entry_t) * tlb->num_entries);

    // Reset metadata (optional, depending on your implementation)
    tlb->num_valid_entries = 0;
    tlb->last_access_idx = 0; // Reset any tracking of LRU or similar
}
