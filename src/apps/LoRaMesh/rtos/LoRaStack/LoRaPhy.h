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
/* LoRa error codes */
#define LORA_ERR_OK                         0x00U /*!< OK */
#define LORA_ERR_RANGE                      0x01U /*!< Parameter out of range. */
#define LORA_ERR_VALUE                      0x02U /*!< Parameter of incorrect value. */
#define LORA_ERR_OVERFLOW                   0x03U /*!< Timer overflow. */
#define LORA_ERR_ENABLED                    0x04U /*!< Device is enabled. */
#define LORA_ERR_DISABLED                   0x05U /*!< Device is disabled. */
#define LORA_ERR_BUSY                       0x06U /*!< Device is busy. */
#define LORA_ERR_NOTAVAIL                   0x07U /*!< Requested value or method not available. */
#define LORA_ERR_RXEMPTY                    0x08U /*!< No data in receiver. */
#define LORA_ERR_TXFULL                     0x09U /*!< Transmitter is full. */
#define LORA_ERR_OVERRUN                    0x0AU /*!< Overrun error is detected. */
#define LORA_ERR_IDLE                       0x0BU /*!< Idle error is detected. */
#define LORA_ERR_FAULT                      0x0CU /*!< Fault error is detected. */
#define LORA_ERR_CRC                        0x0DU /*!< CRC error is detected. */
#define LORA_ERR_UNDERFLOW                  0x0EU /*!< Underflow error is detected. */
#define LORA_ERR_UNDERRUN                   0x0FU /*!< Underrun error is detected. */
#define LORA_ERR_COMMON                     0x10U /*!< Common error of a device. */
#define LORA_ERR_FAILED                     0x11U /*!< Requested functionality or process failed. */
#define LORA_ERR_QFULL                      0x12U /*!< Queue is full. */

/*!  */
#define LORAPHY_HEADER_SIZE                 (0)
#define LORAPHY_PAYLOAD_SIZE                (LORAMESH_CONFIG_TRANSCEIVER_PAYLOAD_SIZE)
#define LORAPHY_BUFFER_SIZE                 (LORAPHY_HEADER_SIZE+LORAPHY_PAYLOAD_SIZE)

/*! Class A&B receive delay in us  */
#define RECEIVE_DELAY1                      (LORAMESH_CONFIG_RECEIVE_DELAY1)
#define RECEIVE_DELAY2                      (LORAMESH_CONFIG_RECEIVE_DELAY2)

/*! Join accept receive delay in us */
#define JOIN_ACCEPT_DELAY1                  (LORAMESH_CONFIG_JOIN_ACCEPT_DELAY1)
#define JOIN_ACCEPT_DELAY2                  (LORAMESH_CONFIG_JOIN_ACCEPT_DELAY2)

/*! Class A&B maximum receive window delay in us */
#define MAX_RX_WINDOW                       (LORAMESH_CONFIG_MAX_RX_WINDOW)

/* Advertising constants */
#define ADV_CHANNEL_FREQUENCY               (LORAMESH_CONFIG_ADV_CHANNEL_FREQUENCY)
#define ADV_BANDWIDTH                       (LORAMESH_CONFIG_ADV_BANDWIDTH)
#define ADV_DATARATE                        (LORAMESH_CONFIG_ADV_DATARATE)
#define ADV_TX_POWER                        (LORAMESH_CONFIG_ADV_TX_POWER)
#define ADV_INTERVAL                        (LORAMESH_CONFIG_ADV_INTERVAL)
#define ADV_SLOT_DURATION                   (LORAMESH_CONFIG_ADV_SLOT_DURATION)
#define ADV_EXPLICIT_HDR_OFF                (LORAMESH_CONFIG_ADV_EXPLICIT_HDR_OFF)
#define ADV_PACKET_LEN                      (LORAMESH_CONFIG_ADV_PACKET_LEN)
#define ADV_CRC_ON                          (LORAMESH_CONFIG_ADV_CRC_ON)

/* PHY buffer access macros */
#define LORAPHY_BUF_IDX_PAYLOAD             (0) /* <phy payload> index */

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define LORAPHY_BUF_PAYLOAD_START(phy)      ((phy) + LORAPHY_HEADER_SIZE)
/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
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
    uint8_t* pBuf; /* Pointer to the buffer */
    uint8_t bufSize; /* Buffer size */
    uint16_t rssi;
    uint8_t snr;
} RxPacketDesc_t;

typedef struct {
    uint8_t* pBuf;
    uint8_t bufSize;
    uint8_t txConfig;
} TxPacketDesc_t;

typedef void* PacketDesc_t;

/*******************************************************************************
 * API FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * LoRa physical layer initialization.
 */
void LoRaPhy_Init( void );

/*!
 * \brief Puts a packet into the queue to be sent.
 * \param buf Pointer to the packet buffer.
 * \param bufSize Size of the payload buffer.
 * \param payloadSize Size of payload data.
 * \return Error code, ERR_OK for everything fine.
 */
uint8_t LoRaPhy_PutPayload( uint8_t *fBuffer, size_t fBufferSize,
        uint8_t payloadSize );

/*!
 * \brief Returns the PHY payload data.
 * \param[out] packet Pointer to packet descriptor.
 * \return Error code, ERR_OK if everything is ok, ERR_OVERFLOW if buffer is too small, ERR_.
 */
uint8_t LoRaPhy_GetPayload( RxPacketDesc_t *packet );

/*!
 * Initializes and opens the reception window
 *
 * \param [IN] freq window channel frequency
 * \param [IN] datarate window channel datarate
 * \param [IN] bandwidth window channel bandwidth
 * \param [IN] timeout window channel timeout
 */
void LoRaPhy_OpenRxWindow( uint32_t freq, int8_t datarate, uint32_t bandwidth,
        uint16_t timeout, bool rxContinuous );

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
