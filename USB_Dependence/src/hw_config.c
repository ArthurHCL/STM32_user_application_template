#include "hw_config.h"
/* system include */
#include <stdbool.h>
#include <string.h>
/* user config include */
#include "main.h"
/* chip library include */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_exti.h"
#include "misc.h"
/* USB include */
#include "usb_regs.h"
#include "usb_pwr.h"
/* user library include */
#include "delay.h"
#include "eeprom.h"
#include "eeprom_table.h"
#include "eeprom_data_manager.h"
#include "UART_ringFIFO.h"
#include "IIC_driver.h"
#include "IIC_driver_2.h"
#include "SPI_driver.h"
#include "device_abnormal_state_record.h"
/* user include */



/* GPIO optimization for time. */
#if 0
# define APPLICATION_LED_ON     GPIO_ResetBits(APPLICATION_LED_PORT, APPLICATION_LED)
# define APPLICATION_LED_OFF    GPIO_SetBits(APPLICATION_LED_PORT, APPLICATION_LED)
#else
# define APPLICATION_LED_ON     APPLICATION_LED_PORT->BRR = APPLICATION_LED
# define APPLICATION_LED_OFF    APPLICATION_LED_PORT->BSRR = APPLICATION_LED
#endif



static unsigned short use_uuid_serial = 2;

static volatile APPLICATION_LED_STATUS application_led_status_set[APPLICATION_LED_STATUS_TOTAL] = {APPLICATION_LED_STATUS_IDLE_RUNNING};
static volatile unsigned char          application_led_status_set_amount = 1;

static volatile unsigned long one_ms_elapsed;



/****************************** private function ******************************/
static void int_to_base32(
    unsigned long         val,
    unsigned char * const buf,
    const unsigned char   len,
    const bool            is_unicode)
{
    unsigned char i;
    unsigned char bits;
    unsigned char mult = is_unicode ? 2 : 1;

    for (i = 0; i < len; i++) {
        bits = (unsigned char)(val & 0x1F);

        if (bits < 0x0A) {
            buf[i * mult] = bits + '0';
        } else {
            buf[i * mult] = bits + 'A' - 10;
        }

        if (is_unicode) {
            buf[i * mult + 1] = 0;
        }

        val = val >> 5;
    }
}

static void int_to_string(
    unsigned long         val,
    unsigned char * const buf,
    const unsigned char   len)
{
    unsigned char idx;

    for (idx = 0; idx < len; idx++) {
        if ((val >> 28) < 0x0A) {
            buf[idx] = (unsigned char)((val >> 28) + '0');
        } else {
            buf[idx] = (unsigned char)((val >> 28) + 'A' - 10);
        }

        val = val << 4;
    }
}

static void int_to_unicode(
    unsigned long         val,
    unsigned char * const buf,
    const unsigned char   len)
{
    unsigned char idx;

    for (idx = 0; idx < len; idx++) {
        if ((val >> 28) < 0x0A) {
            buf[idx << 1] = (unsigned char)((val >> 28) + '0');
        } else {
            buf[idx << 1] = (unsigned char)((val >> 28) + 'A' - 10);
        }

        val = val << 4;

        buf[(idx << 1) + 1] = 0;
    }
}

/*
    description:
            shut off JTAG since we only use SWD,
        so we can re-use the pins.
        this needs to happen shortly after reset.
*/
static void disable_JTAG_but_enable_SWD_debug_port(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/*
    description:
            backup registers is used for application code,
        indicating bootloader code a request from application code.
            to use related registers,
        must open related clocks(datasheet clarify).
*/
static void backup_registers_read_write_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}

/*
    description:
            configure whether the USB DP pin is actually working in USB mode.
    parameter:
        is_connect:
            false:
                force USB DP wire disconnect.
            true:
                config as USB mode.
*/
static void USB_DP_pin_config(
    const bool is_connect)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    GPIO_InitStructure.GPIO_Pin = USBDP;

    if (is_connect) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(USBDP_PORT, &GPIO_InitStructure);
    } else {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(USBDP_PORT, &GPIO_InitStructure);
        /* make USBDP = 0V then PC thinks USB wire is disconnected. */
        GPIO_ResetBits(USBDP_PORT, USBDP);
    }
}

