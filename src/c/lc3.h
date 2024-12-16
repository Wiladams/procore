
#ifndef LC3_H_INCLUDED
#define LC3_H_INCLUDED

// Implementatioin of a LC-3 virtual machine
// There are many examples in many languages
//
// Nice Tutorial
// https://www.jmeiners.com/lc3-vm/
//
// Small tight implementation
// https://github.com/nomemory/lc3-vm/blob/main/vm.c
// https://www.rodrigoaraujo.me/posts/lets-build-an-lc-3-virtual-machine/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "pcoredef.h"
#include "bithacks.h"
#include "convspan.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef uint16_t lc3_word;
typedef uint16_t lc3_instr;

#define _R(n)		(((n)&7) | 0xa000)		// helper to define register enums, for later usage in GEN_IR
#define NR(r)		((r)&7)					// define a register

enum { R0 = _R(0), R1 = _R(1), R2 = _R(2), R3 = _R(3), R4 = _R(4), R5 = _R(5), R6 = _R(6), R7 = _R(7) };
#define R_COUNT 8

/*
enum LC3_REGISTER
{
    R_R0    = 0,   // general purpose registers (0 - 7)
    R_R1    = 1,
    R_R2    = 2,
    R_R3    = 3,
    R_R4    = 4,
    R_R5    = 5,
    R_R6    = 6,
    R_R7    = 7,
    R_COUNT
};
*/

// Instruction opcodes
enum LC3_OPCODE
{
    OP_BR   = 0b0000, // branch
    OP_ADD  = 0b0001,    // add  
    OP_LD   = 0b0010,     // load 
    OP_ST   = 0b0011,     // store 
    OP_JSR  = 0b0100,    // jump register 
    OP_AND  = 0b0101,    // bitwise and 
    OP_LDR  = 0b0110,    // load register 
    OP_STR  = 0b0111,    // store register 
    OP_RTI  = 0b1000,    // unused 
    OP_NOT  = 0b1001,    // bitwise not 
    OP_LDI  = 0b1010,    // load indirect 
    OP_STI  = 0b1011,    // store indirect 
    OP_JMP  = 0b1100,    // jump , also RET
    OP_RES  = 0b1101,    // reserved (unused) 
    OP_LEA  = 0b1110,    // load effective address
    OP_TRAP = 0b1111,   // execute trap 
    OP_COUNT    // number of operators (16)
};

enum LC3_CONDITION_FLAGS
{
    FL_POS = 1 << 0, /* P */
    FL_ZERO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

enum LC3_TRAP_CODES
{
    TRAP_GETC   = 0x20,     // get character from keyboard, not echoed onto the terminal 
    TRAP_OUT    = 0x21,     // output a character 
    TRAP_PUTS   = 0x22,     // output a word string 
    TRAP_IN     = 0x23,     // get character from keyboard, echoed onto the terminal 
    TRAP_PUTSP  = 0x24,     // output a byte string 
    TRAP_HALT   = 0x25,      // halt the program 
    TRAP_INU16  = 0x26,     // input a 16-bit value
    TRAP_OUTU16 = 0x27      // output a 16-bit value
};

enum LC3_MEM_MAP
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

#define LC3_MEMORY_MAX (1 << 16)
enum { PC_START = 0x3000 };

// Some macros to decode instructions
#define OPC(i) ((i)>>12)
#define DR(i) (((i)>>9)&0x7)
#define SR1(i) (((i)>>6)&0x7)
#define SR2(i) ((i)&0x7)
#define FIMM(i) ((i>>5)&01)
#define IMM(i) ((i)&0x1F)
#define SEXTIMM(i) bhak_sign_extend_u16(IMM(i),5)
#define FCND(i) (((i)>>9)&0x7)
#define POFF(i) bhak_sign_extend_u16((i)&0x3F, 6)
#define POFF9(i) bhak_sign_extend_u16((i)&0x1FF, 9)
#define POFF11(i) bhak_sign_extend_u16((i)&0x7FF, 11)
#define FL(i) (((i)>>11)&1)
#define BRF(i) (((i)>>6)&0x7)
#define TRP(i) ((i)&0xFF)

typedef int (*lc3_loop_f)(void *, uint16_t instr);            // callback function for looping
typedef int (*lc3_check_key_f)();

struct lc3vm_t
{
    // physical machine structure
    uint16_t pc;    // program counter
    uint16_t cflags;// COND - conditional flags
    uint16_t reg[R_COUNT];              // Storage for the registers
    uint16_t memory[LC3_MEMORY_MAX];    // Main Memory; 65536 uint16_t locations, 128K bytes



