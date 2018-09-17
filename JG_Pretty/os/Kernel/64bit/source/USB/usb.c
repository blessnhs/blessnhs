/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f�r die Verwendung dieses Sourcecodes siehe unten
*/

#include "usb.h"
#include "usb_hc.h"
#include "usb_hub.h"
#include "../DynamicMemory.h"
#include "../storage/usb_msd.h"
#include "../task.h"
#include "../fat32/fat_access.h"

list_t usb_interruptTransfers = list_init();


static void analyzeDeviceDescriptor(const struct usb_deviceDescriptor* d, usb_device_t* usbDev);
static void showDevice(const usb_device_t* usbDev, bool first);
static void showConfigurationDescriptor(const struct usb_configurationDescriptor* d);
static void showInterfaceAssociationDescriptor(const struct usb_interfaceAssociationDescriptor* d);
static void showInterfaceDescriptor(const struct usb_interfaceDescriptor* d);
static void showEndpointDescriptor(const struct usb_endpointDescriptor* d);
static void showStringDescriptor(const struct usb_stringDescriptor* d);
static void showUnicodeStringDescriptor(const struct usb_stringDescriptorUnicode* d, usb_device_t* device, uint32_t stringIndex);

usb_device_t* usb_createDevice(port_t* port, usb_speed_t speed)
{
    usb_device_t* device = AllocateMemory(sizeof(usb_device_t));
    device->port = port;
    device->speed = speed;
    list_construct(&device->interfaces);
    list_construct(&device->endpoints);
    element* ep0_elem = list_alloc_elem(sizeof(usb_endpoint_t), "usb_endpoint 0");
    usb_endpoint_t* ep0 = ep0_elem->data;
    ep0->address = 0;
    switch (speed)
    {
        case USB_LOWSPEED: case USB_FULLSPEED:
            ep0->mps = 8; // fullspeed can have MPS between 8 and 64 - we will find out later, start with 8
            break;
        case USB_HIGHSPEED:
            ep0->mps = 64;
            break;
        case USB_SUPERSPEED:
            ep0->mps = 512;
            break;
    }
    ep0->dir = EP_BIDIR;
    ep0->type = EP_CONTROL;
    ep0->toggle = false;
    ep0->interval = 0;
    list_append_elem(&device->endpoints, ep0_elem);
    MemSet(device->serialNumber, 0, 13);
    return (device);
}

void usb_destroyDevice(usb_device_t* device)
{
    if (!device)
        return;

    if (device->usbClass == 0x09) // Hub
        usb_destroyHub(device);

    element *el;
    for (el = device->interfaces.head; el; el = el->next)
    {
        usb_interface_t* interface = el->data;
        if (interface->descriptor.interfaceClass == 0x08 && (interface->descriptor.interfaceSubclass == 0x06 || // MSD (SCSI transparent command set)
                                                             interface->descriptor.interfaceSubclass == 0x04))  // UFI (usb-Floppy)
            usb_destroyMSD(interface);
        else if (interface->descriptor.interfaceClass == 0x03) // HID
            usb_destroyHID(interface);
/*        else if (interface->descriptor.interfaceClass == 0x0E && interface->descriptor.interfaceSubclass == 0x01) // Video Control
            usb_destroyVideo(interface);
*/
    }
    list_destruct(&device->interfaces);
    list_destruct(&device->endpoints);
    FreeMemory(device);
}

