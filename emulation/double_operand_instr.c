#include "emulation.h"
#include "cpu/cpu.h"
#include "decode/decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "emu_helper.h"


emu_stat_t mov_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, src_disp = 0;
	uint16_t dst = 0, src = 0;
	uint16_t dst_mode = 0, src_mode = 0;	

	ADDRESS_MODE_LOW(op, dst_mode);
	ADDRESS_MODE_HIGH(op, src_mode);

	GET_SOURCE(op, src_disp);
	GET_DST(op, dst_disp);

	printf("dst mode: %o\n", dst_mode);
	printf("src mode: %o\n", src_mode);

	printf("dst disp: %d\n", dst_disp);
	printf("src disp: %d\n", src_disp);
	printf("mode: %o\n", mode);

	uint8_t* src_addr;
	src = fetch_op_general(vcpu, src_disp, src_mode, mode, &src_addr);

	uint8_t* dst_addr;
	fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);

	dst = src;

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);

	LOAD_N(vcpu, src, mode);
	LOAD_Z(vcpu, src);
	CLEAR_V(vcpu);	

	return EMU_SUCCESS;

}

emu_stat_t add_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, src_disp = 0;
	uint16_t dst = 0, src = 0, src_dst = 0;
	uint16_t dst_mode = 0, src_mode = 0;	
	
	ADDRESS_MODE_LOW(op, dst_mode);
	ADDRESS_MODE_HIGH(op, src_mode);

	GET_SOURCE(op, src_disp);
	GET_DST(op, dst_disp);

	uint8_t* src_addr;
	uint8_t* dst_addr;

	src = fetch_op_general(vcpu, src_disp, src_mode, mode, &src_addr);
	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);		

	src_dst = dst;

	dst = dst + src;
	
	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);	

	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	ADD_OVF_HANDLER(vcpu, src, src_dst, dst, mode);
	ADD_CARRY_HANDLER(vcpu, src, src_dst, dst, mode);
	
	return EMU_SUCCESS;
}

emu_stat_t sub_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t dst_disp = 0, src_disp = 0;
	uint16_t dst = 0, src = 0, src_dst = 0;
	uint16_t dst_mode = 0, src_mode = 0;	

	ADDRESS_MODE_LOW(op, dst_mode);
	ADDRESS_MODE_HIGH(op, src_mode);

	GET_SOURCE(op, src_disp);
	GET_DST(op, dst_disp);

	uint8_t* src_addr;
	uint8_t* dst_addr;

	src = fetch_op_general(vcpu, src_disp, src_mode, mode, &src_addr);
	dst = fetch_op_general(vcpu, dst_disp, dst_mode, mode, &dst_addr);	

	src_dst = dst;

	dst = dst + (~(src)) + 1;

	writeback_dst_ops(vcpu, dst, dst_disp, dst_mode, mode, dst_addr);
	
	LOAD_N(vcpu, dst, mode);
	LOAD_Z(vcpu, dst);
	SUB_OVF_HANDLER(vcpu, src, src_dst, dst, mode);
	SUB_CARRY_HANDLER(vcpu, src, src_dst, dst, mode);

	return EMU_SUCCESS;
}


