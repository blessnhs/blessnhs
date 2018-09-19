#include "usb_msd.h"
#include "../usb/usb_hc.h"
#include "../DynamicMemory.h"
#include "../Utility.h"


static const uint32_t CSWMagicNotOK = 0x01010101;
static const uint32_t CSWMagicOK    = 0x53425355; // USBS
static const uint32_t CBWMagic      = 0x43425355; // USBC


static uint32_t showResultsRequestSense(const void* addr);
static void     usb_bulkReset(usb_msd_t* msd);
static void     usb_resetRecoveryMSD(usb_msd_t* msd);


void usb_setupMSD(usb_interface_t* interface)
{
	Printf("usb_setupMSD called \n");
	usb_msd_t* msd = AllocateMemory(sizeof(usb_msd_t));
    interface->data = msd;
    msd->interface = interface;

    // Get endpoints
    element* el = interface->device->endpoints.head;
    for (el = interface->device->endpoints.head; el; el = el->next)
    {
        usb_endpoint_t* ep = el->data;
        if (ep->type == EP_INTERRUPT)
            msd->endpointInterruptMSD = ep;
        else if (ep->type == EP_BULK)
        {
            if (ep->dir == EP_OUT)
                msd->endpointOutMSD = ep;
            else if(ep->dir == EP_IN)
                msd->endpointInMSD = ep;
        }
    }

    // Disk
    interface->device->port->insertedDisk = &msd->disk;
    msd->disk.port            = interface->device->port;
    msd->disk.type            = &USB_MSD;
    msd->disk.sectorSize      = 512;
    msd->disk.data            = msd;
    msd->disk.accessRemaining = 0;
    msd->disk.BIOS_driveNum   = 0;
    msd->disk.headCount       = 0;

    bool UFI = interface->descriptor.interfaceSubclass == 0x04;
    if (UFI)
    {
        msd->disk.optAccSecCount  = 18;
        msd->disk.alignedAccess   = true;
    }
    else
    {
        msd->disk.optAccSecCount  = 64;
        msd->disk.alignedAccess   = false;
    }

    msd->disk.secPerTrack     = 0;
//    MemSet(msd->disk.partition, 0, sizeof(partition_t*) * PARTITIONARRAYSIZE);
    deviceManager_attachDisk(&msd->disk);

//    beep(800, 100);
//    beep(1000, 100);

    usb_getStringDescriptor(interface->device);

    uint8_t loop = UFI?2:4;
    uint8_t i;
    for (i = 1; i<loop; i++) // fetch 3 strings
    {
        usb_getUnicodeStringDescriptor(interface->device, i);
    }

//    testMSD(msd); // test with some SCSI commands
}

char buffer[515];
void UsbReadSector2( int index )
{
    // 맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후, 라인 1로 커서 이동
    //ClearScreen();

	Printf("UsbReadSector Called usb_msd_count [%d]\n",index);

    buffer[513] = 0;

    disk_t *disk = GetDisk(0);
    if(disk == 0)
    	return;

    Printf("disk 0 Addr = %q\n",disk);
    Printf("disk 0 Sector Size = %q\n",disk->size);
    Printf("disk 0 Sector Name = %s\n",disk->port->name);

    usb_readSectors(1,buffer,1,disk);

    Printf("buff %s\n",buffer);
}

void usb_destroyMSD(usb_interface_t* interface)
{
	Printf("usb_destroyMSD called \n");

	usb_msd_t* msd = interface->data;
//    deviceManager_destructDisk(&msd->disk);
    interface->data = 0;
    FreeMemory(msd);
}

// Bulk-Only Mass Storage get maximum number of Logical Units
/*static uint8_t usb_getMaxLUN(usb_msd_t* msd) // only for bulk-only
{
  #ifdef _USB_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\nUSB2: usbTransferBulkOnlyGetMaxLUN, dev: %X interface: %u", device, numInterface);
    textColor(TEXT);
  #endif

    uint8_t maxLUN;

    usb_transfer_t transfer;
    usb_constructTransfer(msd->device, &transfer, USB_CONTROL, msd->device->endpoints.head->data);

    // bmRequestType bRequest  wValue wIndex    wLength   Data
    // 10100001b     11111110b 0000h  Interface 0001h     1 byte
    usb_setupTransaction(&transfer, 0xA1, 0xFE, 0, 0, msd->interface->interfaceNumber, 1);
    usb_inTransaction(&transfer, false, &maxLUN, 1);
    usb_outTransaction(&transfer, true, 0, 0); // handshake

    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);

    return maxLUN;
}*/

