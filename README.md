
# multi-layer-page-table-sim

This project is a functional simulator of a 4-layer page table supporting 3 page sizes: 4 KiB, 2 MiB, and 1 GiB. It is designed to simulate translation lookaside buffer (TLB) behavior and multi-level page table traversal, providing a detailed view of the interaction between these components in a memory management unit (MMU).

## Key Features

### TLB:

Dynamically manages entries using a modified "LFU with decay" eviction algorithm. This algorithm is simple to implement and offers performance comparable to LRU and pseudo-LRU schemes by prioritizing frequently accessed entries while decaying older usage statistics to adapt to access patterns dynamically.

### Page Table Walk Routine:

imulates the process of walking through a 4-level hierarchical page table structure, accommodating varying page sizes and simulating access costs.

## Address Details

The virtual address system supports three page sizes: 4 KiB, 2 MiB, and 1 GiB, and the address structure changes depending on the page size.

### 4 KiB Pages
For 4 KiB pages, the system uses 4 levels of translation. Each level contributes 9 bits to the final physical address, and the page offset is 12 bits.

+---------+---------+---------+---------+----------------------+
| Level 4 | Level 3 | Level 2 | Level 1 | Page Offset          |
+---------+---------+---------+---------+----------------------+
|   9b    |   9b    |   9b    |   9b    |        12b           |
+---------+---------+---------+---------+----------------------+
### 2 MiB Pages
For 2 MiB pages, 3 levels of translation are used. The page offset increases to 21 bits, reducing the number of layers needed to resolve the address.

+---------+---------+---------+---------+----------------------+
| Level 4 | Level 3 | Level 2 | Level 1 | Page Offset          |
+---------+---------+---------+---------+----------------------+
|   9b    |   9b    |   9b    |        21b                     |
+---------+---------+---------+---------+----------------------+
### 1 GiB Pages
For 1 GiB pages, only 2 levels of translation are required. The offset expands to 30 bits, simplifying the address resolution process.

+---------+---------+---------+---------+----------------------+
| Level 4 | Level 3 | Level 2 | Level 1 | Page Offset          |
+---------+---------+---------+---------+----------------------+
|   9b    |   9b    |           30b                            |
+---------+---------+---------+---------+----------------------+

### Hypothetical Larger Pages
If a single-level translation were added for 512 GiB pages, the offset would  be 39 bits, leaving only the 9-bit Level 4 index. However, this configuration is impractical for most use cases due to excessive page size. 

We only use the fourth level of translation to ensure a uniform page table size - each block of PTEs consumes exactly one 4 KiB page.

## TLB Details

### TLBs

The simulator models three separate TLBs, each optimized for one of the three page sizes:

4 KiB TLB: Handles the most granular translations and has the highest entry count.
2 MiB TLB: Serves medium-sized pages, balancing capacity and coverage.
1 GiB TLB: Designed for coarse-grained translations, minimizing overhead for very large contiguous memory regions.

Since at time of lookup, a page size is not known, all 3 TLBs are always searched.

### TLB Eviction Policy: "LFU with Decay"
The TLB employs a modified LFU with decay eviction algorithm:

LFU (Least Frequently Used): Tracks the frequency of accesses to each TLB entry.
Decay Mechanism: Periodically reduces the frequency counters to prevent stale entries from persisting indefinitely.
This approach offers a practical trade-off between complexity and performance, closely approximating LRU or pseudo-LRU behavior.
Page Table Details

Each page table level contains:

- 512 entries (9-bit index per level, 2**9 = 512), and
- 8 bytes per entry, meaning each table occupies a single 4 KiB page.

Page tables are dynamically allocated, ensuring memory efficiency by only creating entries for active virtual address regions.


## Translation Flow

This model assumes translation occurs after a cache miss but before accessing main memory. The sequence is as follows:

1. TLB Lookup:
	a. Check the 3 TLBs in parallel (4 KiB, 2 MiB, or 1 GiB).
	b. If a hit occurs, return the corresponding physical address immediately.
	c. If a miss occurs, perform an eviction if necessary
2. Page Table Walk (on TLB miss):
	a. Traverse the multi-level page table hierarchy, starting from Level 4.
		i. Use each level's index (9 bits) to locate the next page table.
	b. Continue until reaching the Level 1 table (for 4 KiB pages), Level 2 table (for 2 MiB pages), the Level 3 table (for 1 GiB pages), or resolving the address.

3. Populate TLB:
	a. After resolving the physical address, populate the appropriate TLB.
	b. If the TLB is full, evict an entry using the "LFU with decay" algorithm.

4. Physical Address Computation:
   a. Combine the resolved base physical page frame with the page offset to compute the final address.


## Future Work

Add rudimentary cycle clunting for performance estimation
Variable Page Sizes:
Investigate supporting additional page sizes, such as 512 KiB or 16 GiB, with corresponding address structure updates.
Cache Simulation:
Model a memory cache to simulate pre-fetching and caching effects on translation performance. Also, model page tables being stored in the cache
Performance Metrics:
Add tracking for metrics such as TLB hit/miss rates, page table walk latency, and memory access pattern