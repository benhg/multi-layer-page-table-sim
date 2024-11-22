#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief store config related stuff
 */

/**
 * Misc. useful macros
 */

#define VA_SIZE 48
#define PA_SIZE 48

// 30 bits are needed for offset into 1G page
#define VPN_MASK_1GB (~((1ULL << (VA_SIZE - 30ULL)) -1))
#define OFFSET_MASK_1GB (~VPN_MASK_1GB)

// 21 bits are needed for offset into 2M page
#define VPN_MASK_2MB (~((1ULL << (VA_SIZE - 21ULL)) -1))
#define OFFSET_MASK_2MB (~VPN_MASK_2MB)

// 12 bits are needed as an offset into the 4k page
#define VPN_MASK_4KB (~((1ULL << (VA_SIZE - 12ULL)) -1))
#define OFFSET_MASK_4KB (~VPN_MASK_4KB)

#define MAX_PID 32

/**
 * Fault codes
 */

#define EINVAL 1
#define EFAULT 2
#define EUNAUTHORIZED 3
#define EACCESS 4

#endif