/*
    description:
            general IO port settings.
*/
static void general_GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    /************************ USBDM ************************/
    GPIO_InitStructure.GPIO_Pin  = USBDM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USBDM_PORT, &GPIO_InitStructure);

    /************************ USBDP ************************/
    /* fisrt disable USB port at the code start. */
    USB_DP_pin_config(false);
}

/*
    description:
            application LED configuration.
    parameter:
        is_config_as_PWM:
            true:
                    config IO port of the LED as PWM.
            false:
                    config IO port of the LED as GPIO.
*/
static void application_LED_config(
    const bool is_config_as_PWM)
{
    static bool             is_first_config = true;
    static bool             is_config_as_PWM_last;
    GPIO_InitTypeDef        GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    if (is_first_config) {
        is_first_config = false;
        is_config_as_PWM_last = is_config_as_PWM;
    } else {
        if (is_config_as_PWM == is_config_as_PWM_last) {
            return;
        }

        is_config_as_PWM_last = !is_config_as_PWM_last;
    }

    if (is_config_as_PWM) {
#if 0
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_OCInitTypeDef       TIM_OCInitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

        GPIO_InitStructure.GPIO_Pin  = APPLICATION_LED;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(APPLICATION_LED_PORT, &GPIO_InitStructure);

        /* now timer is 1000Hz. */
        TIM_TimeBaseStructure.TIM_Prescaler         = 720 - 1;
        TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_Period            = 100;
        TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0xFF;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

        TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse       = TIM_TimeBaseStructure.TIM_Period >> 1;
        TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

        TIM_Cmd(TIM2, ENABLE);
#endif
    } else {
#if 0
        /* make sure PWM function is turned off. */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        TIM_Cmd(TIM2, DISABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
#endif

        APPLICATION_LED_OFF;
        GPIO_InitStructure.GPIO_Pin  = APPLICATION_LED;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(APPLICATION_LED_PORT, &GPIO_InitStructure);
    }
}


/*
    description:
            configure interrupt grouping,
        and then set the interrupt preemption priority and enable.
*/
static void interrupt_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /****** interrupt config of USB_HP_CAN1_TX_IRQn ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /****** interrupt config of USB_LP_CAN1_RX0_IRQn ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /****** EXTI config of USBWakeUp_IRQn ******/
    EXTI_ClearITPendingBit(EXTI_Line18);
    EXTI_InitStructure.EXTI_Line    = EXTI_Line18;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
    /****** interrupt config of USBWakeUp_IRQn ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /****** interrupt config of TIM4_IRQn ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /****** interrupt config of TIM3_IRQn ******/
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
    description:
            enable USB module clock.
*/
static void set_USB_clock(void)
{
    /* USB module need 48MHz to work. */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*
    description:
            periodic timer config.
*/
static void periodic_timer_config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* TIM4 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* now timer is 4Hz. */
    TIM_TimeBaseStructure.TIM_Prescaler         = 3600 - 1;           // 36M Hz / 3600 = 10K Hz
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 5000;               // 1S: 2 * 10000
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0xFF;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM4, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);

    /* TIM3 */
    /*
            if you need to modify the timing interval,
        you need to test the oscilloscope to see if the timing is normal,
        because the interrupt code consumes time and may not be able to handle it.
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* now timer is 100Hz. */
    TIM_TimeBaseStructure.TIM_Prescaler         = 360 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 2000;
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0xFF;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
}

/*
    description:
            get the highest priority LED status in record.
    return:
            the highest priority LED status.
*/
static APPLICATION_LED_STATUS application_LED_status_highest_priority_get(void)
{
    if (application_led_status_set_amount) {
        return application_led_status_set[0];
    }

    return APPLICATION_LED_STATUS_IDLE_RUNNING;
}

