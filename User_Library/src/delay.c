#include "delay.h"
 /* chip library include */
#include "stm32f10x.h"
#include "misc.h"
 
 
 
 static unsigned char  fac_us;
 static unsigned short fac_ms;
 
 
 
 void delay_init(void)
 {
	 SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
 
	 fac_us = SystemCoreClock / 8000000;
	 fac_ms = (unsigned short)fac_us * 1000;
 }
 
 void delay_us(
	 const unsigned long nus)
 {
	 unsigned long temp;
 
	 SysTick->LOAD = nus * fac_us;
	 SysTick->VAL = 0x00;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	 do {
		 temp = SysTick->CTRL;
	 } while ((temp & 0x01) && (!(temp & (1 << 16))));
	 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	 SysTick->VAL = 0x00;
 }
 
 void delay_ms(
	 const unsigned short nms)
 {
	 unsigned long temp;
 
	 SysTick->LOAD = (unsigned long)nms * fac_ms;
	 SysTick->VAL = 0x00;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	 do {
		 temp = SysTick->CTRL;
	 } while ((temp & 0x01) && (!(temp & (1 << 16))));
	 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	 SysTick->VAL = 0x00;
 }

