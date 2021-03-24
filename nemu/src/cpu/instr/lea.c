#include "cpu/instr.h"
/*make_instr_func(lea)
{
    OPERAND rm,r;
    int len=1;
    rm.data_size=data_size;
    r.data_size=data_size;
    len+=modrm_r_rm(cpu.eip+1,&r,&rm);
    operand_read(&rm);
    r.val=rm.addr;
    operand_write(&r);
    return len;
}*/
static void instr_execute_2op() 
{
	opr_dest.val = opr_src.addr;
	operand_write(&opr_dest);
}

make_instr_impl_2op(lea, rm, r, v);