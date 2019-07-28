#include "main.h"
/* system include */
#include <stdbool.h>
/* chip library include */
#include "stm32f10x.h"
/* USB include */
#include "usb_init.h"
/* user library include */
#include "delay.h"
#include "USB_message_process.h"
/* user application include */
#include "hw_config.h"



static unsigned long time_count;



/* all of uncertain problem is noted as "// question:??????". */
int main(void)
{
    /* it must be placed at the beginning of the code. */
    Set_System();

    /* restart USB connection(you can see device manager list change in PC). */
    USB_Port_Set(false);
    delay_ms(200); /* delay to make PC have enough time to check disconnect. */
    USB_Port_Set(true);
    delay_ms(200);

    USB_Init();

    while (1) {
        time_count++;
        USB_RX_Message_Process_In_Main_While();

#if !IS_DISABLE_SLEEP_FUNCTION_IN_MAIN_WHILE_FOR_DEBUG_OBSERVE
        __WFI();
#endif
    }
}

