#include "usb_mem.h"
/* chip library include */
#include "stm32f10x.h"
/* USB include */
#include "usb_type.h"
#include "usb_regs.h"
#include "usb_conf.h"
#include "usb_pwr.h"
/* user application include */
#include "hw_config.h"



static volatile bool is_USB_TX_locked = false;



/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf: pointer to user memory area.
*                  - wPMABufAddr: address into PMA.
*                  - wNBytes: no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UserToPMABufferCopy(
    const u8  *pbUsrBuf,
    const u16  wPMABufAddr,
    const u16  wNBytes)
{
    u32  n = (wNBytes + 1) >> 1;
    u32  i, temp1, temp2;
    u16 *pdwVal;

    pdwVal = (u16 *)((wPMABufAddr << 1) + PMAAddr);
    for (i = n; i != 0; i--) {
        temp1 = (u16) * pbUsrBuf;
        pbUsrBuf++;
        temp2 = temp1 | (u16) *pbUsrBuf << 8;
        *pdwVal++ = temp2;
        pdwVal++;
        pbUsrBuf++;
    }
}

/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void PMAToUserBufferCopy(
    u8        *pbUsrBuf,
    const u16  wPMABufAddr,
    const u16  wNBytes)
{
    u32  n = (wNBytes + 1) >> 1;
    u32  i;
    u32 *pdwVal;

    pdwVal = (u32 *)((wPMABufAddr << 1) + PMAAddr);
    for (i = n; i != 0; i--) {
        *(volatile u16 *)pbUsrBuf++ = *pdwVal++;
        pbUsrBuf++;
    }
}



/*
    description:
            we may need to send USB data to PC in interrupt or in main(),
        in order to avoid data transmission collision,
        we must use the function to avoid transmission collision.
*/
void Is_USB_TX_Locked_Clear(void)
{
    __set_PRIMASK(1);
    is_USB_TX_locked = false;
    __set_PRIMASK(0);
}

/*
    description:
            we may need to send USB data to PC in interrupt or in main(),
        in order to avoid data transmission collision,
        we must use the function to avoid transmission collision.
    parameter:
        *data:
                the data buffer we are going to send.
        data_length:
                number of data to send(<=64).
    return:
        false:
                send request fail.
        true:
                send request success.
*/
bool Send_USB_Message_Exclusively(
    unsigned char * const data,
    const unsigned char   data_length)
{
    static unsigned long last_time = 0;

    /*
            if USB module is not in normal state,
        then we can not send any USB message.
    */
    if (CONFIGURED != bDeviceState) {
        return false;
    }

    __set_PRIMASK(1);
    if (!is_USB_TX_locked) {
        is_USB_TX_locked = true;
        __set_PRIMASK(0);

        UserToPMABufferCopy(data, ENDP2_TXADDR, data_length);
        SetEPTxValid(ENDP2);

        last_time = Current_Time_Get();

        return true;
    }
    __set_PRIMASK(0);

    /*
            in some abnormal situations,
        such as USB connect->disconnect->connect(make PC consider it is unchanged--tested in Linux),
        we may lose USB ACK response,
        so we need record the time to clear lock flag,
        to make the device is able to be reconnected.
    */
    if (30 < Elapsed_Time_Get(last_time)) {
        Is_USB_TX_Locked_Clear();
    }

    /* quit because another USB transmission has not been completed. */
    return false;
}

