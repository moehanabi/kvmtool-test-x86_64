#include <asm.h>
#include <mmu.h>
#include <init.h>
#define IO_PIC		0x20
#define IRQ_OFFSET	32
#define IO_PIT		0x40
#define TIMER_FREQ	1193182
#define TIMER_DIV(x)	((TIMER_FREQ+(x)/2)/(x))

	.code16gcc
	.section .text.entry
	.globl	_start
	.type	_start, @function
_start:
	FILL_NOOP_IDT
	PRINT real_mode_msg, real_mode_msg_end
	SETA20
	PROTECT_MODE

.code32	# Tell assembler to generate 32-bit code now.
	START32
	SETUP_SSE
	# Set up the stack pointer and call into C.
	movl	$_start, %esp
	call	main
	REBOOT
	JUST_LOOP

noop_handler:
	iretw

	.section .rodata
	.align	4
real_mode_msg:
	.asciz "\nIn Real Mode\n"
real_mode_msg_end:

// Global Descriptor Table
gdt32:
	SEG_NULLASM	# null seg
	SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
	SEG_ASM(STA_W, 0x0, 0xffffffff)	# data seg

gdtdesc32:
	.word	(gdtdesc32 - gdt32 - 1)
	.long	gdt32

gdt64:
	.quad	0 // zero entry

gdt64_code_entry:
	.set	gdt64_code_seg, gdt64_code_entry - gdt64
	.quad	(1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) // code segment

gdt64_data_entry:
	.set	gdt64_data_seg, gdt64_data_entry - gdt64
	.quad	(1<<44) | (1<<47) | (1<<41) // data segment

gdt64_pointer:
	.set	gdt64_limit, gdt64_pointer - gdt64 - 1
	.word	gdt64_limit
	.quad	gdt64