// Bulk-Only Mass Storage Reset
static void usb_bulkReset(usb_msd_t* msd)  // only for bulk-only
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_CYAN);
    Printf("\nusbTransferBulkOnlyMassStorageReset");
    textColor(TEXT);
  #endif

    // bmRequestType bRequest  wValue wIndex    wLength   Data
    // 00100001b     11111111b 0000h  Interface 0000h     none
    usb_controlSet(msd->interface->device, 0x21, 0xFF, 0, 0, msd->interface->descriptor.interfaceNumber);
}

static void formatSCSICommand(uint8_t SCSIcommand, struct usb_CommandBlockWrapper* cbw, uint32_t LBA, uint16_t TransferLength, usb_msd_t* msd)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\nSCSICommand Bulk-only: %u LBA %q TransferLength: %u", SCSIcommand, LBA, TransferLength);
  #endif

    MemSet(cbw, 0, sizeof(struct usb_CommandBlockWrapper));
    cbw->CBWSignature  = CBWMagic;                      // magic
    cbw->CBWTag        = 0x42424200 | SCSIcommand;      // device echoes this field in the CSWTag field of the associated CSW
    cbw->CBWDataTransferLength = TransferLength;        // Transfer length in bytes (only data)
    cbw->commandByte[0]        = SCSIcommand;           // Operation code
    switch (SCSIcommand)
    {
        case 0x00: // test unit ready(6)
            cbw->CBWFlags              = 0x00;          // Out: 0x00  In: 0x80
            cbw->CBWCBLength           = 6;             // only bits 4:0
            break;
        case 0x03: // Request Sense(6)
            cbw->CBWFlags              = 0x80;          // Out: 0x00  In: 0x80
            cbw->CBWCBLength           = 6;             // only bits 4:0
            cbw->commandByte[4]        = 18;            // Allocation length (max. bytes)
            break;
        case 0x12: // Inquiry(6)
            cbw->CBWFlags              = 0x80;          // Out: 0x00  In: 0x80
            cbw->CBWCBLength           = 6;             // only bits 4:0
            cbw->commandByte[4]        = 36;            // Allocation length (max. bytes)
            break;
        case 0x25: // read capacity(10)
            cbw->CBWFlags              = 0x80;          // Out: 0x00  In: 0x80
            cbw->CBWCBLength           = 10;            // only bits 4:0
            cbw->commandByte[2]        = BYTE4(LBA);    // LBA MSB
            cbw->commandByte[3]        = BYTE3(LBA);    // LBA
            cbw->commandByte[4]        = BYTE2(LBA);    // LBA
            cbw->commandByte[5]        = BYTE1(LBA);    // LBA LSB
            break;
        case 0x28: // read(10)
            cbw->CBWFlags               = 0x80;                  // Out: 0x00  In: 0x80
            cbw->CBWCBLength            = 10;                    // only bits 4:0
            cbw->commandByte[2]         = BYTE4(LBA);            // LBA MSB
            cbw->commandByte[3]         = BYTE3(LBA);            // LBA
            cbw->commandByte[4]         = BYTE2(LBA);            // LBA
            cbw->commandByte[5]         = BYTE1(LBA);            // LBA LSB
            cbw->commandByte[7]         = BYTE2(TransferLength/msd->disk.sectorSize); // MSB <--- blocks not byte!
            cbw->commandByte[8]         = BYTE1(TransferLength/msd->disk.sectorSize); // LSB
            break;
        case 0x2A: // write(10)
            cbw->CBWFlags               = 0x00;                  // Out: 0x00  In: 0x80
            cbw->CBWCBLength            = 10;                    // only bits 4:0
            cbw->commandByte[2]         = BYTE4(LBA);            // LBA MSB
            cbw->commandByte[3]         = BYTE3(LBA);            // LBA
            cbw->commandByte[4]         = BYTE2(LBA);            // LBA
            cbw->commandByte[5]         = BYTE1(LBA);            // LBA LSB
            cbw->commandByte[7]         = BYTE2(TransferLength/msd->disk.sectorSize); // MSB <--- blocks not byte!
            cbw->commandByte[8]         = BYTE1(TransferLength/msd->disk.sectorSize); // LSB
            break;
    }
}

