#ifndef _EEPROM_H_
#define _EEPROM_H_



/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "stm32f10x.h"



/* Exported constants --------------------------------------------------------*/
/* Define the STM32F10Xxx Flash page size depending on the used STM32 device */
#if defined (STM32F10X_LD) || defined (STM32F10X_MD)
    /* EEPROM start address in Flash */
    #if defined (STM32F10X_LD)
        #define EEPROM_START_ADDRESS    ((uint32_t)0x08007800) /* EEPROM emulation start address:
                                                                  after 30KByte of used Flash memory */
    #else
        #define EEPROM_START_ADDRESS    ((uint32_t)0x0800F800) /* EEPROM emulation start address:
                                                                  after 62KByte of used Flash memory */
    #endif /* STM32F10X_LD */
    #define PAGE_SIZE  (uint16_t)0x400  /* Page size = 1KByte */
#elif defined (STM32F10X_HD) || defined (STM32F10X_CL)
    #define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */
    /* EEPROM start address in Flash */
    #define EEPROM_START_ADDRESS    ((uint32_t)0x08010000) /* EEPROM emulation start address:
                                                              after 64KByte of used Flash memory */
#endif

/* Pages 0 and 1 BASE and END addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                  ((uint16_t)0xFFFF) /* PAGE is empty */
#define RECEIVE_DATA            ((uint16_t)0xEEEE) /* PAGE is marked to receive data */
#define VALID_PAGE              ((uint16_t)0x0000) /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)



/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t EE_ReadVariable(
    const uint16_t   VirtAddress,
    uint16_t * const Data);
uint16_t EE_WriteVariable(
    const uint16_t VirtAddress,
    const uint16_t Data);

uint16_t EE_Init(void);



#endif

