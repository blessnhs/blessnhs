/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f?r die Verwendung dieses Sourcecodes siehe unten
*/

#include "uhci.h"
#include "irq.h"
#include "../DynamicMemory.h"
#include "usb.h"
#include "../utility.h"
#include "../../../32bit/source/page.h"

static list_t uhci = list_init();


static void uhci_start(uhci_t* u);
static void uhci_analysePortStatus(uhci_t* u, uint8_t j, uint16_t val);
static void uhci_handler(registers_t* r, pciDev_t* device);
static void uhci_initHC(uhci_t* u);
static void uhci_resetHC(uhci_t* u);
static void uhci_enablePorts(uhci_t* u);
static void uhci_resetPort(uhci_t* u, uint8_t port);
static uhciTD_t* uhci_createTD_SETUP(hc_port_t* port, uintptr_t next, bool toggle, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t i, uint16_t length, uint32_t device, uint8_t endpoint);
static uhciTD_t* uhci_createTD_IO(hc_port_t* port, uintptr_t next, uint8_t direction, bool toggle, uint16_t tokenBytes, uint32_t device, uint8_t endpoint, void* buffer, size_t duplicates);
static void      uhci_fillQH(uhci_t* u, uhciQH_t* head, const uhciTD_t* firstTD, bool terminate);


void uhci_install(pciDev_t* PCIdev)
{
  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n>>>uhci_install<<<");
  #endif

    static uint8_t numHC = 0;
    element* elem = list_alloc_elem(sizeof(uhci_t), "uhci");
    uhci_t* u = elem->data;
    u->PCIdevice  = PCIdev;
    u->PCIdevice->data = u;
    u->num = numHC++;
    u->enabledPortFlag = false;
    u->periodicScheduled = false;

    uint8_t i;
    for (i = 0; i < 6; i++)
    {
        if (PCIdev->bar[i].memoryType == PCI_IO)
        {
            u->bar = PCIdev->bar[i].baseAddress;
            u->memSize = PCIdev->bar[i].memorySize;

            Printf("bar found %d memSize %d\n",i,u->memSize);
            break;
        }
    }

    list_append_elem(&uhci, elem);

    //char str[10];
    //snPrintf(str, 10, "UHCI %u", u->num+1);

    uhci_start(u);

//    task_t* thread = create_cthread((void*)&uhci_start, str);
    //task_passStackParameter(thread, &u, sizeof(u));
    //scheduler_insertTask(thread);
}

static void uhci_start(uhci_t* u)
{
  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n>>>startUHCI<<<\n");
  #endif

    uhci_initHC(u);

    Printf("\n\n>>> Press key to close this console. <<<");
//    getch();
}

static void uhci_initHC(uhci_t* u)
{
    Printf("Initialize UHCI Host Controller:");

    // prepare PCI command register
    uint16_t pciCommandRegister = pci_configRead(u->PCIdevice, PCI_COMMAND, 2);
    pci_configWrite_word(u->PCIdevice, PCI_COMMAND, pciCommandRegister | PCI_CMD_IO | PCI_CMD_BUSMASTER); // resets status register, sets command register

  #ifdef _UHCI_DIAGNOSIS_
    Printf("\nPCI Command Register before:          %d", pciCommandRegister);
    Printf("\nPCI Command Register plus bus master: %d", pci_configRead(u->PCIdevice, PCI_COMMAND, 2));
 #endif

    irq_installPCIHandler(u->PCIdevice->irq, uhci_handler, u->PCIdevice);

    uhci_resetHC(u);
}

