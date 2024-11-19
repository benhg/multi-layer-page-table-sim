#ifndef PAGE_TABLE_API_H
#define PAGE_TABLE_API_H

#include <stdint.h>

/**
 * @file page_table_api.h
 * @brief this file maintains external API and data structures
 * We use structs to model hardware structures like TLBs, page table walkers, etc.
 */

/**
 * Utility Structs
 */

typedef enum page_size {
	FOUR_K = 0;
	TWO_M = 1;
	ONE_G = 3;
	SIZE_MAX = 4;
} page_size_t;


/**
 * Structs modeling hardware structures
 */

/**
 * TLB entry
 * Note that page size is not stored in the TLB. This is because each page size has a separaate TLB
 */
typedef struct tlb_entry {
			uint64_t va; /* In a real TLB, this is a CAM structure (or some kind of multi-way, set-associative lookup) , so the VA itself is programmed into the block of flipflops that are used for the CAM lookup. So it's not technically wrong to have it here. However, for simplicity, we will implement the TLB lookup in this program as iterating through all the TLB entries and comparing against this.If/when we add cycle counting, we will reflect the true nature of the TLB by undercounting carefully. */
			uint64_t pa_offset; // Offset into page - u64 type because we use this for all 3 page table sizes.
			uint64_t vpn_mask; // What to mask off in order to get the offset
} tlb_entry_t;

// Shorthand
typedef tlb_entry_t tlbe_t;


/**
 * The PTE type
 */
typedef struct page_table_entry {
	  uint64_t vpn; //< the virtual page number this page is encoding
                  //< We keep this here so that the page table doesn't have to
                  // be sorted by VPN in the model. In real memory, the page table is always sorted and indexed by VPN so it doesn't need to be duplicated.
    union {
        uint16_t oneg_pte_index : 8;   //< One gig PTE offset (which page entry do we read)
        uint32_t twom_pte_index : 17;  //< Two meg PTE offset
        uint32_t fourk_pte_index : 24; //< 4K page PTE offset
    } phys_frame;                      //< Physical address base
                                       // The place the page maps to
    // Separated by size to help keep address comparisons clear.
    struct {
        uint32_t pid;                 //< PID the page is assigned to
        enum page_size page_size : 2; //< Page size: 0: 4K, 1: 2M, 2: 1G
        uint8_t permission : 3;       //< R/W/X bits
        uint8_t user_supervisor : 1;  //< 0 for user page, 1 for supervisor page
        uint8_t global : 1;           //< Currently unused global bit
        uint8_t valid : 1;            //< Valid bit
        uint8_t noncacheable : 1; //< Non-cacheable (streaming, last use, etc.)
        uint8_t dirty : 1; //<useful when I implement swapping pages to disk (TNV/PNF) faults
    } page_metadata;              //< Structured page metadata

} page_table_entry_t;

// Shorthand
typedef page_table_entry_t pte_t;


/**
 * Translate function
 * 
 * This is the entry point for our memory translation. Basically, what has happened to get here is a cache miss (assuming our cache is virtually addressed) and we need to go to memory
 * 
 * Later, we may implement an actual cache into the simulator but for now, this is the entry point
 */
uintptr_t translate(uintptr_t va, uint32_t pid, uint8_t user_supervisor, uint8_t permisions);


/**
 * This is mostly going to be used internally
 */
int invalidate_tlb_by_page(uintptr_t va, page_size_t page_size);

/**
 * Use pseudo-LRU algorithm to evict an address from the TLB
 */
int tlb_evict(page_size_t page_size);

#endif