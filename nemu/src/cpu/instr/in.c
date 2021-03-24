#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `in' instructions here.
*/
make_instr_func(in_b)
{
    cpu.eax=pio_read(cpu.edx,1);
    return 1;
}

make_instr_func(in_v)
{
    switch(data_size)
    {
        case 8:
            cpu.eax=pio_read(cpu.edx,1);
            break;
        case 16:
            cpu.eax=pio_read(cpu.edx,2);
            break;
        case 32:
            cpu.eax=pio_read(cpu.edx,4);
            break;
        default:
            cpu.eax=pio_read(cpu.edx,data_size/8);
            break;
    }
    return 1;
}
