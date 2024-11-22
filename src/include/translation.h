/**
 * @file translation.h
 * 
 * entry point for translation. Other stuff will go here too.
 */

#ifndef TRANSLATION_H

/**
 * @brief Translates a virtual address to a physical address.
 * 
 * This function serves as the entry point for memory address translation in the simulator. 
 * It is invoked when a cache miss occurs (assuming a virtually addressed cache) and the 
 * system must consult the page tables to resolve the virtual address (VA) into a physical 
 * address (PA).
 * 
 * The translation process involves walking through the multi-level page table hierarchy 
 * (SDP, PDP, PDE, PTE) to locate the appropriate page table entry for the given VA. 
 * Permissions, validity, and page size metadata are checked at each level to ensure 
 * correctness and access rights. If a valid mapping is found, the physical address is 
 * computed; otherwise, an error code is returned.
 * 
 * This function could later integrate 
 * such components to simulate realistic memory access patterns.
 * 
 * @param a_ctx Pointer to the address context, containing the virtual address, PID, and 
 *              access permissions required for translation.
 * 
 * @return 
 * - On success: The translated physical address.
 * - On failure: An error code indicating the type of fault:
 *     - -EFAULT: Malformed or invalid page table entries.
 *     - -EINVAL: Translation not valid (TNV).
 *     - -EUNAUTHORIZED: Insufficient permissions to access the page.
 *     - -EACCESS: Access violation (e.g., user/supervisor mismatch).
 */
uintptr_t translate(address_context_t *a_ctx);

#endif
