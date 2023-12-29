.altmacro

.macro FILL_NOOP_IDT
LOCAL fill
// fill up noop handlers
	xorw	%ax, %ax
	xorw	%di, %di
	movw	%ax, %es
	movw	$256, %cx
fill:
	movw	$noop_handler, %es:(%di)
	movw	%cs, %es:2(%di)
	add	$4, %di
	loop	fill
.endm

.macro PRINT str_begin, str_end
	mov	$0x3f8,%dx
	cs lea	str_begin, %si
	mov	$(str_end-str_begin), %cx
	cs rep/outsb
.endm

.macro SETA20
LOCAL seta20.1, seta20.2
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
.endm

.macro PROTECT_MODE
	lgdt	gdtdesc32
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0

	ljmp	$(SEG_KCODE<<3), $start32
.endm

.macro START32
start32:
	# Set up the protected-mode data segment registers
	movw	$(SEG_KDATA<<3), %ax	# Our data segment selector
	movw	%ax, %ds				# -> DS: Data Segment
	movw	%ax, %es				# -> ES: Extra Segment
	movw	%ax, %ss				# -> SS: Stack Segment
	movw	$0, %ax					# Zero segments not ready for use
	movw	%ax, %fs				# -> FS
	movw	%ax, %gs				# -> GS
.endm

.macro SETUP_SSE
LOCAL check_sse, enable_sse
	// Check for SSE and enable it. If it's not supported, reboot
check_sse:
	movl	$0x1, %eax
	cpuid
	testl	$1<<25, %edx
	jnz	enable_sse
	REBOOT

enable_sse:
	movl	%cr0, %eax
	andw	$0xFFFB, %ax	// Clear coprocessor emulation CR0.EM
	orw	$0x2, %ax		// Set coprocessor monitoring CR0.MP
	movl	%eax, %cr0
	movl	%cr4, %eax
	orw	$3 << 9, %ax	// set CR4.OSFXSR and CR4.OSXMMEXCPT
						// at the same time
	movl	%eax, %cr4
.endm

.macro REBOOT
	/* Reboot by using the i8042 reboot line */
	mov	$0xfe, %al
	outb	%al, $0x64
.endm

.macro JUST_LOOP
LOCAL inf_loop
inf_loop:
	jmp	inf_loop
.endm