static void formatSCSICommandUFI(uint8_t SCSIcommand, struct usb_CommandBlockWrapperUFI* cbw, uint32_t LBA, uint16_t TransferLength, usb_msd_t* msd)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\nSCSICommand UFI: %u LBA %u TransferLength: %u", SCSIcommand, LBA, TransferLength);
  #endif

    MemSet(cbw, 0, sizeof(struct usb_CommandBlockWrapperUFI));
    cbw->commandByte[0]        = SCSIcommand;           // Operation code
    switch (SCSIcommand)
    {
        case 0x00: // test unit ready(6)
            break;
        case 0x03: // Request Sense(6)
            cbw->commandByte[4]        = 18;            // Allocation length (max. bytes)
            break;
        case 0x12: // Inquiry(6)
            cbw->commandByte[4]        = 36;            // Allocation length (max. bytes)
            break;
        case 0x25: // read capacity(10)
            cbw->commandByte[2]        = BYTE4(LBA);    // LBA MSB
            cbw->commandByte[3]        = BYTE3(LBA);    // LBA
            cbw->commandByte[4]        = BYTE2(LBA);    // LBA
            cbw->commandByte[5]        = BYTE1(LBA);    // LBA LSB
            break;
        case 0x28: // read(10)
            cbw->commandByte[2]         = BYTE4(LBA);            // LBA MSB
            cbw->commandByte[3]         = BYTE3(LBA);            // LBA
            cbw->commandByte[4]         = BYTE2(LBA);            // LBA
            cbw->commandByte[5]         = BYTE1(LBA);            // LBA LSB
            cbw->commandByte[7]         = BYTE2(TransferLength/msd->disk.sectorSize); // MSB <--- blocks not byte!
            cbw->commandByte[8]         = BYTE1(TransferLength/msd->disk.sectorSize); // LSB
            break;
        case 0x2A: // write(10)
            cbw->commandByte[2]         = BYTE4(LBA);            // LBA MSB
            cbw->commandByte[3]         = BYTE3(LBA);            // LBA
            cbw->commandByte[4]         = BYTE2(LBA);            // LBA
            cbw->commandByte[5]         = BYTE1(LBA);            // LBA LSB
            cbw->commandByte[7]         = BYTE2(TransferLength/msd->disk.sectorSize); // MSB <--- blocks not byte!
            cbw->commandByte[8]         = BYTE1(TransferLength/msd->disk.sectorSize); // LSB
            break;
    }
}

