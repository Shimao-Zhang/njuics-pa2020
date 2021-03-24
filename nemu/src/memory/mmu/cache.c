#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include<time.h>
#include<stdlib.h>
#include<nemu.h>

CacheLine cacheline[1024];

// init the cache
void init_cache()
{
	for(int i=0;i<1024;++i)
	{
	    cacheline[i].validbit=0;
	    cacheline[i].sign=0;
	}
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	uint32_t addrin=paddr&0x3f;
	uint32_t group=(paddr>>6)&0x7f;
	uint32_t sign=(paddr>>13)&0x7ffff;
	for(int i=0;i<8;++i)
	{
	    if(cacheline[8*group+i].sign==sign)
	    {
	        if(cacheline[8*group+i].validbit)
	        {
	            if(addrin+len<=64)
	            {
	                /*for(int k=0;k<len;++k)
	                {
	                    cacheline[8*group+i].data[addrin+k]=data&0xff;
	                    data=data>>8;
	                }*/
	                memcpy(cacheline[8*group+i].data+addrin,&data,len);
	                memcpy(hw_mem+paddr,&data,len);
	                break;
	            }
	            else
	            {
	                size_t leave=addrin+len-64;
	                size_t before=len-leave;
	                paddr_t new_paddr=paddr+before;
	                memcpy(hw_mem+paddr,&data,len);
	                cache_write(paddr,before,data);
	                cache_write(new_paddr,leave,data>>(before*8));
	                break;
	            }
	        }
	    }
	    if(i==7)
	    {
	        for(int k=0;k<len;++k)
	        {
	            *(hw_mem+paddr+k)=data&0xff;
	            data=data>>8;
	        }
	        break;
	    }
	}
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t data=0;
	uint32_t addrin=paddr&0x3f;
	uint32_t group=(paddr>>6)&0x7f;
	uint32_t sign=(paddr>>13)&0x7ffff;
	for(int i=0;i<8;++i)
	{
	    if(cacheline[8*group+i].validbit)
	    {
	        if(cacheline[8*group+i].sign==sign)
	        {
	            if(addrin+len<=64)
	            {
	                for(int k=len-1;k>=0;--k)
	                {
	                    if(k!=(len-1))
	                        data=data<<8;
	                    data+=cacheline[8*group+i].data[addrin+k];
	                }
	                break;
	            }
	            else
	            {
	                size_t before=64-addrin;
	                size_t leave=len-before;
	                paddr_t new_paddr=paddr+before;
	                data=cache_read(paddr,before);
	                uint32_t w=cache_read(new_paddr,leave);
	                w=w<<(before*8);
	                data+=w;
	                break;
	            }
	        }
	    }
	    if(i==7)
	    {
	        for(int k=0;k<8;++k)
	        {
	            if(cacheline[8*group+k].validbit==0)
	            {
	                cacheline[8*group+k].validbit=1;
	                cacheline[8*group+k].sign=sign;
	                memcpy(cacheline[8*group+k].data,hw_mem+paddr-addrin,64);
	                data=cache_read(paddr,len);
	                break;
	            }
	            if(k==7)
	            {
	                srand(time(0));
	                uint32_t a=((uint32_t)rand())%8;
	                paddr_t m=paddr-addrin;
	                memcpy(cacheline[8*group+a].data,hw_mem+m,64);
	                cacheline[8*group+a].sign=sign;
	                cacheline[8*group+a].validbit=1;
	                data=cache_read(paddr,len);
	            }
	        }
	    }
	}
    return data;
}

