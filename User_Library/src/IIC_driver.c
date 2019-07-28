#include "IIC_driver.h"
/* system include */
#include <stdbool.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x_gpio.h"



/* GPIO optimization for time. */
#if 0
# define SCL_L    GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL)
# define SCL_H    GPIO_SetBits(IIC_SCL_PORT, IIC_SCL)
#else
# define SCL_L    IIC_SCL_PORT->BRR = IIC_SCL
# define SCL_H    IIC_SCL_PORT->BSRR = IIC_SCL
#endif
#if 0
# define SDA_L    GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA)
# define SDA_H    GPIO_SetBits(IIC_SDA_PORT, IIC_SDA)
#else
# define SDA_L    IIC_SDA_PORT->BRR = IIC_SDA
# define SDA_H    IIC_SDA_PORT->BSRR = IIC_SDA
#endif
#if 0
# define SDA_read    GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA)
#else
# define SDA_read    (IIC_SDA_PORT->IDR & IIC_SDA)
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

static void IIC_start(void)
{
    SDA_H;
    SCL_H;
    IIC_delay();
    SDA_L;
    IIC_delay();
    SCL_L;
}

static void IIC_stop(void)
{
    SCL_L;
    SDA_L;
    IIC_delay();
    SCL_H;
    SDA_H;
    IIC_delay();
}

static void IIC_ack(void)
{
    SCL_L;
    SDA_L;
    IIC_delay();
    SCL_H;
    IIC_delay();
    SCL_L;
}

static void IIC_no_ack(void)
{
    SCL_L;
    SDA_H;
    IIC_delay();
    SCL_H;
    IIC_delay();
    SCL_L;
}

static bool IIC_wait_ack(void)
{
    unsigned char time = 0;

    SDA_H;
    IIC_delay();
    SCL_H;
    IIC_delay();

    while (SDA_read) {
        time++;
        if (250 < time) {
            IIC_stop();

            return false;
        }
    }
    SCL_L;

    return true;
}

static void IIC_send_one_byte(
    unsigned char byte)
{
    unsigned char i;

    SCL_L;
    for (i = 0; i < 8; i++) {
        if (byte & 0x80) {
            SDA_H;
        } else {
            SDA_L;
        }

        byte <<= 1;
        SCL_H;
        IIC_delay();
        SCL_L;
        IIC_delay();
    }
}

static unsigned char IIC_receive_one_byte(
    const bool is_ack)
{
    unsigned char i;
    unsigned char byte = 0;

    for (i = 0; i < 8; i++) {
        SCL_L;
        IIC_delay();
        SCL_H;
        byte <<= 1;

        if (SDA_read) {
            byte++;
        }
        IIC_delay();
    }

    if (!is_ack) {
        IIC_no_ack();
    } else {
        IIC_ack();
    }

    return byte;
}



bool IIC_Write_One_Byte(
    const unsigned char addr,
    const unsigned char reg,
    const unsigned char data)
{
    IIC_start();

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_send_one_byte(reg);
    (void)IIC_wait_ack();

    IIC_send_one_byte(data);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_stop();

    return true;
}

bool IIC_Read_One_Byte(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char       *data)
{
    IIC_start();

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    (void)IIC_wait_ack();

    IIC_send_one_byte(reg);
    (void)IIC_wait_ack();

    IIC_start();

    IIC_send_one_byte((addr << 1) | I2C_DIRECTION_RECEIVER);
    (void)IIC_wait_ack();

    *data = IIC_receive_one_byte(false);

    IIC_stop();

    return true;
}

bool IIC_Write_Buffer(
    const unsigned char         addr,
    const unsigned char         reg,
    const unsigned char         len,
    const unsigned char * const data)
{
    int i;

    IIC_start();

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    if (!IIC_wait_ack()) {
        IIC_stop();

        return false;
    }

    IIC_send_one_byte(reg);
    (void)IIC_wait_ack();

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

bool IIC_Read_Buffer(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char        len,
    unsigned char       *buf)
{
    IIC_start();

    IIC_send_one_byte((addr << 1) | IIC_DIRECTION_TRANSMITTER);
    (void)IIC_wait_ack();

    IIC_send_one_byte(reg);
    (void)IIC_wait_ack();

    IIC_start();

    IIC_send_one_byte((addr << 1) | I2C_DIRECTION_RECEIVER);
    (void)IIC_wait_ack();

    while (len) {
        if (len == 1) {
            *buf = IIC_receive_one_byte(false);
        } else {
            *buf = IIC_receive_one_byte(true);
        }

        buf++;
        len--;
    }

    IIC_stop();

    return true;
}

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    /************************ IIC_SCL ************************/
    SCL_H;
    GPIO_InitStructure.GPIO_Pin  = IIC_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

    /************************ IIC_SDA ************************/
    SDA_H;
    GPIO_InitStructure.GPIO_Pin  = IIC_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);

    IIC_stop();
}

