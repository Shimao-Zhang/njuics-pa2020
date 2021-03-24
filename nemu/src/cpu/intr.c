#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	OPERAND get;
	get.type=OPR_MEM;
	get.data_size=32;
	get.sreg=SREG_SS;
	get.val=cpu.eflags.val&0xffffffff;
	cpu.esp-=get.data_size/8;
	get.addr=cpu.esp;
	operand_write(&get);
	get.data_size=16;
	get.val=cpu.cs.val&0xffff;
	cpu.cs.val=cpu.cs.val&0xffff;
	cpu.esp-=get.data_size/8;
	get.addr=cpu.esp;
	vaddr_write(cpu.esp,SREG_DS,get.data_size/8,cpu.cs.val);
	//operand_write(&get);
	get.data_size=32;
	get.val=cpu.eip;
    	//if(cpu.eip<=0x30000)
            //printf("\n%x\n",cpu.eip);
	assert(cpu.eip>=0x30000);
	cpu.esp-=get.data_size/8;
	get.addr=cpu.esp;
	assert(get.val>=0x30000);
	operand_write(&get);
	
	GateDesc gatedesc;
	paddr_t addr=(uint32_t)hw_mem+page_translate(segment_translate(cpu.idtr.base+8*intr_no,SREG_DS));
	memcpy(gatedesc.val,(void*)addr,sizeof(gatedesc));
	if(gatedesc.type==0xd)
    	{
	    cpu.eflags.IF=0;
    	}
    	if(gatedesc.type==0xe)
    	{
            cpu.eflags.IF=1;
   	}
	cpu.eip=(gatedesc.offset_31_16<<16)|(gatedesc.offset_15_0);
	assert(cpu.eip>=0x30000);
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
