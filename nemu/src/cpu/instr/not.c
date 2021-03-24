#include "cpu/instr.h"
static void instr_execute_1op()
{
    operand_read(&opr_src);
    if(data_size==16)
        opr_src.val=alu_xor(opr_src.val,0xffff,data_size);
    else if(data_size==32)
        opr_src.val=alu_xor(opr_src.val,0xffffffff,data_size);
    operand_write(&opr_src);
}

make_instr_impl_1op(not,rm,b);
make_instr_impl_1op(not,rm,v);