void usb_setupDevice(usb_device_t* device, uint8_t address)
{

    device->num = 0; // device number has to be set to 0
    device->hub = 0;
    device->totalConfigDescriptor = 0;
    bool success = false;


    bool startWith8 = device->speed == USB_FULLSPEED; // Start with 8 bytes, since MPS is unknown
    success = usb_getDeviceDescriptor(device, startWith8?8:18, true);
    if (!success)
    {
        success = usb_getDeviceDescriptor(device, startWith8?8:18, true);
    }
    if (!success)
    {
        Printf("\nGet Device Descriptor failed!");
        return;
    }
  
    device->num = usb_setDeviceAddress(device, address);

    if (startWith8)
        usb_getDeviceDescriptor(device, 18, false); // Get all 18 bytes

 #ifdef _USB_DIAGNOSIS_
    Printf("\ndevice address: %u", device->num);
 #endif

    success = usb_getConfigDescriptor(device);
    if (!success)
    {
        success = usb_getConfigDescriptor(device);
    }

    if (!success)
    {
        Printf("\nConfigDescriptor could not be read! Setup Device interrupted!");
        return;
    }

    usb_setConfiguration(device, 1); // set first configuration

  #ifdef _USB_DIAGNOSIS_
    Printf("\nconfiguration: %u", usb_getConfiguration(device)); // check configuration
  #endif

    if (device->usbClass == 0x09) // Hub
        usb_setupHub(device);
    else
    {
        bool foundSomething = false;
        element* el;
        for (el = device->interfaces.head; el; el = el->next)
        {
            usb_interface_t* interface = el->data;
            if (interface->descriptor.interfaceClass == 0x08 && (interface->descriptor.interfaceSubclass == 0x06 || // MSD (SCSI transparent command set)
                                                                 interface->descriptor.interfaceSubclass == 0x04))  // UFI (usb-Floppy)
            {
                foundSomething = true;
                usb_setupMSD(interface);

                fl_init();
                fl_attach_media(usb_diskio_read,usb_diskio_write);

            }
            else if (interface->descriptor.interfaceClass == 0x03) // HID
            {
                foundSomething = true;
                usb_setupHID(interface);
            }
            else if (interface->descriptor.interfaceClass == 0x0E && interface->descriptor.interfaceSubclass == 0x01) // Video Control
            {
                foundSomething = true;
                //???                 usb_setupVideo(interface);
            }
        }
        if (!foundSomething)
        {
            Printf("\nUnsupported device type!");
        }
    }

    FreeMemory(device->totalConfigDescriptor);
    device->totalConfigDescriptor = 0;
}

bool usb_controlIn(usb_device_t* device, void* buffer, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t index, uint16_t length)
{
    usb_transfer_t transfer;

    usb_constructTransfer(device, &transfer, USB_CONTROL, device->endpoints.head->data);
    usb_setupTransaction(&transfer, type, req, hiVal, loVal, index, length);
    usb_inTransaction(&transfer, false, buffer, length);
    usb_outTransaction(&transfer, true, 0, 0);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);

    return transfer.success;
}

bool usb_controlOut(usb_device_t* device, const void* buffer, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t index, uint16_t length)
{
    usb_transfer_t transfer;
    usb_constructTransfer(device, &transfer, USB_CONTROL, device->endpoints.head->data);
    usb_setupTransaction(&transfer, type, req, hiVal, loVal, index, length);
    usb_outTransaction(&transfer, false, buffer, length);
    usb_inTransaction(&transfer, true, 0, 0);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);
    return transfer.success;
}

bool usb_controlSet(usb_device_t* device, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t index)
{
    usb_transfer_t transfer;
    usb_constructTransfer(device, &transfer, USB_CONTROL, device->endpoints.head->data);
    usb_setupTransaction(&transfer, type, req, hiVal, loVal, index, 0);
    usb_inTransaction(&transfer, true, 0, 0);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);
    return transfer.success;
}

uint8_t usb_setDeviceAddress(usb_device_t* device, uint8_t num)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: SET_ADDRESS: %u", num);
    textColor(TEXT);
  #endif

    uint8_t new_address = num; // indicated port number

    usb_transfer_t transfer;
    usb_constructTransfer(device, &transfer, USB_CONTROL, device->endpoints.head->data);
    new_address = usb_setupTransaction(&transfer, 0x00, SET_ADDRESS, 0, new_address, 0, 0);
    usb_inTransaction(&transfer, true, 0, 0);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);

  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\nnew address: %u", new_address);
  #endif

    return new_address;
}

bool usb_getDeviceDescriptor(usb_device_t* device, uint8_t length, bool first)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\n\nUSB: GET_DESCRIPTOR Device");
  #endif

    struct usb_deviceDescriptor descriptor;

    bool success = usb_controlIn(device, &descriptor, 0x80, GET_DESCRIPTOR, 1, 0, 0, length);

    if (success)
    {
        analyzeDeviceDescriptor(&descriptor, device);
        showDevice(device, first);
    }


    return success;
}

