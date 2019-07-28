#ifndef _CRC_H_
#define _CRC_H_



unsigned long crc32(
    const unsigned char * const s,
    const unsigned long         len);
unsigned short crc16(
    const unsigned char * const s,
    const unsigned long         len);
unsigned char crc8(
    const unsigned char * const s,
    const unsigned long         len);



#endif

