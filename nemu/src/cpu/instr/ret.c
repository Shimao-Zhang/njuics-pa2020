#include "cpu/instr.h"
make_instr_func(ret_near)
{
    OPERAND top;
    top.type=OPR_MEM;
    top.data_size=data_size;
    top.addr=cpu.esp;
    top.sreg=SREG_SS;
    operand_read(&top);
    if(data_size==32)
    {
        cpu.eip=top.val;
        cpu.esp+=0x4;
    }
    if(data_size==16)
    {
        cpu.esp+=0x2;
        cpu.eip=top.val&0xffff;
    }
    return 0;
}

make_instr_func(ret_near_imm16)
{
    OPERAND imm,top;
    imm.type=OPR_IMM;
    imm.addr=eip+1;
    imm.data_size=16;
    operand_read(&imm);
    top.type=OPR_MEM;
    top.data_size=data_size;
    top.addr=cpu.esp;
    top.sreg=SREG_SS;
    operand_read(&top);
    imm.val=sign_ext(imm.val,16);
    cpu.esp+=alu_add(imm.val,data_size/8,32);
    cpu.eip=top.val;
    return 0;
}
