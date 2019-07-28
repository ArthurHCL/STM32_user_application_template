#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_



#include <stdbool.h>



void SPI_Driver_Init(void);

bool SPI_Driver_WriteReadData(
    const unsigned char   write_data,
    unsigned char * const read_data);



#endif

