#include "cpu/instr.h"

static void instr_execute_1op()
{
    OPERAND get;
    operand_read(&opr_src);
    cpu.esp=cpu.esp-data_size/8;
    get.type=OPR_MEM;
    get.addr=cpu.esp;
    get.val=opr_src.val;
    get.data_size=data_size;
    get.sreg=SREG_SS;
    operand_write(&get);
}

make_instr_impl_1op(push,i,b);
make_instr_impl_1op(push,i,v);
make_instr_impl_1op(push,r,v);
make_instr_impl_1op(push,rm,v);

make_instr_func(pusha)
{
    OPERAND a;
    a.val=cpu.esp;
    a.data_size=32;
    
    cpu.esp-=4;
    opr_dest.data_size=32;
    opr_dest.type=OPR_MEM;
    opr_dest.addr=cpu.esp;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.eax;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.data_size=32;
    opr_dest.addr=cpu.esp;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.ecx;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.addr=cpu.esp;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.edx;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.data_size=data_size;
    opr_dest.addr=cpu.esp;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.ebx;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.data_size=data_size;
    opr_dest.addr=cpu.esp;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=a.val;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.data_size=data_size;
    opr_dest.addr-=4;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.ebp;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.type=OPR_MEM;
    opr_dest.addr-=4;
    opr_dest.sreg=SREG_SS;
    opr_dest.val=cpu.esi;
    operand_write(&opr_dest);
    
    cpu.esp-=4;
    opr_dest.addr-=4;
    opr_dest.val=cpu.edi;
    operand_write(&opr_dest);
    return 1;
}