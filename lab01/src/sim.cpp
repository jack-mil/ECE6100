/*********************************************************************
 * File         : sim.cpp
 * Author       : Jackson Miller
 * Date         : Aug 22, 2023
 * Description  : Trace Reader and Analyzer for Lab 1 of CS4290/CS6290/ECE4100/ECE6100
 ********************************************************************/

#include "trace.h"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
 * Globals and Statistics
 ********************************************************************/

FILE *tr_file;
Trace_Rec tr_entry;

uint64_t stat_num_inst = 0;
uint64_t stat_num_cycle = 0;
uint64_t stat_optype_dyn[NUM_OP_TYPE] = {0}; // dynamic count for each op type
uint64_t stat_unique_pc = 0;                 // number of unique PCs

/*********************************************************************
 * Functions
 ********************************************************************/

void die_message(const char *msg) {
	printf("Error! %s. Exiting...\n", msg);
	exit(1);
}
void print_stats();
void analyze_trace_record(Trace_Rec *t);

/*********************************************************************
 * Main
 *********************************************************************/

int main(int argc, char *argv[]) {
	char tr_filename[1024];
	char cmd_string[256];

	if ( argc == 1 ) {
		die_message("Must Provide a Trace File");
	}

	strcpy(tr_filename, argv[1]);

	// ------- Open Trace File -------------------------------------------

	sprintf(cmd_string, "gunzip -c %s", tr_filename);
	if ( (tr_file = popen(cmd_string, "r")) == NULL ) {
		printf("Command string is %s\n", cmd_string);
		die_message("Unable to open the trace file with gzip option \n");
	} else {
		printf("Opened file with command: %s \n", cmd_string);
	}

	// ------- Read From Trace File --------------------------------------
	uint8_t bytes_read = 0;

	while ( !feof(tr_file) ) {
		bytes_read = fread(&tr_entry, 1, sizeof(Trace_Rec), tr_file);

		if ( bytes_read < sizeof(Trace_Rec) ) {
			break;
		}

		stat_num_inst++;
		analyze_trace_record(&tr_entry);
	}

	// ------- Print Statistics ------------------------------------------
	print_stats();
	fclose(tr_file);
	return 0;
}

/*********************************************************************
 * Print Statistics --------- DO NOT MODIFY THIS FUNCTION ------------
 *********************************************************************/

void print_stats() {
	char header[256];
	sprintf(header, "LAB1");

	if ( stat_num_inst == 0 ) {
		stat_num_inst = 1; // To avoid division by zero errors
	}
	double cpi = (double)(stat_num_cycle) / (double)(stat_num_inst);

	printf("\n%s_NUM_INST           \t : %10lu", header, stat_num_inst);
	printf("\n%s_NUM_CYCLES         \t : %10lu", header, stat_num_cycle);

	printf("\n%s_CPI                \t : %6.3f", header, cpi);
	printf("\n%s_UNIQUE_PC          \t : %10lu", header, stat_unique_pc);

	printf("\n");

	printf("\n%s_NUM_ALU_OP         \t : %10lu", header, stat_optype_dyn[OP_ALU]);
	printf("\n%s_NUM_LD_OP          \t : %10lu", header, stat_optype_dyn[OP_LD]);
	printf("\n%s_NUM_ST_OP          \t : %10lu", header, stat_optype_dyn[OP_ST]);
	printf("\n%s_NUM_CBR_OP         \t : %10lu", header, stat_optype_dyn[OP_CBR]);
	printf("\n%s_NUM_OTHER_OP       \t : %10lu", header, stat_optype_dyn[OP_OTHER]);

	printf("\n");

	printf("\n%s_PERC_ALU_OP        \t : %6.3f", header, 100.0 * (double)(stat_optype_dyn[OP_ALU]) / (double)(stat_num_inst));
	printf("\n%s_PERC_LD_OP         \t : %6.3f", header, 100.0 * (double)(stat_optype_dyn[OP_LD]) / (double)(stat_num_inst));
	printf("\n%s_PERC_ST_OP         \t : %6.3f", header, 100.0 * (double)(stat_optype_dyn[OP_ST]) / (double)(stat_num_inst));
	printf("\n%s_PERC_CBR_OP        \t : %6.3f", header, 100.0 * (double)(stat_optype_dyn[OP_CBR]) / (double)(stat_num_inst));
	printf("\n%s_PERC_OTHER_OP      \t : %6.3f", header, 100.0 * (double)(stat_optype_dyn[OP_OTHER]) / (double)(stat_num_inst));

	printf("\n\n");
}

/*********************************************************************
 * ------------- DO NOT MODIFY THE CODE ABOVE THIS LINE -------------*
 *********************************************************************/

/*********************************************************************
 * Trace Analysis (Students need to write this function)
 *********************************************************************/

// We have to keep track of every unique pc seen so far
// in theory, every address could be unique, and would require
// array of size 10000000. In practice, no trace file has more than
// 1000 unique PCs.
// A hash set would be more efficient to check for unique elements,
// but is more difficult to implement in pure C.
uint64_t cringe[10000] = {0};

void analyze_trace_record(Trace_Rec *t) {
	// Unpack struct
	uint64_t addr = t->inst_addr;
	Op_Type opcode = (Op_Type)t->opcode;

	// Increase opcode counter
	stat_optype_dyn[opcode]++;

	// Track number of cycles per opcode type
	switch ( opcode ) {
		case OP_CBR:
			stat_num_cycle += 3;
			break;
		case OP_LD:
		case OP_ST:
			stat_num_cycle += 2;
			break;
		case OP_ALU:
		case OP_OTHER:
			stat_num_cycle += 1;
			break;
		default:
			die_message("Unrecognized opcode");
	}

	// Search the list of previous values to determine
	// if this PC is unique. stat_unique_pc serves as the list head.
	bool unique = true;
	for ( size_t i = 0; i < stat_unique_pc; i++ ) {
		if ( addr == cringe[i] ) {
			unique = false;
			break;
		};
	};

	// If this is a unique opcode, update the list and stat counter.
	if ( unique ) {
		cringe[stat_unique_pc] = addr;
		stat_unique_pc++;
	}
}
