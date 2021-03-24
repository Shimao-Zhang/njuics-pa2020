#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/
make_instr_func(iret)
{
    OPERAND get;
    get.data_size=32;
    get.type=OPR_MEM;
    get.addr=cpu.esp;
    get.sreg=SREG_SS;
    operand_read(&get);
    assert(cpu.eip>=0x30000);
    cpu.eip=get.val&0xffffffff;
    assert(cpu.eip>=0x30000);
    cpu.esp+=get.data_size/8;
    get.data_size=16;
    get.addr=cpu.esp;
    operand_read(&get);
    cpu.cs.val=vaddr_read(cpu.esp,SREG_SS,get.data_size/8);
    cpu.cs.val=get.val&0xffff;
    cpu.esp+=get.data_size/8;
    get.data_size=32;
    get.addr=cpu.esp;
    operand_read(&get);
    cpu.eflags.val=get.val&0xffffffff;
    cpu.esp+=get.data_size/8;
    return 0;
}
