#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	//printf("\nPlease implement page_translate()\n");
	//fflush(stdout);
	//assert(0);
	uint32_t dir=(laddr>>22)&0x3ff;
	uint32_t page=(laddr&0x3ff000)>>12;
	uint32_t offset=laddr&0xfff;
	PDE pde;
	uint32_t addr_pde=(cpu.cr3.pdir<<12)+4*dir;
	memcpy(&pde.val,(void*)(hw_mem+addr_pde),4);
	if(pde.present==0)
	{
	    printf("\n%x\n",cpu.eip);
	}
    assert(pde.present==1);
	uint32_t addr_pte=(pde.page_frame<<12)+page*4;
	PTE pte;
	memcpy(&pte.val,(void*)(hw_mem+addr_pte),4);
	assert(pte.present==1);
	return (pte.page_frame<<12)+offset;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
