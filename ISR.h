#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef void (*ISR_t) (uint32_t);
extern ISR_t ISR_table [0x30];

void ISR_table_initialize (ISR_t default_ISR);

#endif