static void uhci_resetHC(uhci_t* u)
{
  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n\n>>>uhci_resetHostController<<<\n");
  #endif

    // http://www.lowlevel.eu/wiki/Universal_Host_Controller_Interface#Informationen_vom_PCI-Treiber_holen

    uint16_t legacySupport = pci_configRead(u->PCIdevice, UHCI_PCI_LEGACY_SUPPORT, 2);

    OutPortWord(u->bar + UHCI_USBCMD, UHCI_CMD_GRESET);
    WaitUsingDirectPIT(100); // at least 50 msec
    OutPortWord(u->bar + UHCI_USBCMD, 0);

    // get number of valid root ports
    uint8_t i;
    uint8_t rootPortCount = (u->memSize - UHCI_PORTSC1) / 2; // each port has a two byte PORTSC register
    for (i=2; i<rootPortCount; i++)
    {
        uint16_t portVal = InPortWord(u->bar + UHCI_PORTSC1 + i * 2);
        if (((portVal & UHCI_PORT_VALID) == 0) || // reserved bit 7 is already read as 1
             (portVal == 0xFFFF))
        {
            rootPortCount = i;
            break;
        }
    }

    if (rootPortCount > UHCIPORTMAX)
    {
        rootPortCount = UHCIPORTMAX;
    }

    hc_constructRootPorts(&u->hc, rootPortCount, &USB_UHCI);

  #ifdef _UHCI_DIAGNOSIS_
    Printf("\nUHCI root ports: %d", u->hc.rootPortCount);
  #endif

    uint16_t uhci_usbcmd = InPortWord(u->bar + UHCI_USBCMD);
    if ((legacySupport & ~(UHCI_PCI_LEGACY_SUPPORT_STATUS | UHCI_PCI_LEGACY_SUPPORT_NO_CHG | UHCI_PCI_LEGACY_SUPPORT_PIRQ)) ||
         (uhci_usbcmd & UHCI_CMD_RS)   ||
         (uhci_usbcmd & UHCI_CMD_CF)   ||
        !(uhci_usbcmd & UHCI_CMD_EGSM) ||
         (InPortWord(u->bar + UHCI_USBINTR) & UHCI_INT_MASK))
    {
        OutPortWord(u->bar + UHCI_USBSTS, UHCI_STS_MASK);    // reset all status bits
        WaitUsingDirectPIT(1);                             // wait one frame
        pci_configWrite_word(u->PCIdevice, UHCI_PCI_LEGACY_SUPPORT, UHCI_PCI_LEGACY_SUPPORT_STATUS); // resets support status bits in Legacy support register
        OutPortWord(u->bar + UHCI_USBCMD, UHCI_CMD_HCRESET); // reset hostcontroller

        WAIT_FOR_CONDITION(!(InPortWord(u->bar + UHCI_USBCMD) & UHCI_CMD_HCRESET), 50, 10, "USBCMD_HCRESET timed out!");

        OutPortWord(u->bar + UHCI_USBINTR, 0); // switch off all interrupts
        OutPortWord(u->bar + UHCI_USBCMD,  0); // switch off the host controller

        Printf("\nUHCI lagacy disable root ports: %d", u->hc.rootPortCount);

        for (i=0; i<u->hc.rootPortCount; i++) // switch off the valid root ports
        {
            OutPortWord(u->bar + UHCI_PORTSC1 + i*2, 0);
        }

    }

    // We work with a single QH for bulk and control transfers
    uhciQH_t* qh = AllocateMemory(sizeof(uhciQH_t));
    uhci_fillQH(u, qh, 0, true);
    u->qhPointerVirt = qh;

    // frame list
    u->framelistAddrVirt = (frPtr_t*)AllocateMemory(PAGE_DEFAULTSIZE);
    MemSetl(u->framelistAddrVirt->frPtr, paging_getPhysAddr(qh) | BIT_QH, 1024);

    // define each millisecond one frame, provide physical address of frame list, and start at frame 0
    OutPortByte(u->bar + UHCI_SOFMOD, 0x40); // SOF cycle time: 12000. For a 12 MHz SOF counter clock input, this produces a 1 ms Frame period.

    OutPortDWord(u->bar + UHCI_FRBASEADD, (DWORD)paging_getPhysAddr((void*)u->framelistAddrVirt->frPtr));
    OutPortWord(u->bar + UHCI_FRNUM, 0);

    // set PIRQ
    pci_configWrite_word(u->PCIdevice, UHCI_PCI_LEGACY_SUPPORT, UHCI_PCI_LEGACY_SUPPORT_PIRQ);

    // start hostcontroller and mark it configured with a 64-byte max packet
    OutPortWord(u->bar + UHCI_USBCMD, UHCI_CMD_RS | UHCI_CMD_CF | UHCI_CMD_MAXP);
    OutPortWord(u->bar + UHCI_USBINTR, UHCI_INT_MASK);  // switch on all interrupts

    for (i=0; i<u->hc.rootPortCount; i++) // reset the CSC of the valid root ports
    {
        OutPortWord(u->bar + UHCI_PORTSC1 + i*2, UHCI_PORT_CS_CHANGE);
    }

    OutPortWord(u->bar + UHCI_USBCMD, UHCI_CMD_RS | UHCI_CMD_CF | UHCI_CMD_MAXP | UHCI_CMD_FGR);
    WaitUsingDirectPIT(20);
    OutPortWord(u->bar + UHCI_USBCMD, UHCI_CMD_RS | UHCI_CMD_CF | UHCI_CMD_MAXP);
    WaitUsingDirectPIT(100);

  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n\nRoot-Ports   port1: %d  port2: %d\n", InPortWord (u->bar + UHCI_PORTSC1), InPortWord (u->bar + UHCI_PORTSC2));
  #endif

    bool run = InPortWord(u->bar + UHCI_USBCMD) & UHCI_CMD_RS;

    if (!(InPortWord(u->bar + UHCI_USBSTS) & UHCI_STS_HCHALTED))
    {
        Printf("\n\nRunStop bit: %d\n", run);

        uhci_enablePorts(u); // attaches the ports
    }
    else
    {
        Printf("\nFatal Error: UHCI - HCHalted. Ports will not be enabled.");
        Printf("\nRunStop Bit: %d  Frame Number: %d", run, InPortWord(u->bar + UHCI_FRNUM));
    }


}

