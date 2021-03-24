#include "cpu/instr.h"
static void instr_execute_1op()
{
    operand_read(&opr_src);
    opr_src.val=alu_add(opr_src.val,0x00000001,opr_src.data_size);
    operand_write(&opr_src);
}

make_instr_impl_1op(inc,rm,v);
make_instr_impl_1op(inc,r,v);
