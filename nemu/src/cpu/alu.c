#include "cpu/cpu.h"

void set_ZF(uint32_t result,size_t data_size)
{
    result=result&(0xFFFFFFFF >>(32-data_size));
    cpu.eflags.ZF=(result==0);
}

void set_CF_add(uint32_t result, uint32_t src, size_t data_size)
{
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF=result<src;
}

void set_CF_adc(uint32_t result,uint32_t src,size_t data_size)
{
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    if(cpu.eflags.CF==0)
    {
        cpu.eflags.CF=result<src;
    }
    else if(cpu.eflags.CF==1)
    {
        cpu.eflags.CF=result<=src;
    }
}

void set_CF_sub(uint32_t result,uint32_t src,size_t data_size)
{
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF=result>src;
}

void set_CF_sbb(uint32_t result,uint32_t src,size_t data_size)
{
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    if(cpu.eflags.CF==0)
        cpu.eflags.CF=(result>src);
    else
        cpu.eflags.CF=(result>=src);
}

void set_CF_shl(uint32_t src,uint32_t dest,size_t data_size)
{
    uint32_t a=0;
    switch(data_size)
    {
        case 8:
            for(int i=1;i<=src;++i)
            {
                a=dest&0x80;
                if(a!=0&&i==src)
                {
                    cpu.eflags.CF=1;
                    break;
                }
                else
                {
                    dest=dest<<1;   
                    cpu.eflags.CF=0;
                }
            }
            break;
        case 16:
            for(int i=1;i<=src;++i)
            {
                a=dest&0x8000;
                if(a!=0&&i==src)
                {
                    cpu.eflags.CF=1;
                    break;
                }
                else
                {
                    dest=dest<<1;
                    cpu.eflags.CF=0;
                }
            }
            break;
        default:
            for(int i=1;i<=src;++i)
            {
                if(sign(dest)==1&&i==src)
                    cpu.eflags.CF=1;
                else
                {
                    dest=dest<<1;
                    cpu.eflags.CF=0;
                }
            }
            break;
    }
}

void set_CF_shr(uint32_t src,uint32_t dest,size_t data_size)
{
    uint32_t a=0;
    for(int i=1;i<=src;++i)
    {
        a=dest&0x1;
        if(a==1&&i==src)
            cpu.eflags.CF=1;
        else
        {
            dest=dest>>1;
            cpu.eflags.CF=0;
        }
    }
}

void set_CF_sar(uint32_t src,uint32_t dest,size_t data_size)
{
    uint32_t a=0;
    switch(data_size)
	{
	    case 8:
	        for(int i=1;i<=src;++i)
	        {
	           cpu.eflags.CF=dest&0x1;
	           a=dest&0x80;
	           if(a>0)
	           {
	               dest=dest>>1;
	               dest=dest|0x80;
	           }
	           else
	               dest=dest>>1;
	        }
	        break;
	   case 16:
	        for(int i=1;i<=src;++i)
	        {
	            cpu.eflags.CF=dest&0x1;
	            a=dest&0x8000;
	            if(a>0)
	            {
	                dest=dest>>1;
	                dest=dest|0x8000;
	            }
	            else
	                dest=dest>>1;
	        }
	        break;
	   default:
	        for(int i=1;i<=src;++i)
	        {
	            cpu.eflags.CF=dest&0x1;
	            a=dest&0x80000000;
	            if(a>0)
	            {
	                dest=dest>>1;
	                dest=dest|0x80000000;
	            }
	            else
	                dest=dest>>1;
	        }
	        break;
	}
}

void set_CF_and_OF_mul(uint64_t res,size_t data_size)
{
    switch(data_size)
    {
        case 8:
            if((res&0xFF00)==0)
            {
                cpu.eflags.CF=0;
                cpu.eflags.OF=0;
            }
            else
            {
                cpu.eflags.CF=1;
                cpu.eflags.OF=1;
            }
            break;
        case 16:
            if((res&0xFFFF0000)==0)
            {
                cpu.eflags.CF=0;
                cpu.eflags.OF=0;
            }
            else
            {
                cpu.eflags.CF=1;
                cpu.eflags.OF=1;
            }
            break;
        case 32:
            if((res&0xFFFFFFFF00000000)==0)
            {
                cpu.eflags.CF=0;
                cpu.eflags.OF=0;
            }
            else
            {
                cpu.eflags.CF=1;
                cpu.eflags.OF=1;
            }
            break;
    }
}

void set_SF(uint32_t result,size_t data_size)
{
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.SF=sign(result);
}

void set_PF(uint32_t result)
{
    int a=0;
    for(int i=1;i<=8;++i)
    {
        if((result % 2)!= 0)
            ++a;
        result=result>>1;
    }
    if(a%2==0)
        cpu.eflags.PF=1;
    else
        cpu.eflags.PF=0;
}

void set_OF_add(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result=sign_ext(result&0xFF,8);
            src=sign_ext(src&0xFF,8);
            dest=sign_ext(dest&0xFF,8);
            break;
        case 16:
            result=sign_ext(result&0xFFFF,16);
            src=sign_ext(src&0xFFFF,16);
            dest=sign_ext(dest&0xFFFF,16);
            break;
        default:
            break;
    }
    if(sign(dest)==sign(src))
    {
        if(sign(src)!=sign(result))
            cpu.eflags.OF=1;
        else
            cpu.eflags.OF=0;
    }
    else
        cpu.eflags.OF=0;
}

