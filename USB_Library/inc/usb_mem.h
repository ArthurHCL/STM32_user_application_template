#ifndef _USB_MEM_H_
#define _USB_MEM_H_



#include "usb_type.h"



void UserToPMABufferCopy(
    const u8  *pbUsrBuf,
    const u16  wPMABufAddr,
    const u16  wNBytes);
void PMAToUserBufferCopy(
    u8        *pbUsrBuf,
    const u16  wPMABufAddr,
    const u16  wNBytes);

void Is_USB_TX_Locked_Clear(void);
bool Send_USB_Message_Exclusively(
    unsigned char * const data,
    const unsigned char   data_length);



#endif

