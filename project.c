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

    //printf("Current Instruction: %x\n", Mem[PC>>2]); //#uncomment for debug

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
    //bitwise AND to select ONLY that partition's section of the instruction
    *op     = 0b11111100000000000000000000000000 & instruction; //0xFC00 0000
    *r1     = 0b00000011111000000000000000000000 & instruction; //0x03E0 0000
    *r2     = 0b00000000000111110000000000000000 & instruction; //0x001F 0000
    *r3     = 0b00000000000000001111100000000000 & instruction; //0x0000 F800
    *funct  = 0b00000000000000000000000000111111 & instruction; //0x0000 003F
    *offset = 0b00000000000000001111111111111111 & instruction; //0x0000 FFFF
    *jsec   = 0b00000011111111111111111111111111 & instruction; //0x03FF FFFF
    
    // printf("op code: %o\n", *op>>26);//#uncomment for debug    
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

    //set all controls to don't care values
    controls->RegDst = 0b10;
    controls->Jump = 0b10;
    controls->Branch = 0b10;
    controls->MemRead = 0b10;
    controls->MemtoReg = 0b10;
    controls->ALUOp = 0b000;
    controls->MemWrite = 0b10;
    controls->ALUSrc = 0b10;
    controls->RegWrite = 0b10;

    //shift op 26 bits
    op>>=26;

    //check for valid op codes
    //op codes 03X are invalid
    if(op>>3 == 03)
        return 1;
    //op codes 2X, 6X, and 7X are invalid when X > 1
    if(op>>3 == 02 || op>>3 == 06 || op>>3 == 07)
        if((op & 0b000111) > 01)
            return 1;
    //op codes 54 and 55 are invalid
    if(op == 054 || op == 055)
        return 1;
    
    switch(op)
    {
        case 002: //jump
            //printf("\njump op\n");//#uncomment for degub
            controls->Jump = 0b01;
            break;
        case 003: //jump and link
            controls->Jump = 0b01;
            break;
        case 004: //branch eq
            controls->Branch = 0b01;
            break;
        case 005: //branch not eq
            controls->Branch = 0b01;
            break;
        case 010: //addi (subraction?)
            controls->ALUOp = 0b001;
            break;
        case 011: //addiu
            controls->ALUOp = 0b000;
            break;
        case 012: //slti
            controls->ALUOp = 0b010;
            break;
        case 013: //sltiu
            controls->ALUOp = 0b011;
            break;
        case 014: //andi
            controls->ALUOp = 0b100;
            break;
        case 015: //ori
            controls->ALUOp = 0b101;
            break;
        case 000: //r-type
            controls->ALUOp = 0b111;
            break;
        default:
            printf("\nop code not made yet, but is valid\n");
    }
    //no errors
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    //printf("\nRegister1: %x(%d)\nRegister2: %x(%d)\n", r1>>21, r2>>16);//#uncomment for debug
    *data1 = Reg[r1>>21];
    *data2 = Reg[r2>>16];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //TODO confirm that all this is is setting extended value equal to offset
    *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

    //TODO figure out what Zero pointer and ALUSrc are for
    //if ALUOp is BEQ
    switch(ALUOp)
    {
        case 0b000: //addition
            *ALUresult = data1 + data2;
            return 0;
        case 0b001: //subtraction
            *ALUresult = data1 - data2;
            return 0;
        case 0b010: //set less than
            return 0;
        case 0b011: //set less than unsigned
            return 0;
        case 0b100: //AND
            return 0;
        case 0b101: //OR
            return 0;
        case 0b110: //shift left extended
            *ALUresult = extended_value << 16;
            return 0;
    }
    //if ALUOp is 0b111 then R-type, check if funct is valid
    if(ALUOp == 0b111)
    {
        // *Comment numbers are in base 8*
        //functs 01 and 05 don't exist
        if(funct == 01 || funct == 05)
            return 1;
        //functs 12, 13, 16, and 17 don't exist
        if(funct == 012 || funct == 013 || funct == 016 || funct == 017)
            return 1;
        //functs 2X or 3X don't exist past X3
        if(funct>>3 == 02 || funct>>3 == 03)
            if(funct & 0b000111 > 03)
                return 1;
        //functs 50 and 51 don't exist
        if(funct == 050 || funct == 051)
            return 1;
        //functs above 53 don't exist
        if(funct > 053)
            return 1;
    }
    //printf("made it to end of alu ops");//#uncomment for debug
    //no errors
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //TODO figure out which data is supposed to go to where in memory
    //if MemWrite or MemRead are asserted and ALUresult isn't an address, HALT
    if((MemWrite == 0b01 || MemRead == 0b01) && ALUresult%4 != 0)
        return 1;

    //no errors
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //TODO figure out where I'm supposed to put the data 
    r2>>=16;
    r3>>=11;
    printf("\nmemdata: %d\nALUresult: %d\nRegWrite: %d\nRegDst: %d\nMemtoReg: %d\n", memdata, ALUresult, RegWrite, RegDst, MemtoReg);
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //Jump bit is on
    if(Jump == 0b01)
    {
        printf("\njsec: %x\n", jsec);//#uncomment for debug
        *PC = 4*jsec;
    }
    if(Branch == 0b01)
    {

    }
    else
    {
        *PC+=4;
    }

    printf("\nCurrent PC count: %x\n", *PC);//#uncomment for debug

    // //temp updater to just advance to the next step
    // *PC+=4;
}

