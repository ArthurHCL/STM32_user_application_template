#include "message_resolve_hook.h"
/* user application include */
#include "device_abnormal_state_record.h"



unsigned char Device_Abnormal_State_Get_Hook(
    unsigned char * const abnormal_state_buffer)
{
    return Device_Abnormal_State_Transform_Query(abnormal_state_buffer);
}

