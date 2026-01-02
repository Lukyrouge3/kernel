#pragma once

#include "io/io.h"

#define MASTER_PIC_CTRL_ADDR 0x20
#define MASTER_PIC_CTRL_DATA 0x21
#define SLAVE_PIC_CTRL_ADDR 0xA0
#define SLAVE_PIC_CTRL_DATA 0xA1

void pic_remap(void);