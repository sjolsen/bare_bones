.section .text.isr

	# Do-nothing interrupt handler
	.global null_ISR
	.type null_ISR, @function
null_ISR:
	iret
	.size null_ISR, . - null_ISR

	# Declare an interrupt handler delegating to C
	.macro .isr isrname cname
	.global \isrname
	.type \isrname, @function
\isrname:
	pushal
	call cname
	popal
	iret
	.size \isrname, . - \isrname
	.endm
