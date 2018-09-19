/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f? die Verwendung dieses Sourcecodes siehe unten
*/

#include "usb_hid.h"
#include "../usb/usb_hc.h"
#include "../DynamicMemory.h"
//#include "mouse.h"
#include "../keyboard.h"
#include "../mouse.h"

static bool usb_getHidDescriptor(usb_device_t* device, uint8_t descType, uint8_t descIndex, uint16_t interfaceNum);
static void parseMouseReport(usb_transfer_t* transfer, void* data);
static void parseKeyboardReport(usb_transfer_t* transfer, void* data);

static const KEY_t usageIdToKey[] =
{
//  0           1           2           3           4           5            6             7
//---------------------------------------------------------------------------------------------------------
    0,          0,          0,          0,          USB_KEY_A,      USB_KEY_B,      USB_KEY_C,       USB_KEY_D,      // 0
    USB_KEY_E,      USB_KEY_F,      USB_KEY_G,      USB_KEY_H,      USB_KEY_I,      USB_KEY_J,       USB_KEY_K,        USB_KEY_L,
    USB_KEY_M,      USB_KEY_N,      USB_KEY_O,      USB_KEY_P,      USB_KEY_Q,      USB_KEY_R,       USB_KEY_S,        USB_KEY_T,      // 1
    USB_KEY_U,      USB_KEY_V,      USB_KEY_W,      USB_KEY_X,      USB_KEY_Y,      USB_KEY_Z,       USB_KEY_1,        USB_KEY_2,
    USB_KEY_3,      USB_KEY_4,      USB_KEY_5,      USB_KEY_6,      USB_KEY_7,      USB_KEY_8,       USB_KEY_9,        USB_KEY_0,      // 2
    USB_KEY_ENTER,  USB_KEY_ESC,    USB_KEY_BACK,   USB_KEY_TAB,    USB_KEY_SPACE,  USB_KEY_MINUS,   USB_KEY_EQUAL,    USB_KEY_OSQBRA,
    USB_KEY_CSQBRA, USB_KEY_BACKSL, 0,          	USB_KEY_SEMI,   0,          	USB_KEY_ACC,     USB_KEY_COMMA,    USB_KEY_DOT,    // 3
    USB_KEY_SLASH,  USB_KEY_CAPS,   USB_KEY_F1,     USB_KEY_F2,     USB_KEY_F3,     USB_KEY_F4,      USB_KEY_F5,       USB_KEY_F6,
    USB_KEY_F7,     USB_KEY_F8,     USB_KEY_F9,     USB_KEY_F10,    USB_KEY_F11,    USB_KEY_F12,     USB_KEY_PRINT,    USB_KEY_SCROLL, // 4
    USB_KEY_PAUSE,  USB_KEY_INS,    USB_KEY_HOME,   USB_KEY_PGUP,   USB_KEY_DEL,    USB_KEY_END,     USB_KEY_PGDWN,    USB_KEY_ARRR,
    USB_KEY_ARRL,   USB_KEY_ARRD,   USB_KEY_ARRU,   USB_KEY_NUM,    USB_KEY_KPSLASH,USB_KEY_KPMULT,  USB_KEY_KPMIN,    USB_KEY_KPPLUS, // 5
    USB_KEY_KPEN,   USB_KEY_KP1,    USB_KEY_KP2,    USB_KEY_KP3,    USB_KEY_KP4,    USB_KEY_KP5,     USB_KEY_KP6,      USB_KEY_KP7,
    USB_KEY_KP8,    USB_KEY_KP9,    USB_KEY_KP0,    USB_KEY_KPDOT,  0,          				0,           0,            0,          // 6
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 7
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 8
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 9
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 10
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 11
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 12
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 13
    0,          0,          0,          0,          0,          0,           0,            0,
    USB_KEY_LCTRL,  USB_KEY_LSHIFT, USB_KEY_LALT,   USB_KEY_LGUI,   USB_KEY_RCTRL,  USB_KEY_RSHIFT,  USB_KEY_ALTGR,    USB_KEY_RGUI,   // 14
    0,          0,          0,          0,          0,          0,           0,            0,
    0,          0,          0,          0,          0,          0,           0,            0,          // 15
    0,          0,          0,          0,          0,          0,           0,            0
};


