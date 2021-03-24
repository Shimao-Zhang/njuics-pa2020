#include "cpu/instr.h"
make_instr_func(pop_r_v)
{
    OPERAND top;
    top.type=OPR_MEM;
    top.addr=cpu.esp;
    top.data_size=data_size;
    top.sreg=SREG_SS;
    operand_read(&top);
    OPERAND back;
    back.type=OPR_REG;
    back.data_size=top.data_size;
    back.addr=opcode&0x7;
    back.sreg=SREG_DS;
    back.val=top.val;
    operand_write(&back);
    cpu.esp+=data_size/8;
    return 1;
}

//make_instr_impl_1op(pop,r,v);

make_instr_func(popa)
{
    opr_src.type=OPR_MEM;
    opr_src.data_size=data_size;
    opr_src.sreg=SREG_SS;
    opr_src.addr=cpu.esp;
    operand_read(&opr_src);
    cpu.edi=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.data_size=data_size;
    opr_src.sreg=SREG_SS;
    opr_src.addr=cpu.esp;
    operand_read(&opr_src);
    cpu.esi=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.data_size=32;
    opr_src.sreg=SREG_SS;
    opr_src.addr+=data_size/8;
    operand_read(&opr_src);
    cpu.ebp=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.data_size=32;
    opr_src.sreg=SREG_SS;
    opr_src.addr+=data_size/8;
    operand_read(&opr_src);
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.data_size=data_size;
    opr_src.sreg=SREG_SS;
    opr_src.addr=cpu.esp;
    operand_read(&opr_src);
    cpu.ebx=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.data_size=32;
    opr_src.sreg=SREG_SS;
    opr_src.addr+=data_size/8;
    operand_read(&opr_src);
    cpu.edx=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.type=OPR_MEM;
    opr_src.addr+=data_size/8;
    operand_read(&opr_src);
    cpu.ecx=opr_src.val;
    
    cpu.esp+=data_size/8;
    opr_src.addr+=data_size/8;
    operand_read(&opr_src);
    cpu.eax=opr_src.val;
    
    cpu.esp+=data_size/8;
    return 1;
}