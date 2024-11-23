/**
 * @file hw_structures.h
 *
 * Structs that reflect HW structures
 */

#ifndef HW_STRUCTURES_H
#define HW_STRUCTURES_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "util.h"

#define TLB_ENTRY_COUNT 32

typedef enum page_size {
  FOUR_K = 0,
  TWO_M = 1,
  ONE_G = 3,
  PG_SIZE_MAX = 4
} page_size_t;

/**
 * TLB entry
 * Note that page size is not stored in the TLB. This is because each page size
 * has a separaate TLB
 */
typedef struct tlb_entry {
  uint64_t
      va; /* In a real TLB, this is a CAM structure (or some kind of multi-way,
             set-associative lookup) , so the VA itself is programmed into the
             block of flipflops that are used for the CAM lookup. So it's not
             technically wrong to have it here. However, for simplicity, we will
             implement the TLB lookup in this program as iterating through all
             the TLB entries and comparing against this.If/when we add cycle
             counting, we will reflect the true nature of the TLB by
             undercounting carefully. */
  uint64_t phys_frame; // Offset into page - u64 type because we use this for
                       // all 3 page table sizes.
  uint8_t user_supervisor : 1;
  permissions_t permissions;
  uint32_t pid;
  uint8_t plru_counter; // Counter for PLRU eviction
  uint8_t valid : 1;
} tlb_entry_t;

// Shorthand
typedef tlb_entry_t tlbe_t;

/**
 * TLB
 */
typedef struct tlb {
  tlbe_t arr[TLB_ENTRY_COUNT];
  uint8_t slots_in_use;
  bool occupancy[TLB_ENTRY_COUNT];
} tlb_t;

/**
 * The PTE type
 */
typedef struct page_table_entry {
  uint64_t vpn; //< the virtual page number this page is encoding
                //< We keep this here so we can do sanity checking in the model
                // be sorted by VPN in the model. In real memory, the page table
                // is always sorted and indexed by VPN so it doesn't need to be
                // duplicated.
  union {
    uintptr_t
        oneg_pte_index : 8; //< One gig PTE offset (which page entry do we read)
    uintptr_t twom_pte_index : 17;  //< Two meg PTE offset
    uintptr_t fourk_pte_index : 24; //< 4K page PTE offset
  } phys_frame;                     //< Physical address base
                                    // The place the page maps to
  // Separated by size to help keep address comparisons clear.
  struct {
    uint32_t pid; //< PID the page is assigned to
                  // Also not stored in hardware. PID enforcement is done by the
                  // fact that each process has its own page table.
    page_size_t page_size;       //< Page size: 0: 4K, 1: 2M, 2: 1G
    permissions_t permissions;   //< R/W/X bits
    uint8_t user_supervisor : 1; //< 0 for user page, 1 for supervisor page
                                 // Uint8 instead of bool to use bitfield
    uint8_t global : 1;          //< Currently unused global bit
    uint8_t valid : 1;           //< Valid bit
    uint8_t noncacheable : 1;    //< Non-cacheable (streaming, last use, etc.)
    uint8_t dirty : 1; //<useful when I implement swapping pages to disk
                       //(TNV/PNF) faults
  } page_metadata;     //< Structured page metadata

} page_table_entry_t;

// Shorthand
typedef page_table_entry_t pte_t;

#endif
