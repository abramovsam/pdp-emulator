//#include "emu_helper.h"
#include "cpu/cpu.h"
#include "decode/decode.h"
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>

#define GET_BYTE_OPERAND(addr)	*(uint8_t*)addr
#define GET_WORD_OPERAND(addr)	*(uint16_t*)addr

uint16_t fetch_word_from_mem(vcpu_t* vcpu)
{
	uint16_t operand = *(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[PC]);
	vcpu->regs[PC] += 2;
	return operand;
}

uint16_t get_reg(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode)
{
	if (isa_mode)
        return *((uint8_t*)&((vcpu->regs)[disp]));
	else
		return vcpu->regs[disp];

	return 0;
}

uint16_t get_reg_def(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);	
	else
		return GET_WORD_OPERAND(*address);	
	
	return 0;
}

uint16_t get_autoinc(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	uint16_t operand = 0;	
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);
	
	if (isa_mode)
	{
		operand = GET_BYTE_OPERAND(*address);
		vcpu->regs[disp] +=1;			
	}
	else
	{
		operand = GET_WORD_OPERAND(*address);		
		vcpu->regs[disp] +=2;		
	}

	return operand;
}


uint16_t get_autodec(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	uint16_t operand = 0;	
	uint16_t reg_val = 0;

	if (isa_mode)
	{
		vcpu->regs[disp] -= 1;
		*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);
		operand = GET_BYTE_OPERAND(*address);
	}
	else
	{
		vcpu->regs[disp] -=2;
		*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);
		operand = GET_WORD_OPERAND(*address);	
	}

	return operand;	
}

uint16_t get_autoinc_def(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	uint16_t operand = 0;															
	uint16_t operand_addr = *(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);		
	*address = ((uint8_t*)vcpu->mem_entry + operand_addr);	

	if (isa_mode) 
		operand = GET_BYTE_OPERAND(*address);
	else
		operand = GET_WORD_OPERAND(*address);
		
	vcpu->regs[disp] += 2;

	return operand;
}

uint16_t get_autodec_def(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	vcpu->regs[disp] -= 2;															
	uint16_t operand_addr = *(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[disp]);	
	*address = ((uint8_t*)vcpu->mem_entry + operand_addr);
	
	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);
}

uint16_t get_index(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	uint16_t post_word = fetch_word_from_mem(vcpu);
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[disp] + post_word);

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);	
}

uint16_t get_index_def(vcpu_t* vcpu, uint16_t disp, uint16_t isa_mode, uint8_t** address)
{
	uint16_t post_word = fetch_word_from_mem(vcpu);
	uint16_t operand_addr = *(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[disp] + post_word);	
	*address = ((uint8_t*)vcpu->mem_entry + operand_addr);

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);	 
	else
		return GET_WORD_OPERAND(*address);
}

uint16_t get_imm_pc(vcpu_t* vcpu)
{
	return fetch_word_from_mem(vcpu);
}

uint16_t get_abs_pc(vcpu_t* vcpu, uint16_t isa_mode, uint8_t** address)
{
	uint16_t post_word = fetch_word_from_mem(vcpu);
	*address = ((uint8_t*)vcpu->mem_entry + post_word);

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);	
}

uint16_t get_rel_pc(vcpu_t* vcpu, uint16_t isa_mode, uint8_t** address)
{
	uint16_t post_word = fetch_word_from_mem(vcpu);
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[PC] + post_word);	

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);
}

uint16_t get_rel_def_pc(vcpu_t* vcpu, uint16_t isa_mode, uint8_t** address)
{
	uint16_t post_word = fetch_word_from_mem(vcpu);
	uint16_t operand_addr = *((uint8_t*)vcpu->mem_entry + vcpu->regs[PC] + post_word);
	*address = ((uint8_t*)vcpu->mem_entry + operand_addr);	

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);
}

uint16_t get_autoinc_sp(vcpu_t* vcpu, uint16_t isa_mode, uint8_t** address)
{
	uint16_t operand = 0;	
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[SP]);
	
	if (isa_mode)
		operand = GET_BYTE_OPERAND(*address);									
	else
		operand = GET_WORD_OPERAND(*address);					

	vcpu->regs[SP] +=2;
	
	return operand;
}

