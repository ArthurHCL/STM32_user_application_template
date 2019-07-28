#include "IIC_driver_2.h"
/* system include */
#include <stdbool.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x_gpio.h"



/* GPIO optimization for time. */
#if 0
# define SCL_L    GPIO_ResetBits(IIC_SCL_PORT_2, IIC_SCL_2)
# define SCL_H    GPIO_SetBits(IIC_SCL_PORT_2, IIC_SCL_2)
#else
# define SCL_L    IIC_SCL_PORT_2->BRR = IIC_SCL_2
# define SCL_H    IIC_SCL_PORT_2->BSRR = IIC_SCL_2
#endif
#if 0
# define SDA_L    GPIO_ResetBits(IIC_SDA_PORT_2, IIC_SDA_2)
# define SDA_H    GPIO_SetBits(IIC_SDA_PORT_2, IIC_SDA_2)
#else
# define SDA_L    IIC_SDA_PORT_2->BRR = IIC_SDA_2
# define SDA_H    IIC_SDA_PORT_2->BSRR = IIC_SDA_2
#endif
#if 0
# define SDA_read    GPIO_ReadInputDataBit(IIC_SDA_PORT_2, IIC_SDA_2)
#else
# define SDA_read    (IIC_SDA_PORT_2->IDR & IIC_SDA_2)
#endif



#define	IIC_DIRECTION_TRANSMITTER    0
#define	I2C_DIRECTION_RECEIVER       1



static void IIC_delay(void)
{
    volatile int i = 7;

    while (i) {
        i--;
    }
}

static bool IIC_start(void)
{
    SDA_H;
    SCL_H;
    IIC_delay();
    if (!SDA_read) {
        return false;
    }

    SDA_L;
    IIC_delay();
    if (SDA_read) {
        return false;
    }

    return true;
}

static void IIC_stop(void)
{
    SCL_L;
    IIC_delay();
    SDA_L;
    IIC_delay();
    SCL_H;
    IIC_delay();
    SDA_H;
    IIC_delay();
}

static void IIC_ack(void)
{
    SCL_L;
    IIC_delay();
    SDA_L;
    IIC_delay();
    SCL_H;
    IIC_delay();
    SCL_L;
    IIC_delay();
}

static void IIC_no_ack(void)
{
    SCL_L;
    IIC_delay();
    SDA_H;
    IIC_delay();
    SCL_H;
    IIC_delay();
    SCL_L;
    IIC_delay();
}

static bool IIC_wait_ack(void)
{
    SCL_L;
    IIC_delay();
    SDA_H;
    IIC_delay();
    SCL_H;
    IIC_delay();

    if (SDA_read) {
        SCL_L;

        return false;
    }

    SCL_L;

    return true;
}

static void IIC_send_one_byte(
    unsigned char byte)
{
    unsigned char i = 8;

    while (i--) {
        SCL_L;
        IIC_delay();

        if (byte & 0x80) {
            SDA_H;
        } else {
            SDA_L;
        }

        byte <<= 1;
        IIC_delay();
        SCL_H;
        IIC_delay();
    }

    SCL_L;
}

static unsigned char IIC_receive_one_byte(void)
{
    unsigned char i = 8;
    unsigned char byte = 0;

    SDA_H;

    while (i--) {
        byte <<= 1;
        SCL_L;
        IIC_delay();
        SCL_H;
        IIC_delay();

        if (SDA_read) {
            byte |= 0x01;
        }
    }

    SCL_L;

    return byte;
}



bool IIC_Read_Buffer_2(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char        len,
    unsigned char       *buf)
{
    if (!IIC_start()) {
        return false;
    }

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_send_one_byte(reg);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_start();

    IIC_send_one_byte((addr << 1) | I2C_DIRECTION_RECEIVER);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    while (len) {
        *buf = IIC_receive_one_byte();

        if (len == 1) {
            IIC_no_ack();
        } else {
            IIC_ack();
        }

        buf++;
        len--;
    }

    IIC_stop();

    return true;
}

bool IIC_Write_Buffer_2(
    const unsigned char         addr,
    const unsigned char         reg,
    const unsigned char         len,
    const unsigned char * const data)
{
    int i;

    if (!IIC_start()) {
        return false;
    }

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_send_one_byte(reg);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    for (i = 0; i < len; i++) {
        IIC_send_one_byte(data[i]);
        if (!IIC_wait_ack()) {
            IIC_stop();

            return false;
        }
    }

    IIC_stop();

    return true;
}

bool IIC_Write_One_Byte_2(
    const unsigned char addr,
    const unsigned char reg,
    const unsigned char data)
{
    return IIC_Write_Buffer_2(addr, reg, 1, &data);
}

void IIC_Init_2(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    /************************ IIC_SCL ************************/
    SCL_H;
    GPIO_InitStructure.GPIO_Pin  = IIC_SCL_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SCL_PORT_2, &GPIO_InitStructure);

    /************************ IIC_SDA ************************/
    SDA_H;
    GPIO_InitStructure.GPIO_Pin  = IIC_SDA_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SDA_PORT_2, &GPIO_InitStructure);

    IIC_stop();
}

