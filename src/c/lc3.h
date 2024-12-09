
#ifndef LC3_H_INCLUDED
#define LC3_H_INCLUDED

// Implementatioin of a LC-3 virtual machine
// https://www.jmeiners.com/lc3-vm/


#include <stdio.h>

#include "pcoredef.h"
#include "bithacks.h"


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
    OP_TRAP    // execute trap 
};

enum LC3_CONDITION_FLAGS
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

enum LC3_TRAP_CODES
{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

enum LC3_MEM_MAP
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

#define LC3_MEMORY_MAX (1 << 16)
enum { PC_START = 0x3000 };


struct lc3vm_t
{
    uint16_t memory[LC3_MEMORY_MAX];  // Memory; 65536 locations, 128K bytes

    // Storage for the registers
    uint16_t reg[R_COUNT];
    int running;
};
typedef struct lc3vm_t lc3vm;

int lc3_vm_init(lc3vm *vm)PC_NOEXCEPT_C;

// Console IO
static int lc3_check_key()PC_NOEXCEPT_C;
static uint16_t lc3_getchar()PC_NOEXCEPT_C;

static void lc3_vm_mem_write(lc3vm *vm, uint16_t address, uint16_t val) PC_NOEXCEPT_C;
static uint16_t lc3_vm_mem_read(lc3vm *vm, uint16_t address) PC_NOEXCEPT_C;
static int lc3_update_flags(lc3vm *vm, uint16_t r) PC_NOEXCEPT_C;

// Operator definitions
static int lc3_op_add(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_ldi(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_and(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_not(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_br(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_jmp(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_jsr(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_ld(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_ldr(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_lea(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_st(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_sti(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;
static int lc3_op_str(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C;

// Trap routines
static int lc3_vm_trap_puts(lc3vm *vm) PC_NOEXCEPT_C;


static int lc3_vm_exec(lc3vm * vm) PC_NOEXCEPT_C;


// Implementation
    // since exactly one condition flag should be set at any given time, set the Z flag
    //vm->reg[R_COND] = FL_ZRO;

    // set the PC to starting position 
    // 0x3000 is the default
    //enum { PC_START = 0x3000 };
    //vm->reg[R_PC] = PC_START;
int lc3_vm_init(lc3vm *vm)
{
    vm->running = 0;
    vm->reg[R_COND] = FL_ZRO;
    vm->reg[R_PC] = PC_START;

    return 0;
}

static int lc3_check_key() PC_NOEXCEPT_C
{
    return 0;
}

static uint16_t lc3_getchar() PC_NOEXCEPT_C
{
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
        if (lc3_check_key())
        {
            vm->memory[MR_KBSR] = (1 << 15);
            vm->memory[MR_KBDR] = lc3_getchar();
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
}

static int lc3_op_add(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    // destination register (DR) 
    uint16_t r0 = (instr >> 9) & 0x7;
    // first operand (SR1) 
    uint16_t r1 = (instr >> 6) & 0x7;
    // whether we are in immediate mode
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag)
    {
        uint16_t imm5 = bhak_sign_extend_u16(instr & 0x1F, 5);
        vm->reg[r0] = vm->reg[r1] + imm5;
    }
    else
    {
        uint16_t r2 = instr & 0x7;
        vm->reg[r0] = vm->reg[r1] + vm->reg[r2];
    }

    lc3_update_flags(vm, r0);

    return 0;
}

// LDI
static int lc3_op_ldi(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    // destination register (DR)
    uint16_t r0 = (instr >> 9) & 0x7;
    // PCoffset 9
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    // add pc_offset to the current PC, look at that memory location to get the final address
    vm->reg[r0] = lc3_vm_mem_read(vm, lc3_vm_mem_read(vm, vm->reg[R_PC] + pc_offset));
    lc3_update_flags(vm, r0);
}

// AND
static int lc3_op_and(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag)
    {
        uint16_t imm5 = bhak_sign_extend_u16(instr & 0x1F, 5);
        vm->reg[r0] = vm->reg[r1] & imm5;
    }
    else
    {
        uint16_t r2 = instr & 0x7;
        vm->reg[r0] = vm->reg[r1] & vm->reg[r2];
    }
    lc3_update_flags(vm, r0);

    return 0;
}

// NOT
static int lc3_op_not(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;

    vm->reg[r0] = ~vm->reg[r1];
    lc3_update_flags(vm, r0);

    return 0;
}

// BR
static int lc3_op_br(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    uint16_t cond_flag = (instr >> 9) & 0x7;
    if (cond_flag & vm->reg[R_COND])
    {
        vm->reg[R_PC] += pc_offset;
    }

    return 0;
}

// JMP
static int lc3_op_jmp(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    // Also handles RET
    uint16_t r1 = (instr >> 6) & 0x7;
    vm->reg[R_PC] = vm->reg[r1];

    return 0;
}

// JSR
static int lc3_op_jsr(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t long_flag = (instr >> 11) & 1;
    vm->reg[R_R7] = vm->reg[R_PC];
    if (long_flag)
    {
        uint16_t long_pc_offset = bhak_sign_extend_u16(instr & 0x7FF, 11);
        vm->reg[R_PC] += long_pc_offset;  /* JSR */
    }
    else
    {
        uint16_t r1 = (instr >> 6) & 0x7;
        vm->reg[R_PC] = vm->reg[r1]; /* JSRR */
    }
}

// LD
static int lc3_op_ld(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    vm->reg[r0] = lc3_vm_mem_read(vm, vm->reg[R_PC] + pc_offset);
    lc3_update_flags(vm, r0);

    return 0;
}


// LDR
static int lc3_op_ldr(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t offset = bhak_sign_extend_u16(instr & 0x3F, 6);
    vm->reg[r0] = lc3_vm_mem_read(vm, vm->reg[r1] + offset);
    lc3_update_flags(vm, r0);

    return 0;
}

// LEA
static int lc3_op_lea(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    vm->reg[r0] = vm->reg[R_PC] + pc_offset;
    lc3_update_flags(vm, r0);
    
    return 0;
}

// ST
static int lc3_op_st(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    lc3_vm_mem_write(vm, vm->reg[R_PC] + pc_offset, vm->reg[r0]);

    return 0;
}

// STI
static int lc3_op_sti(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = bhak_sign_extend_u16(instr & 0x1FF, 9);
    lc3_vm_mem_write(vm, lc3_vm_mem_read(vm, vm->reg[R_PC] + pc_offset), vm->reg[r0]);

    return 0;
}

// STR
static int lc3_op_str(lc3vm *vm, uint16_t instr, uint16_t op) PC_NOEXCEPT_C
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t offset = bhak_sign_extend_u16(instr & 0x3F, 6);
    lc3_vm_mem_write(vm, vm->reg[r1] + offset, vm->reg[r0]);
}

// TRAP Routines
static int lc3_vm_trap_getc(lc3vm *vm) PC_NOEXCEPT_C
{
    // read a single ASCII char
    vm->reg[R_R0] = (uint16_t)getchar();
    lc3_update_flags(vm, R_R0);

    return 0;
}

static int lc3_vm_trap_out(lc3vm *vm) PC_NOEXCEPT_C
{
    putc((char)vm->reg[R_R0], stdout);
    fflush(stdout);
}

static int lc3_vm_trap_in(lc3vm *vm) PC_NOEXCEPT_C
{
    printf("Enter a character: ");
    int c = getchar();
    putc(c, stdout);
    fflush(stdout);
    vm->reg[R_R0] = (uint16_t)c;
    lc3_update_flags(vm, R_R0);

    return 0;
}

static int lc3_vm_trap_puts(lc3vm *vm) PC_NOEXCEPT_C
{
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

static int lc3_vm_trap_putsp(lc3vm *vm) PC_NOEXCEPT_C
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

static int lc3_vm_trap_halt(lc3vm *vm) PC_NOEXCEPT_C
{
    puts("HALT");
    fflush(stdout);
    vm->running = 0;
}

// lc3_vm_exec()
//
// run an actual program
// assume lc3_vm_init has been called on the vm before we get here
//
static int lc3_vm_exec(lc3vm * vm) PC_NOEXCEPT_C
{
    //@{Load Arguments}
    //@{Setup}

    vm->running = 1;
    while (vm->running)
    {
        // FETCH
        uint16_t instr = lc3_vm_mem_read(vm, vm->reg[R_PC]++);
        uint16_t op = instr >> 12;

        switch (op)
        {
            case OP_ADD:
                lc3_op_add(vm, instr, op);
                break;
            
            case OP_AND:
                lc3_op_and(vm, instr, op);
                break;
            
            case OP_NOT:
                lc3_op_not(vm, instr, op);
                break;
            
            case OP_BR:
                lc3_op_br(vm, instr, op);
                break;
            
            case OP_JMP:
                lc3_op_jmp(vm, instr, op);
                break;
            
            case OP_JSR:
                lc3_op_jsr(vm, instr, op);
                break;
            
            case OP_LD:
                lc3_op_ld(vm, instr, op);
                break;
            
            case OP_LDI:
                lc3_op_ldi(vm, instr, op);
                break;
            
            case OP_LDR:
                lc3_op_ldr(vm, instr, op);
                break;
            
            case OP_LEA:
                lc3_op_lea(vm, instr, op);
                break;
            
            case OP_ST:
                lc3_op_st(vm, instr, op);
                break;
            
            case OP_STI:
                lc3_op_sti(vm, instr, op);
                break;
            
            case OP_STR:
                lc3_op_str(vm, instr, op);
                break;

            case OP_TRAP:
                vm->reg[R_R7] = vm->reg[R_PC];

                switch (instr & 0xFF)
                {
                    case TRAP_GETC:
                        lc3_vm_trap_getc(vm);
                        break;
                    case TRAP_OUT:
                        lc3_vm_trap_out(vm);
                        break;
                    case TRAP_PUTS:
                        lc3_vm_trap_puts(vm);
                        break;
                    case TRAP_IN:
                        lc3_vm_trap_in(vm);
                        break;
                    case TRAP_PUTSP:
                        lc3_vm_trap_putsp(vm);
                        break;
                    case TRAP_HALT:
                        lc3_vm_trap_halt(vm);
                        break;
                }
            
                // Restore the PC here
                // not strictly needed, because our trap routines are not written
                // in assembly, so they won't be changing the registers
                vm->reg[R_PC]= vm->reg[R_R7];

                break;

            case OP_RES:
            case OP_RTI:
                //NO_OP
                break; 
            default:
                //@{BAD OPCODE}
                break;
        }
    }
    //@{Shutdown}
}

#ifdef __cplusplus
}
#endif

#endif // LC3_H_INCLUDED
