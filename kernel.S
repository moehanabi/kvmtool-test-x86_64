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