void usb_setupHID(usb_interface_t* interface)
{
    Printf("\n\nSetup HID...");

    usb_hid_t* hid = AllocateMemory(sizeof(usb_hid_t));
    interface->data = hid;
    hid->interface = interface;
    hid->buffer = 0;
    hid->LEDs = 0;

    // Get interrupt endpoint (IN)
    usb_endpoint_t* endpointInterrupt = 0;
    element* el;
    for (el = interface->device->endpoints.head; el; el = el->next)
    {
        usb_endpoint_t* ep = el->data;
        if (ep->type == EP_INTERRUPT && ep->dir == EP_IN)
        {
            endpointInterrupt = ep;
            break;
        }
    }
    if (!endpointInterrupt)
    {
        Printf("\nHID has no interrupt endpoint!");
        return;
    }

     bool success = usb_getHidDescriptor(interface->device, DESC_HID, 0, interface->descriptor.interfaceNumber);
    if (!success)
    {
        Printf("\nHID-Descriptor could not be read!");
        return;
    }

    Printf("\nInterrupt endpoint: mps=%u, interval=%u", endpointInterrupt->mps, endpointInterrupt->interval);
    Printf("\nInterface protocol: ");

    switch (interface->descriptor.interfaceProtocol)
    {
        case 1:
            Printf("Keyboard ");
            if (interface->descriptor.interfaceSubclass == 1)
            {
                hid->buffer = AllocateMemory(8); // Keyboard sends reports of eight bytes
                usb_startInterruptInTransfer(&hid->interruptTransfer, hid->interface->device, endpointInterrupt, hid->buffer, 8, &parseKeyboardReport, hid, endpointInterrupt->interval);
            }
            break;
        case 2:
        	Printf("Mouse ");
            if (interface->descriptor.interfaceSubclass == 1)
            {
                size_t size = clamp(3, endpointInterrupt->mps, 8); // Mouse sends reports of three or more bytes. We are only interested in the first three.
                hid->buffer = AllocateMemory(size);
                usb_startInterruptInTransfer(&hid->interruptTransfer, hid->interface->device, endpointInterrupt, hid->buffer, size, &parseMouseReport, hid, endpointInterrupt->interval);
            }
            break;
        case 0: default:
            Printf("None (%u)", interface->descriptor.interfaceProtocol);
            break;
    }


}

void usb_destroyHID(usb_interface_t* interface)
{
    usb_hid_t* hid = interface->data;
    if (hid->buffer)
    {
        usb_destructTransfer(&hid->interruptTransfer);
        FreeMemory(hid->buffer);
    }
    FreeMemory(hid);
}

static void parseMouseReport(usb_transfer_t* transfer, void* data)
{
    usb_hid_t* hid = data;

    BYTE Status = 0;

    if(hid->buffer[0] & BIT(0))
    	Status = MOUSE_LBUTTONDOWN;
    else if(hid->buffer[0] & BIT(1))
    	Status = MOUSE_MBUTTONDOWN;
    else if(hid->buffer[0] & BIT(2))
    	Status = MOUSE_RBUTTONDOWN;

    AccumulateMouseDataAndPutQueueUSB(hid->buffer[1],hid->buffer[2],Status);
}

static void parseFlag(uint8_t flag, uint8_t bit, KEY_t key)
{
    bool pressed = (flag & BIT(bit)) != 0;
    if (pressed != keyPressed(key))
    {
        keyboard_keyPressedEvent(key, pressed);
    }
}



