#ifdef __ASSEMBLER__
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

#endif

#ifndef __ASSEMBLER__
#include <stdio.h>
#include <mmu.h>
static inline int check_long_mode_support() {
	int eax, edx, max;

	/* check maximum extended function number */
	__asm__ __volatile__ (
		"mov $0x80000000, %%eax\n\t"
		"cpuid\n\t"
		: "=a"(max)
		: 
		: "bx", "cx", "dx"
	);
	if (max < 0x80000001) {
		kprintf("Extended function not supported. Can not enter long mode.\n");
		return 0;
	}

	/* check long mode support (the 29th bit in EDX) */
	__asm__ __volatile__ (
		"mov $0x80000001, %%eax\n\t"
		"cpuid\n\t"
		: "=a"(eax), "=d"(edx)
		:
		: "cc"
	);
	return edx & (1 << 29);
}

static inline void init_paging_entries(uint64_t* p4_table, uint64_t* p3_table, uint64_t* p2_table) {
	uint64_t flag = PTE_P | PTE_W;	// present + writable
	uint64_t flag_huge = PTE_PS | PTE_P | PTE_W;	// huge + present + writable
	p4_table[511] = ((uint64_t)p4_table) | flag;
	p4_table[0] = ((uint64_t)p3_table) | flag;
	p3_table[0] = ((uint64_t)p2_table) | flag;

	// Map every P2 entry to a 2MiB huge page
	for(int i = 0; i < 512; i++) {
		p2_table[i] = (i * 0x200000) | flag_huge;
	}
}

#define ENABLE_LONG_IN_EFER(void) ({\
	__asm__ __volatile__ (\
		"mov $0xC0000080, %%ecx\n\t" \
		"rdmsr\n\t" \
		"orl $1 << 8, %%eax\n\t" \
		"wrmsr\n\t" \
		: \
		: \
		: "eax", "ecx", "edx" \
	);\
})

#define READ_CRx(_x) ({\
	uint64_t _cr;\
	__asm__ __volatile__ (\
		"mov %%cr" #_x ", %0"\
		: "=r"(_cr)\
	);\
	_cr;\
})

#define WRITE_CRx(_x, _value) do {\
	__asm__ __volatile__ (\
		"mov %0, %%cr" #_x ""\
		:\
		: "r"(_value)\
	);\
} while(0)

#define SWITCH_TO_LONG_MODE(_p4_table_ptr) do { \
	uint64_t _cr0,\
			 _cr4;\
	WRITE_CRx(3, (uint64_t)_p4_table_ptr);\
	_cr4 = READ_CRx(4);\
	_cr4 |= CR4_PAE;\
	WRITE_CRx(4, _cr4);\
	ENABLE_LONG_IN_EFER();\
	_cr0 = READ_CRx(0);\
	_cr0 |= CR0_PG;\
	WRITE_CRx(0, _cr0);\
} while(0)
#endif
