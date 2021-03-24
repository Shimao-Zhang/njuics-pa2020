#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
make_instr_func(int_)
{
    opr_src.type=OPR_IMM;
    opr_src.data_size=8;
    opr_src.addr=eip+1;
    opr_src.sreg=SREG_CS;
    operand_read(&opr_src);
    raise_sw_intr((uint8_t)(opr_src.val&0xff));
    return 0;
}

