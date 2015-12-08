/**
 * \file LoRaMesh.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 07.12.2015
 * \version 1.0
 *
 * \brief LoRa stack common file
 */

#ifndef __LORAMESH_H_
#define __LORAMESH_H_

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "LoRaMesh-config.h"
#include "LoRaFrm.h"
#include "LoRaMac.h"
#include "LoRaPhy.h"

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define LORAMESH_HEADER_SIZE                 (0)
#define LORAMESH_PAYLOAD_SIZE                (LORAFRM_PAYLOAD_SIZE-LORAMESH_HEADER_SIZE)
#define LORAMESH_BUFFER_SIZE                 (LORAFRM_BUFFER_SIZE)

/* PHY buffer access macros */
#define LORAMESH_BUF_IDX_HDR                 (0) /* <Hdr> index */
#define LORAMESH_BUF_IDX_PAYLOAD             (LORAMESH_HEADER_SIZE) /* <app payload> index */
/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORAMESH_BUF_PAYLOAD_START(phy)      (LORAMESH_HEADER_SIZE)

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

typedef union {
    uint8_t Value;
    struct {
        uint8_t upLinkCounterFixed :1; /* 1: Frame counter up link is fixed */
        uint8_t rxWindowsDisabled :1; /* 1: Reception windows disabled */
        uint8_t dutyCycleCtrlOff :1; /* 1: Duty cycle control inactive */
        uint8_t reserved :5;
    } Bits;
} LoRaDbgFlags_t;

typedef union {
    uint8_t Value;
    struct {
        uint8_t nwkPublic :1; /* 1: Network is public */
        uint8_t nwkJoined :1; /* 1: Network joined successfully */
        uint8_t adrCtrlOn :1; /* 1: Adaptive data rate control active */
        uint8_t ackPending :1; /* 1: Node has sent a confirmed packet */
        uint8_t ackRequested :1; /* 1: Node has received a confirmed packet */
        uint8_t reserved :3;
    } Bits;
} LoRaMeshCtrlFlags_t;

/*! LoRa network layer events structure */
typedef struct LoRaMeshCallbacks_s {
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
} LoRaMeshCallbacks_t;

typedef struct {
    uint32_t netId; /* Network ID (24-Bit) */
    uint32_t devAddr; /* Network device address */
    uint8_t *devEui; /* Device extended unique identifier (64-Bit) */
    uint16_t devNonce; /* Device nonce */
    DeviceClass_t devClass; /* Device class */
    uint8_t *appEui; /* Application extended unique identifier (64-Bit) */
    uint8_t *appKey; /* Application key AES 128-Bit */
    uint8_t *nwkSKey; /* Network session key AES 128-Bit */
    uint8_t *appSKey; /* Application session key AES 128-Bit */
    uint32_t upLinkCounter; /* Frame counter up link */
    uint32_t downLinkCounter; /* Frame counter down link */
    uint32_t adrAckCounter; /* Adaptive data rate acknowledgement counter */
    uint8_t nbRep; /* Configured redundancy [1:15] (automatic uplink message repetition) */
    uint8_t nbRepCounter; /* Automatic repetition counter */
    uint16_t channelsMask[6]; /* Channel mask to disable channels from the channel list */
    uint8_t curChannelIdx; /* Index of the currently selected channel from the channel list */
    uint8_t curDatarateIdx; /* Currently selected data rate */
    uint8_t curTxPowerIdx; /* Currently selected output power */
    uint8_t macCmdBuffer[15]; /* MAC command buffer of commands to be added to FOpts field */
    uint8_t macCmdBufferSize; /* MAC command buffer size */
    LoRaMeshCtrlFlags_t ctrlFlags; /* Network flags */
    LoRaDbgFlags_t dbgFlags; /* Debug flags */
} LoRaDevice_t;

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
/*! Data rates table definition */
const uint8_t Datarates[] = { 12, 11, 10, 9, 8, 7, 7, 50 };

/*! Maximum payload with respect to the datarate index. Cannot operate with repeater. */
const uint8_t MaxPayloadByDatarate[] = { 51, 51, 51, 115, 242, 242, 242, 242 };

/*! Maximum payload with respect to the datarate index. Can operate with repeater. */
const uint8_t MaxPayloadByDatarateRepeater[] = { 51, 51, 51, 115, 222, 222, 222, 222 };

/*! Tx output powers table definition */
const uint8_t TxPowers[] = { 20, 14, 11, 8, 5, 2 };

/*! LoRa device used throughout the stack */
extern LoRaDevice_t* pLoRaDevice;

/*******************************************************************************
 * API FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 *
 * \param [IN] callabcks       Pointer to a structure defining the LoRaMAC
 *                             callback functions.
 */
void LoRaMesh_Init( LoRaMeshCallbacks_t *callbacks );

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
void LoRaMesh_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey );

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
uint8_t LoRaMesh_SendFrame( uint8_t *fPayload, size_t fPayloadSize, uint8_t fPort, bool confirmed );

/*!
 * Initiates the Over-the-Air activation
 *
 * \param [IN] devEui Pointer to the device EUI array ( 8 bytes )
 * \param [IN] appEui Pointer to the application EUI array ( 8 bytes )
 * \param [IN] appKey Pointer to the application AES128 key array ( 16 bytes )
 *
 * \retval status [0: OK, 1: Tx error, 2: Already joined a network]
 */
uint8_t LoRaMesh_JoinReq( uint8_t *devEui, uint8_t *appEui, uint8_t *appKey );

/*!
 * Sends a LinkCheckReq MAC command on the next uplink frame
 *
 * \retval status Function status [0: OK, 1: Busy]
 */
uint8_t LoRaMesh_LinkCheckReq( void );

/*!
 *
 */
bool LoRaMesh_IsNetworkJoined( void );

/*!
 * \brief Print out child nodes.
 *
 * \param reverseOrder Print out the list in reversed order.
 */
void LoRaMesh_PrintChildNodes( bool reverseOrder );

/*!
 * \brief Print out multicast groups.
 *
 * \param reverseOrder Print out the list in reversed order.
 */
void LoRaMesh_PrintMulticastGroups( bool reverseOrder );

/*******************************************************************************
 * SETUP FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * \brief Set network to public.
 *
 * \param enable Enable public network
 */
void LoRaMesh_SetPublicNetwork( bool enable );

/*!
 * Enables/Disables the ADR (Adaptive Data Rate)
 *
 * \param [IN] enable [true: ADR ON, false: ADR OFF]
 */
void LoRaMesh_SetAdrOn( bool enable );

/*******************************************************************************
 * TEST FUNCTION PROTOTYPES (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
/*!
 * Disables/Enables the duty cycle enforcement (EU868)
 *
 * \param   [IN] enable - Enabled or disables the duty cycle
 */
void LoRaMesh_TestSetDutyCycleCtrlOff( bool enable );

/*!
 * Disables/Enables the reception windows opening
 *
 * \param [IN] enable [true: enable, false: disable]
 */
void LoRaMesh_TestSetRxWindowsOn( bool enable );

/*!
 * Enables the MIC field test
 *
 * \param [IN] upLinkCounter Fixed Tx packet counter value
 */
void LoRaMesh_TestSetMicMode( uint16_t upLinkCounter );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAMESH_H_ */
