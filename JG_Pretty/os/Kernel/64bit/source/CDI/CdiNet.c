
#include "../network/net.h"
#include "../pci.h"
#include "../network/network.h"

struct pciDev_t;

static uint32_t netcard_id = 0;


void cdi_net_driver_init(struct cdi_net_driver* driver)
{
    driver->drv.type = CDI_NETWORK;
    cdi_driver_init((struct cdi_driver*)driver);
}

void cdi_net_driver_destroy(struct cdi_net_driver* driver)
{
    cdi_driver_destroy((struct cdi_driver*) driver);
}

void cdi_net_device_init(struct cdi_net_device* device)
{
    device->number = netcard_id++;

    struct cdi_pci_device* pcidev = (struct cdi_pci_device*)device->dev.bus_data;
    pcidev->meta.cdiDev = (struct cdi_device*)device;

    pciDev_t *dev =pcidev->meta.dev;
    dev->data= device->dev.bus_data;

  //  pcidev->meta.dev->data = device->dev.bus_data;

    device->dev.driver = pcidev->meta.driver;

    network_adapter_t* adapter = device->dev.backdev = network_createDevice(pcidev->meta.dev);
    uint8_t i;
    for (i = 0; i < 6; i++) // Copy MAC bytewise due to design failure in CDI
        adapter->MAC[i] = device->mac>>(i*8);
    network_installCDIDevice(device->dev.backdev);
}

void cdi_net_receive(struct cdi_net_device* device, void* buffer, size_t size)
{
    network_receivedPacket(device->dev.backdev, buffer, size);
}
