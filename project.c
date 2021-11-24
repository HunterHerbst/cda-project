/*
 | Hunter Herbst
 | PID: 5095633
 */

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch(ALUControl)
    {
        case 0b000: //add
            *ALUresult = A + B;
            if(*ALUresult == 0) *Zero = 0;
            break;
        case 0b001: //sub
            *ALUresult = A - B;
            if(*ALUresult == 0) *Zero = 0;
            break;
        case 0b010:
            *ALUresult = ((signed)A < (signed)B) ? 1:0;
            break;
        case 0b011:
            *ALUresult = (A < B) ? 1:0;
            break;
        case 0b100:
            *ALUresult = A & B;
            break;
        case 0b101:
            *ALUresult = A | B;
            break;
        case 0b110:
            *ALUresult = B<<16;
            break;
        case 0b111:
            *ALUresult = ~A;
            break;
    }
    *Zero = (*ALUresult == 0) ? 1:0;
    
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    //ensure word aligned
    if(PC%4 == 0)
        *instruction = Mem[PC>>2];
    else
        return 1; //not aligned so halt
    
    //printf("\nCurrent instruction: %x\n", *instruction);
    
    //no errors
    return 0;
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
    
    // printf("op code: %o\n", *op>>26);
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
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
        case 000: //r-type
            controls->RegDst = 0b1;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b111;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b0;
            controls->RegWrite = 0b1;
            break;
        case 002: //jump
            controls->RegDst = 0b0;
            controls->Jump = 0b1;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b000;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b0;
            controls->RegWrite = 0b0;
            break;
        case 004: //branch eq
            controls->RegDst = 0b10;
            controls->Jump = 0b0;
            controls->Branch = 0b1;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b10;
            controls->ALUOp = 0b001;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b0;
            controls->RegWrite = 010;
            break;
        case 010: //addi
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b000;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 012: //slti
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b010;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 013: //sltiu
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b011;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 014: //andi
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b100;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 015: //ori
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b101;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 017: //lui
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b0;
            controls->MemtoReg = 0b0;
            controls->ALUOp = 0b110;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 043: //load word
            controls->RegDst = 0b0;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b1;
            controls->MemtoReg = 0b1;
            controls->ALUOp = 0b000;
            controls->MemWrite = 0b0;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b1;
            break;
        case 053: //store word
            controls->RegDst = 0b10;
            controls->Jump = 0b0;
            controls->Branch = 0b0;
            controls->MemRead = 0b1;
            controls->MemtoReg = 0b10;
            controls->ALUOp = 0b000;
            controls->MemWrite = 0b1;
            controls->ALUSrc = 0b1;
            controls->RegWrite = 0b0;
            break;
        default:
            //printf("\nop code not implemented, but is valid\n");
            return 1;
    }

    //no errors
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    //printf("\nRegister1: %x(%d)\nRegister2: %x(%d)\n", r1>>21, r2>>16);
    *data1 = Reg[r1>>21];
    *data2 = Reg[r2>>16];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //check if neg and extend if so
    //if not, extend normal
    if(offset>>15 == 1)
        *extended_value = offset | 0b11111111111111110000000000000000;
    else
        *extended_value = offset & 0b00000000000000001111111111111111;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
        //determine which data operation is being performed on
        if(ALUSrc == 0b1)
            data2 = extended_value;
        
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

        //if ALUOp is R-type, set it's proper operator
        if(ALUOp == 0b111)
        {
            //printf("\nOp is Rtype, assigning\n");
            switch(funct)
            {
                case 004: //sllv
                    ALUOp = 0b110;
                    break;
                case 040: //add
                    ALUOp = 0b000;
                    break;
                case 042: //subtract
                    ALUOp = 0b001;
                    break;
                case 044: //AND
                    ALUOp = 0b100;
                    break;
                case 045: //OR
                    ALUOp = 0b101;
                    break;
                case 052: //slt
                    ALUOp = 0b010;
                    break;
                case 053: //sltu
                    ALUOp = 0b011;
                    break;
                default: //case doesn't exist (or was not implemented)
                    return 1;
            }
            //printf("\nNew op assigned\n");
        }
        //send to ALU
        ALU(data1, data2, ALUOp, ALUresult, Zero);

    //printf("made it to end of alu ops");
    
    //no errors
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //if MemWrite or MemRead are asserted and ALUresult isn't an address, HALT
    if((MemWrite == 0b01 || MemRead == 0b01) && ALUresult%4 != 0)
        return 1;

    //if writing
    if(MemWrite == 0b1)
        Mem[ALUresult>>2] = data2;
    
    //if reading
    if(MemRead == 0b1)
        *memdata = Mem[ALUresult>>2];
    
    //no errors
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //shift r2 16 bits and r3 11 bits
    r2>>=16;
    r3>>=11;

    //printf("\nmemdata: %d\nALUresult: %d\nRegWrite: %d\nRegDst: %d\nMemtoReg: %d\n", memdata, ALUresult, RegWrite, RegDst, MemtoReg);//#uncomment for debug
    
    //mem->reg
    if(MemtoReg == 0b1 && RegDst == 0b0)
        Reg[r2] = memdata;
    
    //mem->reg (r3)
    if(MemtoReg == 0b1 && RegDst == 0b1)
        Reg[r3] = memdata;
    
    //ALUresult->reg
    if(MemtoReg == 0b0 && RegDst == 0b0)
        Reg[r2] = ALUresult;

    //ALUresult->reg (r3)
    if(MemtoReg == 0b0 && RegDst == 0b1)
        Reg[r3] = ALUresult;

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //printf("\nold pc: %x\n", *PC);
    //inc PC
    *PC+=4;

    //if branch & zero, add ev
    if(Branch == 0b1 && Zero == 1)
    {
        //printf("\nbranching\n");
        *PC+=extended_value<<2;
    }

    //if jump shift instruction bitwise-OR w/ PC
    if(Jump == 0b1)
    {
        //printf("jumping");
        *PC = (jsec<<2) | (*PC & 0b11110000000000000000000000000000);
    }
    //printf("\nnew pc: %x\n", *PC);
}
