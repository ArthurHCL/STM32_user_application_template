#include "SPI_driver.h"
/* system include */
#include <stdbool.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"



void SPI_Driver_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = NRF24L01_SCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(NRF24L01_SCK_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = NRF24L01_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(NRF24L01_MISO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = NRF24L01_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(NRF24L01_MOSI_PORT, &GPIO_InitStructure);

    /* set SPI rate at 4.5MHz. */
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

bool SPI_Driver_WriteReadData(
    const unsigned char   write_data,
    unsigned char * const read_data)
{
    unsigned short i = 1000;

    while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)) {
        if (!(i--)) {
            return false;
        }
    }
   	SPI_I2S_SendData(SPI1, write_data);

    while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    *read_data = (unsigned char)SPI_I2S_ReceiveData(SPI1);

    return true;
}

