# Set up space for the stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
	.skip 16384 # 16 KiB
stack_top:

# The linker script specifies _start as the entry point to the kernel and the
# bootloader will jump to this position once the kernel has been loaded.
.section .text
	.global _start
	.type _start, @function
_start:
	# Initialize the stack.
	movl $stack_top, %esp

	# Call the C entry point with the arguments supplied by the bootloader.
	pushl %eax
	pushl %ebx
	call kernel_main

	# Hang indefinitely.
	cli
	.Lhang:
	hlt
	jmp .Lhang
	.size _start, . - _start
