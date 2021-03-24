#include "cpu/instr.h"
make_instr_func(leave)
{
    OPERAND back;
    cpu.esp=cpu.ebp;
    back.type=OPR_MEM;
    back.addr=cpu.ebp;
    back.sreg=SREG_SS;
    back.data_size=data_size;
    operand_read(&back);
    cpu.esp+=data_size/8;
    cpu.ebp=back.val;
    return 1;
}
