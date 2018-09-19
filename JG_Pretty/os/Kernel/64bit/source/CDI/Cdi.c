
#include "cdi.h"

static cdi_list_t drivers = 0;

extern struct cdi_driver* _cdi_start; // Declared in kernel.ld
extern struct cdi_driver* _cdi_end;   // Declared in kernel.ld

// Initialisiert alle PCI-Geraete
static void init_pci_devices(void)
{
 /*   // Liste der PCI-Geraete holen
    cdi_list_t pciDevices = cdi_list_create();
    cdi_pci_get_all_devices(pciDevices);

    struct cdi_pci_device* pci;
    // Fuer jedes Geraet einen Treiber suchen
    for (int i = 0; (pci = cdi_list_get(pciDevices, i)); i++)
    {
        struct cdi_device* device = 0;
        struct cdi_driver* driver;
        for (size_t j = 0; (driver = cdi_list_get(drivers, j)); j++)
        {
            if (driver->bus == CDI_PCI && driver->init_device)
            {
                pci->meta.driver = driver;
                device = driver->init_device(&pci->bus_data);
                pci->meta.cdiDev = device;
                break;
            }
        }

        if (device != 0)
        {
            cdi_list_push(driver->devices, device);
        }
        else
        {
            cdi_pci_device_destroy(pci);
        }
    }

    cdi_list_destroy(pciDevices);*/
}
static void run_drivers(void)
{
    // PCI-Geraete suchen
/*    init_pci_devices();

    // Geraete initialisieren
    struct cdi_driver* driver;
    for (size_t i = 0; (driver = cdi_list_get(drivers, i)); i++)
    {
        struct cdi_device* device;
        for (size_t j = 0; (device = cdi_list_get(driver->devices, j)); j++)
        {
            device->driver = driver;
        }
    }*/
}

void cdi_init()
{
    // Interne Strukturen initialisieren
 /*   drivers = cdi_list_create();

    // Alle in dieser Binary verfuegbaren Treiber aufsammeln
    struct cdi_driver** pdrv = &_cdi_start;
    while (pdrv < &_cdi_end)
    {
        struct cdi_driver* drv = *pdrv;
        if (drv->init != 0)
        {
            drv->init();
            cdi_driver_register(drv);
        }
        pdrv++;
    }

    // Treiber starten
    run_drivers();*/
}

void cdi_driver_init(struct cdi_driver* driver)
{
  //  driver->devices = (list_create());
}

void cdi_driver_destroy(struct cdi_driver* driver)
{
  //  cdi_list_destroy(driver->devices);
}

void cdi_driver_register(struct cdi_driver* driver)
{
 /*   cdi_list_push(drivers, driver);

    switch (driver->type)
    {
        case CDI_STORAGE:
            //cdi_storage_driver_register((struct cdi_storage_driver*) driver);
            break;
        case CDI_FILESYSTEM:
            cdi_fs_driver_register((struct cdi_fs_driver*) driver);
            break;
        default:
            break;
    }*/
}

int cdi_provide_device(struct cdi_bus_data* device);

int cdi_provide_device_internal_drv(struct cdi_bus_data* device, struct cdi_driver* driver);

void cdi_handle_bus_device(struct cdi_driver* drv, struct cdi_bus_device_pattern* pattern);

int __attribute__((weak)) main(void)
{
    cdi_init();
    return (0);
}
