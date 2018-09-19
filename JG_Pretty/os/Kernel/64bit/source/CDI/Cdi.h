
#ifndef CDI_H
#define CDI_H

#include "../types.h"
#include "../usb/list2.h"

typedef enum {
    CDI_UNKNOWN         = 0,
    CDI_NETWORK         = 1,
    CDI_STORAGE         = 2,
    CDI_SCSI            = 3,
    CDI_VIDEO           = 4,
    CDI_AUDIO           = 5,
    CDI_AUDIO_MIXER     = 6,
    CDI_USB_HCD         = 7,
    CDI_USB             = 8,
    CDI_FILESYSTEM      = 9,
    CDI_PCI             = 10,
    CDI_AHCI            = 11,
} cdi_device_type_t;

struct cdi_driver;

typedef list_t* cdi_list_t;

struct cdi_bus_data {
    cdi_device_type_t   bus_type;
};

struct cdi_bus_device_pattern {
    cdi_device_type_t bus_type;
};

// Describes a device
struct cdi_device {
    // Name of the device (must be unique among the devices of the same driver)
    const char*             name;

    // Driver used for the device
    struct cdi_driver*      driver;

    // Bus specific data for the device
    struct cdi_bus_data*    bus_data;

    // PrettyOS specific
    void*                   backdev;
};

// Describes a CDI driver
struct cdi_driver {
    cdi_device_type_t   type;
    cdi_device_type_t   bus;
    const char*         name;
    bool                initialised;

    struct cdi_device* (*init_device)(struct cdi_bus_data* bus_data);
    void (*remove_device)(struct cdi_device* device);

    int (*init)(void);
    int (*destroy)(void);
};

void cdi_init(void);

void cdi_driver_init(struct cdi_driver* driver);

void cdi_driver_destroy(struct cdi_driver* driver);

void cdi_driver_register(struct cdi_driver* driver);

int cdi_provide_device(struct cdi_bus_data* device);

int cdi_provide_device_internal_drv(struct cdi_bus_data* device, struct cdi_driver* driver);

void cdi_handle_bus_device(struct cdi_driver* drv, struct cdi_bus_device_pattern* pattern);

#endif
