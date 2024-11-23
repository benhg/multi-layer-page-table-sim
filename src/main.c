/**
 * @file main.c - contains the place that memory is allocated and setup is done
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Source files
#include "hw_structures.h"
#include "translation.h"
#include "page_table.h"
#include "page_table_api.h"
#include "tlb.h"
#include "util.h"

// Test files
#include "test_utils.h"
#include "simple_mapping.h"


static void print_test_results(uint64_t test_counter, uint64_t test_run){
	for (uint8_t i=0; i<64; i++){
		if ((((1<<i) & test_run) >> i) == 1){
			printf("Result of test %hhu was %llu\n", i, (((1<<i) & test_counter) >> i) );
		}
	}
}


int main(){

	ptw_sim_context_t sim_ctx = {0};

	uint64_t result = 0;
	uint64_t test_run = 0;

	uint8_t test_counter = 0;
	printf("Test %hhu is simple mapping test\n", test_counter);
	test_run |= (1<< test_counter);
	result |= (run_simple_mapping_test(&sim_ctx) << test_counter);
	test_counter++;


	print_test_results(result, test_run);

	return (result == 0);
}


