#include "stm32f10x_it.h"
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
/* USB include */
#include "usb_istr.h"
#include "usb_int.h"
#include "usb_pwr.h"
/* user library include */
#include "UART_ringFIFO.h"
/* user application include */
#include "hw_config.h"



/*
    STM32F103C8(72M Hz):
            if baudrate is 115200,
        then each byte interval is 86 us,
        according to oscilloscope observe,
        the interrupt waste about 3us.
            so I think three UART receiver interrupt is OK,
        will not miss any realtime data.
    STM32L151C8(32M Hz):
        if baudrate is 256000,
        then each byte interval is 39 us,
        according to oscilloscope observe,
        the interrupt waste about 3us.
            so I think three UART receiver interrupt is OK,
        will not miss any realtime data.
*/
void USART1_IRQHandler(void)
{
    USART1_Event_Handler();
}
void USART2_IRQHandler(void)
{
    USART2_Event_Handler();
}
void USART3_IRQHandler(void)
{
    USART3_Event_Handler();
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
        Timer_4_Event_Handler();
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
        Timer_3_Event_Handler();
    }
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
    CTR_HP();
}

void USBWakeUp_IRQHandler(void)
{
    Resume(RESUME_EXTERNAL);
    EXTI_ClearITPendingBit(EXTI_Line18);
}

