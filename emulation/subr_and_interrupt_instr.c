#include "emulation.h"
#include "cpu/cpu.h"
#include "decode/decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "emu_helper.h"

emu_stat_t rti_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)	
{
	uint16_t sp = 0;
	uint8_t* sp_addr;

	sp = fetch_op_general(vcpu, 0b110, AUTOINC_ADDR, 0, &sp_addr);	// FIXME: Need to implement separate function 
	SET_PC(vcpu, sp);												// special for this calback to fetch in same time pc and psw 
																	// from the stack because now pipelining for this insruction 
																	// is not possible
	sp = fetch_op_general(vcpu, 0b110, AUTOINC_ADDR, 0, &sp_addr);
	SET_PSW(vcpu, sp);		

	return EMU_SUCCESS;
}

emu_stat_t jsr_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t tmp = 0;
	uint16_t dst_disp = 0;
	uint16_t dst_mode = 0;
	uint16_t dst = 0;
	uint16_t reg_disp = 0;

	ADDRESS_MODE_LOW(op, dst_mode);
	GET_DST(op, dst_disp);	

	uint8_t* dst_addr;
//	dst = fetch_op_general(vcpu, dst_disp, dst_mode, 0, &dst_addr);	
	fetch_op_general(vcpu, dst_disp, dst_mode, 0, &dst_addr);
	dst_addr -= (uint8_t*)vcpu->mem_entry;

//	tmp = dst;	
	tmp = dst_addr;

	GET_REG_JSR(op, reg_disp);	

	uint16_t sp = 0;
	uint8_t* sp_addr;

	sp = fetch_op_general(vcpu, SP, SP_AUTODEC, 0, &sp_addr);
	writeback_dst_ops(vcpu, vcpu->regs[reg_disp], SP, SP_AUTODEC, 0, sp_addr);

	writeback_dst_ops(vcpu, vcpu->regs[PC], reg_disp, REG_ADDR, 0, NULL);	

	vcpu->regs[PC] = tmp;

	return EMU_SUCCESS;
}

emu_stat_t rts_emu(vcpu_t* vcpu, struct instr_desc *instr, instr_t op, instr_mode_t mode)
{
	uint16_t reg_disp = 0;
	uint16_t reg = 0;

	GET_DST(op, reg_disp);	

	reg = vcpu->regs[reg_disp];
	vcpu->regs[PC] = reg;

	uint16_t sp = 0;
	uint8_t* sp_addr; 

	sp = fetch_op_general(vcpu, SP, SP_AUTOINC, 0, &sp_addr);
	
	writeback_dst_ops(vcpu, sp, reg_disp, REG_ADDR, 0, NULL);	

	return EMU_SUCCESS;
}