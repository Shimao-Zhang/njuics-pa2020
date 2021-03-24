#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
    uint32_t addr=cpu.segReg[sreg].base+offset;
	return addr;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	paddr_t offset=(paddr_t)(cpu.segReg[sreg].index*8+cpu.gdtr.base);
	SegDesc* get=(SegDesc*)hw_mem+offset;
	//memcpy(&get,hw_mem+offset,8);
	get->val[0]=laddr_read(offset,4);
	get->val[1]=laddr_read(offset+4,4);
    //printf("%x\n",cpu.segReg[sreg].index);
	//printf("%x\n",cpu.gdtr.base);
	//printf("%x\n",offset);
	//printf("%d\n",get.limit_15_0);
	//printf("%d\n",get.limit_19_16);
	//printf("%d\n",cpu.gdtr.limit);
	//fflush(stdout);
	assert(get->limit_19_16==0xf);
	assert(get->limit_15_0==0xffff);
	cpu.segReg[sreg].base=(get->base_15_0&0xffff)+((get->base_23_16&0xff)<<16)+((get->base_31_24&0xff)<<24);
	cpu.segReg[sreg].limit=(get->limit_15_0&0xffff)+((get->limit_19_16&0xf)<<16);
	cpu.segReg[sreg].type=(get->type&0xf)+((get->segment_type&0x1)<<4);
	cpu.segReg[sreg].privilege_level=get->privilege_level;
	cpu.segReg[sreg].soft_use=get->soft_use&0x1;
	//printf("%d\n",cpu.segReg[sreg].limit);
	assert(cpu.segReg[sreg].base==0);
	assert(cpu.segReg[sreg].limit==0xfffff);
	assert(get->granularity==1);
}
