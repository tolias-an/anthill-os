#include <stdint.h>

#include "drivers/uart.h"
#include "drivers/emmc.h"
#include "drivers/irq.h"
#include "drivers/timer.h"

#include "kernel/fork.h"
#include "kernel/kprintf.h"
#include "kernel/scheduler.h"

extern unsigned char bss_end;
/*
* A process for testing purposes.
*/
void test_process(char *string) {
    while (1) {
        for (int i = 0; i < 5; i++) {
            uint32_t counter = 100000;

            uart_send_char(string[i]);
            while (counter--) {
                asm volatile("nop");
            }
        }
    }
}

void init_process() {
    preempt_disable();

    // Setup
    uart_init();
    emmc_init();

    preempt_enable();
    while (1) {;}
}

void main() {
    timer_init();
    enable_interrupt_controller();
    enable_irq();

    uint8_t err = move_to_user_mode((uintptr_t) &init_process);
    if (err) { kprintf("Error while starting init process\n"); }

    while (1) {
        schedule();
    }
}
