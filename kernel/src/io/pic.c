#include "io/pic.h"

//all of this is in little endian
void pic_remap(void) {
    //initialize PIC (ICW1) tells cpu start initialization
    outb(MASTER_PIC_CTRL_ADDR, 0b00010001);//see icw1 bits meaning
    outb(SLAVE_PIC_CTRL_ADDR, 0b00010001);//see icw1 bits meaning

    //set offset cause cpu uses base PIC interupt addresses for cpu interupts. offset 20 is the norm (ICW2)
    outb(MASTER_PIC_CTRL_DATA, 0x20);   // ICW2: master offset = 0x20
    outb(SLAVE_PIC_CTRL_DATA, 0x28);   // ICW2: slave offset = 0x28

    // Tell master/slave wiring (ICW3)
    outb(MASTER_PIC_CTRL_DATA, 0b00000100);//00000100 PIC has 8 connections. this tells the master PIC which connection has a slave on it. it is physical wires dependant called IRQ0-IRQ7 mapped to which bit 
    outb(SLAVE_PIC_CTRL_DATA, 0x02);//2 in binary. literally requires the number of the IRQ it's connected to

    //ICW4
    outb(MASTER_PIC_CTRL_DATA, 0b00000001);//bit 0 set to 1 means we are on x84 cpu cause this shit was used for older cpus
    outb(SLAVE_PIC_CTRL_DATA, 0b00000001);//same

    // Mask all except keyboard (IRQ1)
    outb(MASTER_PIC_CTRL_DATA, 0b11111101);//see x86 Assembly/Programmable Interrupt Controller
    outb(SLAVE_PIC_CTRL_DATA, 0b11111111);//see x86 Assembly/Programmable Interrupt Controller
}