#pragma once

#ifndef LC3JIT_H_INCLUDED
#define LC3JIT_H_INCLUDED

#include "lc3.h"


#ifdef __cplusplus
extern "C" {
#endif
	


struct lc3_image_t
{
	lc3_word origin;		// the origin of where program starts
	lc3_word asmbuf[0xffff];	// memory of the image
	lc3_word *asmptr;			// Program Counter
};
typedef struct lc3_image_t lc3_image;
static int lc3_image_init(lc3_image* img, lc3_word origin) PC_NOEXCEPT_C;

/*
static int lc3_image_init(lc3_image* img, lc3_word origin) PC_NOEXCEPT_C
{
	memset(img->asmbuf, 0, sizeof(img->asmbuf));
	img->origin = origin;
	img->asmptr = img->asmbuf + origin;

	return 0;
}
*/


#define GEN_DR(r)		(NR(r)<<9)				
#define GEN_SR(r)		(NR(r)<<6)
#define GEN_IR(x)		(((x >> 12) == 0xa) ? NR(x) : ((x & 0x1f) | 0x20))
#define GEN_A9(a)		((a) & 0x1ff)






// IMPLEMENTATION


static int asmbuf[0xffff];	// memory of the image
static int *asmptr = asmbuf;	// Program Counter
static int asmrun = 0;

#define _L(x)   { x; return asmptr - asmbuf - 1; }
static int PC() { return asmptr - asmbuf; }
static int DW(int n) { *asmptr++ = n; return PC() - 1; }
static int LL(int label) { return label - PC() - 1; }

static int BR(int addr) { _L(*asmptr++ = 0x0e00 | GEN_A9(LL(addr))); }
static int BRN(int addr) { _L(*asmptr++ = 0x0800 | GEN_A9(LL(addr))); }
static int BRZ(int addr) { _L(*asmptr++ = 0x0400 | GEN_A9(LL(addr))); }
static int BRP(int addr) { _L(*asmptr++ = 0x0200 | GEN_A9(LL(addr))); }
static int BRNZ(int addr) { _L(*asmptr++ = 0x0c00 | GEN_A9(LL(addr))); }
static int BRNP(int addr) { _L(*asmptr++ = 0x0a00 | GEN_A9(LL(addr))); }
static int BRZP(int addr) { _L(*asmptr++ = 0x0600 | GEN_A9(LL(addr))); }

static int ADD(int x, int y, int z) { _L(*asmptr++ = 0x1000 | GEN_DR(x) | GEN_SR(y) | GEN_IR(z)); }
static int AND(int x, int y, int z) { _L(*asmptr++ = 0x5000 | GEN_DR(x) | GEN_SR(y) | GEN_IR(z)); }
static int NOT(int x, int y) { _L(*asmptr++ = 0x9000 | GEN_DR(x) | GEN_SR(y) | 0x3f); }

static int LEA(int x, int addr) { _L(*asmptr++ = 0xe000 | GEN_DR(x) | GEN_A9(LL(addr))); }
static int LD(int x, int addr) { _L(*asmptr++ = 0x2000 | GEN_DR(x) | GEN_A9(LL(addr))); }
static int LDI(int x, int addr) { _L(*asmptr++ = 0xa000 | GEN_DR(x) | GEN_A9(LL(addr))); }
static int ST(int x, int addr) { _L(*asmptr++ = 0x3000 | GEN_DR(x) | GEN_A9(LL(addr))); }
static int STI(int x, int addr) { _L(*asmptr++ = 0xb000 | GEN_DR(x) | GEN_A9(LL(addr))); }
static int LDR(int x, int y, int z) { _L(*asmptr++ = 0x6000 | GEN_DR(x) | GEN_SR(y) | (z & 0x3f)); }
static int STR(int x, int y, int z) { _L(*asmptr++ = 0x7000 | GEN_DR(x) | GEN_SR(y) | (z & 0x3f)); }

static int JMP(int x) { _L(*asmptr++ = 0xc000 | GEN_SR(x)); }
static int JSR(int addr) { _L(*asmptr++ = 0x4800 | GEN_A9(LL(addr))); }
static int RET() { _L(*asmptr++ = 0xc000 | GEN_SR(R7)); }

static int TRAP(int op) { _L(*asmptr++ = 0xf000 | (op & 0xff)); }
static int GETC() { _L(TRAP(0x20)); }
static int PUTC() { _L(TRAP(0x21)); }
static int HALT() { _L(TRAP(0x25)); }

#define LC3ASM(img) lc3_word *asmptr = img->asmptr; lc3_word *asmbuf = img->asmbuf; lc3_word origin = img->origin; int asmrun; for (asmrun=2; asmrun && (asmptr = asmbuf + origin); asmrun--)

/*
// Insert the specified instruction at the current PC position in memory
// and advance the PC
static int lc3_asm_inject_instruction(lc3vm* vm, uint16_t instr) PC_NOEXCEPT_C
{
	uint16_t pos = vm->reg[R_PC];
	vm->memory[pos] = instr;
	vm->reg[R_PC]++;
	
	return 0;
}



static lc3_instr lc3_gen_br(lc3vm* vm, uint8_t n, uint8_t z, uint8_t p, uint16_t pcOffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_BR) | GEN_FCND(n | z | p) | GEN_POFF9(pcOffset9);}

static lc3_instr lc3_gen_add(lc3vm* vm, uint8_t dr, uint8_t sr1, uint8_t imm, uint8_t sr2) PC_NOEXCEPT_C
{

	if (imm)
		return GEN_OPC(OP_ADD) | GEN_DR(dr) | GEN_SR(sr1) | GEN_FIMM(imm) | GEN_SR2(sr2);
	else 
		return GEN_OPC(OP_ADD) | GEN_DR(dr) | GEN_SR(sr1) | GEN_IMM(sr2);
	
}

static lc3_instr lc3_gen_ld(lc3vm* vm, uint8_t dr, uint16_t pcoffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_LD) | GEN_DR(dr) | GEN_POFF9(pcoffset9);}

static lc3_instr lc3_gen_st(lc3vm* vm, uint8_t sr, uint16_t pcoffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_ST) | GEN_SR(sr) | GEN_POFF9(pcoffset9);}

static lc3_instr lc3_gen_jsr(lc3vm* vm, uint16_t pcoffset11, uint8_t flag) PC_NOEXCEPT_C {return GEN_OPC(OP_JSR) | GEN_FL(flag) | GEN_POFF11(pcoffset11);}

static lc3_instr lc3_gen_and(lc3vm* vm, uint8_t op, uint8_t dr, uint8_t sr1, uint8_t imm, uint8_t sr2)
{

	if (imm)
		return GEN_OPC(op) | GEN_DR(dr) | GEN_SR(sr1) | GEN_FIMM(imm) | GEN_SR2(sr2);
	else
		return GEN_OPC(op) | GEN_DR(dr) | GEN_SR(sr1) | GEN_IMM(sr2);
}

static lc3_instr lc3_gen_ldr(lc3vm* vm, uint8_t dr, uint8_t baseR, uint16_t pcoffset6) PC_NOEXCEPT_C {return GEN_OPC(OP_LDR) | GEN_DR(dr) | GEN_SR(baseR) | GEN_POFF(pcoffset6);}

static lc3_instr lc3_gen_str(lc3vm* vm, uint8_t sr, uint8_t baseR, uint16_t pcoffset6) PC_NOEXCEPT_C {return GEN_OPC(OP_STR) | GEN_SR(sr) | GEN_SR2(baseR) | GEN_POFF(pcoffset6);}

static lc3_instr lc3_gen_rti(lc3vm* vm) PC_NOEXCEPT_C {return 0;}

static lc3_instr lc3_gen_not(lc3vm* vm, uint8_t dr, uint8_t sr) PC_NOEXCEPT_C {return GEN_OPC(OP_NOT) | GEN_DR(dr) | GEN_SR(sr) | 0b111111;}

static lc3_instr lc3_gen_ldi(lc3vm* vm, uint8_t dr, uint16_t pcoffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_LDI) | GEN_DR(dr) | GEN_POFF9(pcoffset9);}

static lc3_instr lc3_gen_sti(lc3vm* vm, uint8_t sr, uint16_t pcoffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_STI) | GEN_SR(sr) | GEN_POFF9(pcoffset9);}

static lc3_instr lc3_gen_jmp(lc3vm* vm, uint8_t baseR) PC_NOEXCEPT_C {return GEN_OPC(OP_JMP) | GEN_SR(baseR);}

static lc3_instr lc3_gen_ret(lc3vm* vm) PC_NOEXCEPT_C {return GEN_OPC(OP_JMP) | GEN_SR(7);}

static lc3_instr lc3_gen_res(lc3vm* vm) PC_NOEXCEPT_C {return 0;}

static lc3_instr lc3_gen_lea(lc3vm* vm, uint8_t dr, uint16_t pcoffset9) PC_NOEXCEPT_C {return GEN_OPC(OP_LEA) | GEN_DR(dr) | GEN_POFF9(pcoffset9);}

static lc3_instr lc3_gen_trap(lc3vm* vm, uint8_t tvec) PC_NOEXCEPT_C {return GEN_OPC(OP_TRAP) | GEN_TRP(tvec);}
*/

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
struct LC3Image
{
	int asmbuf[0xffff] = { 0 };		// memory of the image
	int* asmptr = asmbuf;			// Program Counter
	int fOrigin = 0;					// Where the program starts
	
	LC3Image(int origin)
	{
		fOrigin = origin;
		asmptr = asmbuf + origin;
	}

	void fixup()
	{

	}
	
	int PC() const { return asmptr - asmbuf; }
	int DW(int n) { *asmptr++ = n; return PC() - 1; }
	int LL(int label) const { return label - PC() - 1; }

	int BR(int addr) { _L(*asmptr++ = 0x0e00 | GEN_A9(LL(addr))); }
	int BRN(int addr) { _L(*asmptr++ = 0x0800 | GEN_A9(LL(addr))); }
	int BRZ(int addr) { _L(*asmptr++ = 0x0400 | GEN_A9(LL(addr))); }
	int BRP(int addr) { _L(*asmptr++ = 0x0200 | GEN_A9(LL(addr))); }
	int BRNZ(int addr) { _L(*asmptr++ = 0x0c00 | GEN_A9(LL(addr))); }
	int BRNP(int addr) { _L(*asmptr++ = 0x0a00 | GEN_A9(LL(addr))); }
	int BRZP(int addr) { _L(*asmptr++ = 0x0600 | GEN_A9(LL(addr))); }

	int ADD(int x, int y, int z) { _L(*asmptr++ = 0x1000 | GEN_DR(x) | GEN_SR(y) | GEN_IR(z)); }
	int AND(int x, int y, int z) { _L(*asmptr++ = 0x5000 | GEN_DR(x) | GEN_SR(y) | GEN_IR(z)); }
	int NOT(int x, int y) { _L(*asmptr++ = 0x9000 | GEN_DR(x) | GEN_SR(y) | 0x3f); }

	int LEA(int x, int addr) { _L(*asmptr++ = 0xe000 | GEN_DR(x) | GEN_A9(LL(addr))); }
	int LD(int x, int addr) { _L(*asmptr++ = 0x2000 | GEN_DR(x) | GEN_A9(LL(addr))); }
	int LDI(int x, int addr) { _L(*asmptr++ = 0xa000 | GEN_DR(x) | GEN_A9(LL(addr))); }
	int ST(int x, int addr) { _L(*asmptr++ = 0x3000 | GEN_DR(x) | GEN_A9(LL(addr))); }
	int STI(int x, int addr) { _L(*asmptr++ = 0xb000 | GEN_DR(x) | GEN_A9(LL(addr))); }
	int LDR(int x, int y, int z) { _L(*asmptr++ = 0x6000 | GEN_DR(x) | GEN_SR(y) | (z & 0x3f)); }
	int STR(int x, int y, int z) { _L(*asmptr++ = 0x7000 | GEN_DR(x) | GEN_SR(y) | (z & 0x3f)); }

	int JMP(int x) { _L(*asmptr++ = 0xc000 | GEN_SR(x)); }
	int JSR(int addr) { _L(*asmptr++ = 0x4800 | GEN_A9(LL(addr))); }
	int RET() { _L(*asmptr++ = 0xc000 | GEN_SR(R7)); }

	int TRAP(int op) { _L(*asmptr++ = 0xf000 | (op & 0xff)); }
	int GETC() { _L(TRAP(0x20)); }
	int PUTC() { _L(TRAP(0x21)); }
	int HALT() { _L(TRAP(0x25)); }
};

	
#endif


#endif
