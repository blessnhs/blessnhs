
#ifndef _E1000_IO_H_
#define _E1000_IO_H_

#include "e1000.h"

#if 0
static inline void reg_outb
    (struct e1000_device* netcard, uint16_t reg, uint8_t value)
{
    cdi_outb(netcard->port_base + reg, value);
}

static inline void reg_outw
    (struct e1000_device* netcard, uint16_t reg, uint16_t value)
{
    cdi_outw(netcard->port_base + reg, value);
}
#endif

static inline void reg_outl
    (struct e1000_device* netcard, uint16_t reg, uint32_t value)
{
    volatile uint32_t* mmio = (uint32_t*) (((char*)netcard->mem_base) + reg);
    *mmio = value;
}


#if 0
static inline uint8_t reg_inb(struct e1000_device* netcard, uint16_t reg)
{
    return cdi_inb(netcard->port_base + reg);
}

static inline uint16_t reg_inw(struct e1000_device* netcard, uint16_t reg)
{
    return cdi_inw(netcard->port_base + reg);
}
#endif

static inline uint32_t reg_inl(struct e1000_device* netcard, uint16_t reg)
{
    volatile uint32_t* mmio = (uint32_t*) (((char*)netcard->mem_base) + reg);
    return *mmio;
}


/* Flash register access */

static inline void flash_reg_outw
    (struct e1000_device* netcard, uint16_t reg, uint16_t value)
{
    volatile uint16_t* mmio = (uint16_t*) (((char*)netcard->flash_base) + reg);
    *mmio = value;
}

static inline void flash_reg_outl
    (struct e1000_device* netcard, uint16_t reg, uint32_t value)
{
    volatile uint32_t* mmio = (uint32_t*) (((char*)netcard->flash_base) + reg);
    *mmio = value;
}

static inline uint16_t flash_reg_inw(struct e1000_device* netcard, uint16_t reg)
{
    volatile uint16_t* mmio = (uint16_t*) (((char*)netcard->flash_base) + reg);
    return *mmio;
}

static inline uint32_t flash_reg_inl(struct e1000_device* netcard, uint16_t reg)
{
    volatile uint32_t* mmio = (uint32_t*) (((char*)netcard->flash_base) + reg);
    return *mmio;
}

#endif