// ports

static void uhci_enablePorts(uhci_t* u)
{
  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n\n>>>uhci_enablePorts<<<\n");
  #endif

    uint8_t j=0;
    for (j=0; j < u->hc.rootPortCount; j++)
    {
        uhci_resetPort(u, j);
    }

    u->enabledPortFlag = true;
    for (j=0; j < u->hc.rootPortCount; j++)
    {
        uint16_t val = InPortWord(u->bar + UHCI_PORTSC1 + 2*j);
        Printf("\nport %d: %d ", j+1, val);
        uhci_analysePortStatus(u, j, val);
    }
}

static void uhci_resetPort(uhci_t* u, uint8_t port)
{
    OutPortWord(u->bar + UHCI_PORTSC1 + 2*port, UHCI_PORT_RESET);
    WaitUsingDirectPIT(50); // do not delete this wait
    OutPortWord(u->bar + UHCI_PORTSC1 + 2*port, InPortWord(u->bar + UHCI_PORTSC1 + 2*port) & ~UHCI_PORT_RESET); // clear reset bit

    // wait and check, whether reset bit is really zero
    WAIT_FOR_CONDITION((InPortWord(u->bar + UHCI_PORTSC1 + 2 * port) & UHCI_PORT_RESET) == 0, 20, 20, "\nTimeour Error: Port did not reset! ");

    WaitUsingDirectPIT(10);

    // Enable
    OutPortWord(u->bar + UHCI_PORTSC1 + 2 * port, UHCI_PORT_CS_CHANGE | UHCI_PORT_ENABLE_CHANGE // clear Change-Bits Connected and Enabled
             | UHCI_PORT_ENABLE);      // set Enable-Bit
    WaitUsingDirectPIT(10);

  #ifdef _UHCI_DIAGNOSIS_
    Printf("Port Status: %d", InPortWord(u->bar + UHCI_PORTSC1 + 2 * port));
    //waitForKeyStroke();
  #endif
}


/*******************************************************************************************************
*                                                                                                      *
*                                              uhci handler                                            *
*                                                                                                      *
*******************************************************************************************************/

