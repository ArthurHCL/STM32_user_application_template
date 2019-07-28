#ifndef _UART_RINGFIFO_H_
#define _UART_RINGFIFO_H_



#include <stdbool.h>
#include "stm32f10x.h"



void UARTx_Baudrate_Set(
    USART_TypeDef       *USARTx,
    const unsigned long  baudrate);

void UART_RingFIFO_Init(void);

void USART1_Event_Handler(void);
void USART2_Event_Handler(void);
void USART3_Event_Handler(void);

bool USART1_Read_One_Byte(
    unsigned char * const data);
bool USART2_Read_One_Byte(
    unsigned char * const data);
bool USART3_Read_One_Byte(
    unsigned char * const data);

void USART1_Write_One_Byte(
    const unsigned char data);
void USART2_Write_One_Byte(
    const unsigned char data);
void USART3_Write_One_Byte(
    const unsigned char data);



#endif