static int checkSCSICommand(void* MSDStatus, usb_msd_t* msd, uint16_t TransferLength, uint8_t SCSIOpcode)
{
    // CSW Status
  #ifdef _USB_DIAGNOSIS_
    putch('\n');
    memshow(MSDStatus,13, false);
    putch('\n');
  #endif

    int error = 0;

    // check signature 0x53425355 // DWORD 0 (byte 0:3)
    uint32_t CSWsignature = *(uint32_t*)MSDStatus; // DWORD 0
    if (CSWsignature == CSWMagicOK)
    {
      #ifdef _USB_DIAGNOSIS_
        textColor(SUCCESS);
        Printf("\nCSW signature OK    ");
        textColor(TEXT);
      #endif
    }
    else if (CSWsignature == CSWMagicNotOK)
    {
        Printf("\nCSW signature wrong (not processed)");
        return -1;
    }
    else
    {
        Printf("\nCSW signature wrong (processed, but wrong value)");
        error = -2;
    }

    // check matching tag
    uint32_t CSWtag = *(((uint32_t*)MSDStatus)+1); // DWORD 1 (byte 4:7)

    if ((BYTE1(CSWtag) == SCSIOpcode) && (BYTE2(CSWtag) == 0x42) && (BYTE3(CSWtag) == 0x42) && (BYTE4(CSWtag) == 0x42))
    {
      #ifdef _USB_DIAGNOSIS_
        textColor(SUCCESS);
        Printf("CSW tag %yh OK    ",BYTE1(CSWtag));
        textColor(TEXT);
      #endif
    }
    else
    {
        Printf("\nError: CSW tag wrong");
        error = -3;
    }

    // check CSWDataResidue
    uint32_t CSWDataResidue = *(((uint32_t*)MSDStatus)+2); // DWORD 2 (byte 8:11)
    if (CSWDataResidue == 0)
    {
      #ifdef _USB_DIAGNOSIS_
        textColor(SUCCESS);
        Printf("\tCSW data residue OK    ");
        textColor(TEXT);
      #endif
    }
    else
    {
        Printf("\nCSW data residue: %u", CSWDataResidue);
    }

    // check status byte // DWORD 3 (byte 12)
    uint8_t CSWstatusByte = *(((uint8_t*)MSDStatus)+12); // byte 12 (last byte of 13 bytes)

    switch (CSWstatusByte)
    {
        case 0x00:
          #ifdef _USB_DIAGNOSIS_
            Printf("\tCSW status OK");
          #endif
            break;
        case 0x01:
            Printf("\nCommand %yh failed.", SCSIOpcode);
            return -4;
        case 0x02:
            Printf("\nPhase Error. ");
            Printf("Reset recovery is needed.");
            usb_resetRecoveryMSD(msd);
            return -5;
        default:
            Printf("\nCSW status byte: undefined value (error).");
            return -6;
    }

    return error;
}

/// cf. http://www.beyondlogic.org/usbnutshell/usb4.htm#Bulk
static bool usb_sendSCSICommand(usb_msd_t* msd, uint8_t SCSIcommand, uint32_t LBA, uint16_t TransferLength, void* dataBuffer, void* statusBuffer)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\nsendSCSICMD OUT part");
  #endif

    if(msd == 0)
    {
    	Printf("msd == 0\n");
    	return false;
    }

    bool UFI = msd->interface->descriptor.interfaceSubclass == 0x04;
    usb_transfer_t transfer;

    if(!UFI)
    {
        struct usb_CommandBlockWrapper cbw;
        formatSCSICommand(SCSIcommand, &cbw, LBA, TransferLength, msd);
        usb_constructTransfer(msd->interface->device, &transfer, USB_BULK, msd->endpointOutMSD);
        usb_outTransaction(&transfer, false, &cbw, 31);
        usb_scheduleTransfer(&transfer);
        usb_waitForTransfer(&transfer);
        usb_destructTransfer(&transfer);
    }
    else
    {
        struct usb_CommandBlockWrapperUFI cbw;
        formatSCSICommandUFI(SCSIcommand, &cbw, LBA, TransferLength, msd);
        Printf("usb_sendSCSICommand formatSCSICommandUFI \n");
        transfer.success = usb_controlOut(msd->interface->device, &cbw, 0x21, 0, 0, 0, msd->interface->descriptor.interfaceNumber, 12);
    }

    if (!transfer.success)
        return false;

  /**************************************************************************************************************************************/

  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\nsendSCSICMD IN part");
  #endif

    char tempStatusBuffer[13];
    if (statusBuffer == 0)
        statusBuffer = tempStatusBuffer;

    usb_constructTransfer(msd->interface->device, &transfer, USB_BULK, msd->endpointInMSD);
    if (TransferLength > 0)
    {
        usb_inTransaction(&transfer, false, dataBuffer, TransferLength);
    }
    if (!UFI)
        usb_inTransaction(&transfer, false, statusBuffer, 13);
    usb_scheduleTransfer(&transfer);
    usb_waitForTransfer(&transfer);
    usb_destructTransfer(&transfer);
    if (!transfer.success)
        return false;


  #ifdef _USB_DIAGNOSIS_
    if (TransferLength) // byte
    {
        putch('\n');
        memshow(dataBuffer, TransferLength, false);
        putch('\n');

        if ((TransferLength==512) || (TransferLength==36)) // data block (512 byte), inquiry feedback (36 byte)
        {
            memshow(dataBuffer, TransferLength, true); // alphanumeric
            putch('\n');
        }
    }
  #endif

    if (!UFI && checkSCSICommand(statusBuffer, msd, TransferLength, SCSIcommand) != 0)
        return false;
    return true;
}

