#ifndef IO_H
#define IO_H

static inline unsigned char inb(unsigned short port)
{
	unsigned char val;
	__asm__ __volatile__("inb %w1, %0":"=a"(val):"Nd"(port));
	return val;
}

static inline void outb(unsigned short port, unsigned char val)
{
	__asm__ volatile ("outb %0, %1"::"a" (val), "d"(port));
}

#endif
