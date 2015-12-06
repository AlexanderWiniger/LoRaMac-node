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
#define LORAMAC_PAYLOAD_SIZE                (LORAPHY_PAYLOAD_SIZE-LORAMAC_HEADER_SIZE)
#define LORAMAC_BUFFER_SIZE                 (LORAPHY_BUFFER_SIZE)

/* PHY buffer access macros */
#define LORAMAC_BUF_IDX_HDR                 (LORAPHY_BUF_IDX_PAYLOAD+0) /* <Hdr> index */
#define LORAMAC_BUF_IDX_PAYLOAD             (LORAMAC_BUF_IDX_HDR+LORAMAC_HEADER_SIZE) /* <nwk payload> index */
/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORAMAC_BUF_HDR(phy)                ((phy)[LORAMAC_BUF_IDX_HDR])

#define LORAMAC_BUF_PAYLOAD_START(phy)      (LORAPHY_BUF_PAYLOAD_START(phy) \
                                             + LORAMAC_HEADER_SIZE)
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
/*! LoRaWAN devices classes definition */
typedef enum {
    CLASS_A, CLASS_B, CLASS_C, CLASS_D /* LoRaMesh class */,
} DeviceClass_t;

/*! LoRaMAC channels parameters definition */
typedef union {
    int8_t Value;
    struct {
        int8_t Min :4;
        int8_t Max :4;
    } Fields;
} DrRange_t;

typedef struct {
    uint16_t DCycle;
    int8_t TxMaxPower;
    uint64_t LastTxDoneTime;
    uint64_t TimeOff;
} Band_t;

typedef struct {
    uint32_t Frequency;   // Hz
    DrRange_t DrRange; // Max datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
                       // Min datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
    uint8_t Band;        // Band index
} ChannelParams_t;

typedef struct {
    uint32_t Frequency;   // Hz
    uint8_t Datarate; // [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
} Rx2ChannelParams_t;

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
 *
 */
uint8_t LoRaMac_PutPayload( uint8_t* fBuffer, uint8_t fBufferSize,
        uint8_t payloadSize, uint8_t pktHdrSize, LoRaMessageType_t type );

/*!
 * \brief Set network and application session key
 */
void LoRaMac_SetSessionKeys( uint8_t *nwkSKey, uint8_t *appSKey );

/*!
 * \brief Set device class
 *
 * \param deviceClass End-device class
 */
void LoRaMac_SetDeviceClass( DeviceClass_t deviceClass );

/*
 * TODO: Add documentation
 */
void LoRaMac_SetChannel( uint8_t id, ChannelParams_t params );

/*
 * TODO: Add documentation
 */
void LoRaMac_SetRx2Channel( Rx2ChannelParams_t param );

/*!
 * Sets channels tx output power
 *
 * \param [IN] txPower [TX_POWER_20_DBM, TX_POWER_14_DBM,
 TX_POWER_11_DBM, TX_POWER_08_DBM,
 TX_POWER_05_DBM, TX_POWER_02_DBM]
 */
void LoRaMac_SetChannelsTxPower( int8_t txPower );

/*!
 * Sets channels datarate
 *
 * \param [IN] datarate eu868 - [DR_0, DR_1, DR_2, DR_3, DR_4, DR_5, DR_6, DR_7]
 *                      us915 - [DR_0, DR_1, DR_2, DR_3, DR_4]
 */
void LoRaMac_SetChannelsDatarate( int8_t datarate );

/*
 * TODO: Add documentation
 */
void LoRaMac_SetChannelsMask( uint16_t *mask );

/*
 * TODO: Add documentation
 */
void LoRaMac_SetChannelsNbRep( uint8_t nbRep );

/*
 * TODO: Add documentation
 */
void LoRaMac_ChannelRemove( uint8_t id );

/*!
 * LoRaMesh add child node to the network.
 *
 * \param [IN] nodeAddr Child node address.
 */
void LoRaMac_AddChildNode( uint32_t nodeAddr );

/*!
 * LoRaMesh add multicast group to the network.
 *
 * \param [IN] grpAddr Multicast group address
 */
void LoRaMac_AddMulticastGroup( uint32_t grpAddr );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAMAC_H_ */
