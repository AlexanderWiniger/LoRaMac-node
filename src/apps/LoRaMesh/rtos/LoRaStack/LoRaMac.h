/**
 * \file LoRaMac.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa stack medium access layer implementation
 */

#ifndef __LORAMAC_H_
#define __LORAMAC_H_

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "LoRaMesh-config.h"
#include "LoRaPhy.h"

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define LORAMAC_HEADER_SIZE                 (1)
#define LORAMAC_MIC_SIZE                    (4)
#define LORAMAC_PAYLOAD_SIZE                (LORAPHY_PAYLOAD_SIZE-LORAMAC_HEADER_SIZE)
#define LORAMAC_BUFFER_SIZE                 (LORAPHY_BUFFER_SIZE)

/* PHY buffer access macros */
#define LORAMAC_BUF_IDX_HDR                 (LORAPHY_BUF_IDX_PAYLOAD+0) /* <Hdr> index */
#define LORAMAC_BUF_IDX_PAYLOAD             (LORAMAC_BUF_IDX_HDR+LORAMAC_HEADER_SIZE) /* <nwk payload> index */
/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORAMAC_BUF_HDR(phy)                        ((phy)[LORAMAC_BUF_IDX_HDR])

#define LORAMAC_BUF_MIC_START(phy, payloadSize)     (LORAPHY_BUF_PAYLOAD_START(phy) + payloadSize)
#define LORAMAC_BUF_PAYLOAD_START(phy)              (LORAPHY_BUF_PAYLOAD_START(phy) + LORAMAC_HEADER_SIZE)
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
/*! LoRaMAC message types */
typedef enum {
    MSG_TYPE_JOIN_REQ = 0x00,
    MSG_TYPE_JOIN_ACCEPT = 0x01,
    MSG_TYPE_DATA_UNCONFIRMED_UP = 0x02,
    MSG_TYPE_DATA_UNCONFIRMED_DOWN = 0x03,
    MSG_TYPE_DATA_CONFIRMED_UP = 0x04,
    MSG_TYPE_DATA_CONFIRMED_DOWN = 0x05,
    MSG_TYPE_RFU = 0x06,
    MSG_TYPE_PROPRIETARY = 0x07,
} LoRaMessageType_t;

/*! LoRaMAC header field definition */
typedef union {
    uint8_t Value;
    struct {
        uint8_t Major :2;
        uint8_t RFU :3;
        uint8_t MType :3;
    } Bits;
} LoRaMacHeader_t;

/*******************************************************************************
 * API FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * LoRa medium access layer initialization.
 */
void LoRaMac_Init( void );

/*!
 * \brief
 *
 * \param packet Pointer to the packet descriptor
 * \return Error code, ERR_OK if everything is ok, ERR_OVERFLOW if buffer is too small.
 */
uint8_t LoRaMac_OnPacketRx( LoRaPhy_PacketDesc *packet );

/*!
 * \brief Puts a payload into the buffer queue to be sent asynchronously.
 * \param fBuffer Message buffer with payload.
 * \param fBufferSize Size of message buffer, must be of LORAMAC_BUFFER_SIZE.
 * \param payloadSize Size of the payload in bytes.
 * \param type LoRa messsage type.
 *
 * \return Error code, ERR_OK if everything is ok, ERR_OVERFLOW if buffer is too small.
 */
uint8_t LoRaMac_PutPayload( uint8_t* bif, size_t bufSize, size_t payloadSize,
        LoRaMessageType_t type );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAMAC_H_ */
