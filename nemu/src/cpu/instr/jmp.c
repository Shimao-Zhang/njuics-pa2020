#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short)
{
    OPERAND rel;
    rel.type=OPR_IMM;
    rel.addr=eip+1;
    //modrm_rm(cpu.eip+1,&rel);
    rel.data_size=8;
    rel.sreg=SREG_CS;
    operand_read(&rel);
    rel.val=sign_ext(rel.val,rel.data_size);
    cpu.eip+=rel.val;
    return 2;
}

make_instr_func(jmp_near_indirect)
{
    OPERAND rm;
    rm.type=OPR_MEM;
    rm.data_size=data_size;
    modrm_rm(eip+1,&rm);
    operand_read(&rm);
    if(data_size==32)
    {
        cpu.eip=rm.val;
    }
    else if(data_size==16)
    {
        cpu.eip=rm.val&0xffff;
    }
    return 0;
}

make_instr_func(ljmp)
{
    OPERAND val;
    val.type=OPR_IMM;
    val.data_size=data_size;
    val.addr=eip+1;
    val.sreg=SREG_CS;
    operand_read(&val);
    switch(data_size)
    {
        case 8:
            val.val=val.val&0xff;
            break;
        case 16:
            val.val=val.val&0xffff;
            break;
        case 32:
            val.val=val.val&0xffffffff;
            break;
        default:
            val.val=val.val;
            break;
    }
    cpu.eip=val.val;
    switch(data_size)
    {
        case 8:
            val.addr=eip+2;
            break;
        case 16:
            val.addr=eip+3;
            break;
        case 32:
            val.addr=eip+5;
            break;
        default:
            val.addr=val.addr;
            break;
    }
    val.type=OPR_IMM;
    val.data_size=16;
    operand_read(&val);
    cpu.segReg[1].val=val.val;
    load_sreg(1);
    return 0;
}