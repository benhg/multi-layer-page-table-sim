/**
 * @file translation.h
 * 
 * entry point for translation. Other stuff will go here too.
 */

#ifndef TRANSLATION_H

/**
 * Translate function
 * 
 * This is the entry point for our memory translation. Basically, what has happened to get here is a cache miss (assuming our cache is virtually addressed) and we need to go to memory
 * 
 * Later, we may implement an actual cache into the simulator but for now, this is the entry point
 */
uintptr_t translate(uintptr_t va, uint32_t pid, uint8_t user_supervisor, uint8_t permisions);

#endif
