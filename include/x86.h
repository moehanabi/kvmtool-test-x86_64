#ifndef X86_H
#define X86_H

static inline void reboot(void) {
    __asm__ volatile(
        "mov	$0xfe, %al\n\t"
        "outb	%al, $0x64\n\t"
    );
}

#endif