static bool get_stored_serial(
    unsigned char * const serial)
{
    unsigned char  i;
    unsigned short temp_chars;

    for (i = 0; i < EE_SERIAL_NUM_SIZE; i++) {
        if (EE_ReadVariable(EE_SERIAL_NUM_START + i, &temp_chars)) {
            return false;
        }

        *((unsigned short *)serial + i) = temp_chars;
    }

    return true;
}

static bool get_use_serial(void)
{
    unsigned short serial_set;

    if (use_uuid_serial) {
        return true;
    }

    if(EE_ReadVariable(EE_SERIAL_SET, &serial_set)) {
        return false;
    }

    if (serial_set) {
        use_uuid_serial = serial_set;
        return true;
    }

    return false;
}



/****************************** export function ******************************/
/*
    description:
            read backup registers.
    parameter:
        start_reg_num:
            registers number to start reading(1 ~ 10).
        *data:
            array to store read value.
        data_len:
            number of registers to read(array size must not less than it).
*/
void Backup_Registers_Read(
    const unsigned short   start_reg_num,
    unsigned short * const data,
    const unsigned char    data_len)
{
    unsigned char i;

    for (i = 0; i < data_len; i++) {
        data[i] = BKP_ReadBackupRegister(BKP_DR1 + (start_reg_num - 1 + i) * 4);
    }
}

/*
    description:
            write backup registers.
    parameter:
        start_reg_num:
            registers number to start writing(1 ~ 10).
        *data:
            array value being written.
        data_len:
            number of registers to write(array size must not less than it).
*/
void Backup_Registers_Write(
    const unsigned short         start_reg_num,
    const unsigned short * const data,
    const unsigned char          data_len)
{
    unsigned char i;

    PWR_BackupAccessCmd(ENABLE);
    for (i = 0; i < data_len; i++) {
        BKP_WriteBackupRegister(BKP_DR1 + (start_reg_num - 1 + i) * 4, data[i]);
    }
    PWR_BackupAccessCmd(DISABLE);
}

/*
    description:
            get current time in ms.
    return:
            return current time in ms.
*/
unsigned long Current_Time_Get(void)
{
    return one_ms_elapsed;
}

/*
    description:
            get elapsed time in ms compared with last recorded time.
    parameter:
        last_time:
                last recorded time in ms.
    return:
            return elapsed time in ms.
*/
unsigned long Elapsed_Time_Get(
    const unsigned long last_time)
{
    return one_ms_elapsed - last_time;
}

/*
    description:
            insert the requested LED status in record.
    parameter:
        status:
                the requested LED status.
*/
void Application_LED_Status_Insert(
    const APPLICATION_LED_STATUS status)
{
    int i;
    int insert_position;

    /* find out insert position. */
    for (i = 0; i < application_led_status_set_amount; i++) {
        /* check if the status has already existed. */
        if (status == application_led_status_set[i]) {
            return;
        }

        if (status < application_led_status_set[i]) {
            insert_position = i;

            break;
        }
    }
    if (i == application_led_status_set_amount) {
        application_led_status_set[i] = status;
        application_led_status_set_amount++;

        return;
    }

    /* insert the status in ascending order. */
    for (i = application_led_status_set_amount; i > insert_position; i--) {
        application_led_status_set[i] = application_led_status_set[i - 1];
    }
    application_led_status_set[insert_position] = status;
    application_led_status_set_amount++;
}

/*
    description:
            remove the requested LED status in record.
    parameter:
        status:
                the requested LED status.
*/
void Application_LED_Status_Remove(
    const APPLICATION_LED_STATUS status)
{
    int i;
    int remove_position;

    /* find out remove position. */
    for (i = 0; i < application_led_status_set_amount; i++) {
        if (status == application_led_status_set[i]) {
            remove_position = i;

            break;
        }
    }
    if (i == application_led_status_set_amount) {
        return;
    }

    /* remove the status in ascending order. */
    for (i = remove_position + 1; i < application_led_status_set_amount; i++) {
        application_led_status_set[i - 1] = application_led_status_set[i];
    }
    application_led_status_set_amount--;
}

