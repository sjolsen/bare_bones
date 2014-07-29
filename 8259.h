#ifndef _8259_H
#define _8259_H

#include <stdint.h>
#include <stdbool.h>

void remap_8259_PIC (uint8_t master_base, uint8_t slave_base);
void IRQ_disable (uint8_t IRQ);
void IRQ_enable (uint8_t IRQ);
bool IRQ_requested (uint8_t IRQ);
bool IRQ_in_service (uint8_t IRQ);

#endif