static bool usb_sendSCSICommand_out(usb_msd_t* msd, uint8_t SCSIcommand, uint32_t LBA, uint16_t TransferLength, const void* dataBuffer, void* statusBuffer)
{

    bool UFI = msd->interface->descriptor.interfaceSubclass == 0x04;
    usb_transfer_t transfer;

    if (!UFI) // bulk-only
    {
        struct usb_CommandBlockWrapper cbw;
        formatSCSICommand(SCSIcommand, &cbw, LBA, TransferLength, msd);

        usb_constructTransfer(msd->interface->device, &transfer, USB_BULK, msd->endpointOutMSD);
        usb_outTransaction(&transfer, false, &cbw, 31);
        WaitUsingDirectPIT(10);
        usb_outTransaction(&transfer, false, dataBuffer, TransferLength);
        WaitUsingDirectPIT(10);
        usb_scheduleTransfer(&transfer);
        WaitUsingDirectPIT(10);
        usb_waitForTransfer(&transfer);
        WaitUsingDirectPIT(10);
        usb_destructTransfer(&transfer);
        WaitUsingDirectPIT(10);
        if (!transfer.success)
            return false;
    }
    else // UFI
    {
        struct usb_CommandBlockWrapperUFI cbw;
        formatSCSICommandUFI(SCSIcommand, &cbw, LBA, TransferLength, msd);
        // control EP
        usb_controlOut(msd->interface->device, &cbw, 0x21, 0, 0, 0, msd->interface->descriptor.interfaceNumber, 12);
        // bulk out EP
        usb_constructTransfer(msd->interface->device, &transfer, USB_BULK, msd->endpointOutMSD);
        usb_outTransaction(&transfer, false, dataBuffer, TransferLength);
        usb_scheduleTransfer(&transfer);
        usb_waitForTransfer(&transfer);
        usb_destructTransfer(&transfer);
        return transfer.success;
    }

  /**************************************************************************************************************************************/

    char tempStatusBuffer[13];
    if (statusBuffer == 0)
        statusBuffer = tempStatusBuffer;

    usb_constructTransfer(msd->interface->device, &transfer, USB_BULK, msd->endpointInMSD);
    usb_inTransaction(&transfer, false, statusBuffer, 13);
    WaitUsingDirectPIT(10);
    usb_scheduleTransfer(&transfer);
    WaitUsingDirectPIT(10);
    usb_waitForTransfer(&transfer);
    WaitUsingDirectPIT(10);
    usb_destructTransfer(&transfer);
    WaitUsingDirectPIT(10);
    return transfer.success;
}

static uint8_t testDeviceReady(usb_msd_t* msd)
{
    uint8_t maxTest = 500;

    bool UFI = msd->interface->descriptor.interfaceSubclass == 0x04;
    if (UFI)
        maxTest = 2;

    uint32_t timeout = maxTest;
    uint8_t statusByte = 0;

    while (timeout != 0)
    {
        timeout--;

        char statusBuffer[13];

        if (!UFI)
        {
          #ifdef _USB_FUNCTION_DIAGNOSIS_
            Printf("\n\nSCSI: test unit ready");
          #endif

            bool success = usb_sendSCSICommand(msd, 0x00, 0, 0, 0, statusBuffer); // dev, cmd, LBA, transfer length

            uint8_t statusByteTestReady = success ? statusBuffer[12] : 0xFF;

            if (timeout >= maxTest/2 && statusByteTestReady != 0)
            {
            	WaitUsingDirectPIT(50);
                continue;
            }
        }

      #ifdef _USB_FUNCTION_DIAGNOSIS_
        Printf("\n\nSCSI: request sense");
      #endif

        char dataBuffer[18];
        if (usb_sendSCSICommand(msd, 0x03, 0, 18, dataBuffer, statusBuffer)) // dev, cmd, LBA, transfer length
        {
            if (!UFI)
                statusByte = statusBuffer[12];

            uint32_t sense = showResultsRequestSense(dataBuffer);
            if (sense == 0 || sense == 6)
            {
                break;
            }
        }
        WaitUsingDirectPIT(50);
    }
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    waitForKeyStroke();
  #endif

    return statusByte;
}

