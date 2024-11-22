/**
 * @file main.c - contains the place that memory is allocated and setup is done
 */


// Source files
#include "translation.h"
#include "page_table.h"
#include "page_table_api.h"
#include "tlb.h"
#include "util.h"

// Test files
#include "test_utils.h"
#include "simple_mapping.h"


void print_test_results(test_counter){
	for (uint8_t i=0; i<64; i++){
		printf("Result of test %d was %d\n", i, (((1<<i) & test_counter) >> i) );
	}
}


int main(){

	ptw_sim_ctx_t sim_ctx = {0};

	uint64_t result = 0;

	uint8_t test_counter = 0;
	printf("Test %d is simple mapping test\n", test_counter);
	result |= (run_simple_mapping_test(&ctx) << test_counter);
	test_counter++;


	print_test_results(result);

	return (result == 0);
}


