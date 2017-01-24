#include "cpu.h"
#include <stdio.h>
#include <sys/mman.h>	
#include <stdlib.h>	
#include <decode/decode.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define SHRINK_FACTOR (8/1)

int load_from_rom(char* path, void* mem)
{
	int fd = open(path, O_RDONLY);

	if (fd == -1)
	{
		printf("Can not open file\n");
		abort();
	}

	long long size = lseek(fd, 0, SEEK_END);
	void* mapped_area = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    uint8_t* buf = malloc(size/SHRINK_FACTOR);

	int i = 0, j = 0, res = 0;

	for (i = 0; i < size / 8; i++)
	{
 		res = 0;	
 		
 		for (j = 0; j < 8; j++)
  			res += (((uint8_t*)mapped_area)[i * 8 + j] - '0') * ((1 << (8 - j - 1)));

		buf[i] = res;
	}

    memcpy((uint8_t*)mem + 0x0, buf, size/SHRINK_FACTOR);

	return 0;
}

int vcpu_init(vcpu_t* vcpu, void* mem, char* path_to_rom)
{
	vcpu->mem_entry = mem;
	vcpu->regs = (uint16_t*)((uint8_t*)mem + MEM_SPACE_SIZE);
	
	vcpu->regs[PC] = 0x0;
	vcpu->regs[SP] = SP_DISP;

	vcpu->vram = (uint16_t*)((uint8_t*)mem + VRAM_BASE_ADDR); 
	vcpu->psw = (uint16_t*)((uint8_t*)mem + PS_ADDR);
	vcpu->br_points = (uint8_t*)((uint8_t*)mem + BR_POINT_ADDR);
	vcpu->kb_stat_reg = (uint16_t*)((uint8_t*)mem + KB_STAT_REG);
	vcpu->kb_data_reg = (uint16_t*)((uint8_t*)mem + KB_DATA_REG);
	vcpu->out_stat_reg = (uint16_t*)((uint8_t*)mem + OUT_STAT_REG);
	vcpu->out_data_reg = (uint16_t*)((uint8_t*)mem + OUT_DATA_REG);

	INIT_OUT_STAT_REG(vcpu);
	KB_INTERRUPT_ON(vcpu);	
	PS_INIT(vcpu);

	RESET_RUN_FLAG(vcpu);
	RESET_STOP_FLAG(vcpu);
	RESET_STEP_FLAG(vcpu);

	load_from_rom(path_to_rom, vcpu->mem_entry);

	return 0;
}

int vcpu_restore(vcpu_t* vcpu, char* path_to_rom)
{
//	printf("Restore vcpu \n");
	memset((uint8_t*)vcpu->regs, 0, sizeof(uint16_t) * 8);
	memset(vcpu->mem_entry, 0, MEM_SPACE_SIZE);

	vcpu->regs[PC] = 0x0;
	vcpu->regs[SP] = SP_DISP;

	RESET_RUN_FLAG(vcpu);
	RESET_STOP_FLAG(vcpu);
	RESET_STEP_FLAG(vcpu);
	
	INIT_OUT_STAT_REG(vcpu);
	KB_INTERRUPT_ON(vcpu);	
	PS_INIT(vcpu);

	load_from_rom(path_to_rom, vcpu->mem_entry);	

	return 0;
}

int vcpu_deinit (vcpu_t* vcpu)
{
	return munmap(vcpu->mem_entry, ADDR_SPACE_SIZE); 
}

int emu_init(vcpu_t* vcpu, char* path_to_rom)
{
	void* mem_entry = mmap(NULL, ADDR_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); 

	if (mem_entry == (void*)-1)
	{
		printf("Mem allocation failed (mmap)\n");
		abort();
	}

	vcpu_init(vcpu, mem_entry, path_to_rom);

	return 0;
}

uint16_t fetch_instr(vcpu_t* vcpu)
{
//	printf(" ------- \n");
//	printf("Instruction fetch \n");
	uint16_t op = 0;
	memcpy(&op, (uint8_t*)(vcpu->mem_entry) + vcpu->regs[PC], sizeof(uint8_t) * 2);
//	printf("op: 0x%x\n", op);
	vcpu->regs[PC] += 2;

	return op;
}

void vcpu_print(vcpu_t* vcpu)
{
	printf("REG 0: %o REG 1: %o REG 2: %o REG 3: %o REG 4: %o REG 5: %o REG 6: %o REG 7: %o\n",  
		vcpu->regs[REG0], vcpu->regs[REG1], vcpu->regs[REG2], vcpu->regs[REG3], 
		vcpu->regs[REG4], vcpu->regs[REG5], vcpu->regs[SP], vcpu->regs[PC]);

	printf("flags: n:%d z:%d v:%d c:%d\n", GET_N(vcpu), GET_Z(vcpu), GET_V(vcpu), GET_C(vcpu));

}

int is_break(vcpu_t* vcpu, uint16_t address) 
{
	int set = address / 8;
	int disp = address % 8;
	uint8_t mask = vcpu->br_points[set];
	
	mask &= (0x01 << (8 - disp));
	mask = (mask >> (8 - disp)); //

	if (mask)
	{	
		printf("Breakpoint was set at address: 0x%x\n", address);
	}

	return mask;
}

emu_stat_t cpu_exec(vcpu_t* vcpu)
{
	instr_t op = fetch_instr(vcpu);

	instr_desc_t* instr;
	lookup_table(op, &instr);

	if (instr == NULL)				
		return EMU_UNDEFINED;	

	emu_stat_t st = instr->execute(vcpu, instr, op, instr->mode);

	return st;
}

int is_kb_interrupt_rec(vcpu_t* vcpu)
{
	if (IS_KB_DATA_AVAILABLE(vcpu) && IS_INTERRUPT_ON(vcpu))
		return 1;
	else
		return 0;
}

void kb_interrupt_handler(vcpu_t* vcpu)
{
	KB_INTERRUPT_OFF(vcpu);	

	*(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[SP]) =  (*(vcpu->psw)).reg_val; 
	vcpu->regs[SP] -= 2;
	*(uint16_t*)((uint8_t*)vcpu->mem_entry + vcpu->regs[SP]) =	vcpu->regs[PC];
	vcpu->regs[SP] -= 2;

	(*(vcpu->psw)).reg_val = *(uint16_t*)((uint8_t*)vcpu->mem_entry + KB_INTERRUPT_VEC + 2);

	SET_PC(vcpu, KB_INTERRUPT_VEC);
//	printf("After receiving kb interrupt PC is set to: %o\n", vcpu->regs[PC]);
	RESET_KB_STAT_REG(vcpu);
	KB_INTERRUPT_ON(vcpu);
}



