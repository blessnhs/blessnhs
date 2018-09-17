/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f占� die Verwendung dieses Sourcecodes siehe unten
*/

#include "usb_hub.h"
#include "usb_hc.h"
#include "usb.h"
#include "xhci.h"
#include "array.h"
#include "../Synchronization.h"
#include "../DynamicMemory.h"
#include "../task.h"

static list_t hubs = list_init();
static MUTEX mutex = {
		 .qwTaskID = TASK_INVALIDID,
		 .dwLockCount = 0,
		 .bLockFlag = FALSE
};

//static void usb_showHubPortStatus(usb_device_t* device);


static bool usb_getHubDescriptor(usb_device_t* device)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: GET_HUB_DESCRIPTOR");
  #endif

    usb_hubDescriptor_t* hd = &(((usb_hub_t*)device->hub)->hubDesc);

    bool success = usb_controlIn(device, hd, GET_HUB, GET_DESCRIPTOR, 0x29, 0, 0, 9);

    if (success)
    {
        Printf("\nNumber of Ports:  %u\n", hd->numberPorts);
    }
    else
    {
        Printf("\nTransfer not successful!");
    }

    return success;
}

void usb_setupHub(usb_device_t* device)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: SETUP_HUB");
  #endif

    Printf("\nThis is a hub.");

    // init usb_hub_t
    usb_hub_t* hub = AllocateMemory(sizeof(usb_hub_t));
    device->hub = hub;
    hub->device = device;
    MemSet(hub->portNumber, 0, 16 * sizeof(*hub->portNumber));

    // Get interrupt endpoint
    hub->endpointInterrupt = 0;
    element* el;

    for (el = device->endpoints.head; el; el = el->next)
    {
        usb_endpoint_t* ep = el->data;
        if (ep->type == EP_INTERRUPT)
        {
            hub->endpointInterrupt = ep;
            break;
        }
    }

    bool success = usb_getHubDescriptor(device);
    if (!success) {
        Printf("\nHubDescriptor could not be read!");
        return;
    }
    Printf("\nThe hub owns %u downstream ports.",  hub->hubDesc.numberPorts);

    usb_getHubStatus(device);
    uint8_t j=1;
    for (j=1; j<=hub->hubDesc.numberPorts; j++)
    {
        // handle hub ports
        portStatusAndChange_t portStatusAndChange;

        usb_getPortStatus(device, j, &portStatusAndChange);

        uint8_t num = hc_addPort(device);
        hub->portNumber[j-1] = num;

        usb_setPortFeature(device, PORT_POWER, 0, j);
        WaitUsingDirectPIT(50);
        usb_setPortFeature(device, PORT_RESET, 0, j);
        WaitUsingDirectPIT(200);

        if (usb_isHubPortConnected(device, j))
        {
          #ifdef _USB_HUB_DIAGNOSIS_
            Printf("\nEnumerating now device at port %u as: %u\n", j, num);
          #endif
            hc_setupUSBDevice(((hc_port_t*)device->port->data)->hc, num, USB_HIGHSPEED); // TODO: Set correct speed
            usb_clearPortFeature(device, C_PORT_CONNECTION, j);
            WaitUsingDirectPIT(100);
        }
    }

    //usb_pollInterruptInTransfer(hub->device, hub->endpointInterrupt, 1);

    Lock(&mutex);
    list_append(&hubs, hub->device);
  #ifdef _USB_HUB_DIAGNOSIS_
    list_show(&hubs);
  #endif
    Unlock(&mutex);
}

void usb_destroyHub(usb_device_t* device)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: DESTROY_HUB");
  #endif

    Lock(&mutex);
    list_delete(&hubs, list_find(&hubs, device)); // take device out of watchdog list
    Unlock(&mutex);

  #ifdef _USB_HUB_DIAGNOSIS_
    list_show(&hubs);
  #endif

    hc_t* hc = ((hc_port_t*)device->port->data)->hc;

    uint8_t j=1;
    for (j=1; j<=(((usb_hub_t*)device->hub)->hubDesc).numberPorts; j++)
    {
        uint8_t num = ((usb_hub_t*)device->hub)->portNumber[j - 1];

        hc_destroyPort(hc, num);
    }

    FreeMemory(device->hub); // usb_hub_t
}

bool usb_clearHubFeature(usb_device_t* device, uint8_t feature)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: ClearHubFeature");
  #endif

    bool success = usb_controlSet(device, SET_HUB, CLEAR_FEATURE, 0, feature, 0); // check hi/lo val

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nClearHubFeature: Transfer not successful!");
    }

    return success;
}