/*
    description:
            make MCU system reset,
        just like a power on procedure.
*/
void MCU_System_Reset(void)
{
    __set_FAULTMASK(1); /* close all interrupt. */
    NVIC_SystemReset(); /* MCU reset. */
}

/*
    description:
            used in main(),
        for all board initialization,
        must put it at the beginning of main().
*/
void Set_System(void)
{
    disable_JTAG_but_enable_SWD_debug_port();

    backup_registers_read_write_init();

    delay_init();

    /* restart MCU if FLASH initialization fail. */
    FLASH_Unlock();
    if (FLASH_COMPLETE != EE_Init()) {
        MCU_System_Reset();
    }
    FLASH_Lock();

    general_GPIO_config();
    application_LED_config(false);

    //IIC_Init();
    //IIC_Init_2();
    //SPI_Driver_Init();

    interrupt_config();

    set_USB_clock();

    //UART_RingFIFO_Init();

    periodic_timer_config();

    EEPROM_Data_Manager_Init();
}

/*
    description:
            used in timer interrupt,
        to indicate timer running and deal with some periodic event.
    attention:
            handle this interrupt callback function as soon as possible,
        otherwise it may take too long and the next interrupt will happen again.
*/
void Timer_4_Event_Handler(void)
{
    static unsigned long i = 0;

    i++;

    switch (application_LED_status_highest_priority_get()) {
    case APPLICATION_LED_STATUS_MCU_WORK_ERROR:
        APPLICATION_LED_OFF;
        break;
    case APPLICATION_LED_STATUS_SLEEP:
        APPLICATION_LED_OFF;
        break;
    case APPLICATION_LED_STATUS_IDLE_RUNNING:
        /*
                if bluetooth is unconnected but already paired,
            slow shining is needed(0.5s OFF / 0.5s ON).
        */
        if ((i % 4) <= 1) {
            APPLICATION_LED_OFF;
        } else {
            APPLICATION_LED_ON;
        }
        break;
    default:
        break;
    }
}

/*
    description:
            used in timer interrupt,
        to indicate timer running and deal with some periodic event.
    attention:
            handle this interrupt callback function as soon as possible,
        otherwise it may take too long and the next interrupt will happen again.
*/
void Timer_3_Event_Handler(void)
{
    one_ms_elapsed += 10;
}

/*
    description:
            configure whether the USB module is ON/OFF.
    parameter:
        is_enable:
            true:
                USB module is ON.
            false:
                USB module is OFF.
*/
void USB_Port_Set(
    const bool is_enable)
{
    if (is_enable) {
        _SetCNTR(_GetCNTR() & (~(1 << 1))); /* exit power down. */
    } else {
        _SetCNTR(_GetCNTR() | (1 << 1)); /* enter power down mode. */
    }

    USB_DP_pin_config(is_enable); /* make PC know USB module ON/OFF. */
}

/*
    description:
            indicate USB module is power on/power off.
    parameter:
        NewState:
            DISABLE:
                USB module is power off.
            ENABLE:
                USB module is power on.
*/
void USB_Cable_Config(
    const FunctionalState NewState)
{
    if (NewState != DISABLE) {
        //USB_POWER_ONOFF_LED_ON;
    } else {
        //USB_POWER_ONOFF_LED_OFF;
    }
}

/*
    description:
            USB module enter into low power mode.
*/
void Enter_LowPowerMode(void)
{
    bDeviceState = SUSPENDED;
}

/*
    description:
            USB module leave out low power mode.
*/
void Leave_LowPowerMode(void)
{
    if (pInformation->Current_Configuration) {
        bDeviceState = CONFIGURED;
    } else {
        bDeviceState = ATTACHED;
    }
}

