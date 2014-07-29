#ifndef _8259_H
#define _8259_H

#include <stdint.h>

void remap_8259_PIC (uint8_t master_base, uint8_t slave_base);

#endif
