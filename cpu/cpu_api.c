#include "cpu_api.h"

#include <stdio.h>
#include <sys/mman.h>	// for mmap
#include <stdlib.h>	// for abort
#include <decode/decode.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int emulator_initialized;
int emulator_halted;

void throw_kb_interrupt(vcpu_t* vcpu, uint8_t data)
{
	SET_KB_DATA_REG(vcpu, data);	
	printf("Keyboard interrupt was received\n");	
	SET_KB_STAT_REG(vcpu);
}

void run_emulator(vcpu_t* vcpu)
{
	if (!(vcpu->is_running))
	{	
		vcpu->is_running = 1;
		vcpu->stop_flag = 0;
	}
	else
	{	
		vcpu->stop_flag = 0;
	}
}

void reset_emulator(vcpu_t* vcpu)
{
	RESET_RUN_FLAG(vcpu);
	RESET_STEP_FLAG(vcpu);
	RESET_STOP_FLAG(vcpu);	
}

int cpu_emulation(vcpu_t** vcpu, char* path)
{
	emu_init(*vcpu, path);	
	vcpu_print(*vcpu);

	emulator_initialized = 1;
	emulator_halted = 0;

	printf("emulator initialized\n");	

	while (1)
	{
		if (emulator_halted)
			break;

		if ((*vcpu)->is_running || (*vcpu)->stop_flag)
		{
			emu_stat_t exec_st = EMU_SUCCESS;
			
			while (1)
			{
				if (!((*vcpu)->is_running) && !(*vcpu)->stop_flag)
					break; 
				
				if (emulator_halted)
					break; 

				if (is_break(*vcpu, (*vcpu)->regs[PC]))
					SET_STOP_FLAG((*vcpu));

				if (!(*vcpu)->stop_flag || (*vcpu)->step_flag)
				{
					if ((*vcpu)->stop_flag)
						RESET_STEP_FLAG((*vcpu));

					if (!((*vcpu)->is_running) && !(*vcpu)->stop_flag)
						break;

					exec_st = cpu_exec((*vcpu));			
					vcpu_print(*vcpu);

					if (exec_st == EMU_END)
						break;
				}
			}			

			if (emulator_halted)
				break;

			if (!((*vcpu)->is_running))
				vcpu_restore(*vcpu, path);				

			if (exec_st == EMU_END || exec_st == EMU_UNDEFINED)
				vcpu_restore(*vcpu, path); 
		}	
	}

	vcpu_deinit (vcpu);

	return 0;
}

void halt_emulator(vcpu_t* vcpu)
{
	vcpu->is_running = 0;
	emulator_halted = 1;
}

void stop_emulator(vcpu_t* vcpu)
{
	vcpu->stop_flag = 1;
}

void step_emulator(vcpu_t* vcpu)
{
	vcpu->step_flag = 1;
}

void set_breakpoint(vcpu_t* vcpu, uint16_t address)		// FIXME: Need to check
{
	int set = address / 8;
	int disp = address % 8;
	vcpu->br_points[set] |= (0x01 << (8 - disp)); 
}

void remove_breakpoint(vcpu_t* vcpu, uint16_t address)
{
	int set = address / 8;
	int disp = address % 8;
	vcpu->br_points[set] &= (0xff ^ (0x1 << (8 - disp)));	
}

uint16_t get_register(vcpu_t* vcpu, uint8_t num)
{
	return GET_CPU_REG(vcpu, num);
}

uint8_t get_nflag(vcpu_t* vcpu)
{
	return GET_N(vcpu);
}

uint8_t get_cflag(vcpu_t* vcpu)
{
	return GET_C(vcpu);
}

uint8_t get_vflag(vcpu_t* vcpu)
{
	return GET_V(vcpu);
}

uint8_t get_zflag(vcpu_t* vcpu)
{
	return GET_Z(vcpu);
}

int is_emu_halt()
{
	return emulator_halted;
}

int is_emu_init()
{
	return emulator_initialized;
}

uint16_t get_vram_val(vcpu_t* vcpu, int offset)	// FIXME: Need to handle 
{
	if (offset > VRAM_SIZE)
		return 0xffff; 
	
	return *(vcpu->vram + offset);
}