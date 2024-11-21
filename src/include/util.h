#ifndef UTIL_H
#define UTIL_H

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

/**
 * Saturated sum - sum only if val is less than 255
 */
static inline uint8_t sat_sum(uint8_t a, uint8_t b){
    if ((EIGHT_BIT_MASK - a) <= b){
        return EIGHT_BIT_MASK;
    }

    return a + b;
}

/**
 * Saturated Increment
 */
static inline uint8_t sat_inc(uint8_t a){
    return sat_sum(a, 1);
}


/**
 * Check if a page's permissions are more or less strict than the permissions requested
 * 
 * Return true if the request would be permitted. EG. read request to a R/W page is allowed, but W to an RO page isn't
 */
static bool check_permissions(permissions_t permissions_req, permissions_t permissions_page)



#endif
