#include <stdio.h>
#include <pic.h>
#include <pit.h>

int main(void){
    kprintf("In Long Mode\n");
    pic_init(0x11, 0x20, 0x4, 0x3, 0x28);
    pic_unmask(0);
    pit_set(2, 0, 1000);
}
