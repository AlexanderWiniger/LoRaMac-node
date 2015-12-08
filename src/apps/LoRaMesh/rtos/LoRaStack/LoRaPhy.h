/**
 * \file LoRaPhy.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 12.11.2015
 * \version 1.0
 *
 * \brief LoRa stack physical layer implementation
 */

#ifndef __LORAPHY_H_
#define __LORAPHY_H_

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "LoRaMac-board.h"
#include "LoRaMesh-config.h"

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
/*!
 * Payload format is:
 *
 * PHY: <flags><size>|<phy payload>
 * MAC:               <mhdr><mac payload><mic>
 * FRM:                     <fhdr><fport><frm payload>
 * MSH:                                  <mesh payload>
 */
#define LORAPHY_HEADER_SIZE                 (2)
#define LORAPHY_PAYLOAD_SIZE                (LORAMESH_CONFIG_TRANSCEIVER_PAYLOAD_SIZE)
#define LORAPHY_BUFFER_SIZE                 (LORAPHY_HEADER_SIZE+LORAPHY_PAYLOAD_SIZE)

/* PHY buffer access macros */
#define LORAPHY_BUF_IDX_FLAGS               (0) /* <flags> index */
#define LORAPHY_BUF_IDX_SIZE                (1) /* <size> index */
#define LORAPHY_BUF_IDX_PAYLOAD             (2) /* <phy payload> index */

/* flag bits inside PacketDesc below */
#define LORAPHY_PACKET_FLAGS_NONE           (0)
/*!< initialization value */
#define LORAPHY_PACKET_FLAGS_TX_ADVERTISING (1<<0)
/*!< valid ACK received */
#define LORAPHY_PACKET_FLAGS_TX_REGULAR     (1<<1)
/*!< request acknowledge */
#define LORAPHY_PACKET_FLAGS_TX_MULTICAST   (1<<2)
/*!< power down transceiver */

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORAPHY_BUF_FLAGS(phy)              ((phy)[LORAPHY_BUF_IDX_FLAGS])
#define LORAPHY_BUF_SIZE(phy)               ((phy)[LORAPHY_BUF_IDX_SIZE])
#define LORAPHY_BUF_PAYLOAD_START(phy)      ((phy) + LORAPHY_HEADER_SIZE)
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
typedef enum {
    PHY_INITIAL_STATE,
    PHY_IDLE,
    PHY_RECEIVING,
    PHY_POWER_DOWN,
    PHY_WAIT_FOR_TXDONE,
    PHY_TIMEOUT
} LoRaPhy_AppStatus_t;

/*! Rx reception window type */
typedef enum {
    RX_TYPE_ADV, RX_TYPE_WINDOW1, RX_TYPE_WINDOW2, RX_TYPE_NORMAL
} RxWindowType_t;

/*! Rx config structure */
typedef struct {
    uint32_t freq;
    int8_t datarate;
    uint32_t bandwidth;
    uint16_t timeout;
    bool rxContinuous;
} RadioRxConfig_t;

/*! Tx config structure */
typedef struct {
    RadioModems_t modem;
    int8_t power;
    uint32_t fdev;
    uint32_t bandwidth;
    uint32_t datarate;
    uint8_t coderate;
    uint16_t preambleLen;
    bool fixLen;
    bool crcOn;
    bool FreqHopOn;
    uint8_t HopPeriod;
    bool iqInverted;
    uint32_t timeout;
} RadioTxConfig_t;

typedef struct {
    uint8_t flags;/*!< flags, see LORAPHY_PACKET_FLAGS_XXXX above */
    uint8_t *phyData; /*!< pointer to the PHY data buffer */
    size_t phySize; /*!< size of PHY data buffer */
    uint8_t *rxtx; /*!< pointer into phyData, start of TX/RX data */
} LoRaPhy_PacketDesc;

/*******************************************************************************
 * API FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * LoRa physical layer initialization.
 */
void LoRaPhy_Init( void );

/*!
 * LoRa physical layer process.
 */
uint8_t LoRaPhy_Process( void );

/*!
 * \brief
 *
 * \param packet Pointer to the packet descriptor
 * \return Error code, ERR_OK if everything is ok, ERR_OVERFLOW if buffer is too small.
 */
uint8_t LoRaPhy_OnPacketRx( LoRaPhy_PacketDesc *packet );

/*!
 * \brief Puts a packet into the queue to be sent.
 * \param buf Pointer to the packet buffer.
 * \param bufSize Size of the payload buffer.
 * \param payloadSize Size of payload data.
 * \return Error code, ERR_OK for everything fine.
 */
uint8_t LoRaPhy_PutPayload( uint8_t *buf, size_t bufSize, size_t payloadSize,
        uint8_t flags );

/*!
 * Initializes and opens the reception window
 *
 * \param [IN] freq window channel frequency
 * \param [IN] datarate window channel datarate
 * \param [IN] bandwidth window channel bandwidth
 * \param [IN] timeout window channel timeout
 */
void LoRaPhy_OpenRxWindow( uint32_t freq, uint8_t datarate, uint32_t bandwidth,
        uint16_t timeout, bool rxContinuous );

/*!
 * Returns a radomly generated 16-bit value called nonce to generate session keys
 */
uint16_t LoRaPhy_GenerateNonce( void );

/*
 * \brief Maximal duration a reception window will be opened for
 *
 * \param duration Max. reception window duration in ms
 */
void LoRaPhy_SetMaxRxWindow( uint32_t duration );

/*
 * \brief Set the delay of the first reception window after Tx
 * of a regular message
 *
 * \param delay Delay in ms
 */
void LoRaPhy_SetReceiveDelay1( uint32_t delay );

/*!
 * \brief Set the delay of the second reception window after Tx
 * of a regular message
 *
 * \param delay Delay in ms
 */
void LoRaPhy_SetReceiveDelay2( uint32_t delay );

/*!
 * \brief Set the delay of the first reception window after Tx
 * of a join request message
 *
 * \param delay Delay in ms
 */
void LoRaPhy_SetJoinAcceptDelay1( uint32_t delay );

/*
 * \brief Set the delay of the second reception window after Tx
 * of a join request message
 *
 * \param delay Delay in ms
 */
void LoRaPhy_SetJoinAcceptDelay2( uint32_t delay );

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAPHY_H_ */
