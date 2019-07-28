#ifndef _EEPROM_TABLE_H_
#define _EEPROM_TABLE_H_

/*
        the header file is the original file for current software system,
    include bootloader code and application code.
        bootloader code is always stable except important version change,
    while application code may often update.
        application code may want to add more enum member,
    such as EE_XXXXXXXXXX definition,
    so in order to meet the requirement,
    application code can add its self definitions in designated position,
    please look at the enum definition for details.
*/



#define EE_NUM_ADDR    1

/* unique serial number size of MCU. */
#define EE_SERIAL_NUM_SIZE    (12 / 2)

/*
        it is only used in bootloader code,
    for storing private data in the future.
*/
#define EE_BOOTLOADER_CODE_UNDEFINED_2BYTES_SPACE_AMOUNT    100

/* max amount of using contiguous virtual address. */
#define EE_MAX_CONTIGUOUS    (EE_MAX_CONTIGUOUS_END_HERE + 1)

/* it indicates whether application code is valid/invalid. */
#define SET_APPLICATION_CODE_INVALID    ((unsigned short)0x1234)
#define SET_APPLICATION_CODE_VALID      ((unsigned short)0x4321)

enum {
    /*
            below definitions are fixed,
        they are used for USB communication,
        to store unique serial number.
            please do not change them,
        put them as fixed definitions.
    */
    EE_SERIAL_SET,
    EE_SERIAL_NUM_START,
    EE_SERIAL_NUM_END = EE_SERIAL_NUM_START + EE_SERIAL_NUM_SIZE - 1,

    /*
            it is only used in bootloader code!
            if application code is valid(= SET_APPLICATION_CODE_VALID),
        user can jump from bootloader code to application code.
            if not then we can only run bootloader code,
        waiting for application code to be updated.
    */
    EE_IS_APPLICATION_CODE_VALID,

    /* it is only used in bootloader code for storing private data in the future. */
    EE_BOOTLOADER_CODE_UNDEFINED_2BYTES_SPACE_START,
    EE_BOOTLOADER_CODE_UNDEFINED_2BYTES_SPACE_END = EE_BOOTLOADER_CODE_UNDEFINED_2BYTES_SPACE_START + EE_BOOTLOADER_CODE_UNDEFINED_2BYTES_SPACE_AMOUNT - 1,

    /* below position is used for application code for private data. */
    EE_FLASH_OR_EEPROM_WRITE_READ_TEST,
    /* above position is used for application code for private data. */

    /* all definitions must be contiguous before the definition. */
    EE_MAX_CONTIGUOUS_END_HERE,

    /* it is a fixed reserved definition so do not change it. */
    EE_DFU_BOOT = 0x0100
};



#endif

