#include "decode.h"
#include "cpu/cpu.h"
#include <stdlib.h>
#include "emulation/emulation.h"


instr_desc_t instr_table[] = 
{
	{0xf000, 0x1000, "mov", mov_emu, WORD_MODE},
	{0xf000, 0x9000, "movb", mov_emu, BYTE_MODE},
	{0xf000, 0x6000, "add", add_emu, WORD_MODE},	
	{0xf000, 0xe000, "sub", sub_emu, WORD_MODE},
	{0xffc0, 0x0a00, "clr", clr_emu, WORD_MODE},
	{0xffc0, 0x8a00, "clrb", clr_emu, BYTE_MODE},
	{0xffc0, 0x0A40, "com", com_emu, WORD_MODE},
	{0xffc0, 0x8A40, "comb", com_emu, BYTE_MODE},
	{0xffc0, 0x0A80, "inc", inc_emu, WORD_MODE},
	{0xffc0, 0x8A80, "incb", inc_emu, BYTE_MODE},
	{0xffc0, 0x0AC0, "dec", dec_emu, WORD_MODE},
	{0xffc0, 0x8AC0, "decb", dec_emu, BYTE_MODE},
	{0xffc0, 0x0B00, "neg", neg_emu, WORD_MODE},
	{0xffc0, 0x8B00, "negb", neg_emu, BYTE_MODE},	
	{0xffc0, 0x0BC0, "tst", tst_emu, WORD_MODE},
	{0xffc0, 0x8BC0, "tstb", tst_emu, BYTE_MODE},	
	{0xffc0, 0x0C80, "asr", asr_emu, WORD_MODE},
	{0xffc0, 0x8C80, "asrb", asr_emu, BYTE_MODE},	
	{0xffc0, 0x0CC0, "asl", asl_emu, WORD_MODE},
	{0xffc0, 0x8CC0, "aslb", asl_emu, BYTE_MODE},
	{0xffc0, 0x0C00, "ror", ror_emu, WORD_MODE},
	{0xffc0, 0x8C00, "rorb", ror_emu, BYTE_MODE},
	{0xffc0, 0x0C40, "rol", rol_emu, WORD_MODE},
	{0xffc0, 0x8C40, "rolb", rol_emu, BYTE_MODE},
	{0xffc0, 0x00C0, "swab", swab_emu, WORD_MODE},
	{0xff00, 0x0100, "br", br_emu, WORD_MODE},
	{0xff00, 0x0200, "bne", bne_emu, WORD_MODE},
	{0xff00, 0x0300, "beq", beq_emu, WORD_MODE},
	{0xff00, 0x8000, "bpl", bpl_emu, WORD_MODE},
	{0xff00, 0x8100, "bmi", bmi_emu, WORD_MODE},
	{0xff00, 0x8400, "bvc", bvc_emu, WORD_MODE},
	{0xff00, 0x8500, "bvs", bvs_emu, WORD_MODE},
	{0xff00, 0x8600, "bcc", bcc_emu, WORD_MODE},
	{0xff00, 0x8700, "bcs", bcs_emu, WORD_MODE},
	{0xff00, 0x0400, "bge", bge_emu, WORD_MODE},
	{0xff00, 0x0500, "blt", blt_emu, WORD_MODE},
	{0xff00, 0x0600, "bgt", bgt_emu, WORD_MODE},
	{0xff00, 0x0700, "ble", ble_emu, WORD_MODE},
	{0xff00, 0x8200, "bhi", bhi_emu, WORD_MODE},
	{0xff00, 0x8300, "blos", blos_emu, WORD_MODE},
	{0xff00, 0x8600, "bhis", bhis_emu, WORD_MODE},
	{0xff00, 0x8700, "blo", blo_emu, WORD_MODE},
	{0x7FC0, 0x0040, "jmp", jmp_emu, WORD_MODE},
	{0xffff, 0x0002, "rti", rti_emu, WORD_MODE},
	{0xfe00, 0x0800, "jsr", jsr_emu, WORD_MODE},
	{0xfff8, 0x0080, "rts", rts_emu, WORD_MODE},
	{0xffff, 0x0000, "halt", halt_emu, WORD_MODE},
	{0x0, 0x0, NULL, NULL, 0x0}
};

