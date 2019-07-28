#include "usb_istr.h"
/* USB include */
#include "usb_mem.h"
/* user library include */
#include "USB_message_process.h"



void EP1_OUT_Callback(void)
{
    USB_RX_Message_Process_In_Interrupt();
}

void EP2_IN_Callback(void)
{
    Is_USB_TX_Locked_Clear();
}

