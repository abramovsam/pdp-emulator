#include "decode.h"
#include "emulation/emulation.h"
#include "cpu/cpu.h"
#include <stdlib.h>	
#include <stdio.h>

extern instr_desc_t instr_table[];

int table_cmp(uint16_t mask, uint16_t value, uint16_t op)
{
	if ((mask & op) == value)
		return 1;

	return 0;
}

void lookup_table(uint16_t op, instr_desc_t** instr_desc)
{
	*instr_desc = instr_table;

	do {

		if (table_cmp((*instr_desc)->mask, (*instr_desc)->value, op))
		{
//			printf("Instruction: %s\n", (*instr_desc)->name);
			return;
		}

	} while ((++(*instr_desc))->mask);

	*instr_desc = NULL;
	
//	printf("Unknown instruction: 0x%x\n", op);
}



