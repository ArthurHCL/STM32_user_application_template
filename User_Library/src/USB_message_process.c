#include "USB_message_process.h"
/* system include */
#include <stdbool.h>
#include <string.h>
/* user config include */
#include "main.h"
/* USB include */
#include "usb_mem.h"
#include "usb_conf.h"
#include "usb_regs.h"
/* user library include */
#include "hw_config.h"
#include "message_resolve.h"



/* host computer sends this string to try to enter bootloader code. */
#define BOOTLOADER_CODE_ENTER_COMMAND_STRING    "user application enter bootloader"

/*
        we may have mistakenly downloaded an uncorresponding application code,
    it means VID and PID of USB is uncorresponding between bootloader code and application code,
    in this situation,
    we can only send the common USB command to try to download corresponding application code again.
*/
#define BOOTLOADER_CODE_ENTER_AND_APPLICATION_CODE_CLEAR_COMMAND_STRING    "user enter bootloader and clear application"



/*
        if host computer needs to update firmware by bootloader code,
    it needs to indicate bootloader code the message.
        the content is several characters named as 'BOOT'(42 4F 4F 54),
    bootloader code will clean it.
*/
static const unsigned short backup_registers_bootloader_enter_command[2] = {
    0x4F42, 0x544F
};

/*
        if host computer needs to correct incorrect application code by bootloader code,
    it needs to indicate bootloader code the message.
        the content is several characters named as 'ANEW'(41 4E 45 57),
    bootloader code will clean it.
*/
static const unsigned short backup_registers_bootloader_enter_application_clear_command[2] = {
    0x4E41, 0x5745
};



static unsigned char                      USB_RX_buffer[CUSTOM_HID_OUT_REPORT_SIZE];
static APPLICATION_CODE_PACKET_RX * const application_code_packet_RX =
    (APPLICATION_CODE_PACKET_RX *)USB_RX_buffer;

static unsigned char                      USB_TX_buffer[CUSTOM_HID_IN_REPORT_SIZE];
static APPLICATION_CODE_PACKET_TX * const application_code_packet_TX =
    (APPLICATION_CODE_PACKET_TX *)USB_TX_buffer;

static volatile bool is_new_USB_message_existed = false;



void USB_RX_Message_Process_In_Interrupt(void)
{
    /* ignore the message because the previous message has not been handled. */
    if (is_new_USB_message_existed) {
        SetEPRxStatus(ENDP1, EP_RX_VALID);

        return;
    }

    /* get the new message. */
    PMAToUserBufferCopy(USB_RX_buffer, ENDP1_RXADDR, sizeof(USB_RX_buffer));
    SetEPRxStatus(ENDP1, EP_RX_VALID);
    is_new_USB_message_existed = true;

    /* if it is an enter bootloader code command then.... */
    if (!strcmp((char *)&USB_RX_buffer[0], BOOTLOADER_CODE_ENTER_COMMAND_STRING)) {
        Backup_Registers_Write(1, backup_registers_bootloader_enter_command, 2);
        MCU_System_Reset();
    }
    if (!strcmp((char *)&USB_RX_buffer[0], BOOTLOADER_CODE_ENTER_AND_APPLICATION_CODE_CLEAR_COMMAND_STRING)) {
        Backup_Registers_Write(1, backup_registers_bootloader_enter_application_clear_command, 2);
        MCU_System_Reset();
    }
}

void USB_RX_Message_Process_In_Main_While(void)
{
    /* directly finish the function because there is no new message. */
    if (!is_new_USB_message_existed) {
        return;
    }

    /* start to resolve message.... */
    if (Application_Code_Packet_Resolve(
            application_code_packet_RX,
            application_code_packet_TX,
            true))
    {
        is_new_USB_message_existed = false;

        /*
                when MCU is sending USB message to PC positively,
            the USB communication may be using,
            so we should wait until USB communication is free,
            then we can response the command from PC.
        */
        while (!Send_USB_Message_Exclusively(USB_TX_buffer, sizeof(USB_TX_buffer)));
    } else {
        is_new_USB_message_existed = false;
    }
}