uint16_t get_autodec_sp(vcpu_t* vcpu, uint16_t isa_mode, uint8_t** address)
{	
	vcpu->regs[SP] -=2;
	*address = ((uint8_t*)vcpu->mem_entry + vcpu->regs[SP]);

	if (isa_mode)
		return GET_BYTE_OPERAND(*address);
	else
		return GET_WORD_OPERAND(*address);
}

uint16_t fetch_op_general(vcpu_t* vcpu, uint16_t disp, uint16_t mode, uint16_t isa_mode, uint8_t** addr)  
{
	uint16_t operand = 0;

	switch (mode)
	{
		case REG_ADDR:
			return get_reg(vcpu, disp, isa_mode);
			
		case REG_DEF_ADDR:
			return get_reg_def(vcpu, disp, isa_mode, addr);

		case AUTOINC_ADDR:
			return get_autoinc(vcpu, disp, isa_mode, addr);

		case AUTOINC_DEF_ADDR:
			return get_autoinc_def(vcpu, disp, isa_mode, addr);

		case AUTODEC_ADDR:
			return get_autodec(vcpu, disp, isa_mode, addr);

		case AUTODEC_DEF_ADDR:
			return get_autodec_def(vcpu, disp, isa_mode, addr);

		case INDEX_ADDR:
			return get_index(vcpu, disp, isa_mode, addr);

		case INDEX_DEF_ADDR:
			return get_index_def(vcpu, disp, isa_mode, addr);
		
		case IMM_PC:
			return get_imm_pc(vcpu);

		case ABS_PC:
			return get_abs_pc(vcpu, isa_mode, addr);

		case REL_PC:
			return get_rel_pc(vcpu, isa_mode ,addr);

		case REL_DEF_PC:
			return get_rel_def_pc(vcpu, isa_mode, addr);		

		case SP_DEF:
			return get_reg_def(vcpu, disp, isa_mode, addr);
		
		case SP_AUTOINC:
			return	get_autoinc_sp(vcpu, isa_mode, addr);

		case SP_AUTOINC_DEF:
			return get_autoinc_def(vcpu, disp, isa_mode, addr);

		case SP_AUTODEC:
			return get_autodec_sp(vcpu, isa_mode, addr);

		case SP_INDEX:
			return get_index(vcpu, disp, isa_mode, addr);

		case SP_INDEX_DEF:
			return get_index_def(vcpu, disp, isa_mode, addr);

		default:
			printf("Operands' fetch failed\n");
			abort();
	}	

	return 0;
}

void set_reg_op(vcpu_t* vcpu, uint16_t disp, uint16_t val, uint16_t isa_mode)
{
	if (isa_mode)
        *((uint8_t*)&((vcpu->regs)[disp])) = val;
	else
		vcpu->regs[disp] = val;	
}

void set_mem_op(vcpu_t* vcpu, uint8_t* addr, uint16_t* val, uint16_t isa_mode)
{
	if (isa_mode)
		*(uint8_t*)addr = *(uint8_t*)val;
	else
		*(uint16_t*)addr = *(uint16_t*)val;
}

void writeback_dst_ops(vcpu_t* vcpu, uint16_t val, uint16_t disp, uint16_t mode, uint16_t isa_mode, uint8_t* addr)
{
	switch (mode)
	{
		case REG_ADDR:
			set_reg_op(vcpu, disp, val, isa_mode);
			break;

		case REG_DEF_ADDR:
		case AUTOINC_ADDR:
		case AUTOINC_DEF_ADDR:
		case AUTODEC_ADDR:
		case AUTODEC_DEF_ADDR:
		case INDEX_ADDR:
		case INDEX_DEF_ADDR:
		case ABS_PC:
		case REL_PC:
		case REL_DEF_PC:
		case SP_DEF:
		case SP_AUTOINC:
		case SP_AUTOINC_DEF:
		case SP_AUTODEC:
		case SP_INDEX:
		case SP_INDEX_DEF:
			set_mem_op(vcpu, addr, &val, isa_mode);
			break;

		default:
			printf("Operands' fetch failed\n");
			abort();
	}	
}
