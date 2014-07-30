#include "ISR.h"
#include "8259.h"
#include <stddef.h>

static
void null_ISR (uint32_t __attribute__ ((unused)) interrupt)
{
}


// Extern functions

ISR_t ISR_table [0x30];

void initialize_ISR_table (void)
{
	for (size_t i = 0; i < 0x30; ++i)
		ISR_table [i] = &null_ISR;
}

void ISR_entry (uint32_t interrupt)
{
	if (interrupt == 0x27 && !IRQ_in_service (7))
		return;

	(*ISR_table [interrupt]) (interrupt);

	if (0x20 <= interrupt && interrupt < 0x28)
		IRQ_EOI_master ();
	else if (0x28 <= interrupt)
		IRQ_EOI_slave ();
}
