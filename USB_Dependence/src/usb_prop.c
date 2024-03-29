#include "usb_prop.h"
/* user config include */
#include "main.h"
/* USB include */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_init.h"
#include "usb_pwr.h"
#include "usb_regs.h"
#include "usb_def.h"
/* user application include */
#include "hw_config.h"



u32 ProtocolValue;



DEVICE Device_Table = {
    EP_NUM,
    1
};

DEVICE_PROP Device_Property = {
    CustomHID_init,
    CustomHID_Reset,
    CustomHID_Status_In,
    CustomHID_Status_Out,
    CustomHID_Data_Setup,
    CustomHID_NoData_Setup,
    CustomHID_Get_Interface_Setting,
    CustomHID_GetDeviceDescriptor,
    CustomHID_GetConfigDescriptor,
    CustomHID_GetStringDescriptor,
    0,
    0x40 /* MAX PACKET SIZE */
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
    CustomHID_GetConfiguration,
    CustomHID_SetConfiguration,
    CustomHID_GetInterface,
    CustomHID_SetInterface,
    CustomHID_GetStatus,
    CustomHID_ClearFeature,
    CustomHID_SetEndPointFeature,
    CustomHID_SetDeviceFeature,
    CustomHID_SetDeviceAddress
};

static ONE_DESCRIPTOR Device_Descriptor = {
    (u8 *)CustomHID_DeviceDescriptor,
    CUSTOMHID_SIZ_DEVICE_DESC
};

static ONE_DESCRIPTOR Config_Descriptor = {
    (u8 *)CustomHID_ConfigDescriptor,
    CUSTOMHID_SIZ_CONFIG_DESC
};

static ONE_DESCRIPTOR CustomHID_Report_Descriptor = {
    (u8 *)CustomHID_ReportDescriptor,
    CUSTOMHID_SIZ_REPORT_DESC
};

static ONE_DESCRIPTOR CustomHID_Hid_Descriptor = {
    (u8 *)CustomHID_ConfigDescriptor + CUSTOMHID_OFF_HID_DESC,
    CUSTOMHID_SIZ_HID_DESC
};

static ONE_DESCRIPTOR String_Descriptor[4] = {
    {(u8 *)CustomHID_StringLangID, CUSTOMHID_SIZ_STRING_LANGID},
    {(u8 *)CustomHID_StringVendor, CUSTOMHID_SIZ_STRING_VENDOR},
    {(u8 *)CustomHID_StringProduct, CUSTOMHID_SIZ_STRING_PRODUCT},
    {(u8 *)CustomHID_StringSerial, CUSTOMHID_SIZ_STRING_SERIAL}
};



/*******************************************************************************
* Function Name  : CustomHID_init.
* Description    : Custom HID init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_init(void)
{
    /* Update the serial number string descriptor with the data from the unique ID */
    Get_SerialNum(&CustomHID_StringSerial[2], true);

    pInformation->Current_Configuration = 0;
    /* Connect the device */
    PowerOn();
    /* USB interrupts initialization */
    _SetISTR(0); /* clear pending interrupts */
    wInterrupt_Mask = IMR_MSK;
    SetCNTR(wInterrupt_Mask); /* set interrupts mask */

    bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : CustomHID_Reset.
* Description    : Custom HID reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_Reset(void)
{
    /* Set Joystick_DEVICE as not configured */
    pInformation->Current_Configuration = 0;
    pInformation->Current_Interface = 0; /*the default Interface */

    /* Current Feature initialization */
    pInformation->Current_Feature = CustomHID_ConfigDescriptor[7];

    SetBTABLE(BTABLE_ADDRESS);

    /* Initialize Endpoint 0 */
    SetEPType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    SetEPRxAddr(ENDP0, ENDP0_RXADDR);
    SetEPTxAddr(ENDP0, ENDP0_TXADDR);
    Clear_Status_Out(ENDP0);
    SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
    SetEPRxValid(ENDP0);

    /* Initialize Endpoint 1 */
    SetEPType(ENDP1, EP_INTERRUPT);
    SetEPRxAddr(ENDP1, ENDP1_RXADDR);
    SetEPRxCount(ENDP1, CUSTOM_HID_OUT_REPORT_SIZE);
    SetEPRxStatus(ENDP1, EP_RX_VALID);
    //SetEPTxStatus(ENDP1, EP_TX_DIS);

    /* Initialize Endpoint 2 */
    SetEPType(ENDP2, EP_INTERRUPT);
    SetEPTxAddr(ENDP2, ENDP2_TXADDR);
    SetEPTxCount(ENDP2, CUSTOM_HID_IN_REPORT_SIZE);
    //SetEPTxStatus(ENDP2, EP_TX_DIS);
    SetEPTxStatus(ENDP2, EP_TX_NAK);

    bDeviceState = ATTACHED;

    /* Set this device to response on default address */
    SetDeviceAddress(0);
}

