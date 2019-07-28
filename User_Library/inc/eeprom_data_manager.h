#ifndef _EEPROM_DATA_MANAGER_H_
#define _EEPROM_DATA_MANAGER_H_



void Firmware_Version_Get(
    unsigned short * const major,
    unsigned short * const minor,
    unsigned short * const revision);

void EEPROM_Data_Manager_Init(void);



#endif