bool usb_getConfigDescriptor(usb_device_t* device)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\n\nUSB: GET_DESCRIPTOR Config");
  #endif

    struct usb_configurationDescriptor cfgdesc;
    bool success = usb_controlIn(device, &cfgdesc, 0x80, GET_DESCRIPTOR, 2, 0, 0, sizeof(cfgdesc));

    if (!success)
        return false;

    const uint16_t bufferSize = cfgdesc.totalLength;
    char buffer[bufferSize];

    success = usb_controlIn(device, buffer, 0x80, GET_DESCRIPTOR, 2, 0, 0, bufferSize);

    if (success)
    {
        uint16_t configSize = min(bufferSize, *(uint16_t*)(buffer + 2));

      #ifdef _USB_TRANSFER_DIAGNOSIS_
        Printf("\n---------------------------------------------------------------------\n");
        Printf("\nconfig descriptor - total length: %u", configSize);
      #endif
        device->totalConfigDescriptor = AllocateMemory(configSize);
        MemCpy(device->totalConfigDescriptor, buffer, configSize);

      #ifdef _USB_TRANSFER_DIAGNOSIS_
        //memshow(buffer, configSize, false);
        Printf('\n');
      #endif

        void* addr     = buffer;
        void* lastByte = addr + configSize; // totalLength (WORD)

        // Analyze descriptors
        while ((uintptr_t)addr < (uintptr_t)lastByte)
        {
            uint8_t length  = *(uint8_t*)(addr+0);
            uint8_t type    = *(uint8_t*)(addr+1);

            if (length == 9 && type == CONFIGURATION) // CONFIGURATION descriptor
            {
                struct usb_configurationDescriptor* descriptor = addr;
                showConfigurationDescriptor(descriptor);
            }
            else if (length == 8 && type == 0x0B) // INTERFACE ASSOCIATION Descriptor
            {
                struct usb_interfaceAssociationDescriptor* descriptor = addr;
                showInterfaceAssociationDescriptor(descriptor);
            }
            else if (length == 9 && type == INTERFACE) // INTERFACE descriptor
            {
                struct usb_interfaceDescriptor* descriptor = addr;
                showInterfaceDescriptor(descriptor);

                element* elem = list_alloc_elem(sizeof(usb_interface_t), "usb_interface_t");
                usb_interface_t* interface = elem->data;
                MemCpy(&interface->descriptor, descriptor, sizeof(struct usb_interfaceDescriptor));
                interface->device = device;
                interface->data   = 0;
                list_append_elem(&device->interfaces, elem);
            }
            else if (length == 7 && type == ENDPOINT) // ENDPOINT descriptor
            {
                struct usb_endpointDescriptor* descriptor = addr;
                showEndpointDescriptor(descriptor);

                element* elem = list_alloc_elem(sizeof(usb_endpoint_t), "usb_endpoint_t");
                usb_endpoint_t* ep = elem->data;

                if (descriptor->endpointAddress & 0x80)
                    ep->dir = EP_IN;
                else
                    ep->dir = EP_OUT;

                ep->address  = descriptor->endpointAddress & 0xF;
                ep->mps      = descriptor->maxPacketSize;
                ep->interval = descriptor->interval;
                ep->toggle   = false;
                ep->type     = descriptor->attributes & 0x3;

              #ifdef _USB_TRANSFER_DIAGNOSIS_
                Printf("\nep address: %u", ep->address);
              #endif

                list_append_elem(&device->endpoints, elem);
            }
            else
            {
              #ifdef _USB_TRANSFER_DIAGNOSIS_
                Printf("\nUnknown part of ConfigDescriptor: length: %u type: %u\n", length, type);
                //waitForKeyStroke();
              #endif
            }
            addr += length;
        }//while

        hc_updateEndpointInformation(device->port->data); // We now have information about all EPs
    }//if

    return success;
}

void usb_getStringDescriptor(usb_device_t* device)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: GET_DESC string, dev: %X endpoint: 0 languageIDs", device);
    textColor(TEXT);
  #endif

    struct usb_stringDescriptor descriptor;
    usb_controlIn(device, &descriptor, 0x80, GET_DESCRIPTOR, 3, 0, 0, sizeof(descriptor));

  #ifdef _USB_TRANSFER_DIAGNOSIS_
    //memshow(&descriptor, sizeof(descriptor), false);
    Printf('\n');
  #endif
    showStringDescriptor(&descriptor);
}

