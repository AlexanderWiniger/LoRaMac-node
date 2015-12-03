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

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/

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
    DrRange_t DrRange;   // Max datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
                         // Min datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
    uint8_t Band;        // Band index
} ChannelParams_t;

typedef struct {
    uint32_t Frequency;   // Hz
    uint8_t Datarate;    // [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
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
} LoRaNetFrameType_t;

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
void LoRaMac_SetPublicNetwork( bool enable );

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
void LoRaMacSetChannelsTxPower( int8_t txPower );

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

/*
 * TODO: Add documentation
 */
uint32_t LoRaMac_GetUpLinkCounter( void );

/*
 * TODO: Add documentation
 */
uint32_t LoRaMac_GetDownLinkCounter( void );

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
 * SETUP FUNCTION PROTOTYPES (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
/*!
 * Disables/Enables the duty cycle enforcement (EU868)
 *
 * \param   [IN] enable - Enabled or disables the duty cycle
 */
void LoRaMac_TestSetDutyCycleOn( bool enable );

/*!
 * Disables/Enables the reception windows opening
 *
 * \param [IN] enable [true: enable, false: disable]
 */
void LoRaMac_TestRxWindowsOn( bool enable );

/*!
 * Enables the MIC field test
 *
 * \param [IN] upLinkCounter Fixed Tx packet counter value
 */
void LoRaMac_TestSetMic( uint16_t upLinkCounter );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAMAC_H_ */
