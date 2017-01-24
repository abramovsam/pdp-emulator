#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define GPR_NUM	8
#define ADDR_SPACE_SIZE		0400000	 // 128K in order to contain registers
#define MEM_SPACE_SIZE		0200000	 // 64K	
#define BR_POINT_ADDR		0200020  // 65536 + 16
#define KB_INTERRUPT_VEC	060	 	 //	48 in dec
#define KB_STAT_REG			0177560
#define KB_DATA_REG			0177562
#define OUT_STAT_REG		0177564
#define OUT_DATA_REG		0177566 
#define VRAM_BASE_ADDR		0100000	// 32 KB
#define VRAM_LIMIT_ADDR		0160000	// 56 KB
#define VRAM_SIZE	(VRAM_LIMIT_ADDR - VRAM_BASE_ADDR)	
#define PS_ADDR			0177776
#define PS_INIT_MASK 	0x0003		//0b0000000000000011	
#define BREAK_BIT_MAP_SIZE_BYTES	1024 	

typedef uint16_t reg16_t;
typedef uint16_t instr_t;

typedef enum reg_id
{
	REG0,
	REG1,
	REG2,
	REG3,
	REG4,
	REG5,
	REG6,
	REG7,

	SP = REG6,
	PC = REG7
} reg_t;

typedef struct ps	
{
	union 
	{
		reg16_t reg_val;
		struct 
		{	
			reg16_t curmode:2;					
			reg16_t prevmode:2;
			reg16_t reserved:4;
			reg16_t priority:3;
			reg16_t t:1;
			reg16_t n:1;
			reg16_t z:1;
			reg16_t v:1;
			reg16_t c:1;
		};
	};
} __attribute__((packed)) ps_t;

typedef struct vcpu
{
	reg16_t* regs;
	ps_t* psw;
	void* mem_entry;
	uint16_t* vram;
	uint8_t stop_flag;
	uint8_t step_flag;
	uint8_t is_running;
	uint8_t* br_points;
	uint16_t* kb_stat_reg;
	uint16_t* kb_data_reg;
	uint16_t* out_stat_reg;
	uint16_t* out_data_reg;

} vcpu_t;

typedef enum emu_stat
{	
	EMU_SUCCESS,
	EMU_END,
	EMU_UNDEFINED
} emu_stat_t;

#define GET_HI_BIT(val, mode)	((val >> (15 - 8 * mode)) & 0x0001)			

#define SET_N(vcpu)	\
	do {	\
		(*(vcpu->psw)).n = 1;	\
	} while (0)

#define CLEAR_N(vcpu)	\
	do {	\
		(*(vcpu->psw)).n = 0;	\
	} while (0)

#define SET_Z(vcpu)	\
	do {	\
		(*(vcpu->psw)).z = 1;	\
	} while (0)

#define CLEAR_Z(vcpu)	\
	do {	\
		(*(vcpu->psw)).z = 0;	\
	} while (0)

#define SET_V(vcpu)	\
	do {	\
		(*(vcpu->psw)).v = 1;	\
	} while (0)

#define CLEAR_V(vcpu)	\
	do {	\
		(*(vcpu->psw)).v = 0;	\
	} while (0)

#define SET_C(vcpu)	\
	do {	\
		(*(vcpu->psw)).c = 1;	\
	} while (0)

#define CLEAR_C(vcpu)	\
	do {	\
		(*(vcpu->psw)).c = 0;	\
	} while (0)

#define LOAD_N(vcpu, val, mode)	\
	do {	\
		(*(vcpu->psw)).n = ((val & (0x8000 >> (mode * 8))) >> (15 - 8 * mode));	\
	} while (0)

#define LOAD_Z(vcpu, val)	\
	do {	\
		if (val == 0)	\
			SET_Z(vcpu);	\
		else	\
			CLEAR_Z(vcpu); \
	} while (0)

#define LOAD_V(vcpu, flag)	\
	do {	\
		(*(vcpu->psw)).v = flag;	\
	} while (0)

#define ADD_OVF_HANDLER(vcpu, src, dst_src, dst, mode)	\
	do {	\
		if ((GET_HI_BIT(src, mode) == 1 && GET_HI_BIT(dst_src, mode) == 1 && GET_HI_BIT(dst, mode) == 0) ||	\
			(GET_HI_BIT(src, mode) == 0 && GET_HI_BIT(dst_src, mode) == 0 && GET_HI_BIT(dst, mode) == 1))	\
			SET_V(vcpu);	\
		else	\
			CLEAR_V(vcpu);	\
	} while (0)