void usb_getUnicodeStringDescriptor(usb_device_t* device, uint8_t stringIndex)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: GET_DESC unicode string, dev: %X endp.: 0 strIndex: %u", device, stringIndex);
    textColor(TEXT);
  #endif

    char buffer[64];
    usb_controlIn(device, buffer, 0x80, GET_DESCRIPTOR, 3, stringIndex, 0x0409, 64);

  #ifdef _USB_TRANSFER_DIAGNOSIS_
    //memshow(buffer, 64, false);
    Printf('\n');
  #endif

    showUnicodeStringDescriptor((struct usb_stringDescriptorUnicode*)buffer, device, stringIndex);
}

// http://www.lowlevel.eu/wiki/USB#SET_CONFIGURATION
void usb_setConfiguration(usb_device_t* device, uint8_t configuration)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: SET_CONFIGURATION %u", configuration);
    textColor(TEXT);
  #endif

    usb_controlSet(device, 0x00, SET_CONFIGURATION, 0, configuration, 0);
}

uint8_t usb_getConfiguration(usb_device_t* device)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: GET_CONFIGURATION");
    textColor(TEXT);
  #endif

    uint8_t configuration;
    usb_controlIn(device, &configuration, 0x80, GET_CONFIGURATION, 0, 0, 0, 1);
    return configuration;
}

void usb_setInterface(usb_device_t* device, uint8_t alternateInterface, uint16_t interfaceID)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: SET_INTERFACE %u alternate Interface: %u", interfaceID, alternateInterface);
    textColor(TEXT);
  #endif

    usb_controlSet(device, 0x01, SET_INTERFACE, 0, alternateInterface, interfaceID);
}

uint8_t usb_getInterface(usb_device_t* device, uint16_t interfaceID)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: GET_INTERFACE %u", interfaceID);
    textColor(TEXT);
  #endif

    uint8_t alternateInterface;
    usb_controlIn(device, &alternateInterface, 0x81, GET_INTERFACE, 0, 0, interfaceID, 1);
    return alternateInterface;
}

// seems not to work correct, does not set HALT ???
void usb_setFeatureHALT(usb_device_t* device, usb_endpoint_t* endpoint)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: usbSetFeatureHALT, endpoint: %u", endpoint);
    textColor(TEXT);
  #endif

    usb_controlSet(device, 0x02, SET_FEATURE, 0, 0, endpoint->address);

  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\nset HALT at dev: %X endpoint: %u", device, endpoint);
  #endif
}

void usb_clearFeatureHALT(usb_device_t* device, usb_endpoint_t* endpoint)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: usbClearFeatureHALT, endpoint: %u", endpoint);
    textColor(TEXT);
  #endif

    usb_controlSet(device, 0x02, CLEAR_FEATURE, 0, 0, endpoint->address);

  #ifdef _USB_DIAGNOSIS_
    Printf("\nclear HALT at dev: %X endpoint: %u", device, endpoint);
  #endif
}

uint16_t usb_getStatus(usb_device_t* device, usb_endpoint_t* endpoint)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: usbGetStatus at device: %X endp.: %u", device, endpoint);
    textColor(TEXT);
  #endif

    uint16_t status;
    usb_controlIn(device, &status, 0x02, GET_STATUS, 0, 0, 0, 2);
    return status;
}

static void analyzeDeviceDescriptor(const struct usb_deviceDescriptor* d, usb_device_t* usbDev)
{
    usbDev->usbSpec              = d->bcdUSB;
    usbDev->usbClass             = d->deviceClass;
    usbDev->usbSubclass          = d->deviceSubclass;
    usbDev->usbProtocol          = d->deviceProtocol;
    usb_endpoint_t* ep0 = usbDev->endpoints.head->data;
    bool changedMPS = ep0->mps != d->maxPacketSize;
    if (changedMPS)
    {
        ep0->mps = d->maxPacketSize;
        hc_updateEndpointInformation(usbDev->port->data);
    }
    if (d->length > 8)
    {
        usbDev->vendor               = d->idVendor;
        usbDev->product              = d->idProduct;
        usbDev->releaseNumber        = d->bcdDevice;
        usbDev->manufacturerStringID = d->manufacturer;
        usbDev->productStringID      = d->product;
        usbDev->serNumberStringID    = d->serialNumber;
        usbDev->numConfigurations    = d->numConfigurations;
    }
}

