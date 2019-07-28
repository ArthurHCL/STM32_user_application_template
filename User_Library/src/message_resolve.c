#include "message_resolve.h"
#include "message_resolve_hook.h"
/* system include */
#include <stdbool.h>
/* user library include */
#include "crc.h"
#include "eeprom_data_manager.h"



/*
    description:
            calculate CRC to compared with given CRC.
    parameter:
        *application_code_packet_RX:
                packet to check.
    return:
        true:
                correct CRC.
        false:
                wrong CRC.
*/
bool Is_CRC_Of_Application_Code_Packet_RX_Right(
    const APPLICATION_CODE_PACKET_RX * const application_code_packet_RX)
{
    unsigned char CRC_calculated;
    unsigned char CRC_compared;

    CRC_calculated = crc8(&application_code_packet_RX->common.command, application_code_packet_RX->common.packet_length);
    CRC_compared   = *(unsigned char *)application_code_packet_RX->common.crc;

    if (CRC_compared == CRC_calculated) {
        return true;
    } else {
        return false;
    }
}

/*
    description:
            update CRC.
    parameter:
        *application_code_packet_TX:
                packet which CRC is needed to be updated.
    attention:
            packet_length should be updated before calling the function.
*/
void CRC_Of_Application_Code_Packet_TX_Set(
    APPLICATION_CODE_PACKET_TX * const application_code_packet_TX)
{
    *(unsigned char *)application_code_packet_TX->common.crc =
        crc8(&application_code_packet_TX->common.command, application_code_packet_TX->common.packet_length);
}



/*
    description:
            resolve application code packet.
    parameter:
        *application_code_packet_RX:
                input data buffer to be resolved.
        *application_code_packet_TX:
                output data buffer to be transfered.
        is_called_by_USB:
                air command should only be used on the air if the device is aimed to be used on the air.
    return:
        true:
                output data should be transfered back now if it is intermediate communication site.
        false:
                input data should be transfered continue if it is intermediate communication site.
    attention:
            if it is final communication site(not intermediate communication site),
        then ignore the return value,
        and it must give response!
*/
bool Application_Code_Packet_Resolve(
    const APPLICATION_CODE_PACKET_RX * const application_code_packet_RX,
    APPLICATION_CODE_PACKET_TX * const       application_code_packet_TX,
    const bool                               is_called_by_USB)
{
    bool is_return_now = true;

    /* set fixed TX message. */
    application_code_packet_TX->common.fixed_head_1 = FIXED_HEAD_1;
    application_code_packet_TX->common.fixed_head_2 = FIXED_HEAD_2;
    application_code_packet_TX->common.command = application_code_packet_RX->common.command;

    /* at first set packet_length to basic value to reduce the amount of subsequent code. */
    application_code_packet_TX->common.packet_length = FIXED_TX_PACKET_HEADER_LENGTH;
    /* at first set error_code to OK to reduce the amount of subsequent code. */
    application_code_packet_TX->error_code = APPLICATION_CODE_PACKET_ERROR_OK;

    /* check if protocol header is false. */
    if ((FIXED_HEAD_1 != application_code_packet_RX->common.fixed_head_1) ||
        (FIXED_HEAD_2 != application_code_packet_RX->common.fixed_head_2))
    {
        application_code_packet_TX->error_code = APPLICATION_CODE_PACKET_ERROR_FALSE_PROTOCOL_HEADER;

        goto return_now;
    }

    /* check if protocol CRC is false. */
    if (!Is_CRC_Of_Application_Code_Packet_RX_Right(application_code_packet_RX)) {
        application_code_packet_TX->error_code = APPLICATION_CODE_PACKET_ERROR_FALSE_PROTOCOL_CRC;

        goto return_now;
    }

    /* start to handle different command. */
    switch (application_code_packet_RX->common.command) {
    case APPLICATION_CODE_PACKET_VERSION_GET:
        application_code_packet_TX->common.packet_length += 6;

        Firmware_Version_Get(
            (unsigned short *)application_code_packet_TX->TX_operation.firmware_version.major,
            (unsigned short *)application_code_packet_TX->TX_operation.firmware_version.minor,
            (unsigned short *)application_code_packet_TX->TX_operation.firmware_version.revision);

        break;
    default:
        application_code_packet_TX->error_code = APPLICATION_CODE_PACKET_ERROR_UNSUPPORTED_COMMAND;

        goto return_now;
    }

return_now:
    CRC_Of_Application_Code_Packet_TX_Set(application_code_packet_TX);

    return is_return_now;
}

