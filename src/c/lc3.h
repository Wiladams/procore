
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


#include "pcoredef.h"
#include "bithacks.h"
#include "convspan.h"

#ifdef __cplusplus
extern "C" {
#endif


enum LC3_REGISTER
{
    R_R0 = 0,   // general purpose registers (0 - 7)
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,       // program counter
    R_COND,     // Conditional Flags
    R_COUNT
};

// Instruction opcodes
enum LC3_OPCODE
{
    OP_BR = 0, // branch
    OP_ADD,    // add  
    OP_LD,     // load 
    OP_ST,     // store 
    OP_JSR,    // jump register 
    OP_AND,    // bitwise and 
    OP_LDR,    // load register 
    OP_STR,    // store register 
    OP_RTI,    // unused 
    OP_NOT,    // bitwise not 
    OP_LDI,    // load indirect 
    OP_STI,    // store indirect 
    OP_JMP,    // jump 
    OP_RES,    // reserved (unused) 
    OP_LEA,    // load effective address
    OP_TRAP,   // execute trap 
    OP_COUNT    // number of operators (16)
};

enum LC3_CONDITION_FLAGS
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

enum LC3_TRAP_CODES
{
    TRAP_GETC   = 0x20,     // get character from keyboard, not echoed onto the terminal 
    TRAP_OUT    = 0x21,     // output a character 
    TRAP_PUTS   = 0x22,     // output a word string 
    TRAP_IN     = 0x23,     // get character from keyboard, echoed onto the terminal 
    TRAP_PUTSP  = 0x24,     // output a byte string 
    TRAP_HALT   = 0x25      // halt the program 
};

enum LC3_MEM_MAP
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

#define LC3_MEMORY_MAX (1 << 16)
enum { PC_START = 0x3000 };

// Some macros to make common things easier
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
#define BR(i) (((i)>>6)&0x7)
#define TRP(i) ((i)&0xFF)

typedef void (*lc3_loop_f)(void *, uint16_t instr);            // callback function for looping


struct lc3vm_t
{
    uint16_t memory[LC3_MEMORY_MAX];  // Memory; 65536 locations, 128K bytes

    // Storage for the registers
    uint16_t reg[R_COUNT];
    int running;

    lc3_loop_f  fLoopFun;
    int hasNewKey;
    uint16_t newKey;
};
typedef struct lc3vm_t lc3vm;

typedef int (*op_ex_f)(lc3vm*, uint16_t i);     // function pointer for operator
typedef int (*trp_ex_f)(lc3vm *);                     // function pointer for trap function

int lc3_vm_init(lc3vm *vm) PC_NOEXCEPT_C;

// Console IO
//static int lc3_check_key()PC_NOEXCEPT_C;
//static uint16_t lc3_getchar()PC_NOEXCEPT_C;

static void lc3_vm_mem_write(lc3vm *vm, uint16_t address, uint16_t val) PC_NOEXCEPT_C;
static uint16_t lc3_vm_mem_read(lc3vm *vm, uint16_t address) PC_NOEXCEPT_C;
static int lc3_update_flags(lc3vm *vm, uint16_t r) PC_NOEXCEPT_C;



// Implementation
    // since exactly one condition flag should be set at any given time, set the Z flag
    //vm->reg[R_COND] = FL_ZRO;

    // set the PC to starting position 
    // 0x3000 is the default
    //enum { PC_START = 0x3000 };
    //vm->reg[R_PC] = PC_START;
int lc3_vm_init(lc3vm *vm) PC_NOEXCEPT_C
{
    vm->fLoopFun = nullptr;

    vm->running = 0;
    vm->reg[R_COND] = FL_ZRO;
    vm->reg[R_PC] = PC_START;

    vm->hasNewKey = false;
    vm->newKey = 0;

    return 0;
}

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

        if (lc3_check_key(vm))
        {
            vm->memory[MR_KBSR] = (1 << 15);
            vm->memory[MR_KBDR] = lc3_getchar(vm);
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
        vm->reg[R_COND] = FL_ZRO;
    }
    else if (vm->reg[r] >> 15) // a 1 in the left-most bit indicates negative
    {
        vm->reg[R_COND] = FL_NEG;
    }
    else
    {
        vm->reg[R_COND] = FL_POS;
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
    //fprintf(stdout, "LDI: 0x%x\n", vm->reg[R_PC] + POFF9(instr));

    // add pc_offset to the current PC, look at that memory location to get the final address
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, lc3_vm_mem_read(vm, vm->reg[R_PC] + POFF9(instr)));
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
    if (FCND(instr) & vm->reg[R_COND])
    {
        vm->reg[R_PC] += POFF9(instr);
    }

    return 0;
}

// JMP
static int lc3_op_jmp(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    // Also handles RET
    vm->reg[R_PC] = vm->reg[SR1(instr)];

    return 0;
}

// JSR
static int lc3_op_jsr(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[R_R7] = vm->reg[R_PC];
    if (FL(instr))
    {
        vm->reg[R_PC] += POFF11(instr);  // JSR
    }
    else
    {
        vm->reg[R_PC] = vm->reg[BR(instr)];    // JSRR 
    }

    return 0;
}

// LD
static int lc3_op_ld(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, vm->reg[R_PC] + POFF9(instr));
    lc3_update_flags(vm, DR(instr));

    return 0;
}


