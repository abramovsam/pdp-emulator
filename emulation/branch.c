#include "emulation.h"
#include "cpu/cpu.h"
#include "decode/decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "emu_helper.h"


/* FIXME: Need to implement some branches with single template function */ 

emu_stat_t br_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	

	GET_OFFSET(op, offset);
	GET_PC(vcpu, pc);

	pc = pc + offset * 2;

	SET_PC(vcpu, pc);

	return EMU_SUCCESS;
}


emu_stat_t bne_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;				
	uint8_t bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_Z(vcpu, bit_z);

	if (!bit_z)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}

	return EMU_SUCCESS;
}

emu_stat_t beq_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;			
	uint8_t bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_Z(vcpu, bit_z);

	if (bit_z)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}

	return EMU_SUCCESS;
}

emu_stat_t bpl_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;		
	uint8_t bit_n = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);

	if (!bit_n)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bmi_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;		
	uint8_t bit_n = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);

	if (bit_n)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bvc_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_v = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_V(vcpu, bit_v);

	if (!bit_v)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bvs_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_v = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_V(vcpu, bit_v);

	if (bit_v)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bcc_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);

	if (!bit_c)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}


emu_stat_t bcs_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);

	if (bit_c)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bge_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_n = 0, bit_v = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);
	GET_V(vcpu, bit_v);

	if (!(bit_n ^ bit_v))
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t blt_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_n = 0, bit_v = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);
	GET_V(vcpu, bit_v);

	if (bit_n ^ bit_v)
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bgt_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_n = 0, bit_v = 0, bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);
	GET_V(vcpu, bit_v);
	GET_Z(vcpu, bit_z);

	if (!(bit_z ^ (bit_n ^ bit_v))) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t ble_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_n = 0, bit_v = 0, bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_N(vcpu, bit_n);
	GET_V(vcpu, bit_v);
	GET_Z(vcpu, bit_z);

	if (bit_z ^ (bit_n ^ bit_v)) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t bhi_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0, bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);
	GET_Z(vcpu, bit_z);

	if (bit_c == 0 && bit_z == 0) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t blos_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0, bit_z = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);
	GET_Z(vcpu, bit_z);

	if (bit_c ^ bit_z) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}


emu_stat_t bhis_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);

	if (!bit_c) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}

emu_stat_t blo_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t pc = 0;
	int8_t offset = 0;	
	uint8_t bit_c = 0;

	GET_OFFSET(op, offset);
	
	GET_PC(vcpu, pc);
	GET_C(vcpu, bit_c);

	if (bit_c) 
	{
		pc = pc + offset * 2;
		SET_PC(vcpu, pc);
	}	

	return EMU_SUCCESS;
}


emu_stat_t jmp_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;
	
	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	if (dst_mode == 0)
		return EMU_UNDEFINED;	

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);
	
	uint16_t pc = 0;
	GET_PC(vcpu, pc);

	pc = dst;
	
	SET_PC(vcpu, pc);

	return EMU_SUCCESS;
}