#include "asm.h"
#include "mmu.h"
#define IO_PIC		0x20
#define IRQ_OFFSET	32
#define IO_PIT		0x40
#define TIMER_FREQ	1193182
#define TIMER_DIV(x)	((TIMER_FREQ+(x)/2)/(x))

	.code16gcc
	.text
	.globl	_start
	.type	_start, @function
_start:
/*
 * fill up noop handlers
 */
	xorw	%ax, %ax
	xorw	%di, %di
	movw	%ax, %es
	movw	$256, %cx
fill_noop_idt:
	movw	$noop_handler, %es:(%di)
	movw	%cs, %es:2(%di)
	add	$4, %di
	loop	fill_noop_idt

seta20.1:
	inb	$0x64,%al
	testb	$0x2,%al
	jnz	seta20.1 

	movb	$0xd1,%al
	outb	%al,$0x64

seta20.2:
	inb	$0x64,%al
	testb	$0x2,%al
	jnz	seta20.2

	movb	$0xdf,%al
	outb	%al,$0x60

protected_mode:
	lgdt	gdtdesc32
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0

test_ok:
	mov	$0x3f8,%dx
	cs lea	msg2, %si
	mov	$(msg2_end-msg2), %cx
	cs rep/outsb

	/* Reboot by using the i8042 reboot line */
	mov	$0xfe, %al
	outb	%al, $0x64

loop:
	1:
	jmp	1b

noop_handler:
	iretw

msg2:
	.asciz "\nTest OK\n"
msg2_end:


	// Global Descriptor Table
	.section .rodata
	.align	4
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
