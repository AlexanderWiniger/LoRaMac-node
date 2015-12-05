/**
 * \file LoRaNet.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa stack network layer implementation
 */

#ifndef __LORANET_H_
#define __LORANET_H_

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define LORANET_HEADER_SIZE_MAX             (22)
#define LORANET_HEADER_SIZE_MIN             (7)
#define LORANET_PAYLOAD_SIZE                (LORAMAC_PAYLOAD_SIZE-LORANET_HEADER_SIZE_MIN)
#define LORANET_BUFFER_SIZE                 (LORAMAC_BUFFER_SIZE)

/* PHY buffer access macros */
#define LORANET_BUF_IDX_TYPE                 (RNWK_BUF_IDX_PAYLOAD+0) /* <type> index */
#define LORANET_BUF_IDX_SIZE                 (RNWK_BUF_IDX_PAYLOAD+1) /* <size> index */
#define LORANET_BUF_IDX_PAYLOAD              (RNWK_BUF_IDX_PAYLOAD+2) /* <app payload> index */
/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORANET_BUF_DEVADDR(phy)                ((phy)[RAPP_BUF_IDX_TYPE])
#define LORANET_BUF_FCTRL(phy)                ((phy)[RAPP_BUF_IDX_SIZE])

#define LORANET_BUF_PAYLOAD_START(phy)       (RNWK_BUF_PAYLOAD_START(phy)+RAPP_HEADER_SIZE)

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
/*! LoRa network frame header field definition */
typedef union {
    uint8_t Value;
    struct {
        uint8_t FOptsLen :4;
        uint8_t FPending :1;
        uint8_t Ack :1;
        uint8_t AdrAckReq :1;
        uint8_t Adr :1;
    } Bits;
} LoRaFrameCtrl_t;

/*! LoRa network layer events structure */
typedef struct LoRaNetCallbacks_s {
    /*!
     * Function callback to get the current battery level needed for status
     * request command
     *
     * \retval batteryLevel Current battery level
     */
    uint8_t (*GetBatteryLevel)( void );
    /*!
     * Function callback prototype to handle proprietary message types that can not
     * be handled by the LoRa stack
     *
     * \param buf       Pointer to received message buffer
     * \param bufSize   Length of the received message
     * \param handled   Pointer to a handled flag to be set true if the callback recipient has
     *                  dealt with the message
     */
    void (*HandleProprietaryRxMsg)( uint8_t* buf, uint8_t bufSize, bool* handled );
    /*!
     * Function callback prototype to handle received message
     *
     * \param buf       Pointer to received message buffer
     * \param bufSize   Length of the received message
     * \param fPort     Port on which the message was received
     * \param handled   Pointer to a handled flag to be set true if the callback recipient has
     *                  dealt with the message
     */
    void (*HandleRxMsg)( uint8_t* buf, uint8_t bufSize, uint8_t fPort, bool* handled );
} LoRaNetCallbacks_t;

/*******************************************************************************
 * API FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * LoRa network layer initialization
 *
 * \param [IN] callabcks       Pointer to a structure defining the LoRaMAC
 *                             callback functions.
 */
void LoRaNet_Init( LoRaNetCallbacks_t *callabcks );

/*!
 * Enables/Disables the ADR (Adaptive Data Rate)
 *
 * \param [IN] enable [true: ADR ON, false: ADR OFF]
 */
void LoRaNet_SetAdrOn( bool enable );

/*!
 * Initializes the network IDs. Device address,
 * network session AES128 key and application session AES128 key.
 *
 * \remark To be only used when Over-the-Air activation isn't used.
 *
 * \param [IN] netID   24 bits network identifier
 *                     ( provided by network operator )
 * \param [IN] devAddr 32 bits device address on the network
 *                     (must be unique to the network)
 * \param [IN] nwkSKey Pointer to the network session AES128 key array
 *                     ( 16 bytes )
 * \param [IN] appSKey Pointer to the application session AES128 key array
 *                     ( 16 bytes )
 */
void LoRaNet_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey );

/*!
 * Initiates the Over-the-Air activation
 *
 * \param [IN] devEui Pointer to the device EUI array ( 8 bytes )
 * \param [IN] appEui Pointer to the application EUI array ( 8 bytes )
 * \param [IN] appKey Pointer to the application AES128 key array ( 16 bytes )
 *
 * \retval status [0: OK, 1: Tx error, 2: Already joined a network]
 */
uint8_t LoRaNet_JoinReq( uint8_t *devEui, uint8_t *appEui, uint8_t *appKey );

/*!
 * Sends a LinkCheckReq MAC command on the next uplink frame
 *
 * \retval status Function status [0: OK, 1: Busy]
 */
uint8_t LoRaNet_LinkCheckReq( void );

/*!
 * LoRaMAC layer send frame
 *
 * \param [IN] fBuffer     MAC data buffer to be sent
 * \param [IN] fBufferSize MAC data buffer size
 * \param [IN] fPort       MAC payload port (must be > 0)
 * \param [IN] confirmed   Confirmed message
 * \param [IN] nofRetries  Number of retries to receive the acknowledgement
 *
 * \retval status          [0: OK, 1: Busy, 2: No network joined,
 *                          3: Length or port error, 4: Unknown MAC command
 *                          5: Unable to find a free channel
 *                          6: Device switched off]
 */
uint8_t LoRaMacSendFrame( void *fPayload, uint16_t fPayloadSize, uint8_t fPort, bool confirmed,
        uint8_t nofRetries );

/*!
 * \brief Print out child nodes.
 *
 * \param reverseOrder Print out the list in reversed order.
 */
void LoRaNet_PrintChildNodes( bool reverseOrder );

/*!
 * \brief Print out multicast groups.
 *
 * \param reverseOrder Print out the list in reversed order.
 */
void LoRaNet_PrintMulticastGroups( bool reverseOrder );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORANET_H_ */