static void parseKeyboardReport(usb_transfer_t* transfer, void* data)
{
 	 usb_hid_t* hid = data;

	 // The first byte of the report consists of 8 flags indicating the status of the Ctrl, Shift, Alt and GUI Keys
	 parseFlag(hid->buffer[0], 0, USB_KEY_LCTRL);
	 parseFlag(hid->buffer[0], 1, USB_KEY_LSHIFT);
	 parseFlag(hid->buffer[0], 2, USB_KEY_LALT);
	 parseFlag(hid->buffer[0], 3, USB_KEY_LGUI);
	 parseFlag(hid->buffer[0], 4, USB_KEY_RCTRL);
	 parseFlag(hid->buffer[0], 5, USB_KEY_RSHIFT);
	 parseFlag(hid->buffer[0], 6, USB_KEY_ALTGR);
	 parseFlag(hid->buffer[0], 7, USB_KEY_RGUI);

    // There are no notifications when keys are released, instead we have to check all pressed keys if they are still reported as pressed.
	 KEY_t key;
    for (key = 0; key < _USB_KEY_LAST; key++)
    {
	    if (key == USB_KEY_LCTRL || key == USB_KEY_LSHIFT || key == USB_KEY_LALT || key == USB_KEY_LGUI ||
	        key == USB_KEY_RCTRL || key == USB_KEY_RSHIFT || key == USB_KEY_ALTGR || key == USB_KEY_RGUI)

	    	continue;

	        if (keyPressed(key))
	        {
	            bool stillPressed = false;
	            int i;
	            for (i = 0; i < 6; i++)
	            {
	                KEY_t pressedKey = usageIdToKey[hid->buffer[2 + i]];
	                if (pressedKey == key)
	                {
	                    stillPressed = true;
	                    hid->buffer[2 + i] = 0; // Avoid spurious repetitions of keyPressedEvent in the code below
	                    break;
	                }
	            }
	            if (!stillPressed)
	            {
	                keyboard_keyPressedEvent(key, false);
	            }
	        }
	    }

	    // USB Boot Keyboards support up to 6-Key-Rollover (in contrast to PS/2, which supports up to N-Key-Rollover)
    	uint8_t i;
	    for (i = 0; i < 6; i++)
	    {
	        KEY_t key = usageIdToKey[hid->buffer[2 + i]];
	        keyboard_keyPressedEvent(key, true);
	        if (key == USB_KEY_CAPS || key == USB_KEY_NUM || key == USB_KEY_SCROLL)
	        {
	            if (key == USB_KEY_CAPS)
	                hid->LEDs ^= BIT(1);
	            else if (key == USB_KEY_NUM)
	                hid->LEDs ^= BIT(0);
	            else if (key == USB_KEY_SCROLL)
	                hid->LEDs ^= BIT(2);
	            usb_controlOut(hid->interface->device, &hid->LEDs, 0x21, HID_SET_REPORT, 2, 0, hid->interface->descriptor.interfaceNumber, 1);
	        }
	    }


}

#ifdef _USB_HID_DIAGNOSIS_
static void showHidDescriptor(usb_hidDescriptor_t* d)
{
    textColor(IMPORTANT);
    Printf("\nHID descriptor (size: %u):", d->length);
    textColor(TEXT);

    Printf("\n    HID descriptor type:    ");
    switch (d->descType)
    {
        case DESC_HID:      Printf("HID Descriptor"); break;
        case DESC_REPORT:   Printf("Report Descriptor"); break;
        case DESC_PHYSICAL: Printf("Physical Descriptor"); break;
    }

    Printf("\n    Specification release:  %x", d->cdHID);
    Printf("\n    Country code:           %u", d->countryCode);
    Printf("\n    Number of class desc:   %u", d->numDescriptors);

    Printf("\n    Descriptor type 2:      ");
    switch (d->descType2)
    {
        case DESC_HID:      Printf("HID Descriptor"); break;
        case DESC_REPORT:   Printf("Report Descriptor"); break;
        case DESC_PHYSICAL: Printf("Physical Descriptor"); break;
    }

    Printf(" (size: %u)\n", d->descLength);
}
#endif

static bool usb_getHidDescriptor(usb_device_t* device, uint8_t descType, uint8_t descIndex, uint16_t interfaceNum)
{
  #ifdef _USB_TRANSFER_DIAGNOSIS_
    Printf("\n\nUSB: GET_DESCRIPTOR HID");
  #endif

    usb_hidDescriptor_t descriptor;

    bool success = usb_controlIn(device, &descriptor, 0x81, 6, descType, descIndex, interfaceNum, sizeof(descriptor));

    if (success)
    {
      #ifdef _USB_HID_DIAGNOSIS_
        showHidDescriptor(&descriptor);
      #endif
    }

    return success;
}
