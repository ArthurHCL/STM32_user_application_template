#include "device_abnormal_state_record.h"



//static 
volatile DEVICE_ABNORMAL_STATE_RECORD device_abnormal_state_record_set[DEVICE_ABNORMAL_STATE_RECORD_TOTAL];
//static 
volatile unsigned char                device_abnormal_state_record_amount;



/*
    description:
            insert the requested device abnormal state in record.
    parameter:
        state:
                the requested device abnormal state.
*/
void Device_Abnormal_State_Insert(
    const DEVICE_ABNORMAL_STATE_RECORD state)
{
    int i;
    int insert_position;

    /* find out insert position. */
    for (i = 0; i < device_abnormal_state_record_amount; i++) {
        /* check if the status has already existed. */
        if (state == device_abnormal_state_record_set[i]) {
            return;
        }

        if (state < device_abnormal_state_record_set[i]) {
            insert_position = i;

            break;
        }
    }
    if (i == device_abnormal_state_record_amount) {
        device_abnormal_state_record_set[i] = state;
        device_abnormal_state_record_amount++;

        return;
    }

    /* insert the status in ascending order. */
    for (i = device_abnormal_state_record_amount; i > insert_position; i--) {
        device_abnormal_state_record_set[i] = device_abnormal_state_record_set[i - 1];
    }
    device_abnormal_state_record_set[insert_position] = state;
    device_abnormal_state_record_amount++;
}

/*
    description:
            remove the requested device abnormal state in record.
    parameter:
        state:
                the requested device abnormal state.
*/
void Device_Abnormal_State_Remove(
    const DEVICE_ABNORMAL_STATE_RECORD state)
{
    int i;
    int remove_position;

    /* find out remove position. */
    for (i = 0; i < device_abnormal_state_record_amount; i++) {
        if (state == device_abnormal_state_record_set[i]) {
            remove_position = i;

            break;
        }
    }
    if (i == device_abnormal_state_record_amount) {
        return;
    }

    /* remove the status in ascending order. */
    for (i = remove_position + 1; i < device_abnormal_state_record_amount; i++) {
        device_abnormal_state_record_set[i - 1] = device_abnormal_state_record_set[i];
    }
    device_abnormal_state_record_amount--;
}

/*
    description:
            query all device abnormal state in record.
    parameter:
        *state:
                output all device abnormal state.
    return:
            return amount of device abnormal state.
*/
unsigned char Device_Abnormal_State_Query(
    DEVICE_ABNORMAL_STATE_RECORD * const state)
{
    int i;

    for (i = 0; i < device_abnormal_state_record_amount; i++) {
        state[i] = device_abnormal_state_record_set[i];
    }

    return device_abnormal_state_record_amount;
}

/*
    description:
            query all device abnormal state in record and transform them into char.
    parameter:
        *state:
                output all device abnormal state.
    return:
            return amount of device abnormal state.
*/
unsigned char Device_Abnormal_State_Transform_Query(
    unsigned char * const state)
{
    int i;

    for (i = 0; i < device_abnormal_state_record_amount; i++) {
        state[i] = (unsigned char)device_abnormal_state_record_set[i];
    }

    return device_abnormal_state_record_amount;
}

