#include <kernel/task.h>

#include <stddef.h>

#include <boot/entry.h>
#include <drivers/irq.h>
#include <kernel/cpu_context.h>
#include <kernel/elf.h>
#include <kernel/kprintf.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/ramdisk.h>

static struct task init_task = { .priority = 1};
struct task *current_task = &init_task;
struct task *tasks[TOTAL_TASKS] = {&init_task, };

Elf64_Ehdr *linker_page;

void task_tick(void) {
    current_task->counter--;
    if (current_task->counter > 0 || current_task->preempt_count > 0) {
        return;
    }
    enable_irq();
    task_schedule();
    disable_irq();
}

/* Load _ehdr_ in _address_ */
static void _task_load(const Elf64_Ehdr *ehdr, const void *address) {
    const Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    size_t phdr_count;

    for (phdr_count = ehdr->e_phnum; phdr_count--; phdr++) {
        if (phdr->p_type != PT_LOAD && phdr->p_type != PT_DYNAMIC)
            continue;

        memcpy((void *) ELF_OFF(address, phdr->p_vaddr),
               (void *) ELF_OFF(ehdr, phdr->p_offset),
               phdr->p_filesz);

        /* Zero bss segment */
        const unsigned long size_diff = phdr->p_memsz - phdr->p_filesz;
        if (size_diff)
            memset((void *) (ELF_OFF(address, phdr->p_vaddr) + phdr->p_filesz), 0, size_diff);
    }
}

short task_exec(const void *file) {
    current_task->preempt_count++;

    /* Load linker if not already loaded*/
    if (!linker_page) {
        const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) ramdisk_lookup("./lib/ld.so");

        /* Get pages */
        linker_page = get_free_pages(elf_get_image_size(ehdr));
        if (!linker_page) {
            kprintf("Error loading linker\n");
            return TASK_INIT_ERROR;
        };

        _task_load(ehdr, linker_page);
    }

    const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) file;
    const unsigned long elf_memory_size = elf_get_image_size(ehdr);
    const unsigned long process_size = elf_memory_size + sizeof(struct task);

    const void *process_addr = get_free_pages(process_size);
    if (!process_addr) {
        kprintf("Error loading process\n");
        return TASK_INIT_ERROR;
    }

    _task_load(ehdr, process_addr);

    const size_t heap_offset = ELF_OFF(process_addr, elf_get_heap_offset(ehdr));
    const size_t stack_offset = ELF_OFF(process_addr, elf_get_stack_offset(ehdr));

    /* Add process pages to stack */
    size_t *sp = (unsigned long *) stack_offset;
    *(--sp) = heap_offset;
    *(--sp) = (size_t) process_addr;
    *(--sp) = (size_t) file;

    /* Create task struct */
    struct task *new_task = (struct task*) ELF_OFF(process_addr, elf_memory_size);

    new_task->process_address =  (unsigned long) process_addr;
    new_task->priority = current_task->priority;
    new_task->state = TASK_RUNNING;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;

    new_task->cpu_context.x19 = (size_t) ELF_OFF(linker_page, linker_page->e_entry);
    new_task->cpu_context.x20 = (size_t) sp;
    new_task->cpu_context.pc  = (size_t) start_user;

    __asm__(
        "mov x0, sp \n"
        "mov %x0, x0 \n"
        : "=r" (new_task->cpu_context.sp)
        : : "x0"
    );

    /* Add task */
    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        if (!tasks[i]) {
            tasks[i] = new_task;
            break;
        }
    }

    current_task->preempt_count--;
    return TASK_OK;
}

void task_schedule(void) {
    long next_task;
    struct task *p;

    current_task->counter = 0;
    current_task->preempt_count++;

    while(1) {
        long counter = -1;
        // Find the running process with the highest priority
        next_task = 0;

        for (size_t i = 0; i < TOTAL_TASKS; i++) {
            p = tasks[i];
            if (p && p->state == TASK_RUNNING && p->counter > counter) {
                counter = p->counter;
                next_task = i;
            }
        }

        // If such a process exists, switch to it
        if (counter) {
            break;
        }

        // If not, update task counters
        for (size_t i = 0; i < TOTAL_TASKS; i++) {
            p = tasks[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }

    /* Switch task */
    if (current_task != tasks[next_task]) {
        struct task *previous_task = current_task;
        current_task = tasks[next_task];
        cpu_context_switch(previous_task, current_task);
    }

    current_task->preempt_count--;
}

void task_exit(void) {
    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        if (tasks[i] == current_task) {
            tasks[i] = NULL;
            break;
        }
    }

    free_pages((void *) current_task->process_address);
    current_task = NULL;
    task_schedule();
}