bool usb_clearPortFeature(usb_device_t* device, uint8_t feature, uint8_t port) // The port number must be a valid port number for that hub, greater than zero.
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: ClearPortFeature");
  #endif

    bool success = usb_controlSet(device, SET_PORT, CLEAR_FEATURE, 0, feature, port); // check hi/lo val and wIndex

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nClearPortFeature: Transfer not successful!");
    }

    return success;
}

bool usb_clearTTBuffer(usb_device_t* device, uint8_t port)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: ClearTTBuffer");
  #endif
    /*
    If the hub supports a TT per port, then wIndex must specify the port number of the TT that encountered the high-speed errors (e.g., with the busy TT buffer).
    If the hub provides only a single TT, then wIndex must be set to one.
    */

    bool success = usb_controlSet(device, SET_PORT, CLEAR_TT_BUFFER, device->num, 0/*endpoint*/, port);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nClearTTBuffer: Transfer not successful!");
    }

    return success;
}

bool usb_getHubStatus(usb_device_t* device)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: GetHubStatus");
  #endif

    bool success = usb_controlIn(device, &((usb_hub_t*)device->hub)->status, GET_HUB, GET_STATUS, 0, 0, 0, 4);

    if (success)
    {
      #ifdef _USB_HUB_DIAGNOSIS_
        Printf("\nStatus/Change: LocalPowerSource: %u, Over-current condition: %u,\nLPS change: %u, OC change: %u",
            ((usb_hub_t*)device->hub)->status.localPowerSource,
            ((usb_hub_t*)device->hub)->status.overCurrent,
            ((usb_hub_t*)device->hub)->status.localPowerStatusChange,
            ((usb_hub_t*)device->hub)->status.overCurrentChange);
      #endif
    }
    else
    {
        Printf("\nGetHubStatus: Transfer not successful!");
    }

    return success;
}

bool usb_getPortStatus(usb_device_t* device, uint8_t port, portStatusAndChange_t* portStatusAndChange)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: GetPortStatus");
  #endif

    bool success = usb_controlIn(device, portStatusAndChange, GET_PORT, GET_STATUS, 0, 0, port, 4);

    if (success)
    {
        // ...
    }
    else
    {
        Printf("\nGetPortStatus: Transfer not successful!");
    }

    return success;
}

bool usb_isHubPortConnected(usb_device_t* device, uint8_t port)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: isHubPortEnabled");
  #endif

    portStatusAndChange_t portStatusAndChange;

    bool success = usb_controlIn(device, &portStatusAndChange, GET_PORT, GET_STATUS, 0, 0, port, 4);

    if (success)
    {
        return (bool)portStatusAndChange.currentConnectStatus;
    }
    else
    {
        Printf("\nisHubPortConnected - GetPortStatus: Transfer not successful!");
    }
    return false;
}

bool usb_getTTState(usb_device_t* device, uint8_t TT_flags, uint8_t port) // for vendor specific Debugging
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: GetTTState");
  #endif
    /*
    The TT_Flags bits 7..0 are reserved for future USB definition and must be set to zero. <== loVal
    The TT_Flags bits 15..8 are for vendor specific usage. <== hiVal
    */

    uint32_t TTstate = 0; // Bits 15..0 are reserved for future USB definition and must be set to zero.
                          // Bits 31..16 are for vendor specific usage.

    bool success = usb_controlIn(device, &TTstate, GET_PORT, GET_TT_STATE, TT_flags, 0, port, 4);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nGetTTState: Transfer not successful!");
    }

    return success;
}

bool usb_resetTT(usb_device_t* device, uint8_t port)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: ResetTT");
  #endif
    /*
    If the hub supports multiple TTs, then wIndex must specify the port number of the TT that is to be reset.
    If the hub provides only a single TT, then Port must be set to one.
    For a single TT Hub, the Hub can ignore the Port number.
    */
    bool success = usb_controlSet(device, SET_PORT, RESET_TT, 0, 0, port);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nResetTT: Transfer not successful!");
    }

    return success;
}

bool usb_setHubDescriptor(usb_device_t* device, uint8_t* hdData, uint8_t hdLength) // write hd in once
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: SetHubDescriptor");
  #endif

    bool success = usb_controlOut(device, hdData, SET_HUB, SET_DESCRIPTOR, 0x29, 0, 0, hdLength);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nSetHubDescriptor: Transfer not successful!");
    }

    return success;
}