static void uhci_handler(registers_t* r, pciDev_t* device)
{
    // Check if an UHCI controller issued this interrupt
    uhci_t* u = device->data;
    bool found = false;
    element* el;
    for (el = uhci.head; el != 0; el = el->next)
    {
        if (el->data == u)
        {
            found = true;
            break;
        }
    }

    if (!found || u == 0) // Interrupt did not came from UHCI device
    {
      #ifdef _UHCI_DIAGNOSIS_
        Printf("Interrupt did not came from UHCI device!\n");
      #endif
        return;
    }

    uint16_t reg = u->bar + UHCI_USBSTS;
    uint16_t val = InPortWord(reg);

    if (val==0) // Interrupt came from another UHCI device
    {
      #ifdef _UHCI_DIAGNOSIS_
        Printf("Interrupt came from another UHCI device!\n");
      #endif
        return;
    }

    if (!(val & UHCI_STS_USBINT))
    {
        Printf("\nUSB UHCI %d: ", u->num);
    }


    if (val & UHCI_STS_USBINT)
    {
      #ifdef _UHCI_DIAGNOSIS_
        Printf("Frame: %d - USB transaction completed", InPortWord(u->bar + UHCI_FRNUM));
      #endif
        OutPortWord(reg, UHCI_STS_USBINT); // reset interrupt
    }

    if (val & UHCI_STS_RESUME_DETECT)
    {
        Printf("Resume Detect");
        OutPortWord(reg, UHCI_STS_RESUME_DETECT); // reset interrupt
    }

    if (val & UHCI_STS_HCHALTED)
    {
        Printf("Host Controller Halted");
        OutPortWord(reg, UHCI_STS_HCHALTED); // reset interrupt
    }

    if (val & UHCI_STS_HC_PROCESS_ERROR)
    {
        Printf("Host Controller Process Error");
        OutPortWord(reg, UHCI_STS_HC_PROCESS_ERROR); // reset interrupt
    }

    if (val & UHCI_STS_USB_ERROR)
    {
        Printf("USB Error");
        OutPortWord(reg, UHCI_STS_USB_ERROR); // reset interrupt
    }

    if (val & UHCI_STS_HOST_SYSTEM_ERROR)
    {
        Printf("Host System Error");
        OutPortWord(reg, UHCI_STS_HOST_SYSTEM_ERROR); // reset interrupt
        pci_analyzeHostSystemError(u->PCIdevice);
    }
}


/*******************************************************************************************************
*                                                                                                      *
*                                              PORT CHANGE                                             *
*                                                                                                      *
*******************************************************************************************************/

static void uhci_showPortState(uint16_t val)
{
  #ifdef _UHCI_DIAGNOSIS_
    if (val & UHCI_PORT_RESET)           {Printf(" RESET");}

    if (val & UHCI_SUSPEND)              {Printf(" SUSPEND");}
    if (val & UHCI_PORT_RESUME_DETECT)   {Printf(" RESUME DETECT");}

    if (val & UHCI_PORT_LOWSPEED_DEVICE) {Printf(" LOWSPEED DEVICE");}
    else                                 {Printf(" FULLSPEED DEVICE");}
    if (val & BIT(5))                    {Printf(" Line State: D-");}
    if (val & BIT(4))                    {Printf(" Line State: D+");}

    if (val & UHCI_PORT_ENABLE_CHANGE)   {Printf(" ENABLE CHANGE");}
    if (val & UHCI_PORT_ENABLE)          {Printf(" ENABLED");}

    if (val & UHCI_PORT_CS_CHANGE)       {Printf(" DEVICE CHANGE");}
    if (val & UHCI_PORT_CS)              {Printf(" DEVICE ATTACHED");}
    else                                 {Printf(" NO DEVICE ATTACHED");}
  #endif
}

static void uhci_analysePortStatus(uhci_t* u, uint8_t j, uint16_t val)
{
    hc_port_t* port = hc_getPort(&u->hc, j);

    if (val & UHCI_PORT_LOWSPEED_DEVICE)
        Printf("Lowspeed");
    else
        Printf("Fullspeed");

    if ((val & UHCI_PORT_CS) && !port->connected)
    {
        Printf(" device attached.");
        port->connected = true;
        WaitUsingDirectPIT(100); // Wait 100ms until power is stable (USB 2.0, 9.1.2)
        uhci_resetPort(u, j);   // reset on attached
      #ifdef _UHCI_DIAGNOSIS_
        //waitForKeyStroke();
      #endif

        hc_setupUSBDevice(&u->hc, j, (val&UHCI_PORT_LOWSPEED_DEVICE)?USB_LOWSPEED:USB_FULLSPEED);
    }
    else if (!(val & UHCI_PORT_CS) && port->connected)
    {
        Printf(" device removed.");
        port->connected = false;

        if (port->device)
        {
            usb_destroyDevice(port->device);
            port->device = 0;
        }
    }

    uhci_showPortState(val);


}

