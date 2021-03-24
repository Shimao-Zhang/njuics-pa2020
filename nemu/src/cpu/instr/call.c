#include "cpu/instr.h"
make_instr_func(call_near)
{
    OPERAND imm,called;
    imm.type=OPR_IMM;
    imm.addr=eip+1;
    //modrm_opcode_rm(cpu.eip+1,&opcode,&imm);
    imm.data_size=data_size;
    imm.sreg=SREG_CS;
    operand_read(&imm);
    called.type=OPR_MEM;
    called.data_size=data_size;
    called.addr=cpu.esp;
    called.val=cpu.eip+1+(data_size/8);
    cpu.esp-=data_size/8;
    called.addr=cpu.esp;
    called.sreg=SREG_SS;
    operand_write(&called);
    imm.val=sign_ext(imm.val,imm.data_size);
    return 1+data_size/8+imm.val;
}

make_instr_func(call_further)
{
    operand_read(&opr_src);
    uint8_t p=opr_src.val;
    OPERAND rm,called;
    int len=0;
    rm.type=OPR_MEM;
    rm.data_size=data_size;
    len+=modrm_opcode_rm(cpu.eip+1,&p,&rm);
    operand_read(&rm);
    called.type=OPR_MEM;
    called.data_size=data_size;
    called.val=cpu.eip+1+len;
    cpu.esp-=data_size/8;
    called.addr=cpu.esp;
    operand_write(&called);
    cpu.eip=rm.val;
    return 0;
}