    // vm runtime environment
    //
    int running;


    // Key handling
    int hasNewKey;
    uint16_t newKey;

    lc3_loop_f  fLoopHook;
    lc3_check_key_f fCheckKey;
};
typedef struct lc3vm_t lc3vm;

typedef int (*op_ex_f)(lc3vm*, uint16_t i);     // function pointer for operator
typedef int (*trp_ex_f)(lc3vm *);               // function pointer for trap function

// Function Prototypes
static int lc3_vm_init(lc3vm *vm) PC_NOEXCEPT_C;

static int lc3_vm_set_reg(lc3vm* vm, enum LC3_REGISTER reg, uint16_t val) PC_NOEXCEPT_C;
static uint16_t lc3_vm_get_reg(lc3vm* vm, enum LC3_REGISTER reg) PC_NOEXCEPT_C;

static void lc3_vm_mem_write(lc3vm *vm, uint16_t address, uint16_t val) PC_NOEXCEPT_C;
static uint16_t lc3_vm_mem_read(lc3vm *vm, uint16_t address) PC_NOEXCEPT_C;
static int lc3_update_flags(lc3vm *vm, uint16_t r) PC_NOEXCEPT_C;

static trp_ex_f* lc3_get_trap_table(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_set_trap(lc3vm* vm, trp_ex_f fun, uint16_t trap_code) PC_NOEXCEPT_C;

static int lc3_trap_getc(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_out(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_in(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_puts(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_putsp(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_halt(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_inu16(lc3vm *vm) PC_NOEXCEPT_C;
static int lc3_trap_outu16(lc3vm *vm) PC_NOEXCEPT_C;


// Implementation
// Initialize a new vm structure
int lc3_vm_init(lc3vm *vm) PC_NOEXCEPT_C
{
    // Clear out the memory
    memset(vm->memory, 0, sizeof(vm->memory));

    vm->cflags = FL_ZERO;  // conditional flags zero
    vm->pc = PC_START;   // starting program counter

    // Setup trap table
    // Here, we setup trap routines as C function pointers
    // Another way to do it is have the trap routines be implemented in LC3 machine code
    // and then we can setup the trap table in main memory
    // The lc3_op_trap() must match whichever way it is setup
    trp_ex_f * tbl = lc3_get_trap_table(vm);
    tbl[TRAP_GETC] = lc3_trap_getc;
    tbl[TRAP_OUT] = lc3_trap_out;
    tbl[TRAP_PUTS] = lc3_trap_puts;
    tbl[TRAP_IN] = lc3_trap_in;
    tbl[TRAP_PUTSP] = lc3_trap_putsp;
    tbl[TRAP_HALT] = lc3_trap_halt;
    tbl[TRAP_INU16] = lc3_trap_inu16;
    tbl[TRAP_OUTU16] = lc3_trap_outu16;

    vm->fLoopHook = nullptr;    // no loop hook
    vm->fCheckKey = nullptr;    // no key check, should be set by host environment

    vm->running = 0;

    vm->hasNewKey = false;
    vm->newKey = 0;



    return 0;
}

static int lc3_vm_set_reg(lc3vm* vm, enum LC3_REGISTER regNum, uint16_t regVal) PC_NOEXCEPT_C
{
    vm->reg[regNum] = regVal;
    return 0;
}

static uint16_t lc3_vm_get_reg(lc3vm* vm, enum LC3_REGISTER regNum) PC_NOEXCEPT_C
{
    return vm->reg[regNum];
}
static uint16_t lc3_get_PC(lc3vm* vm) 
{
    return vm->pc;
}

int lc3_vm_set_checkkey(lc3vm *vm, lc3_check_key_f fCheckKey)
{
    vm->fCheckKey = fCheckKey;
    return 0;
}

int lc3_vm_set_loopHook(lc3vm *vm, lc3_loop_f loopHook)
{
    vm->fLoopHook = loopHook;
    return 0;
}

/*
static int lc3_check_key(lc3vm *vm) PC_NOEXCEPT_C
{
    if (vm->hasNewKey)
    {
        vm->hasNewKey = 0;
        return 1;
    }

    return 0;
}

static uint16_t lc3_getchar(lc3vm *vm) PC_NOEXCEPT_C
{
    return vm->newKey;
}
*/

static int lc3_vm_inject_key(lc3vm *vm, uint16_t c)
{
    //printf("inject_key: %d\n", (char )c);
    //fflush(stdout);

    vm->hasNewKey = 1;
    vm->newKey = c;

    return 0;
}


static void lc3_vm_mem_write(lc3vm *vm, uint16_t address, uint16_t val) PC_NOEXCEPT_C
{
    vm->memory[address] = val;
}

static uint16_t lc3_vm_mem_read(lc3vm *vm, uint16_t address) PC_NOEXCEPT_C
{
    
    if (address == MR_KBSR)
    {
        //printf("READING MR_KBSR\n");

        if (vm->fCheckKey != nullptr && vm->fCheckKey())
        {
            vm->memory[MR_KBSR] = (1 << 15);
            vm->memory[MR_KBDR] = getchar();
        }
        else
        {
            vm->memory[MR_KBSR] = 0;
        }
    }
    
    return vm->memory[address];
}

static int lc3_update_flags(lc3vm *vm, uint16_t r) PC_NOEXCEPT_C
{
    if (vm->reg[r] == 0)
    {
        vm->cflags = FL_ZERO;
    }
    else if (vm->reg[r] >> 15) // a 1 in the left-most bit indicates negative
    {
        vm->cflags = FL_NEG;
    }
    else
    {
        vm->cflags = FL_POS;
    }

    return 0;
}

static int lc3_op_add(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    if (FIMM(instr))
    {
        vm->reg[DR(instr)] = vm->reg[SR1(instr)] + SEXTIMM(instr);
    }
    else
    {
        vm->reg[DR(instr)] = vm->reg[SR1(instr)] + vm->reg[SR2(instr)];
    }

    lc3_update_flags(vm, DR(instr));

    return 0;
}

// LDI
static int lc3_op_ldi(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    //fprintf(stdout, "LDI: 0x%x\n", vm->pc + POFF9(instr));

    // add pc_offset to the current PC, look at that memory location to get the final address
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, lc3_vm_mem_read(vm, vm->pc + POFF9(instr)));
    lc3_update_flags(vm, DR(instr));

    return 0;
}

// AND
static int lc3_op_and(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    if (FIMM(instr))
    {
        vm->reg[DR(instr)] = vm->reg[SR1(instr)] & SEXTIMM(instr);
    }
    else
    {
        vm->reg[DR(instr)] = vm->reg[SR1(instr)] & vm->reg[SR2(instr)];
    }
    lc3_update_flags(vm, DR(instr));

    return 0;
}

// NOT
static int lc3_op_not(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = ~vm->reg[SR1(instr)];
    lc3_update_flags(vm, DR(instr));

    return 0;
}

// BR
static int lc3_op_br(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    if (FCND(instr) & vm->cflags)
    {
        vm->pc += POFF9(instr);
    }

    return 0;
}

// JMP
static INLINE int lc3_op_jmp(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    // Also handles RET
    vm->pc = vm->reg[SR1(instr)];

    return 0;
}

// JSR
static INLINE int lc3_op_jsr(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[NR(R7)] = vm->pc;

    if (FL(instr))
    {
        vm->pc += POFF11(instr);  // JSR
    }
    else
    {
        vm->pc = vm->reg[BRF(instr)];    // JSRR 
    }

    return 0;
}

// LD
static INLINE int lc3_op_ld(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, vm->pc + POFF9(instr));
    lc3_update_flags(vm, DR(instr));

    return 0;
}


// LDR
static INLINE int lc3_op_ldr(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, vm->reg[SR1(instr)] + POFF(instr));
    lc3_update_flags(vm, DR(instr));

    return 0;
}

// LEA
static INLINE int lc3_op_lea(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = vm->pc + POFF9(instr);
    lc3_update_flags(vm, DR(instr));
    
    return 0;
}

// RES - reserved
static INLINE int lc3_op_res(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    return 0;
}

// RTI
static INLINE int lc3_op_rti(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    return 0;
}

// ST
static INLINE int lc3_op_st(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, vm->pc + POFF9(instr), vm->reg[DR(instr)]);

    return 0;
}

// STI
static INLINE int lc3_op_sti(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, lc3_vm_mem_read(vm, vm->pc + POFF9(instr)), vm->reg[DR(instr)]);

    return 0;
}

// STR
static INLINE int lc3_op_str(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, vm->reg[SR1(instr)] + POFF(instr), vm->reg[DR(instr)]);
    return 0;
}




//
// TRAP Routines
//
static INLINE int lc3_trap_getc(lc3vm *vm) PC_NOEXCEPT_C
{
    //printf("trap_getc\n");
    //fflush(stdout);

    // read a single ASCII char
    vm->reg[NR(R0)] = (uint16_t)getchar();
    lc3_update_flags(vm, NR(R0));

    return 0;
}

static INLINE int lc3_trap_out(lc3vm *vm) PC_NOEXCEPT_C
{
    putc((char)vm->reg[NR(R0)], stdout);
    fflush(stdout);
    return 0;
}

static INLINE int lc3_trap_in(lc3vm *vm) PC_NOEXCEPT_C
{
    vm->reg[NR(R0)] = getchar(); 
    lc3_update_flags(vm, NR(R0));
    
    putc((char)vm->reg[NR(R0)], stdout);
    fflush(stdout);

    return 0;
}

static int lc3_trap_puts(lc3vm *vm) PC_NOEXCEPT_C
{
    // one char per word
    uint16_t* c = vm->memory + vm->reg[NR(R0)];
    while (*c)
    {
        putc((char)*c, stdout);
        ++c;
    }
    fflush(stdout);

    return 0;
}

static int lc3_trap_putsp(lc3vm *vm) PC_NOEXCEPT_C
{

    // one char per byte (two bytes per word)
    // here we need to swap back to
    // big endian format
    uint16_t* c = vm->memory + vm->reg[NR(R0)];
    while (*c)
    {
        char char1 = (*c) & 0xFF;
        putc(char1, stdout);
        char char2 = (*c) >> 8;
        if (char2) putc(char2, stdout);
        ++c;
    }
    fflush(stdout);

    return 0;
}

static INLINE int lc3_trap_halt(lc3vm *vm) PC_NOEXCEPT_C
{
    vm->running = 0;

    return 0;
}

static INLINE int lc3_trap_inu16(lc3vm *vm) PC_NOEXCEPT_C
{   
    fscanf_s(stdin, "%hu", &vm->reg[NR(R0)]); return 0;
}

static INLINE int lc3_trap_outu16(lc3vm *vm) PC_NOEXCEPT_C 
{ 
    fprintf(stdout, "%hu\n", vm->reg[NR(R0)]); 
    return 0;
}


// create a static function lc3_get_trap_table(lc3vm *vm)
// so there is only a single instance of this table for the entire
// application.
// By having a separate table, we're making a design change to the 
// lc3 architecture, because it is not part of main memory.
// Ideally, this table would be part of main memory so that programs
// could actually alter it.
static trp_ex_f* lc3_get_trap_table(lc3vm *vm) PC_NOEXCEPT_C
{
    static trp_ex_f trap_tbl[256];
    static int initialized = 0;

    if (!initialized)
    {
        memset(&trap_tbl[0], 0, sizeof(trap_tbl));
        initialized = 1;
    }

    return trap_tbl;
}

// set a trap function
static INLINE int lc3_set_trap(lc3vm *vm, trp_ex_f fun, uint16_t trap_code) PC_NOEXCEPT_C
{
    trp_ex_f * tbl = lc3_get_trap_table(vm);
    tbl[trap_code] = fun;

    return 0;
}

// lc3_op_trap
//
// Execute a TRAP instruction
static int lc3_op_trap(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[NR(R7)] = vm->pc;

    trp_ex_f * tbl = lc3_get_trap_table(vm);
    tbl[TRP(instr)](vm); 

    // Restore the PC from the R7 register
    vm->pc = vm->reg[NR(R7)];

    return 0;
}

// lc3_vm_step()
//
// get the next instruction from where the Program Counter sits
// and execute just that single instruction.
// This allows for other things to control the execution
// loop.
//
static int lc3_vm_step(lc3vm *vm) PC_NOEXCEPT_C
{
    // Table of operator functions, indexed by opcode
    static op_ex_f op_ex[OP_COUNT] = { 
        lc3_op_br, lc3_op_add, lc3_op_ld, lc3_op_st, 
        lc3_op_jsr, lc3_op_and, lc3_op_ldr, lc3_op_str, 
        lc3_op_rti, lc3_op_not, lc3_op_ldi, lc3_op_sti, 
        lc3_op_jmp, lc3_op_res, lc3_op_lea, lc3_op_trap };

    uint16_t instr = lc3_vm_mem_read(vm, vm->pc);
    // Explicitly increment the PC
    vm->pc++;
    
    // We call out to the loop hook first, to give
    // it a chance to determine whether we should continue
    // or not.  If it returns '0', we continue, otherwise
    // we return with an error of our own.
    if (vm->fLoopHook != nullptr)
    {
        if (vm->fLoopHook(vm, instr) != 0)
            return -1;
    }

    // Perform the actual operation
    op_ex[OPC(instr)](vm, instr);

    return 0;
}

static int lc3_vm_run(lc3vm * vm) PC_NOEXCEPT_C
{
    vm->running = 1;
    while (vm->running)
    {
        if (lc3_vm_step(vm) != 0)
            break;
    }

    return 0;
}

//
// lc3 programs are encoded as 'big-endian', so depending
// on what we're currently running on, we need to read
// all values, and convert them to local-endian (typically little)
//
static int lc3_load_image_span(lc3vm *vm, bspan * file)
{
    // We'll use src as our cursor into the image 
    bspan src;
    bspan_weak_assign(&src, file);

    // First up is the origin, which tells us where in the VM's
    // memory the program wants to be loaded into.
    uint16_t origin = as_u16_be(bspan_begin(&src));
    bspan_advance(&src, 2);

    // We know how many bytes we have left in the span
    // And we know where we want to start reading,
    // so read each u16 into the proper location
    uint16_t* p = vm->memory + origin;

    // We read one value at a time, ensuring it has the
    // correct endianness in memory
    while (bspan_is_valid(&src))
    {
        *p = as_u16_be(bspan_begin(&src));
        p++;
        bspan_advance(&src, 2);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // LC3_H_INCLUDED
