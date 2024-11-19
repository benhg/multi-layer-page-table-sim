#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief store config and util related stuff
 */

/**
 * Misc. useful macros
 */

#define KB(x) (1024U * x)
#define MB(x) (KB(KB(x)))
#define GB(x) (KB(MB(x)))

#define EIGHT_BIT_MASK 0xff
#define SIXTEEN_BIT_MASK 0xffff
#define TWENTY_SIX_BIT_MASK 0x3FFFFFF
#define THIRTY_TWO_BIT_MASK 0xffffffff
#define SIXTY_FOUR_BIT_MASK 0xffffffffffffffff

/**
 * Safe free() wrapper
 */
#define PTR_FREE(ptr)                                                          \
    if (ptr != NULL) {                                                         \
        free(ptr);                                                             \
    }


#define VA_SIZE 64
#define PA_SIZE 63


#endif