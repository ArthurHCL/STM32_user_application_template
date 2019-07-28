#ifndef _IIC_DRIVER_H_
#define _IIC_DRIVER_H_



#include <stdbool.h>



bool IIC_Read_One_Byte(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char       *data);
bool IIC_Read_Buffer(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char        len,
    unsigned char       *buf);
bool IIC_Write_One_Byte(
    const unsigned char addr,
    const unsigned char reg,
    const unsigned char data);
bool IIC_Write_Buffer(
    const unsigned char         addr,
    const unsigned char         reg,
    const unsigned char         len,
    const unsigned char * const data);

void IIC_Init(void);



#endif

