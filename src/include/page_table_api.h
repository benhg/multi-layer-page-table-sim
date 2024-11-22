/**
 * @file page_table_api.h
 * @brief this file maintains external API and data structures
 * We use structs to model hardware structures like TLBs, page table walkers, etc.
 */

#ifndef PAGE_TABLE_API_H
#define PAGE_TABLE_API_H

#include "hw_structures.h"
#include "config.h"
#include "util.h"
#include <stdint.h>



/**
 * Address context struct
 * Use this to move around address and metadata (PID, etc.)
 * This keeps from "parameter explosion"
 */
typedef struct address_context {
	uint64_t va;
	permissions_t permissions;
	uint8_t user_supervisor: 1;
	uint32_t pid;
} address_context_t;


/**
 * Context struct
 * 
 * This struct stores the context of a run of this program
 * Basically, it's a struct of pointers to the data structures you need to run the program
 */
typedef struct ptw_sim_context {
	/**
	 * Three TLBs
	 */
	tlb_t oneg_tlb;
	tlb_t twom_tlb;
	tlb_t fourk_tlb;
	/**
	 * Array of pointers to page tables
	 * In sim, these are indexes into the PT array
	 * In hardware, these are registers that point at a single SDP entry
	 * 
	 * Each of these pointes must point at 512 valid SDP pages.
	 * 512 SDP entries, each of which is 64b consumes 1 page
	 * In real hardware, we'd shave this to be 64b exactly.
	 * In simulator, add extra metadata to make life easy.
	 */
	pte_t * page_table_pointers[MAX_PID];

	// TODO: add backend management pointers here for easy programming of falid page tables

} ptw_sim_context_t;


#endif