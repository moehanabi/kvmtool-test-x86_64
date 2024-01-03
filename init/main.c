#include <stdio.h>
#include <init.h>
#define SKIP_PROTECTED_MODE 0

extern uint64_t p4_table, p3_table, p2_table;

int main(void){
    kprintf("In Protected Mode\n");
    if (check_long_mode_support()) {
        kprintf("Long Mode Supported\n");
        init_paging_entries(&p4_table, &p3_table, &p2_table);
        SWITCH_TO_LONG_MODE(&p4_table);
        kprintf("In Long Mode\n");
    } else {
        kprintf("Long mode not supported\n");
    }
}
