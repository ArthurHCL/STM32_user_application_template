#ifndef _MESSAGE_RESOLVE_H_
#define _MESSAGE_RESOLVE_H_



#include <stdbool.h>



#define FIXED_HEAD_1    ((unsigned char)0x55)
#define FIXED_HEAD_2    ((unsigned char)0xAA)

#define FIXED_HEAD_1_POSITION     0
#define FIXED_HEAD_2_POSITION     1
#define PACKET_LENGTH_POSITION    2
#define CRC_0_POSITION            3



#define FIXED_TX_PACKET_HEADER_LENGTH    (2)



/* application code command list. */
typedef enum {
    APPLICATION_CODE_PACKET_VERSION_GET = 0x01,
} APPLICATION_CODE_PACKET_TYPE;

/* application code command error code response. */
typedef enum {
    APPLICATION_CODE_PACKET_ERROR_OK,

    APPLICATION_CODE_PACKET_ERROR_UNSUPPORTED_COMMAND,
    APPLICATION_CODE_PACKET_ERROR_FALSE_PROTOCOL_HEADER,
    APPLICATION_CODE_PACKET_ERROR_FALSE_PROTOCOL_CRC,
} APPLICATION_CODE_PACKET_ERROR_CODE;



/* definition to show firmware version. */
typedef struct {
    unsigned char major[2];
    unsigned char minor[2];
    unsigned char revision[2];
} FIRMWARE_VERSION;



/* application code packet common header between TX and RX. */
typedef struct {
    unsigned char fixed_head_1;
    unsigned char fixed_head_2;
    unsigned char packet_length;
    unsigned char crc[1];
    unsigned char command;
} APPLICATION_CODE_PACKET_COMMON_HEADER;

/* application code packet RX. */
typedef struct {
    APPLICATION_CODE_PACKET_COMMON_HEADER common;

    union {
        unsigned char reserve;
    } RX_operation;
} APPLICATION_CODE_PACKET_RX;

/* application code packet TX. */
typedef struct {
    APPLICATION_CODE_PACKET_COMMON_HEADER common;
    unsigned char error_code;

    union {
        FIRMWARE_VERSION firmware_version;
    } TX_operation;
} APPLICATION_CODE_PACKET_TX;



bool Is_CRC_Of_Application_Code_Packet_RX_Right(
    const APPLICATION_CODE_PACKET_RX * const application_code_packet_RX);

void CRC_Of_Application_Code_Packet_TX_Set(
    APPLICATION_CODE_PACKET_TX * const application_code_packet_TX);

bool Application_Code_Packet_Resolve(
    const APPLICATION_CODE_PACKET_RX * const application_code_packet_RX,
    APPLICATION_CODE_PACKET_TX * const       application_code_packet_TX,
    const bool                               is_called_by_USB);



#endif

