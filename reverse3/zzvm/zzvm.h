#ifndef ZZVM_H
#define ZZVM_H

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
#define ZZ_UNIX_ENV
#endif

#ifdef ZZ_UNIX_ENV
#include <fcntl.h>
#include <unistd.h>
#endif

#include <stdint.h>
#include "zzcode.h"

#define ZZ_MEM_LIMIT 0x10000

typedef struct __attribute__((__packed__)) {
    uint8_t op;
    uint8_t reg;
    uint16_t imm;
} ZZ_INSTRUCTION;

typedef struct __attribute__((__packed__)) {
    uint16_t RA;
    uint16_t R1;
    uint16_t R2;
    uint16_t R3;
    uint16_t R4;
    uint16_t R5;
    uint16_t SP;
    uint16_t IP;
} ZZ_REGISTERS;

#define ZZ_RA 0
#define ZZ_R1 1
#define ZZ_R2 2
#define ZZ_R3 3
#define ZZ_R4 4
#define ZZ_R5 5
#define ZZ_SP 6
#define ZZ_IP 7

typedef struct {
    uint64_t random_seed;
    uint8_t memory[ZZ_MEM_LIMIT];
    union {
        uint16_t registers[8];
        ZZ_REGISTERS regs;
    };
} ZZVM_CTX;

typedef uint16_t (*ZZ_SYSCALL_HANDLER)(ZZVM_CTX *);

typedef struct {
    uint32_t state;
	ZZ_SYSCALL_HANDLER syscall_handler;
    ZZVM_CTX ctx;
} ZZVM;

typedef uint16_t ZZ_ADDRESS;

// ZZVM.state
// TODO: More 1337 numb3rs
#define ZZ_ST_SLEEP 0x51EE9555
#define ZZ_ST_FREED 0xCA7CA7CA
#define ZZ_ST_EXEC  0x55665566

// information level
#define ZZ_MSGL_DEBUG 0
#define ZZ_MSGL_MSG   1
#define ZZ_MSGL_WARN  2
#define ZZ_MSGL_ERROR 3
#define ZZ_MSGL_FATAL 4

// ZZVM API status
#define ZZ_HALT                -54
#define ZZ_INVALID_INSTRUCTION -63
#define ZZ_INVALID_REGISTER    -72
#define ZZ_OUT_BOUND           -81
#define ZZ_SUCCESS             0
#define ZZ_FAILED              1

// ZZVM API
int zz_create(ZZVM **p_vm);
int zz_destroy(ZZVM *vm);

int zz_write_mem(ZZVM *vm, ZZ_ADDRESS addr, void *data, size_t len);
int zz_read_mem(ZZVM *vm, ZZ_ADDRESS addr, void *buffer, size_t len);
int zz_put_code(ZZVM *vm, ZZ_ADDRESS addr, ZZ_INSTRUCTION *ins, size_t count);

uint64_t zz_rand(ZZVM_CTX *ctx);

// assistant API for exection
ZZ_INSTRUCTION * zz_fetch(ZZVM_CTX *ctx);

int zz_dump_context(ZZVM_CTX *ctx, char *buffer, size_t buffer_size);
int zz_execute(ZZVM *vm, int count, int *stop_reason);

int zz_reg_syscall_handler(ZZVM *vm, ZZ_SYSCALL_HANDLER handler);

extern FILE *zz_msg_pipe;
extern int zz_msg_level;
void zz_output_message(int level, char *msg, ...);

#define zz_debug(MSG) zz_output_message(ZZ_MSGL_DEBUG, "%s", MSG)
#define zz_msg(MSG)   zz_output_message(ZZ_MSGL_MSG,   "%s", MSG)
#define zz_warn(MSG)  zz_output_message(ZZ_MSGL_WARN,  "%s", MSG)
#define zz_error(MSG) zz_output_message(ZZ_MSGL_ERROR, "ERROR")
#define zz_fatal(MSG) zz_output_message(ZZ_MSGL_FATAL, "FATAL")

#define zz_debug_f(MSG, args...) zz_output_message(ZZ_MSGL_DEBUG, MSG, args)
#define zz_msg_f(MSG, args...)   zz_output_message(ZZ_MSGL_MSG,   MSG, args)
#define zz_warn_f(MSG, args...)  zz_output_message(ZZ_MSGL_WARN,  MSG, args)
#define zz_error_f(MSG, args...) zz_output_message(ZZ_MSGL_ERROR, MSG, args)
#define zz_fatal_f(MSG, args...) zz_output_message(ZZ_MSGL_FATAL, MSG, args)

int zz_disasm(ZZ_ADDRESS ip, ZZ_INSTRUCTION *ins, char *buffer, size_t limit);

#endif
