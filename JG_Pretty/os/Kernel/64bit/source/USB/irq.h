#ifndef IRQ_H
#define IRQ_H

#include "../types.h"
#include "../utility.h"

#define PIC_MASTER_CMD  0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD   0xA0
#define PIC_SLAVE_DATA  0xA1

#define PIC_EOI         0x20  // End-of-interrupt (command code)


struct cdi_device;
struct pciDev;

typedef enum
{
    ISR_invalidOpcode = 6,
    ISR_NM            = 7,
    ISR_GPF           = 13,
    ISR_PF            = 14,
} ISR_NUM_t;
/*
typedef enum
{
    IRQ_TIMER         = 0,
    IRQ_KEYBOARD      = 1,
    IRQ_FLOPPY        = 6,
    IRQ_MOUSE         = 12,
    IRQ_ATA_PRIMARY   = 14,
    IRQ_ATA_SECONDARY = 15,
    IRQ_SYSCALL       = 95 // PrettyOS SYSCALL_NUMBER 127 minus 32 // cf. interrupts.asm
} IRQ_NUM_t;
*/
// This defines what the stack looks like after an ISR was running
typedef struct
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;


#endif
