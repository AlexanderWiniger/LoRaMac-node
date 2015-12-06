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
#include "LoRaMesh-config.h"
#include "LoRaMac.h"
#include "LoRaPhy.h"

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define LORANET_HEADER_SIZE_MAX                 (22)
#define LORANET_HEADER_SIZE_MIN                 (7)
#define LORANET_PAYLOAD_SIZE                    (LORAMAC_PAYLOAD_SIZE-LORANET_HEADER_SIZE_MIN)
#define LORANET_BUFFER_SIZE                     (LORAMAC_BUFFER_SIZE)

/* PHY buffer access macros */
#define LORANET_BUF_IDX_DEVADDR                 (LORAMAC_BUF_IDX_PAYLOAD+0) /* <DevAddr> index */
#define LORANET_BUF_IDX_CTRL                    (LORAMAC_BUF_IDX_PAYLOAD+4) /* <fCtrl> index */
#define LORANET_BUF_IDX_CNTR                    (LORAMAC_BUF_IDX_PAYLOAD+5) /* <fCnt> index */
#define LORANET_BUF_IDX_OPTS                    (LORAMAC_BUF_IDX_PAYLOAD+6) /* <fOpts> index */
/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORANET_BUF_DEVADDR(phy)                ((phy)[LORANET_BUF_IDX_DEVADDR])
#define LORANET_BUF_CTRL(phy)                   ((phy)[LORANET_BUF_IDX_CTRL])
#define LORANET_BUF_CNTR(phy)                   ((phy)[LORANET_BUF_IDX_CNTR])
#define LORANET_BUF_OPTS(phy)                   ((phy)[LORANET_BUF_IDX_OPTS])

#define LORANET_BUF_IDX_PORT(optsLen)           (LORANET_BUF_IDX_OPTS + optsLen)
#define LORANET_BUF_PAYLOAD_START(phy, optsLen) (LORAMAC_BUF_PAYLOAD_START(phy) \
                                                + optsLen + LORANET_HEADER_SIZE_MIN)

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
    void (*HandleProprietaryRxMsg)( uint8_t* buf, uint8_t bufSize,
            bool* handled );
    /*!
     * Function callback prototype to handle received message
     *
     * \param buf       Pointer to received message buffer
     * \param bufSize   Length of the received message
     * \param fPort     Port on which the message was received
     * \param handled   Pointer to a handled flag to be set true if the callback recipient has
     *                  dealt with the message
     */
    void (*HandleRxMsg)( uint8_t* buf, uint8_t bufSize, uint8_t fPort,
            bool* handled );
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
void LoRaNet_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey,
        uint8_t *appSKey );

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
uint8_t LoRaNet_SendFrame( uint8_t *fPayload, uint16_t fPayloadSize,
        uint8_t fPort, bool confirmed );

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
 * SETUP FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * \brief Set network to public.
 *
 * \param enable Enable public network
 */
void LoRaNet_SetPublicNetwork( bool enable );

/*******************************************************************************
 * TEST FUNCTION PROTOTYPES (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
/*!
 * Disables/Enables the duty cycle enforcement (EU868)
 *
 * \param   [IN] enable - Enabled or disables the duty cycle
 */
void LoRaNet_TestSetDutyCycleOn( bool enable );

/*!
 * Disables/Enables the reception windows opening
 *
 * \param [IN] enable [true: enable, false: disable]
 */
void LoRaNet_TestRxWindowsOn( bool enable );

/*!
 * Enables the MIC field test
 *
 * \param [IN] upLinkCounter Fixed Tx packet counter value
 */
void LoRaNet_TestSetMic( uint16_t upLinkCounter );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORANET_H_ */