// http://en.wikipedia.org/wiki/SCSI_Inquiry_Command
static char* analyzeInquiry(const void* addr)
{
    // cf. Jan Axelson, USB Mass Storage, page 140
    uint8_t PeripheralDeviceType = getField(addr, 0, 0, 5); // byte, shift, len
 // uint8_t PeripheralQualifier  = getField(addr, 0, 5, 3);
 // uint8_t DeviceTypeModifier   = getField(addr, 1, 0, 7);
    uint8_t RMB                  = getField(addr, 1, 7, 1);
  #ifdef _USB_DIAGNOSIS_
    uint8_t ANSIapprovedVersion  = getField(addr, 2, 0, 3);
  #endif
 // uint8_t ECMAversion          = getField(addr, 2, 3, 3);
 // uint8_t ISOversion           = getField(addr, 2, 6, 2);
    uint8_t ResponseDataFormat   = getField(addr, 3, 0, 4);
    uint8_t HISUP                = getField(addr, 3, 4, 1);
    uint8_t NORMACA              = getField(addr, 3, 5, 1);
 // uint8_t AdditionalLength     = getField(addr, 4, 0, 8);
    uint8_t CmdQue               = getField(addr, 7, 1, 1);
    uint8_t Linked               = getField(addr, 7, 3, 1);

    char vendorID[9];
    MemCpy(vendorID, addr+8, 8);
    vendorID[8]=0;

    char productID[17];
    MemCpy(productID, addr+16, 16);
    productID[16]=0;

  #ifdef _USB_DIAGNOSIS_
    char productRevisionLevel[5];
    MemCpy(productRevisionLevel, addr+32, 4);
    productRevisionLevel[4]=0;
  #endif

    Printf("\nVendor ID:  %s", vendorID);
    Printf("\nProduct ID: %s", productID);

  #ifdef _USB_DIAGNOSIS_
    Printf("\nRevision:   %s", productRevisionLevel);

    // Book of Jan Axelson, "USB Mass Storage", page 140:
    // Printf("\nVersion ANSI: %u  ECMA: %u  ISO: %u", ANSIapprovedVersion, ECMAversion, ISOversion);
    Printf("\nVersion: %u (4: SPC-2, 5: SPC-3)", ANSIapprovedVersion);
  #endif

    // Jan Axelson, USB Mass Storage, page 140
    if (ResponseDataFormat == 2)
    {
        Printf("\nResponse Data Format OK");
    }
    else
    {
        Printf("\nResponse Data Format is not OK: %u (should be 2)", ResponseDataFormat);
    }

    Printf("\nRemovable device type:            %s", RMB     ? "yes" : "no");
    Printf("\nSupports hierarch. addr. support: %s", HISUP   ? "yes" : "no");
    Printf("\nSupports normal ACA bit support:  %s", NORMACA ? "yes" : "no");
    Printf("\nSupports linked commands:         %s", Linked  ? "yes" : "no");
    Printf("\nSupports tagged command queuing:  %s", CmdQue  ? "yes" : "no");

    static const char* const PeripheralDeviceTypes[] =
    {
        "direct-access device (e.g., magnetic disk)",
        "sequential-access device (e.g., magnetic tape)",
        "printer device",
        "processor device",
        "write-once device",
        "CD/DVD device",
        "scanner device",
        "optical memory device (non-CD optical disk)",
        "medium Changer (e.g. jukeboxes)",
        "communications device",
        "defined by ASC IT8 (Graphic arts pre-press devices)",
        "defined by ASC IT8 (Graphic arts pre-press devices)",
        "Storage array controller device (e.g., RAID)",
        "Enclosure services device",
        "Simplified direct-access device (e.g., magnetic disk)",
        "Optical card reader/writer device",
        "Reserved for bridging expanders",
        "Object-based Storage Device",
        "Automation/Drive Interface",
        "Reserved",
        "Reserved",
        "Reduced block command (RBC) direct-access device",
        "Unknown or no device type"
    };
    Printf("\n%s", PeripheralDeviceTypes[PeripheralDeviceType]);

    return productID;
}