static void showDevice(const usb_device_t* usbDev, bool first)
{
    if (usbDev->usbSpec == 0x0100 || usbDev->usbSpec == 0x0110 ||
        usbDev->usbSpec == 0x0200 || usbDev->usbSpec == 0x0201 || usbDev->usbSpec == 0x0210 || usbDev->usbSpec == 0x0213 ||
        usbDev->usbSpec == 0x0300 || usbDev->usbSpec == 0x0310)
    {
        if (first)
        {
            Printf("\nUSB %d.%d\t", BYTE2(usbDev->usbSpec), BYTE1(usbDev->usbSpec)); // e.g. 0x0210 means 2.10
        }
    }
    else
    {
        Printf("\nInvalid USB version %y.%y!", BYTE2(usbDev->usbSpec), BYTE1(usbDev->usbSpec));
        return;
    }

    if (first)
    {
        if (usbDev->usbClass == 0x09)
        {
            switch (usbDev->usbProtocol)
            {
            case 0:
                Printf(" - Full speed USB hub");
                break;
            case 1:
                Printf(" - Hi-speed USB hub with single TT");
                break;
            case 2:
                Printf(" - Hi-speed USB hub with multiple TTs");
                break;
            }
        }

        usb_endpoint_t* ep0 = usbDev->endpoints.head->data;
        Printf("\nendpoint 0 mps: %u bytes.", ep0->mps);
    }
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\nDevice: Class %d, subclass: %d, protocol %d.\n", usbDev->usbClass, usbDev->usbSubclass, usbDev->usbProtocol);
    Printf("vendor:            %x\n",   usbDev->vendor);
    Printf("product:           %x\t",   usbDev->product);
    Printf("release number:    %x.%x\n", BYTE2(usbDev->releaseNumber), BYTE1(usbDev->releaseNumber));
    Printf("manufacturer:      %x\t",   usbDev->manufacturerStringID);
    Printf("product:           %x\n",   usbDev->productStringID);
    Printf("serial number:     %x\t",   usbDev->serNumberStringID);
    Printf("number of config.: %x\n",    usbDev->numConfigurations); // number of possible configurations
  #endif
}

static void showConfigurationDescriptor(const struct usb_configurationDescriptor* d)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    if (d->length)
    {
        Printf("length:               %u\t\t", d->length);
        Printf("descriptor type:      %u\n", d->descriptorType);
        Printf("total length:         %u\t", d->totalLength);
        Printf("\nNumber of interfaces: %u", d->numInterfaces);
        Printf("ID of config:         %xh\t", d->configurationValue);
        Printf("ID of config name     %xh\n", d->configuration);
        Printf("remote wakeup:        %s\t", (d->attributes & BIT(5)) ? "yes" : "no");
        Printf("self-powered:         %s\n", (d->attributes & BIT(6)) ? "yes" : "no");
        Printf("max power (mA):       %u\n", d->maxPower*2); // 2 mA steps used
    }
  #endif
}

static void showInterfaceAssociationDescriptor(const struct usb_interfaceAssociationDescriptor* d)
{
    Printf("\n\nInterfaceAssociationDescriptor:");
    Printf("\n-------------------------------");
    Printf("\nfirst interface:   %u",   d->firstInterface  );
    Printf("\tinterface count:   %u",   d->interfaceCount  );
    Printf("\nfunction class:    %u",   d->functionClass   );
    Printf("\tfunction subclass: %u",   d->functionSubclass);
    Printf("\nfunction protocol: %u",   d->functionProtocol);
    Printf("\tIndex stringDesc:  %u\n", d->function        );
}

