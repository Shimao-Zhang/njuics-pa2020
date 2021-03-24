#include "cpu/instr.h"
/*
Put the implementations of `not' instructions here.
*/
static void instr_execute_1op()
{
    opr_src.data_size=data_size;
    operand_read(&opr_src);
    if(opr_src.val)
        cpu.eflags.CF=1;
    else
        cpu.eflags.CF=0;
    opr_src.val=-opr_src.val;
    operand_write(&opr_src);
}

make_instr_impl_1op(neg,rm,b);
make_instr_impl_1op(neg,rm,v);