void uhci_pollDisk(port_t* dev)
{
    uhci_t* u = (uhci_t*)((hc_port_t*)dev->data)->hc;

    uint8_t j=0;
    for (j=0; j < u->hc.rootPortCount; j++)
    {
        uint16_t val = InPortWord(u->bar + UHCI_PORTSC1 + 2*j);

        if (val & UHCI_PORT_CS_CHANGE)
        {
            OutPortWord(u->bar + UHCI_PORTSC1 + 2*j, UHCI_PORT_CS_CHANGE);
            Printf("\nUHCI %d: Port %d changed: ", u->num, j+1);
            uhci_analysePortStatus(u, j, val);
        }
    }
}


/*******************************************************************************************************
*                                                                                                      *
*                                            Transactions                                              *
*                                                                                                      *
*******************************************************************************************************/

static bool isTransactionSuccessful(uhciTD_t* uTD);
static void uhci_showStatusbyteTD(uhciTD_t* uTD);

void uhci_setupTransfer(usb_transfer_t* transfer)
{
    uhci_t* u = (uhci_t*)((hc_port_t*)transfer->device->port->data)->hc;
    if (transfer->type == USB_INTERRUPT)
        transfer->data = AllocateMemory(sizeof(uhciQH_t) * 16);
    else
        transfer->data = u->qhPointerVirt; // Use main QH
}

void uhci_setupTransaction(usb_transfer_t* transfer, usb_transaction_t* usbTransaction, bool toggle, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t i, uint16_t length)
{
    hc_port_t* port = (hc_port_t*)transfer->device->port->data;

    uhciTD_t* uTD = usbTransaction->data = uhci_createTD_SETUP(port, BIT_T, toggle, type, req, hiVal, loVal, i, length, transfer->device->num, transfer->endpoint->address);

  #ifdef _UHCI_DIAGNOSIS_
    usb_request_t* request = (usb_request_t*)uTD->virtBuffer;
    Printf("\ntype: %d req: %d valHi: %d valLo: %d index: %d len: %d", request->type, request->request, request->valueHi, request->valueLo, request->index, request->length);
    Printf("\nuhci_setup - \ttoggle: %d \tlength: %d \tdev: %d \tendp: %d", toggle, length, transfer->device->num, transfer->endpoint->address);
  #endif

    if (transfer->transactions.tail)
    {
        uhciTD_t* uTD_last = ((usb_transaction_t*)transfer->transactions.tail->data)->data;
        uTD_last->next = (paging_getPhysAddr(uTD) & 0xFFFFFFF0) | BIT_Vf; // build vertical TD queue // BIT_Vf=1 Depth first, BIT_Vf=0 Breadth first
    }
}

void uhci_inTransaction(usb_transfer_t* transfer, usb_transaction_t* usbTransaction, bool toggle, void* buffer, size_t length)
{
    hc_port_t* port = (hc_port_t*)transfer->device->port->data;

    uhciTD_t* uTD = usbTransaction->data = uhci_createTD_IO(port, BIT_T, UHCI_TD_IN, toggle, length, transfer->device->num, transfer->endpoint->address, buffer, transfer->type == USB_INTERRUPT ? 1 : 0);

  #ifdef _UHCI_DIAGNOSIS_
    Printf("\nuhci_in - \ttoggle: %d \tlength: %d \tdev: %d \tendp: %d", toggle, length, transfer->device->num, transfer->endpoint->address);
  #endif

    if (transfer->transactions.tail)
    {
        uhciTD_t* uTD_last = ((usb_transaction_t*)transfer->transactions.tail->data)->data;
        uTD_last->next = (paging_getPhysAddr(uTD) & 0xFFFFFFF0) | BIT_Vf; // build vertical TD queue // BIT_Vf=1 Depth first, BIT_Vf=0 Breadth first
        if (transfer->type == USB_INTERRUPT) // Refresh duplicate
            uTD_last[1].next = uTD_last[0].next;
    }
}