#define SUB_OVF_HANDLER(vcpu, src, dst_src, dst, mode)	\
	do {	\
		if ((GET_HI_BIT(src, mode) == 1 && GET_HI_BIT(dst_src, mode) == 0 && GET_HI_BIT(dst, mode) == 1) ||	\
			(GET_HI_BIT(src, mode) == 0 && GET_HI_BIT(dst_src, mode) == 1 && GET_HI_BIT(dst, mode) == 0))	\
			SET_V(vcpu);	\
		else	\
			CLEAR_V(vcpu);	\
	} while (0)

#define LOAD_C(vcpu, flag)	\
	do {	\
		(*(vcpu->psw)).c = flag;	\
	} while (0)

#define ADD_CARRY_HANDLER(vcpu, src, dst_src, dst, mode)	\
	do {	\
		if ((GET_HI_BIT(src, mode) == 1 || GET_HI_BIT(dst_src, mode) == 1) && GET_HI_BIT(dst, mode) == 0)	\
			SET_C(vcpu);	\
		else	\
			CLEAR_C(vcpu);	\
	} while (0)

#define SUB_CARRY_HANDLER(vcpu, src, dst_src, dst, mode)	\
	do {	\
		if ((GET_HI_BIT(src, mode) == 1 || GET_HI_BIT(dst_src, mode) == 1) && GET_HI_BIT(dst, mode) == 0)	\
			CLEAR_C(vcpu);	\
		else	\
			SET_C(vcpu);	\
	} while (0)

#define INIT_OUT_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->out_stat_reg) = 0200;	\
 	} while (0)

#define INIT_IN_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->kb_stat_reg) = 0200;	\
 	} while (0)

/*
#define SET_OUT_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->out_stat_reg) = 0200;	\
 	} while (0)

#define RESET_OUT_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->out_stat_reg) &= 0x0000;	\
	} while (0)
*/

/* FIXME: WTF ? Why here &= ??? */

#define SET_KB_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->kb_stat_reg) &= 0x0080;	\
 	} while (0)

#define RESET_KB_STAT_REG(vcpu)	\
	do {	\
		*(vcpu->kb_stat_reg) &= 0x0000;	\
	} while (0)  

#define GET_KB_STAT_REG(vcpu)	(*(vcpu->kb_stat_reg) >> 7)

#define SET_KB_DATA_REG(vcpu, data)	\
	do {	\
		*(vcpu->kb_data_reg) = data;	\
	} while (0)

#define GET_KB_DATA_REG(vcpu)	(*(vcpu->kb_data_reg))

#define SET_PSW(vcpu, value)	\
	do {	\
		(*(vcpu->psw)).reg_val = (value);	\
	} while (0)

#define GET_C(vcpu, bit_c)	\
	do {	\
		bit_c = (*(vcpu->psw)).c;	\
	} while (0)

#define GET_Z(vcpu, bit_z)	\
	do {	\
		bit_z = (*(vcpu->psw)).z;	\
	} while (0)

#define GET_N(vcpu, bit_n)	\
	do {	\
		bit_n = (*(vcpu->psw)).n;	\
	} while (0)

#define GET_V(vcpu, bit_v)	\
	do {	\
		bit_v = (*(vcpu->psw)).v;	\
	} while (0)

#define SET_T(vcpu)	\
	do {	\
		(*(vcpu->psw)).reg_val |= 0x0800;	\
	} while (0)

#define CLEAR_T(vcpu)	\
	do {	\
		(*(vcpu->psw)).reg_val &= 0xf7ff;	\
	} while (0)

#define PS_INIT(vcpu) \
	do {	\
		(*(vcpu->psw)).reg_val &= PS_INIT_MASK;	\
	} while (0)

#define SET_PC(vcpu, value)	\
	do {	\
		vcpu->regs[PC] = value;	\
	} while (0)

#define GET_PC(vcpu, pc)	\
	do {	\
		pc = vcpu->regs[PC];	\
	} while (0)

#define SET_STOP_FLAG(vcpu)	\
	do {	\
		vcpu->stop_flag = 1;	\
	} while (0)

#define SET_STEP_FLAG(vcpu)	\
	do {	\
		vcpu->step_flag = 1;	\
	} while (0)

#define RESET_STOP_FLAG(vcpu)	\
	do {	\
		vcpu->stop_flag = 0;	\
	} while (0)

#define RESET_STEP_FLAG(vcpu)	\
	do {	\
		vcpu->step_flag = 0;	\
	} while (0)

#define CLEAR_RUN_FLAG(vcpu)	\
	do {	\
		vcpu->is_running = 0;	\
	} while(0)

#define SET_RUN_FLAG(vcpu)	\
	do {	\
		vcpu->is_running = 1;	\
	} while (0)

#define GET_CPU_REG(vcpu, num)	vcpu->regs[num]


#endif	// CPU_H