/*******************************************************************************
* Function Name  : CustomHID_SetConfiguration.
* Description    : Udpade the device state to configured and command the ADC 
*                  conversion.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_SetConfiguration(void)
{
    if (pInformation->Current_Configuration != 0) {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
}

/*******************************************************************************
* Function Name  : CustomHID_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_SetDeviceAddress (void)
{
    bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : CustomHID_Status_In.
* Description    : Joystick status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_Status_In(void)
{
}

/*******************************************************************************
* Function Name  : CustomHID_Status_Out
* Description    : Joystick status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CustomHID_Status_Out(void)
{
}

/*******************************************************************************
* Function Name  : CustomHID_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT CustomHID_Data_Setup(u8 RequestNo)
{
    u8 *(*CopyRoutine)(u16);

    CopyRoutine = NULL;

    if ((RequestNo == GET_DESCRIPTOR) &&
        (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) &&
        (pInformation->USBwIndex0 == 0))
    {
        if (pInformation->USBwValue1 == REPORT_DESCRIPTOR) {
            CopyRoutine = CustomHID_GetReportDescriptor;
        }
        else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE) {
            CopyRoutine = CustomHID_GetHIDDescriptor;
        }
    } /* End of GET_DESCRIPTOR */

    /*** GET_PROTOCOL ***/
    else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) &&
             RequestNo == GET_PROTOCOL)
    {
        CopyRoutine = CustomHID_GetProtocolValue;
    }

    if (CopyRoutine == NULL) {
        return USB_UNSUPPORT;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);

    return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : CustomHID_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT CustomHID_NoData_Setup(u8 RequestNo)
{
    if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) &&
        (RequestNo == SET_PROTOCOL))
    {
        return CustomHID_SetProtocol();
    } else {
        return USB_UNSUPPORT;
    }
}

/*******************************************************************************
* Function Name  : CustomHID_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *CustomHID_GetDeviceDescriptor(u16 Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *CustomHID_GetConfigDescriptor(u16 Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *CustomHID_GetStringDescriptor(u16 Length)
{
    u8 wValue0 = pInformation->USBwValue0;

    if (wValue0 > 4) {
        return NULL;
    } else {
        return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
    }
}

/*******************************************************************************
* Function Name  : CustomHID_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *CustomHID_GetReportDescriptor(u16 Length)
{
    return Standard_GetDescriptorData(Length, &CustomHID_Report_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetHIDDescriptor.
* Description    : Gets the HID descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *CustomHID_GetHIDDescriptor(u16 Length)
{
    return Standard_GetDescriptorData(Length, &CustomHID_Hid_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_Get_Interface_Setting.
* Description    : tests the interface and the alternate setting according to the
*                  supported one.
* Input          : - Interface : interface number.
*                  - AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : USB_SUCCESS or USB_UNSUPPORT.
*******************************************************************************/
RESULT CustomHID_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
{
    if (AlternateSetting > 0) {
        return USB_UNSUPPORT;
    }
    else if (Interface > 0) {
        return USB_UNSUPPORT;
    }

    return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : CustomHID_SetProtocol
* Description    : Joystick Set Protocol request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT CustomHID_SetProtocol(void)
{
    u8 wValue0 = pInformation->USBwValue0;

    ProtocolValue = wValue0;

    return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : CustomHID_GetProtocolValue
* Description    : get the protocol value
* Input          : Length.
* Output         : None.
* Return         : address of the protcol value.
*******************************************************************************/
u8 *CustomHID_GetProtocolValue(u16 Length)
{
    if (Length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = 1;
        return NULL;
    } else {
        return (u8 *)(&ProtocolValue);
    }
}