static void showInterfaceDescriptor(const struct usb_interfaceDescriptor* d)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    if (d->length)
    {
    	Printf('\n');

        Printf("---------------------------------------------------------------------\n");
        Printf("length:               %u\t\t", d->length);          // 9
        Printf("descriptor type:      %u\n",   d->descriptorType);  // 4

        switch (d->numEndpoints)
        {
            case 0:
                Printf("Interface %u has no endpoint and belongs to class:\n", d->interfaceNumber);
                break;
            case 1:
                Printf("Interface %u has only one endpoint and belongs to class:\n", d->interfaceNumber);
                break;
            default:
                Printf("Interface %u has %u endpoints and belongs to class:\n", d->interfaceNumber, d->numEndpoints);
                break;
        }

        switch (d->interfaceClass)
        {
            case 0x01:
                Printf("Audio");
                break;
            case 0x02:
                Printf("Communications and CDC Control");
                break;
            case 0x03:
                Printf("HID (Human Interface Device)");
                break;
            case 0x05:
                Printf("Physical");
                break;
            case 0x06:
                Printf("Image");
                break;
            case 0x07:
                Printf("Printer");
                break;
            case 0x08:
                Printf("Mass Storage, ");
                switch (d->interfaceSubclass)
                {
                    case 0x01:
                        Printf("Reduced Block Commands, ");
                        break;
                    case 0x02:
                        Printf("SFF-8020i or MMC-2(ATAPI), ");
                        break;
                    case 0x03:
                        Printf("QIC-157 (tape device), ");
                        break;
                    case 0x04:
                        Printf("UFI (e.g. Floppy Disk), ");
                        break;
                    case 0x05:
                        Printf("SFF-8070i (e.g. Floppy Disk), ");
                        break;
                    case 0x06:
                        Printf("SCSI transparent command set, ");
                        break;
                }
                switch (d->interfaceProtocol)
                {
                    case 0x00:
                        Printf("CBI protocol with command completion interrupt.");
                        break;
                    case 0x01:
                        Printf("CBI protocol without command completion interrupt.");
                        break;
                    case 0x50:
                        Printf("Bulk-Only Transport protocol.");
                        break;
                }
                break;
            case 0x0A:
                Printf("CDC-Data");
                break;
            case 0x0B:
                Printf("Smart Card");
                break;
            case 0x0D:
                Printf("Content Security");
                break;
            case 0x0E:
                Printf("Video, ");
                switch (d->interfaceSubclass)
                {
                    case 0x00:
                        Printf("Subclass: Undefined, ");
                        break;
                    case 0x01:
                        Printf("Subclass: Video Control, ");
                        break;
                    case 0x02:
                        Printf("Subclass: Video Streaming, ");
                        break;
                    case 0x03:
                        Printf("Subclass: Video Interface Collection, ");
                        break;
                }
                switch (d->interfaceProtocol)
                {
                    case 0x00:
                        Printf("Protocol: Undefined.");
                        break;
                    case 0x01:
                        Printf("Protocol: Protocol_15.");
                        break;
                }
                break;
            case 0x0F:
                Printf("Personal Healthcare");
                break;
            case 0xDC:
                Printf("Diagnostic Device");
                break;
            case 0xE0:
                Printf("Wireless Controller, subclass: %yh protocol: %yh.", d->interfaceSubclass, d->interfaceProtocol);
                break;
            case 0xEF:
                Printf("Miscellaneous");
                break;
            case 0xFE:
                Printf("Application Specific");
                break;
            case 0xFF:
                Printf("Vendor Specific");
                break;
        }

        Printf("\nalternate Setting:  %u\n",  d->alternateSetting);
        Printf("interface class:      %u\n",  d->interfaceClass);
        Printf("interface subclass:   %u\n",  d->interfaceSubclass);
        Printf("interface protocol:   %u\n",  d->interfaceProtocol);
        Printf("interface:            %xh\n", d->interface);
    }
  #endif
}

static void showEndpointDescriptor(const struct usb_endpointDescriptor* d)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    if (d->length)
    {
        Printf("\n---------------------------------------------------------------------\n");
        //Printf("length:      %u\t\t",   d->length);         // 7
        //Printf("descriptor type: %u\n", d->descriptorType); // 5
        Printf("endpoint %u: %s, ",     d->endpointAddress & 0xF, (d->endpointAddress & 0x80) ? "IN " : "OUT");
        Printf("attributes: %yh\t",     d->attributes);
        // bit 1:0 00 control    01 isochronous    10 bulk                         11 interrupt
        // bit 3:2 00 no sync    01 async          10 adaptive                     11 sync (only if isochronous)
        // bit 5:4 00 data endp. 01 feedback endp. 10 explicit feedback data endp. 11 reserved (Iso Mode)

        if (d->attributes == 2)
        {
            Printf("\nbulk data,");
        }
        Printf(" mps: %u byte",  d->maxPacketSize);
        Printf(" interval: %u\n",  d->interval);
    }
  #endif
}

