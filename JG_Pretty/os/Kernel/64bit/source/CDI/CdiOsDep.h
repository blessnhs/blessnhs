#ifndef CDI_OSDEP_H
#define CDI_OSDEP_H

#include "../pci.h"

#define cdi_glue(x, y) x ## y
#define cdi_declare_driver(drv, counter) \
    static const void* __attribute__((section(".cdi"), used)) \
        cdi_glue(__cdi_driver_, counter) = &drv;

#define CDI_DRIVER(name, drv, deps...) cdi_declare_driver(drv, __COUNTER__)

struct cdi_device;
struct cdi_driver;

typedef struct
{
    struct pciDev_t* dev;
    struct cdi_device* cdiDev;
    struct cdi_driver* driver;
} cdi_pci_device_osdep;

typedef struct
{
} cdi_usb_device_osdep;

typedef struct
{
} cdi_dma_osdep;

typedef struct
{
    bool malloced;
} cdi_mem_osdep;

typedef struct
{
} cdi_fs_osdep;

#endif
