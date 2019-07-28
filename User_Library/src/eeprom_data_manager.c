#include "eeprom_data_manager.h"
/* system include */
#include <stdbool.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x_flash.h"
/* user library include */
#include "eeprom.h"
#include "eeprom_table.h"
#include "device_abnormal_state_record.h"
/* user application include */
#include "hw_config.h"



/*
    description:
            check if FLASH or EEPROM operation is normal.
    return:
        false:
                FLASH or EEPROM operation is not normal.
        true:
                FLASH or EEPROM operation is normal.
*/
static bool FLASH_or_EEPROM_write_read_test(void)
{
    unsigned short EE_data;

    if (FLASH_COMPLETE != EE_WriteVariable(EE_FLASH_OR_EEPROM_WRITE_READ_TEST, 0x1234)) {
        return false;
    }
    if (EE_ReadVariable(EE_FLASH_OR_EEPROM_WRITE_READ_TEST, &EE_data)) {
        return false;
    }

    if (0x1234 != EE_data) {
        return false;
    }

    return true;
}



/*
    description:
            get firmware version.
    parameter:
        *major:
                firmware major version.
        *minor:
                firmware minor version.
        *revision:
                firmware revision version.
*/
void Firmware_Version_Get(
    unsigned short * const major,
    unsigned short * const minor,
    unsigned short * const revision)
{
    *major    = (unsigned short)APPLICATION_CODE_VERSION_MAJOR;
    *minor    = (unsigned short)APPLICATION_CODE_VERSION_MINOR;
    *revision = (unsigned short)APPLICATION_CODE_VERSION_REVISION;
}

/*
    description:
            initialize EEPROM data manager.
*/
void EEPROM_Data_Manager_Init(void)
{
    if (!FLASH_or_EEPROM_write_read_test()) {
        Device_Abnormal_State_Insert(DEVICE_ABNORMAL_STATE_RECORD_FLASH_OR_EEPROM_WRITE_READ_ABNORMAL);
    }
}

