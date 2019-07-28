#ifndef _MAIN_H_
#define _MAIN_H_



#include "stm32f10x.h"
#include "stm32f10x_gpio.h"



/*
        0:
                it is formal board.
        1:
                it is temporary board for test.
*/
#define IS_TEMPORARY_BOARD_FOR_TEST    1



/*
        0:
                WFE/WFI instruction is enabled in main() while() for saving power.
        1:
                WFE/WFI instruction is disabled in main() while() for debug observe.
*/
#define IS_DISABLE_SLEEP_FUNCTION_IN_MAIN_WHILE_FOR_DEBUG_OBSERVE    1



/* USB VID and PID which can be modified by user self. */
#define APPLICATION_CODE_VID_PID_USER    0x12345678



/* selected using board. */
#define APPLICATION_CODE_VID_PID    APPLICATION_CODE_VID_PID_USER

/* generate VID and PID for USB communication. */
#define APPLICATION_CODE_VID_L    ((unsigned char)(APPLICATION_CODE_VID_PID >> 24))
#define APPLICATION_CODE_VID_H    ((unsigned char)(APPLICATION_CODE_VID_PID >> 16))
#define APPLICATION_CODE_PID_L    ((unsigned char)(APPLICATION_CODE_VID_PID >> 8))
#define APPLICATION_CODE_PID_H    ((unsigned char)APPLICATION_CODE_VID_PID)



/*
        define current application code version,
    such as version 1.0.0,
    it can be read by USB communication command.
*/
#define APPLICATION_CODE_VERSION_MAJOR       1
#define APPLICATION_CODE_VERSION_MINOR       0
#define APPLICATION_CODE_VERSION_REVISION    0



/* UART_INTERFACE */
#define UART_INTERFACE_BAUDRATE_DEFAULT    256000
#define UART_INTERFACE_FIFO_LENGTH         1000



/* IIC_SCL */
#define IIC_SCL_PORT    GPIOA
#define IIC_SCL         GPIO_Pin_0



/* IIC_SDA */
#define IIC_SDA_PORT    GPIOA
#define IIC_SDA         GPIO_Pin_0



/* IIC_SCL_2 */
#define IIC_SCL_PORT_2    GPIOA
#define IIC_SCL_2         GPIO_Pin_0



/* IIC_SDA_2 */
#define IIC_SDA_PORT_2    GPIOA
#define IIC_SDA_2         GPIO_Pin_0



/* NRF24L01_SCK */
#define NRF24L01_SCK_PORT    GPIOA
#define NRF24L01_SCK         GPIO_Pin_0
/* NRF24L01_MISO */
#define NRF24L01_MISO_PORT    GPIOA
#define NRF24L01_MISO         GPIO_Pin_0
/* NRF24L01_MOSI */
#define NRF24L01_MOSI_PORT    GPIOA
#define NRF24L01_MOSI         GPIO_Pin_0



/* APPLICATION_LED */
#if IS_TEMPORARY_BOARD_FOR_TEST
# define APPLICATION_LED_PORT    GPIOC
# define APPLICATION_LED         GPIO_Pin_13
#else

#endif



/* USBDM */
#define USBDM_PORT    GPIOA
#define USBDM         GPIO_Pin_11
/* USBDP */
#define USBDP_PORT    GPIOA
#define USBDP         GPIO_Pin_12



/*
        to ensure future compatibility,
    both send and receive use 64 bytes in USB communication.
*/
#define CUSTOM_HID_IN_REPORT_SIZE     64
#define CUSTOM_HID_OUT_REPORT_SIZE    64



#define ARRAY_SIZE(array)    (sizeof(array) / sizeof((array)[0]))

#define ABS(x)    ((x) < 0 ? (-(x)) : (x))



#endif

