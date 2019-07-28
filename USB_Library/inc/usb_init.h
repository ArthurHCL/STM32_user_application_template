#ifndef _USB_INIT_H_
#define _USB_INIT_H_



#include "usb_type.h"
#include "usb_core.h"



/* The number of current endpoint, it will be used to specify an endpoint */
extern u8 EPindex;

/* The number of current device, it is an index to the Device_Table */
//extern u8	Device_no;

/*
   Points to the DEVICE_INFO structure of current device
   The purpose of this register is to speed up the execution
*/
extern DEVICE_INFO *pInformation;

/*
   Points to the DEVICE_PROP structure of current device
   The purpose of this register is to speed up the execution
*/
extern DEVICE_PROP *pProperty;

/*
   Temporary save the state of Rx & Tx status.
     Whenever the Rx or Tx state is changed, its value is saved
   in this variable first and will be set to the EPRB or EPRA
   at the end of interrupt process
*/
extern u16 SaveState;

extern u16 wInterrupt_Mask;
extern USER_STANDARD_REQUESTS *pUser_Standard_Requests;



void USB_Init(void);



#endif