static void showStringDescriptor(const struct usb_stringDescriptor* d)
{
    if (d->length)
    {

      #ifdef _USB_TRANSFER_DIAGNOSIS_
        Printf("\nlength:          %u\t", d->length);         // 12
        Printf("\tdescriptor type: %u\n", d->descriptorType); //  3
      #endif

        Printf("\n\nlanguages: ");
        uint8_t i=0;
        for (i=0; i<10; i++)
        {
            if (d->wstring[i] >= 0x0400 && d->wstring[i] <= 0x0465)
            {
                switch (d->wstring[i])
                {
                    case 0x401:
                        Printf("Arabic");
                        break;
                    case 0x404:
                        Printf("Chinese");
                        break;
                    case 0x407:
                        Printf("German");
                        break;
                    case 0x409:
                        Printf("English");
                        break;
                    case 0x40A:
                        Printf("Spanish");
                        break;
                    case 0x40C:
                        Printf("French");
                        break;
                    case 0x410:
                        Printf("Italian");
                        break;
                    case 0x411:
                        Printf("Japanese");
                        break;
                    case 0x416:
                        Printf("Portuguese");
                        break;
                    case 0x419:
                        Printf("Russian");
                        break;
                    default:
                        Printf("language code: %xh", d->wstring[i]);
                        break;
                }
            }
        }
        Printf('\n');
    }
}

static void showUnicodeStringDescriptor(const struct usb_stringDescriptorUnicode* d, usb_device_t* device, uint32_t stringIndex)
{
    if (d->length)
    {
      #ifdef _USB_TRANSFER_DIAGNOSIS_
        Printf("\nlength:          %u\t", d->length);
        Printf("\tdescriptor type: %u", d->descriptorType);
        Printf("\nstring:          ");
       #endif
        char asciichar[31] = {0};

        uint8_t i;
        for (i=0; i<min(60, (d->length-2)); i+=2) // show only low value of Unicode character
        {
            if (d->string[i])
            {
              #ifdef _USB_TRANSFER_DIAGNOSIS_
            	Printf(d->string[i]);
              #endif
                asciichar[i/2] = (char)d->string[i];
            }
        }
      #ifdef _USB_TRANSFER_DIAGNOSIS_
        Printf('\t');
      #endif

        if (stringIndex == 2) // product name
        {
            MemCpy(device->productName, asciichar, 15);
            device->productName[15] = 0;

          #ifdef _USB_TRANSFER_DIAGNOSIS_
            Printf(" product name: %s", device->productName);
          #endif
        }
        else if (stringIndex == 3) // serial number
        {
            // take the last 12 characters:

            size_t last = strlen(asciichar); // store last position
            size_t j = (size_t)max((int)(last - 12), 0); // step 12 characters backwards, but not below zero

            MemCpy(device->serialNumber, asciichar + j, 12);
            device->serialNumber[12] = 0;
          #ifdef _USB_TRANSFER_DIAGNOSIS_
            Printf(" serial: %s", device->serialNumber);
          #endif
        }
    }
}

void usb_startInterruptInTransfer(usb_transfer_t* transfer, usb_device_t* device, usb_endpoint_t* epInterrupt, void* buffer, size_t length, void(*handler)(usb_transfer_t*, void*), void* handlerData, uint8_t frequency)
{
    usb_constructTransfer(device, transfer, USB_INTERRUPT, epInterrupt);
    transfer->handler = handler;
    transfer->handlerData = handlerData;
    transfer->frequency = frequency;
    usb_inTransaction(transfer, false, buffer, length);
    usb_scheduleTransfer(transfer);
    list_append(&usb_interruptTransfers, transfer);
}

void usb_pollInterruptTransfers()
{
	while(1)
	{
		element* e;

		for (e = usb_interruptTransfers.head; e; e = e->next)
		{
			usb_transfer_t* transfer = e->data;
			if (usb_pollTransfer(transfer))
			{
				if (transfer->handler)
					transfer->handler(transfer, transfer->handlerData);
			}
		}

		Schedule();
	}

}

void usb_IsochronousInTransfer(usb_device_t* device, usb_endpoint_t* epIsochronous, size_t length, void* buffer, uint16_t times, uint16_t numChains)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\n\nUSB: usb_IsochronousTransfer, dev: %X", device);
    textColor(TEXT);
  #endif

    usb_transfer_t transfer;
    uint32_t numberITD = times<<16 | numChains;
    transfer.data = &numberITD;
    usb_constructTransfer(device, &transfer, USB_ISOCHRONOUS, epIsochronous);
    usb_inTransaction(&transfer, false, buffer, length);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);
}

uintptr_t  paging_getPhysAddr(void* address)
{
	return (uintptr_t)address;
}

/*
* Copyright (c) 2010-2016 The PrettyOS Project. All rights reserved.
*
* http://www.prettyos.de
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