void uhci_outTransaction(usb_transfer_t* transfer, usb_transaction_t* usbTransaction, bool toggle, const void* buffer, size_t length)
{
    hc_port_t* port = (hc_port_t*)transfer->device->port->data;

    uhciTD_t* uTD = usbTransaction->data = uhci_createTD_IO(port, BIT_T, UHCI_TD_OUT, toggle, length, transfer->device->num, transfer->endpoint->address, 0, transfer->type == USB_INTERRUPT ? 1 : 0);

    if (buffer != 0 && length != 0)
    {
        MemCpy(uTD->virtBuffer, buffer, length);
    }

  #ifdef _UHCI_DIAGNOSIS_
    Printf("\nuhci_out - \ttoggle: %d \tlength: %d \tdev: %d \tendp: %d", toggle, length, transfer->device->num, transfer->endpoint->address);
  #endif

    if (transfer->transactions.tail)
    {
        uhciTD_t* uTD_last = ((usb_transaction_t*)transfer->transactions.tail->data)->data;
        uTD_last->next = (paging_getPhysAddr(uTD) & 0xFFFFFFF0) | BIT_Vf; // build vertical TD queue // BIT_Vf=1 Depth first, BIT_Vf=0 Breadth first
        if (transfer->type == USB_INTERRUPT) // Refresh duplicate
            uTD_last[1].next = uTD_last[0].next;
    }
}

void uhci_scheduleTransfer(usb_transfer_t* transfer)
{
  #ifdef _UHCI_DIAGNOSIS_
    Printf("\n\nuhci_issue_Transfer");
  #endif

    uhci_t* u = (uhci_t*)((hc_port_t*)transfer->device->port->data)->hc; // HC
    uhciTD_t* firstTD = ((usb_transaction_t*)transfer->transactions.head->data)->data;
    uhci_fillQH(u, transfer->data, firstTD, transfer->type != USB_INTERRUPT);
    if (transfer->type == USB_INTERRUPT)
    {
        u->periodicScheduled = true;
        size_t i;
        for (i = 0; i < 1024; i += transfer->frequency)
            u->framelistAddrVirt->frPtr[i] = paging_getPhysAddr(transfer->data) | BIT_QH;
    }

    transfer->success = true;

    // start scheduler, if necessary
    if (InPortWord(u->bar + UHCI_USBSTS) & UHCI_STS_HCHALTED)
    {
        OutPortWord(u->bar + UHCI_FRNUM, 0);
        OutPortWord(u->bar + UHCI_USBCMD, InPortWord(u->bar + UHCI_USBCMD) | UHCI_CMD_RS);
    }
}

bool uhci_pollTransfer(usb_transfer_t* transfer)
{
    // check completion
    transfer->success = true;
    bool completed = true;
    element* elem;
    for (elem = transfer->transactions.head; elem; elem = elem->next)
    {
        uhciTD_t* uTD = ((usb_transaction_t*)elem->data)->data;
        if (!uTD->active)
            uhci_showStatusbyteTD(uTD);

        transfer->success = transfer->success && isTransactionSuccessful(uTD);
        completed = completed && !uTD->active;
    }
    if (!completed)
        return false;

    // Schedule it again
    for (elem = transfer->transactions.head; elem; elem = elem->next)
    {
        static bool toggle = false;
        toggle = !toggle;
        uhciTD_t* uTD = ((usb_transaction_t*)elem->data)->data;
        MemCpy(uTD, uTD + 1, sizeof(*uTD));
        uTD->dataToggle = transfer->endpoint->toggle;
        transfer->endpoint->toggle = !transfer->endpoint->toggle;
    }
    uhci_t* u = (uhci_t*)((hc_port_t*)transfer->device->port->data)->hc; // HC
    uhciTD_t* firstTD = ((usb_transaction_t*)transfer->transactions.head->data)->data;
    uhci_fillQH(u, transfer->data, firstTD, transfer->type != USB_INTERRUPT);

    return transfer->success;
}

