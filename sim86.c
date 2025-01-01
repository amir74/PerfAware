#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

//types
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

struct InstructionStream {
    u8 at;
    u8 *bytes;
    u32 size;
};

InstructionStream read_file(char *path) {
    FILE *f = fopen(path, "rb");
    InstructionStream res = {0};
    if (f == nullptr) {
        assert(!"file not found");
        return res;
    }
    fseek(f, 0, SEEK_END);
    res.size = ftell(f);
    fseek(f, 0, SEEK_SET);
    res.bytes = (u8 *)malloc(res.size);
    fread(res.bytes, 1, res.size,f);
    fclose(f);
    return res;
}

enum Register_Code_0 { AL, CL, DL, BL, AH, CH, DH, BH };
enum Register_Code_1 { AX, CX, DX, BX, SP, BP, SI, DI };
char* Register_Code_0_str[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
char* Register_Code_1_str[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

enum OPCode {
    mov_reg  = 0b10001000,
    mov_reg_mask = 0b11111100
};

enum ModeType{
    MOD_reg = 0b11000000
};

void parse_mov_regular(InstructionStream *inst){
    u8 byte[2] = {inst->bytes[inst->at] ,inst->bytes[inst->at+1]};
    bool W = byte[0] & 0b00000001;
    bool D = byte[0] & 0b00000010;
    u8 MOD = byte[1] & 0b11000000;
    u8 REG = (byte[1] & 0b00111000) >> 3;
    u8 RM  = byte[1] &  0b00000111;
    char** reg_str = W ? Register_Code_1_str : Register_Code_0_str;

    if (MOD == MOD_reg){
        printf("mov %s, %s\n", D ? reg_str[REG] : reg_str[RM], D ? reg_str[RM] : reg_str[REG] );
    }

    inst->at+=2;
}

int main(int argc, char **argv){

    InstructionStream inst = read_file(argv[1]);

    printf("bits 16\n\n");

    while (inst.at < inst.size){
        if(check_mask(inst.bytes[inst.at], mov_reg_mask, mov_reg)){
            parse_mov_regular(&inst);
        }
    }

    return 0;
}
