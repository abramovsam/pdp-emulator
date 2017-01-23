#include "emulation.h"
#include "cpu/cpu.h"
#include "decode/decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "emu_helper.h"


emu_stat_t clr_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);		

	dst = 0;
	
	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);	

	CLEAR_N(vcpu);
	SET_Z(vcpu);
	CLEAR_V(vcpu);
	CLEAR_C(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t com_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);		

	dst = ~(dst);
	
	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);	

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	CLEAR_V(vcpu);
	SET_C(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t inc_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	dst = dst + 1;

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);

	if (dst == 077777)		
		SET_V(vcpu);
	else
		CLEAR_V(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t dec_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0, src_dst = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	
	src_dst = dst;

	dst = dst - 1;

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);

	if (src_dst == 0100000)		
		SET_V(vcpu);
	else
		CLEAR_V(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t neg_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	dst = -dst;

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);

	if (dst == 0100000)		
		SET_V(vcpu);
	else
		CLEAR_V(vcpu);

	if (dst == 0)
		CLEAR_C(vcpu);	
	else	
		SET_C(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t tst_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	CLEAR_V(vcpu);
	CLEAR_C(vcpu);

	return EMU_SUCCESS;
}

emu_stat_t asr_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst_mode = 0;
	int16_t dst = 0;
	
	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);

	uint16_t high_bit = 0;
	uint16_t low_bit = 0;	

	high_bit = dst & (0x8000 >> (mode * 8));
	low_bit = (dst & 0x0001);	
	dst = (dst >> 1) | high_bit;
	high_bit = high_bit >> (15 - (8 * mode));
	
	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);	

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	LOAD_V(vcpu, high_bit ^ low_bit);
	LOAD_C(vcpu, low_bit);

	return EMU_SUCCESS;
}

emu_stat_t asl_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);

	uint16_t high_bit = 0;
	uint16_t neg_bit = 0;

	high_bit = dst & (0x8000 >> (8 * mode));
	dst = dst << 1;
	neg_bit = (dst & (0x8000 >> (8 * mode))) >> (15 - (8 * mode));

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	LOAD_V(vcpu, neg_bit ^ high_bit);
	LOAD_C(vcpu, high_bit);

	return EMU_SUCCESS;
}

emu_stat_t ror_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst = 0, dst_mode = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	uint16_t low_bit = 0;;

	low_bit = dst & 0x0001;

	dst = dst >> 1;
	
	dst = dst | (GET_C(vcpu) << (15 - (8 * mode)));

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	LOAD_V(vcpu, GET_C(vcpu) ^ low_bit);
	LOAD_C(vcpu, low_bit);	

	return EMU_SUCCESS;
}


emu_stat_t rol_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst_mode = 0;
	int16_t dst = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	uint16_t high_bit = 0;
	
	high_bit = dst & (0x8000 >> (8 * mode));

	dst = dst << 1;
	dst = dst | GET_C(vcpu);

	uint16_t neg_bit = 0;
	neg_bit = dst & (0x8000 >> (8 * mode)); 

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);
	
	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	LOAD_V(vcpu, neg_bit ^ high_bit);
	LOAD_C(vcpu, high_bit);

	return EMU_SUCCESS;
}


emu_stat_t swab_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, dst_mode = 0;
	int16_t dst = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);

	uint8_t* dst_addr;

	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	uint8_t low_byte = dst; 
	uint8_t high_byte = dst >> 8;
	uint16_t res = 0;

	dst = low_byte;
	dst = (dst << 8) | high_byte;	

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, dst, 0);
	LOAD_Z(vcpu, dst);
	CLEAR_V(vcpu);
	CLEAR_C(vcpu);

	return EMU_SUCCESS;
}



