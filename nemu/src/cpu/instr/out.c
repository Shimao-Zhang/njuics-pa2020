#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `out' instructions here.
*/
make_instr_func(out_b)
{
    pio_write(cpu.edx,1,cpu.eax);
    return 1;   
}

make_instr_func(out_v)
{
    switch(data_size)
    {
        case 8:
            pio_write(cpu.edx,1,cpu.eax);
            break;
        case 16:
            pio_write(cpu.edx,2,cpu.eax);
            break;
        case 32:
            pio_write(cpu.edx,4,cpu.eax);
            break;
        default:
            pio_write(cpu.edx,data_size/8,cpu.eax);
    }
    return 1;
}
