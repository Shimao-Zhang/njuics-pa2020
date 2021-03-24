#include "nemu.h"
#include "cpu/cpu.h"
#include "cpu/instr.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifdef CACHE_ENABLED
    ret=cache_read(paddr,len);
#else
#ifndef HAS_DEVICE_VGA
	ret = hw_mem_read(paddr, len);
#else
    if(is_mmio(paddr)!=-1)
        ret=mmio_read(paddr,len,is_mmio(paddr));
    else
        ret=hw_mem_read(paddr,len);
#endif
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef CACHE_ENABLED
    cache_write(paddr,len,data);
#else
#ifndef HAS_DEVICE_VGA
	hw_mem_write(paddr, len, data);
#else
    if(is_mmio(paddr)!=-1)
        mmio_write(paddr,len,data,is_mmio(paddr));
    else
        hw_mem_write(paddr,len,data);
#endif
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
    assert(len==1||len==2||len==4);
    //printf("\n%d\n%d\n",cpu.cr0.pe,cpu.cr0.pg);
    /*if(cpu.cr0.pe==0)
    {
        printf("\n%d\n",cpu.eip);
    }*/
	if(cpu.cr0.pe&&cpu.cr0.pg)
	{
	    if(laddr+len-1>(laddr&0xfffff000)+0x1000-1)
	    {
	        laddr_t newladdr=(laddr&0xfffff000)+0x1000;
	        size_t newlen=laddr+len-1-((laddr&0xfffff000)+0x1000-1);
	        paddr_t hwaddr=page_translate(laddr);
	        paddr_t newhwaddr=page_translate(newladdr);
	        uint32_t final=0;
	        final=paddr_read(hwaddr,len-newlen);
	        final+=(paddr_read(newhwaddr,newlen)<<((len-newlen)*8));
	        return final;
	    }
	    else
	    {
	        paddr_t hwaddr=page_translate(laddr);
	        return paddr_read(hwaddr,len);
        }
	}
	else
	    return paddr_read(laddr,len);
	return paddr_read(laddr,len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
    assert(len==1||len==2||len==4);
    //printf("%d\n%d\n",cpu.cr0.pe,cpu.cr0.pg);
	if(cpu.cr0.pe&&cpu.cr0.pg)
	{
	    if(laddr+len-1>(laddr&0xfffff000)+0x1000-1)
	    {
	        laddr_t newladdr=(laddr&0xfffff000)+0x1000;
	        size_t newlen=laddr+len-1-((laddr&0xfffff000)+0x1000-1);
	        paddr_t hwaddr=page_translate(laddr);
	        paddr_t newhwaddr=page_translate(newladdr);
	        paddr_write(hwaddr,len-newlen,((data<<8*(newlen))>>(8*newlen)));
	        paddr_write(newhwaddr,newlen,(data>>(8*(len-newlen))));
	    }
	    else
	    {
	        paddr_t hwaddr=page_translate(laddr);
	        paddr_write(hwaddr,len,data);
	    }
	}
	else
	    paddr_write(laddr,len,data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
    uint32_t laddr=vaddr;
    if(cpu.cr0.pe)
    {
        laddr=segment_translate(vaddr,sreg);
    }
    return laddr_read(laddr,len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	laddr_write(vaddr, len, data);
#else
    uint32_t laddr=vaddr;
    if(cpu.cr0.pe)
    {
        laddr=segment_translate(vaddr,sreg);
    }
    laddr_write(laddr,len,data);
#endif
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);

#ifdef CACHE_ENABLED
    init_cache();
#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