void uhci_waitForTransfer(usb_transfer_t* transfer)
{
    uhci_t* u = (uhci_t*)((hc_port_t*)transfer->device->port->data)->hc; // HC

    // wait for completion
    uint32_t timeout = 150; // Wait up to 150*10ms = 1.5 seconds
    element* dlE = transfer->transactions.head;
    while (timeout > 0)
    {
        uhciTD_t* uTD = ((usb_transaction_t*)dlE->data)->data;

        while (!uTD->active)
        {
            dlE = dlE->next;
            if (dlE == 0)
                break;
            uTD = ((usb_transaction_t*)dlE->data)->data;
        }
        if (dlE == 0)
            break;
        WaitUsingDirectPIT(10);
        timeout--;
    }
    if (timeout == 0)
    {
        Printf("\nUHCI: Timeout!");
    }

    // stop scheduler, if nothing more to do
    if (!u->periodicScheduled)
        OutPortWord(u->bar + UHCI_USBCMD, InPortWord(u->bar + UHCI_USBCMD) & ~UHCI_CMD_RS);

    // check conditions and save data
    element* elem;
    for (elem = transfer->transactions.head; elem != 0; elem = elem->next)
    {
        uhciTD_t* uTD = ((usb_transaction_t*)elem->data)->data;
        uhci_showStatusbyteTD(uTD);
        transfer->success = transfer->success && isTransactionSuccessful(uTD); // executed w/o error
    }

    #ifdef _UHCI_DIAGNOSIS_
    Printf("\nQH:%d,QH->tfer:%d,", paging_getPhysAddr(transfer->data), ((uhciQH_t*)transfer->data)->transfer);

    for (elem = transfer->transactions.head; elem != 0; elem = elem->next)
    {
        uhciTD_t* uTD = ((usb_transaction_t*)elem->data)->data;

        if (elem == transfer->transactions.head)
        {
            Printf("\nTD:%d->%d,", paging_getPhysAddr(uTD), uTD->next);
        }
        else
        {
            Printf(" TD:%d->%d,", paging_getPhysAddr(uTD), uTD->next);
        }
    }
    #endif

    if (transfer->success)
    {
        #ifdef _UHCI_DIAGNOSIS_
        Printf("\nTransfer successful.");
        #endif
    }
    else
    {
        Printf("\nTransfer failed.");
    }
}

void uhci_destructTransfer(usb_transfer_t* transfer)
{
	element* elem;
    for (elem = transfer->transactions.head; elem != 0; elem = elem->next)
    {
        uhciTD_t* uTD = ((usb_transaction_t*)elem->data)->data;

        FreeMemory(uTD->virtBuffer);
        FreeMemory(uTD);
    }

  #ifdef _UHCI_DIAGNOSIS_
    //waitForKeyStroke();
  #endif
}


/*******************************************************************************************************
*                                                                                                      *
*                                            uhci QH TD functions                                      *
*                                                                                                      *
*******************************************************************************************************/

static uhciTD_t* uhci_allocTD(uintptr_t next, hc_port_t* port, size_t duplicates)
{
    size_t size = (1 + duplicates) * sizeof(uhciTD_t);
    uhciTD_t* td = (uhciTD_t*)AllocateMemory(size); // 16 byte alignment
    MemSet(td, 0, size);

    if (next != BIT_T)
        td->next = (paging_getPhysAddr((void*)next) & 0xFFFFFFF0) | BIT_Vf;
    else
        td->next = BIT_T;

    td->errorCounter       = 3;                // Stop after three errors
    td->lowSpeedDevice     = (port->device->speed==USB_LOWSPEED)?1:0;
    td->active             = 1;                // to be executed
    td->intOnComplete      = 0;                // We want an interrupt after complete transfer

    return td;
}

static void* uhci_allocTDbuffer(uhciTD_t* td, size_t length)
{
    if (length)
    {
        td->virtBuffer = AllocateMemory(length);
        td->buffer = paging_getPhysAddr(td->virtBuffer);
    }
    else
    {
        td->virtBuffer = 0;
        td->buffer = 0;
    }

    return td->virtBuffer;
}


