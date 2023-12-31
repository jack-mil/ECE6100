#ifndef _TRACE_H
#define _TRACE_H

#include <inttypes.h>
#include <iostream>
#include <stdio.h>

typedef enum Op_Type_Enum {
	OP_ALU,   // ALU(ADD/ SUB/ MUL/ DIV) operation
	OP_LD,    // load operation
	OP_ST,    // store operation
	OP_CBR,   // Conditional Branch
	OP_OTHER, // Other Ops
	NUM_OP_TYPE
} Op_Type;

/* Data structure for Trace Record */
typedef struct Trace_Rec_Struct {
	uint64_t inst_addr;             // instruction address
	uint8_t op_type;                // op-type
	uint8_t dest;                   // Destination
	uint8_t dest_needed;            //
	uint8_t src1_reg;               // Source Register 1
	uint8_t src2_reg;               // Source Register 2
	uint8_t src1_needed;            // Source Register 1 needed by this instruction
	uint8_t src2_needed;            // Source Register 2 needed to this instruction
	uint8_t cc_read;                // Conditional Code Read
	uint8_t cc_write;               // Conditional Code Write
	uint64_t mem_addr;              // Load / Store Memory Address
	uint8_t mem_write;              // Write
	uint8_t mem_read;               // Read
	uint8_t br_dir;                 // Branch Direction Taken / Not Taken
	uint64_t br_target;             // Target Address of Branch
	uint8_t is_exception;           // Will this instruction raise an exception
	uint8_t exception_handler_cost; // Number of cycles an exception handler takes
} Trace_Rec;

/* Data structure for Inst Info Record */
typedef struct Inst_Info_Struct {
	uint64_t inst_num; // sequence number for instructions
	uint8_t op_type;   // op-type
	int dest_reg;      // Destination (-1 if not needed)
	int src1_reg;      // Source 1 reg (-1 if not needed)
	int src2_reg;      // Source 1 reg (-1 if not needed)

	// needed by rename (issue stage)
	int dr_tag;      // after rename (this is same as ROB id)
	int src1_tag;    // -1 if not needed or ready
	int src2_tag;    // -1 if not needed or ready
	bool src1_ready; // true if value ready or not needed
	bool src2_ready; // true if value ready or not needed

	bool is_exception;              // true if instruction raises an exception
	int32_t exception_handler_cost; // Number of cycles an exception handler takes

	// needed for multi-cycle execution
	int exe_wait_cycles; // we will use this for cache lookup
	uint64_t mem_addr; // Load / Store Memory Address

} Inst_Info;

#endif