// LDR
static int lc3_op_ldr(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = lc3_vm_mem_read(vm, vm->reg[SR1(instr)] + POFF(instr));
    lc3_update_flags(vm, DR(instr));

    return 0;
}

// LEA
static int lc3_op_lea(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    vm->reg[DR(instr)] = vm->reg[R_PC] + POFF9(instr);
    lc3_update_flags(vm, DR(instr));
    
    return 0;
}

// RES
static int lc3_op_res(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    return 0;
}

// RTI
static int lc3_op_rti(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    return 0;
}

// ST
static int lc3_op_st(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, vm->reg[R_PC] + POFF9(instr), vm->reg[DR(instr)]);

    return 0;
}

// STI
static int lc3_op_sti(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, lc3_vm_mem_read(vm, vm->reg[R_PC] + POFF9(instr)), vm->reg[DR(instr)]);

    return 0;
}

// STR
static int lc3_op_str(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    lc3_vm_mem_write(vm, vm->reg[SR1(instr)] + POFF(instr), vm->reg[DR(instr)]);
    return 0;
}

//
// TRAP Routines
//
static int lc3_trap_getc(lc3vm *vm) PC_NOEXCEPT_C
{
    //printf("trap_getc\n");
    //fflush(stdout);

    // read a single ASCII char
    vm->reg[R_R0] = (uint16_t)getchar();
    lc3_update_flags(vm, R_R0);

    return 0;
}

static int lc3_trap_out(lc3vm *vm) PC_NOEXCEPT_C
{
    putc((char)vm->reg[R_R0], stdout);
    fflush(stdout);
    return 0;
}

static int lc3_trap_in(lc3vm *vm) PC_NOEXCEPT_C
{
    printf("trap_in\n");
    fflush(stdout);

    vm->reg[R_R0] = getchar(); 
    lc3_update_flags(vm, R_R0);
    
    putc((char)vm->reg[R_R0], stdout);
    fflush(stdout);

    return 0;
}

static int lc3_trap_puts(lc3vm *vm) PC_NOEXCEPT_C
{
    //printf("trap_puts\n");
    //fflush(stdout);

    // one char per word
    uint16_t* c = vm->memory + vm->reg[R_R0];
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

    /* one char per byte (two bytes per word)
       here we need to swap back to
       big endian format */
    uint16_t* c = vm->memory + vm->reg[R_R0];
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

static int lc3_trap_halt(lc3vm *vm) PC_NOEXCEPT_C
{
    //puts("HALT");
    //fflush(stdout);
    vm->running = 0;

    return 0;
}

static int lc3_trap_inu16(lc3vm *vm) 
{   
    fscanf(stdin, "%hu", &vm->reg[R_R0]); return 0;
}

static int lc3_trap_outu16(lc3vm *vm) { fprintf(stdout, "%hu\n", vm->reg[R_R0]); return 0;}


static int lc3_op_trap(lc3vm *vm, uint16_t instr) PC_NOEXCEPT_C
{
    enum { trp_offset = 0x20 };

    static trp_ex_f trp_ex[8] = { 
        lc3_trap_getc, lc3_trap_out, lc3_trap_puts, lc3_trap_in, 
        lc3_trap_putsp, lc3_trap_halt, lc3_trap_inu16, lc3_trap_outu16 };

    vm->reg[R_R7] = vm->reg[R_PC];

    //fprintf(stdout, "lc3_op_trap: %d\n", TRP(instr)-trp_offset);
    //fflush(stdout);

    trp_ex[TRP(instr)-trp_offset](vm); 

    return 0;
}

// lc3_vm_exec()
//
// run just the next instruction
// This allows for other things to control the execution
// loop.
// assume lc3_vm_init has been called on the vm before we get here
//
static int lc3_vm_step(lc3vm *vm) PC_NOEXCEPT_C
{
    static op_ex_f op_ex[OP_COUNT] = { 
        lc3_op_br, lc3_op_add, lc3_op_ld, lc3_op_st, 
        lc3_op_jsr, lc3_op_and, lc3_op_ldr, lc3_op_str, 
        lc3_op_rti, lc3_op_not, lc3_op_ldi, lc3_op_sti, 
        lc3_op_jmp, lc3_op_res, lc3_op_lea, lc3_op_trap };

    uint16_t instr = lc3_vm_mem_read(vm, vm->reg[R_PC]++);

    //fprintf(stdout, "VM WHILE OP: %d\n", OPC(instr));
    //fflush(stdout);
    
    if (vm->fLoopFun != nullptr)
    {
        vm->fLoopFun(vm, instr);
    }

    op_ex[OPC(instr)](vm, instr);

}

static int lc3_vm_exec(lc3vm * vm) PC_NOEXCEPT_C
{
    static op_ex_f op_ex[OP_COUNT] = { 
        lc3_op_br, lc3_op_add, lc3_op_ld, lc3_op_st, 
        lc3_op_jsr, lc3_op_and, lc3_op_ldr, lc3_op_str, 
        lc3_op_rti, lc3_op_not, lc3_op_ldi, lc3_op_sti, 
        lc3_op_jmp, lc3_op_res, lc3_op_lea, lc3_op_trap };

    vm->running = 1;
    while (vm->running)
    {
        lc3_vm_step(vm);
    }

    return 0;
}

//
// lc3 programs are encoded as 'big-endian', so depending
// on what we're currently running on, we need to read
// all values, and convert them to local-endian (typically little)
//
int lc3_load_image_span(lc3vm *vm, bspan * file)
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
