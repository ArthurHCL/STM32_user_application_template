#include "UART_ringFIFO.h"
/* system include */
#include <stdbool.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"



typedef struct {
    int                    head;
    int                    tail;
    volatile int           length;
    volatile unsigned char buffer[UART_INTERFACE_FIFO_LENGTH];
} RINGFIFO;



//static 
RINGFIFO UART1_ringFIFO;
//static 
RINGFIFO UART2_ringFIFO;
//static 
RINGFIFO UART3_ringFIFO;



static void ringFIFO_reset(
    RINGFIFO * const ringFIFO)
{
    ringFIFO->head = 0;
    ringFIFO->tail = 0;

    //__set_PRIMASK(1);
    ringFIFO->length = 0; /* critical region protection is not needed because it is only used in interrupt except initialization. */
    //__set_PRIMASK(0);
}

static bool ringFIFO_write(
    RINGFIFO * const    ringFIFO,
    const unsigned char data)
{
    if (UART_INTERFACE_FIFO_LENGTH <= ringFIFO->length) {
        return false;
    }

    ringFIFO->buffer[ringFIFO->tail++] = data;

    if (UART_INTERFACE_FIFO_LENGTH == ringFIFO->tail) {
        ringFIFO->tail = 0;
    }

    //__set_PRIMASK(1);
    ringFIFO->length++; /* critical region protection is not needed because it is only used in interrupt. */
    //__set_PRIMASK(0);

    return true;
}

static bool ringFIFO_read(
    RINGFIFO * const      ringFIFO,
    unsigned char * const data)
{
    if (!ringFIFO->length) {
        return false;
    }

    *data = ringFIFO->buffer[ringFIFO->head++];

    if (UART_INTERFACE_FIFO_LENGTH == ringFIFO->head) {
        ringFIFO->head = 0;
    }

    __set_PRIMASK(1);
    ringFIFO->length--; /* critical region protection is needed because it is only used in main() while interrupt may break it. */
    __set_PRIMASK(0);

    return true;
}



void UARTx_Baudrate_Set(
    USART_TypeDef       *USARTx,
    const unsigned long  baudrate)
{
    USART_InitTypeDef USART_InitStructure;

    USART_Cmd(USARTx, DISABLE);

    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);

    USART_Cmd(USARTx, ENABLE);
}



static void UART1_ringFIFO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /************************ UART_TX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /************************ UART_RX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    UARTx_Baudrate_Set(USART1, UART_INTERFACE_BAUDRATE_DEFAULT);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC);

    /****** interrupt config of UART ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* reset FIFO. */
    ringFIFO_reset(&UART1_ringFIFO);
}
static void UART2_ringFIFO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /************************ UART_TX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /************************ UART_RX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    UARTx_Baudrate_Set(USART2, UART_INTERFACE_BAUDRATE_DEFAULT);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC);

    /****** interrupt config of UART ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* reset FIFO. */
    ringFIFO_reset(&UART2_ringFIFO);
}
static void UART3_ringFIFO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /************************ UART_TX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /************************ UART_RX ************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    UARTx_Baudrate_Set(USART3, UART_INTERFACE_BAUDRATE_DEFAULT);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TC);

    /****** interrupt config of UART ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* reset FIFO. */
    ringFIFO_reset(&UART3_ringFIFO);
}



void UART_RingFIFO_Init(void)
{
    UART1_ringFIFO_init();
    UART2_ringFIFO_init();
    UART3_ringFIFO_init();
}

void USART1_Event_Handler(void)
{
    if (RESET != USART_GetITStatus(USART1, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

#if 0 /* UART optimization for time. */
        if (!ringFIFO_write(&UART1_ringFIFO, (unsigned char)USART_ReceiveData(USART1)))
#else
        if (!ringFIFO_write(&UART1_ringFIFO, (unsigned char)USART1->DR))
#endif
        {
            ringFIFO_reset(&UART1_ringFIFO);
        }
    }
}
void USART2_Event_Handler(void)
{
    if (RESET != USART_GetITStatus(USART2, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);

#if 0 /* UART optimization for time. */
        if (!ringFIFO_write(&UART2_ringFIFO, (unsigned char)USART_ReceiveData(USART2)))
#else
        if (!ringFIFO_write(&UART2_ringFIFO, (unsigned char)USART2->DR))
#endif
        {
            ringFIFO_reset(&UART2_ringFIFO);
        }
    }
}
void USART3_Event_Handler(void)
{
    if (RESET != USART_GetITStatus(USART3, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

#if 0 /* UART optimization for time. */
        if (!ringFIFO_write(&UART3_ringFIFO, (unsigned char)USART_ReceiveData(USART3)))
#else
        if (!ringFIFO_write(&UART3_ringFIFO, (unsigned char)USART3->DR))
#endif
        {
            ringFIFO_reset(&UART3_ringFIFO);
        }
    }
}

bool USART1_Read_One_Byte(
    unsigned char * const data)
{
    return ringFIFO_read(&UART1_ringFIFO, data);
}
bool USART2_Read_One_Byte(
    unsigned char * const data)
{
    return ringFIFO_read(&UART2_ringFIFO, data);
}
bool USART3_Read_One_Byte(
    unsigned char * const data)
{
    return ringFIFO_read(&UART3_ringFIFO, data);
}

void USART1_Write_One_Byte(
    const unsigned char data)
{
    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));

#if 0 /* UART optimization for time. */
    USART_SendData(USART1, (unsigned short)data);
#else
    USART1->DR = data;
#endif
}
void USART2_Write_One_Byte(
    const unsigned char data)
{
    while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TXE));

#if 0 /* UART optimization for time. */
    USART_SendData(USART2, (unsigned short)data);
#else
    USART2->DR = data;
#endif
}
void USART3_Write_One_Byte(
    const unsigned char data)
{
    while (RESET == USART_GetFlagStatus(USART3, USART_FLAG_TXE));

#if 0 /* UART optimization for time. */
    USART_SendData(USART3, (unsigned short)data);
#else
    USART3->DR = data;
#endif
}