void testMSD(usb_msd_t* msd)
{
    // start with correct endpoint toggles and reset interface
    msd->endpointOutMSD->toggle = msd->endpointInMSD->toggle = false;

/*    bool UFI =  msd->interface->descriptor.interfaceSubclass == 0x04;
    if (!UFI)
    {
    	usb_bulkReset(msd); // Reset Interface
    	Sleep(1);
    }
*/
   // send SCSI command "inquiry (opcode: 0x12)"
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_BLUE);
    Printf("\n\nSCSI: inquiry");
    textColor(TEXT);
  #endif

    char inquiryBuffer[36];
    if (usb_sendSCSICommand(msd, 0x12 /*SCSI opcode*/, 0 /*LBA*/, 36 /*Bytes In*/, inquiryBuffer, 0))
       analyzeInquiry(inquiryBuffer);
    else
        Printf("\n\nInquiry failed.");


    // send SCSI command "test unit ready(6)"
    testDeviceReady(msd);

    // send SCSI command "read capacity(10)"
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    textColor(LIGHT_BLUE);
    Printf("\n\nSCSI: read capacity");
    textColor(TEXT);
  #endif

    uint32_t capacityBuffer[2];
    if (usb_sendSCSICommand(msd, 0x25 /*SCSI opcode*/, 0 /*LBA*/, 8 /*Bytes In*/, capacityBuffer, 0))
    {
        // MSB ... LSB
        capacityBuffer[0] = htonl(capacityBuffer[0]);
        capacityBuffer[1] = htonl(capacityBuffer[1]);

        msd->disk.size = (((uint64_t)capacityBuffer[0]) + 1) * ((uint64_t)capacityBuffer[1]);
        msd->disk.sectorSize = capacityBuffer[1];

        Printf("\n\nCapacity: %q, Last LBA: %q, block size: %q\n", msd->disk.size, capacityBuffer[0], msd->disk.sectorSize);

        //msd->disk.sectorSize = msd->disk.size / 512;

        //deviceManager_analyzeDisk(&msd->disk);
    }
    else
        Printf("\n\nRead Capacity failed.");
}


int ReadUSBSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA,int iSectorCount, char* pcBuffer )
{
	int result = usb_readSectors(dwLBA,pcBuffer,iSectorCount,GetDisk(0));
	if(result == 0)
		return TRUE;
	else
		return FALSE;
}

int WriteUSBSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA,int iSectorCount, char* pcBuffer )
{
	int result = usb_writeSectors(dwLBA,pcBuffer,iSectorCount,GetDisk(0));
	if(result == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL InitializeUSBHDD( DWORD dwTotalSectorCount )
{
    return TRUE;
}

FS_ERROR usb_readSectors(uint32_t sector, void* buffer, uint32_t count, disk_t* device)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\n\n>SCSI: read sector: %u", sector);
  #endif

    usb_msd_t* msd = device->data;

    if (usb_sendSCSICommand(msd, 0x28 /*SCSI opcode*/, sector /*LBA*/, count*device->sectorSize /*Bytes In*/, buffer, 0))
        return CE_GOOD;
    return CE_BAD_SECTOR_READ;
}

FS_ERROR usb_writeSectors(uint32_t sector, const void* buffer, uint32_t count, disk_t* device)
{
  #ifdef _USB_FUNCTION_DIAGNOSIS_
    Printf("\n\n>>> SCSI: write sector: %u", sector);
  #endif

    usb_msd_t* msd = device->data;

    if (usb_sendSCSICommand_out(msd, 0x2A /*SCSI opcode*/, sector /*LBA*/, count * device->sectorSize /*Bytes Out*/, buffer, 0))
        return CE_GOOD;
    return CE_WRITE_ERROR;
}

static void usb_resetRecoveryMSD(usb_msd_t* msd)
{
    // Reset Interface
    usb_bulkReset(msd);

    // TEST ////////////////////////////////////
    //usbSetFeatureHALT(device, device->numEndpointInMSD);
    //usbSetFeatureHALT(device, device->numEndpointOutMSD);

    // Clear Feature HALT to the Bulk-In  endpoint
    Printf("\nGetStatus: %u", usb_getStatus(msd->interface->device, msd->endpointInMSD));
    usb_clearFeatureHALT(msd->interface->device, msd->endpointInMSD);
    Printf("\nGetStatus: %u", usb_getStatus(msd->interface->device, msd->endpointInMSD));

    // Clear Feature HALT to the Bulk-Out endpoint
    Printf("\nGetStatus: %u", usb_getStatus(msd->interface->device, msd->endpointOutMSD));
    usb_clearFeatureHALT(msd->interface->device, msd->endpointOutMSD);
    Printf("\nGetStatus: %u", usb_getStatus(msd->interface->device, msd->endpointOutMSD));

    // set configuration to 1 and endpoint IN/OUT toggles to 0
    usb_setConfiguration(msd->interface->device, 1); // set first configuration
    uint8_t config = usb_getConfiguration(msd->interface->device);
    if (config != 1)
    {
        Printf("\tconfiguration: %u (to be: 1)", config);
    }

    // start with correct endpoint toggles and reset interface
    msd->endpointOutMSD->toggle = false;
    msd->endpointInMSD->toggle = false;
    usb_bulkReset(msd); // Reset Interface
}

static uint32_t showResultsRequestSense(const void* addr)
{
    uint32_t Valid        = getField(addr, 0, 7, 1); // byte 0, bit 7
    uint32_t ResponseCode = getField(addr, 0, 0, 7); // byte 0, bit 6:0
    uint32_t SenseKey     = getField(addr, 2, 0, 4); // byte 2, bit 3:0

    Printf("\n\nResults of \"request sense\":\n");
    if (ResponseCode >= 0x70 && ResponseCode <= 0x73)
    {
        Printf("Valid:\t\t");
        if (Valid == 0)
        {
            Printf("Sense data are not SCSI compliant");
        }
        else
        {
            Printf("Sense data are SCSI compliant");
        }
        Printf("\nResponse Code:\t");
        switch (ResponseCode)
        {
            case 0x70:
                Printf("Current errors, fixed format");
                break;
            case 0x71:
                Printf("Deferred errors, fixed format");
                break;
            case 0x72:
                Printf("Current error, descriptor format");
                break;
            case 0x73:
                Printf("Deferred error, descriptor format");
                break;
            default:
                Printf("No valid response code!");
                break;
        }

        static const char* const SenseKeys[] =
        {
            "No Sense",
            "Recovered Error - last command completed with some recovery action",
            "Not Ready - logical unit addressed cannot be accessed",
            "Medium Error - command terminated with a non-recovered error condition",
            "Hardware Error",
            "Illegal Request - illegal parameter in the command descriptor block",
            "Unit Attention - disc drive may have been reset.",
            "Data Protect - command read/write on a protected block",
            "Undefined",
            "Firmware Error",
            "Undefined",
            "Aborted Command - disc drive aborted the command",
            "Equal - SEARCH DATA command has satisfied an equal comparison",
            "Volume Overflow - buffered peripheral device has reached the end of medium partition",
            "Miscompare - source data did not match the data read from the medium",
            "Undefined"
        };
        Printf("\nSense Key:\t%s", SenseKeys[SenseKey]);
        return SenseKey;
    }

    Printf("No vaild response code!");
    return 0xFFFFFFFF;
}

/*
* Copyright (c) 2010-2017 The PrettyOS Project. All rights reserved.
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
