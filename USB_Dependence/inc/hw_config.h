#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_



#include <stdbool.h>
#include "stm32f10x.h"



typedef enum {
    APPLICATION_LED_STATUS_START,

    /* physically the LED shall shut off if MCU work error. */
    APPLICATION_LED_STATUS_MCU_WORK_ERROR,
    /* MCU is in sleep mode to save power. */
    APPLICATION_LED_STATUS_SLEEP, /* do not enter sleep mode if it is charging. */
    /* MCU is running in idle status. */
    APPLICATION_LED_STATUS_IDLE_RUNNING,

    APPLICATION_LED_STATUS_END,
    APPLICATION_LED_STATUS_TOTAL = APPLICATION_LED_STATUS_END - 1
} APPLICATION_LED_STATUS;



void Backup_Registers_Read(
    const unsigned short   start_reg_num,
    unsigned short * const data,
    const unsigned char    data_len);
void Backup_Registers_Write(
    const unsigned short         start_reg_num,
    const unsigned short * const data,
    const unsigned char          data_len);

unsigned long Current_Time_Get(void);
unsigned long Elapsed_Time_Get(
    const unsigned long last_time);

void Application_LED_Status_Insert(
    const APPLICATION_LED_STATUS status);
void Application_LED_Status_Remove(
    const APPLICATION_LED_STATUS status);

void MCU_System_Reset(void);

void Set_System(void);

void Timer_4_Event_Handler(void);
void Timer_3_Event_Handler(void);

void USB_Port_Set(
    const bool is_enable);
void USB_Cable_Config(
    const FunctionalState NewState);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Set_Use_Serial(void);
void Set_Stored_Serial(
    unsigned char *serial_number_string);
void Get_SerialNum(
    unsigned char * const serial_number_string,
    const bool            is_unicode);



#endif