static uhciTD_t* uhci_createTD_SETUP(hc_port_t* port, uintptr_t next, bool toggle, uint8_t type, uint8_t req, uint8_t hiVal, uint8_t loVal, uint16_t i, uint16_t length, uint32_t device, uint8_t endpoint)
{
    uhciTD_t* td = uhci_allocTD(next, port, 0);

    td->PacketID      = UHCI_TD_SETUP;

    td->dataToggle    = toggle; // Should be toggled every list entry

    td->deviceAddress = device;
    td->endpoint      = endpoint;
    td->maxLength     = 8-1;

    usb_request_t* request = uhci_allocTDbuffer(td, sizeof(usb_request_t));
    request->type    = type;
    request->request = req;
    request->valueHi = hiVal;
    request->valueLo = loVal;
    request->index   = i;
    request->length  = length;

    return (td);
}

static uhciTD_t* uhci_createTD_IO(hc_port_t* port, uintptr_t next, uint8_t direction, bool toggle, uint16_t tokenBytes, uint32_t device, uint8_t endpoint, void* buffer, size_t duplicates)
{
    uhciTD_t* td = uhci_allocTD(next, port, duplicates);

    td->PacketID      = direction; // UHCI_TD_IN or UHCI_TD_OUT

    if (tokenBytes)
        td->maxLength = (tokenBytes-1U) & 0x7FFU;
    else
        td->maxLength = 0x7FF;

    td->dataToggle    = toggle; // Should be toggled every list entry

    td->deviceAddress = device;
    td->endpoint      = endpoint;

    if (buffer)
        td->buffer = paging_getPhysAddr(buffer);
    else
        uhci_allocTDbuffer(td, tokenBytes);

    size_t i = 0;
    for(i = 0; i < duplicates; i++)
    {
        MemCpy(td + i + 1, td, sizeof(*td));
    }
    return (td);
}

static void uhci_fillQH(uhci_t* u, uhciQH_t* head, const uhciTD_t* firstTD, bool terminate)
{
    if (terminate)
        head->next = BIT_T;
    else
        head->next = paging_getPhysAddr(u->qhPointerVirt) | BIT_QH;

    if (firstTD == 0)
        head->transfer = BIT_T;
    else
        head->transfer = (paging_getPhysAddr(firstTD) & 0xFFFFFFF0);
}


////////////////////
// analysis tools //
////////////////////

static void uhci_showStatusbyteTD(uhciTD_t* uTD)
{
    if (uTD->bitstuffError)     Printf("\nBitstuff Error ");          // receive data stream contained a sequence of more than 6 ones in a row
    if (uTD->crc_timeoutError)  Printf("\nNo Response from Device "); // no response from the device (CRC or timeout)
    if (uTD->nakReceived)       Printf("\nNAK received ");            // NAK handshake
    if (uTD->babbleDetected)    Printf("\nBabble detected ");         // Babble (fatal error), e.g. more data from the device than MAXP
    if (uTD->dataBufferError)   Printf("\nData Buffer Error ");       // HC cannot keep up with the data  (overrun) or cannot supply data fast enough (underrun)
    if (uTD->stall)             Printf("\tStalled ");                 // can be caused by babble, error counter (0) or STALL from device
    if (uTD->active)            Printf("\tHC still active ");         // 1: HC will execute   0: set by HC after excution (HC will not excute next time)
  #ifdef _UHCI_DIAGNOSIS_
    if (uTD->intOnComplete)     Printf("\ninterrupt on complete ");   // 1: HC issues interrupt on completion of the frame in which the TD is executed
    if (uTD->isochrSelect)      Printf("\nisochronous TD ");          // 1: Isochronous TD
    if (uTD->lowSpeedDevice)    Printf("\nLowspeed Device ");         // 1: LS      0: FS
    if (uTD->shortPacketDetect) Printf("\nShortPacketDetect ");       // 1: enable  0: disable
  #endif
}

static bool isTransactionSuccessful(uhciTD_t* uTD)
{
    return
    (
        // no error
        (uTD->bitstuffError  == 0) && (uTD->crc_timeoutError == 0) && (uTD->nakReceived == 0) &&
        (uTD->babbleDetected == 0) && (uTD->dataBufferError  == 0) && (uTD->stall       == 0) &&
        // executed
        (uTD->active == 0)
    );
}

/*
* Copyright (c) 2011-2017 The PrettyOS Project. All rights reserved.
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
