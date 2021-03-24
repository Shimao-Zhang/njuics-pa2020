#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt)
{
    int len=1;
    opr_src.data_size=data_size;
    len=len+modrm_rm(eip+1,&opr_src);
    OPERAND a;
    a.data_size=16;
    a.type=OPR_MEM;
    a.sreg=opr_src.sreg;
    a.addr=opr_src.addr;
    operand_read(&a);
    cpu.gdtr.limit=a.val&0xffff;
    a.type=OPR_MEM;
    a.data_size=32;
    a.addr+=2;
    operand_read(&a);
    cpu.gdtr.base=a.val&0xffffffff;
    return len;
}