bool usb_setHubFeature(usb_device_t* device, uint8_t feature) // refer to Table 11-17 for the feature selector definitions
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: SetHubFeature");
  #endif

    bool success = usb_controlSet(device, SET_HUB, SET_FEATURE, 0, feature, 0);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nSetHubFeature: Transfer not successful!");
    }

    return success;
}

bool usb_setPortFeature(usb_device_t* device, uint8_t feature, uint8_t selector, uint8_t port)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: SetPortFeature");
  #endif

    bool success = usb_controlSet(device, SET_PORT, SET_FEATURE, 0, feature, selector << 8 | port);

    if (success)
    {
      #ifdef _USB_TRANSFER_DIAGNOSIS_
        Printf("\nSetPortFeature: Transfer successful! Hub-Port: %u Feature: %u", port, feature);
      #endif
    }
    else
    {
        Printf("\nSetPortFeature: Transfer not successful! Feature: %u", feature);
    }

    return success;
}

bool usb_stopTT(usb_device_t* device, uint8_t port) // Restart a TT after a Stop_TT request via the Reset_TT request.
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: StopTT");
  #endif

    bool success = usb_controlSet(device, SET_PORT, STOP_TT, 0, 0, port);

    if (success)
    {
        //...
    }
    else
    {
        Printf("\nStopTT: Transfer not successful!");
    }

    return success;
}


void usb_hubWatchdog(void)
{
    Lock(&mutex);
    element* elem ;
    for (elem = hubs.head; elem; elem = elem->next)
    {
        usb_device_t* device = (usb_device_t*)elem->data;
        usb_hub_t* hub = (usb_hub_t*)device->hub;

        /*
        bool retVal = usb_getHubStatus(device);
        if (!retVal) // no successful transfer
        {
            usb_destroyHub(device);
            return;
        }
        */
        uint8_t j;
        for (j=1; j<=hub->hubDesc.numberPorts; j++)
        {
            // check hub ports
            portStatusAndChange_t portStatusAndChange;
            usb_getPortStatus(device, j, &portStatusAndChange);

            if (portStatusAndChange.connectStatusChange)
            {
                hc_t* hc = ((hc_port_t*)device->port->data)->hc;
                hc_port_t* port = hc_getPort(hc, hub->portNumber[j-1]);

                bool val = portStatusAndChange.currentConnectStatus;

                if (val && !port->connected) // attached
                {
                    uint8_t portNumber = hub->portNumber[j-1];

                    usb_speed_t speed = USB_HIGHSPEED; // TODO: set correct speed from LS/HS info
                    port->device = usb_createDevice(&port->port, speed);

                    // handle hub port
                    usb_setPortFeature(device, PORT_POWER, 0, j);

                    //usb_getPortStatus(device, j, &portStatusAndChange);

                    usb_setPortFeature(device, PORT_RESET, 0, j);
                    WaitUsingDirectPIT(50);

                    //usb_getPortStatus(device, j, &portStatusAndChange);

                    usb_setupDevice(port->device, portNumber+1);
                    port->connected = true;

                    WaitUsingDirectPIT(50);
                }
                else if (!val && port->connected) //detached
                {
                    Printf("\nDevice detached at hub port: %u", j);

                  #ifdef _USB_HUB_DIAGNOSIS_
                    Printf("\nDevice now to be destroyed: %X", port->device);
                  #endif

                    port->port.insertedDisk = 0;
                    port->connected = false;
                    usb_destroyDevice(port->device);
                }
                usb_clearPortFeature(device, C_PORT_CONNECTION, j);
            }
        }
    }
    Unlock(&mutex);
}

/*
void usb_showHubPortStatus(usb_device_t* device)
{
    usb_hub_t* hub = (usb_hub_t*)device->data;

    for (uint8_t j=1; j<=hub->hubDesc.numberPorts; j++)
    {
        // check hub ports
        portStatusAndChange_t portStatusAndChange;
        usb_getPortStatus(device, j, &portStatusAndChange);
        WaitUsingDirectPIT(50);

        Printf("\nport:%u  CS:%u CSC:%u  LS:%u HS:%u",
            j,
            portStatusAndChange.currentConnectStatus,
            portStatusAndChange.connectStatusChange,
            portStatusAndChange.lowSpeedDevAttached,
            portStatusAndChange.hiSpeedDevAttached  );
    }
}
*/


/*
* Copyright (c) 2013-2015 The PrettyOS Project. All rights reserved.
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
