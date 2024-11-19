#ifndef PAGE_TABLE_API_H


typedef enum page_size {
	FOUR_K = 0;
	TWO_M = 1;
	ONE_G = 3;
	SIZE_MAX = 4;
} page_size_t;


uintptr_t translate(uintptr_t va, uint32_t pid);

int invalidate_tlb_by_page(uintptr_t va, page_size_t page_size);

#endif