/* legacy method for setting the serial number. */
void Set_Use_Serial(void)
{
    get_use_serial();

    /* set the serial if it has been set by the legacy UUID method. */
    if (use_uuid_serial < 2) {
        FLASH_Unlock();
        EE_WriteVariable(EE_SERIAL_SET, 2);
        FLASH_Lock();

        use_uuid_serial = 2;
    }
}

void Set_Stored_Serial(
    unsigned char *serial_number_string)
{
    Set_Use_Serial();
}

void Get_SerialNum(
    unsigned char * const serial_number_string,
    const bool            is_unicode)
{
    unsigned char i;
    unsigned long device_serial_0 = 0xAAAAAAAA;
    unsigned long device_serial_1 = 0xAAAAAAAA;
    unsigned char internal_serial_number[EE_SERIAL_NUM_SIZE * 2] = {0};

    /* if successfully read 12 bytes' serial number from internal FLASH then.... */
    if (get_stored_serial(internal_serial_number)) {
        /* if need to output unicode then.... */
        if (is_unicode) {
            for (i = 0; i < (EE_SERIAL_NUM_SIZE * 2); i++) {
                serial_number_string[i * 2]     = internal_serial_number[i];
                serial_number_string[i * 2 + 1] = 0;
            }
        /* otherwise need to output ASCII then.... */
        } else {
            memcpy(serial_number_string, internal_serial_number, EE_SERIAL_NUM_SIZE * 2);
        }
    /* otherwise, then.... */
    } else {
        /* if use_uuid_serial is confirmed set or not need unicode then.... */
        if (get_use_serial() || !is_unicode) {
            unsigned long serial0 = *(__IO unsigned long *)0x1FFFF7E8;
            unsigned long serial1 = *(__IO unsigned long *)0x1FFFF7EC;
            unsigned long serial2 = *(__IO unsigned long *)0x1FFFF7F0;

            /*
                    the legacy method of generating a serial number from UUID is prone to collision,
                so only use it on devices that were already set up that way.
            */
            if (use_uuid_serial == 1) {
                device_serial_0 = serial0 + serial2;
                device_serial_1 = serial1;

                if (is_unicode) {
                    int_to_unicode(device_serial_0,     &serial_number_string[0],                          8);
                    int_to_unicode(device_serial_1, &serial_number_string[8 * 2], EE_SERIAL_NUM_SIZE * 2 - 8);
                } else {
                    int_to_string(device_serial_0, &serial_number_string[0],                          8);
                    int_to_string(device_serial_1, &serial_number_string[8], EE_SERIAL_NUM_SIZE * 2 - 8);
                }
            /*
                    otherwise try to preserve some uniqueness by going to 60 bits,
                most of the change appears to be happening in the final 32 bits.
            */
            } else {
                device_serial_0 = serial2 ^ ((serial2 & 0xC000) >> 2);
                device_serial_1 = serial1 ^ serial0;
                device_serial_1 = device_serial_1 ^ ((device_serial_1 & 0xC000) >> 2);

                if (is_unicode) {
                    int_to_base32(device_serial_0,     &serial_number_string[0],                          6, is_unicode);
                    int_to_base32(device_serial_1, &serial_number_string[6 * 2], EE_SERIAL_NUM_SIZE * 2 - 6, is_unicode);
                } else {
                    int_to_base32(device_serial_0, &serial_number_string[0],                          6, is_unicode);
                    int_to_base32(device_serial_1, &serial_number_string[6], EE_SERIAL_NUM_SIZE * 2 - 6, is_unicode);
                }
            }
        /* otherwise send string 'AAAAAAAAAAAA' as the serial number in the factory. */
        } else {
            int_to_unicode(device_serial_0,     &serial_number_string[0],                          8);
            int_to_unicode(device_serial_1, &serial_number_string[8 * 2], EE_SERIAL_NUM_SIZE * 2 - 8);
        }
    }
}