void set_OF_adc(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result=sign_ext(result&0xFF,8);
            src=sign_ext(src&0xFF,8);
            dest=sign_ext(dest&0xFF,8);
            break;
        case 16:
            result=sign_ext(result&0xFFFF,16);
            src=sign_ext(src&0xFFFF,16);
            dest=sign_ext(dest&0xFFFF,16);
            break;
        default:
            break;
    }
    if(sign(dest)==sign(src))
    {
        if(sign(src)!=sign(result))
            cpu.eflags.OF=1;
        else
            cpu.eflags.OF=0;
    }
    else
        cpu.eflags.OF=0;
}

void set_OF_sub(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result=sign_ext(result&0xFF,8);
            src=sign_ext(src&0xFF,8);
            dest=sign_ext(dest&0xFF,8);
            break;
        case 16:
            result=sign_ext(result&0xFFFF,16);
            src=sign_ext(src&0xFFFF,16);
            dest=sign_ext(dest&0xFFFF,16);
            break;
        default:
            break;
    }
    if(sign(src)==sign(dest))
        cpu.eflags.OF=0;
    else
        {
            if(sign(result)!=sign(dest))
                cpu.eflags.OF=1;
            else
                cpu.eflags.OF=0;
        }
}

void set_OF_sbb(uint32_t result,uint32_t src,uint32_t dest,size_t data_size)
{
    switch(data_size)
    {
        case 8:
            result=sign_ext(result&0xFF,8);
            src=sign_ext(src&0xFF,8);
            dest=sign_ext(dest&0xFF,8);
            break;
        case 16:
            result=sign_ext(result&0xFFFF,16);
            src=sign_ext(src&0xFFFF,16);
            dest=sign_ext(dest&0xFFFF,16);
            break;
        default:
            break;
    }    
    if(sign(dest)==sign(src))
        cpu.eflags.OF=0;
    else
    {
        if(sign(dest)!=sign(result))
            cpu.eflags.OF=1;
        else
            cpu.eflags.OF=0;
    }
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest+src;
	
	set_CF_add(res,src,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_OF_add(res,src,dest,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest+src+cpu.eflags.CF;
	
	set_CF_adc(res,src,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_OF_adc(res,src,dest,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest-src;
	
	set_CF_sub(res,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_OF_sub(res,src,dest,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest-src-cpu.eflags.CF;
	
	set_OF_sbb(res,src,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	set_CF_sbb(res,dest,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	uint64_t res=0;
	res=(uint64_t)dest*src;
	
	set_CF_and_OF_mul(res,data_size);
	
	return res;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	uint64_t res=0;
	res=(int64_t)dest*src;
	
	return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	uint32_t res=0;
	if(src!=0)
	    res=dest/src;
	else
	{
	    printf("%s/n","Floating Point Exception");
	    return 0;
	}
	
	return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	int32_t res=0;
	if(src!=0)
	    res=(int32_t)dest/src;
	else
	{
	    printf("%s/n","Floating Point Exception");
	    return 0;
	}
	
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	uint32_t res=0;
	res=dest%src;
	
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	int32_t res=0;
	res=(int32_t)dest%src;
	
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest&src;
	
	cpu.eflags.CF=0;
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	cpu.eflags.OF=0;
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest^src;
	
	cpu.eflags.CF=0;
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	cpu.eflags.OF=0;
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest|src;
	
	cpu.eflags.CF=0;
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	cpu.eflags.OF=0;
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest<<src;
	
	set_CF_shl(src,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	uint32_t res=0;
	dest=dest&(0xFFFFFFFF>>(32-data_size));  //avoid the influence of higher bits
	res=dest>>src;
	
	set_CF_shr(src,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	uint32_t res=0;
	uint32_t a=0;
	res=dest&(0xFFFFFFFF>>(32-data_size));
	switch(data_size)
	{
	    case 8:
	        for(int i=1;i<=src;++i)
	        {
	            a=res&0x80;
	            if(a>0)
	            {
	                res=res>>1;
	                res=res|0x80;
	            }
	            else
	                res=res>>1;
	        }
	        break;
	   case 16:
	        for(int i=1;i<=src;++i)
	        {
	            a=res&0x8000;
	            if(a>0)
	            {
	                res=res>>1;
	                res=res|0x8000;
	            }
	            else
	                res=res>>1;
	        }
	        break;
	   default:
	        for(int i=1;i<=src;++i)
	        {
	            a=res&0x80000000;
	            if(a>0)
	            {
	                res=res>>1;
	                res=res|0x80000000;
	            }
	            else
	                res=res>>1;
	        }
	        break;
	}
	
	set_CF_sar(src,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	uint32_t res=0;
	res=dest<<src;
	
	set_CF_shl(src,dest,data_size);
	set_PF(res);
	set_ZF(res,data_size);
	set_SF(res,data_size);
	
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}
