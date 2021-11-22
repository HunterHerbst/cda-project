#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

    printf("Current Instruction: %x\n", Mem[PC>>2]); //#uncomment for debug

    *instruction = Mem[PC>>2];
    printf("%x", *instruction);
    if(*instruction != 0)
        return 0;
    else
        return 1;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // bit breakdown of each section of an instruction
    //          3128 2724 2320 1916 1512 11 8 7  4 3  0
    //op        1111 1100 0000 0000 0000 0000 0000 0000
    //r1        0000 0011 1110 0000 0000 0000 0000 0000
    //r2        0000 0000 0001 1111 0000 0000 0000 0000
    //r3        0000 0000 0000 0000 1111 1000 0000 0000
    //funct     0000 0000 0000 0000 0000 0000 0011 1111
    //offset    0000 0000 0000 0000 1111 1111 1111 1111
    //jsec      0000 0011 1111 1111 1111 1111 1111 1111

    //bitwise AND to select ONLY that partition's section of the instruction
    *op     = 0b11111100000000000000000000000000 & instruction;
    *r1     = 0b00000011111000000000000000000000 & instruction;
    *r2     = 0b00000000000111110000000000000000 & instruction;
    *r3     = 0b00000000000000001111100000000000 & instruction;
    *funct  = 0b00000000000000000000000000111111 & instruction;
    *offset = 0b00000000000000001111111111111111 & instruction;
    *jsec   = 0b00000011111111111111111111111111 & instruction;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //temp updater to just advance to the next step
    *PC+=4;
}

