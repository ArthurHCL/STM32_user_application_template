#ifndef _IIC_DRIVER_2_H_
#define _IIC_DRIVER_2_H_



#include <stdbool.h>



bool IIC_Read_Buffer_2(
    const unsigned char  addr,
    const unsigned char  reg,
    unsigned char        len,
    unsigned char       *buf);
bool IIC_Write_Buffer_2(
    const unsigned char         addr,
    const unsigned char         reg,
    const unsigned char         len,
    const unsigned char * const data);
bool IIC_Write_One_Byte_2(
    const unsigned char addr,
    const unsigned char reg,
    const unsigned char data);

void IIC_Init_2(void);



#endif

