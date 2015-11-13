/**
 * \file LoRaMesh.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 11.11.2015
 * \brief Mesh LoRa network implementation
 *
 */

#ifndef __LORAMESH_H__
#define __LORAMESH_H__

#define     SDUHDR_OPTION_LIST_MASK                 0x0F
#define     SDUHDR_OPTION_LIST_ALT_GPS_MASK         0x08
#define     SDUHDR_OPTION_LIST_ALT_BAR_MASK         0x04
#define     SDUHDR_OPTION_LIST_VEC_TRACK_MASK       0x02
#define     SDUHDR_OPTION_LIST_WIND_SPEED_MASK      0x01

/*!
 * LoRaMAC extended mote MAC commands
 */
typedef enum {
    MOTE_MAC_UPLINK_SLOT_INFO_REQ = 0x80,
    MOTE_MAC_MULTICAST_GRP_INFO_REQ = 0x81,
    MOTE_MAC_ROUTE_DISCOVERY_ANS = 0x82,
} LoRaMacExtMoteCmd_t;

/*!
 * LoRaMesh event flags
 */
typedef union {
    uint8_t Value;
    struct {
        uint8_t Tx :1;
        uint8_t Rx :1;
        uint8_t RxData :1;
        uint8_t Multicast :1;
        uint8_t RxSlot :2;
        uint8_t LinkCheck :1;
        uint8_t JoinAccept :1;
    } Bits;
} LoRaMeshEventFlags_t;

typedef enum {
    LORAMESH_EVENT_INFO_STATUS_OK = 0,
} LoRaMeshEventInfoStatus_t;

/*!
 * LoRaMesh event information
 */
typedef struct {
    LoRaMeshEventInfoStatus_t Status;
    bool TxAckReceived;
    uint8_t TxNbRetries;
    uint8_t TxDatarate;
    uint8_t RxPort;
    uint8_t *RxBuffer;
    uint8_t RxBufferSize;
    int16_t RxRssi;
    uint8_t RxSnr;
    uint16_t Energy;
    uint8_t DemodMargin;
    uint8_t NbGateways;
} LoRaMeshEventInfo_t;

/*!
 * LoRaMAC events structure
 * Used to notify upper layers of MAC events
 */
typedef struct sLoRaMeshCallbacks {
    /*!
     * MAC layer event callback prototype.
     *
     * \param [IN] flags Bit field indicating the MAC events occurred
     * \param [IN] info  Details about MAC events occurred
     */
    void (*MeshEvent)(LoRaMeshEventFlags_t *flags, LoRaMeshEventInfo_t *info);
} LoRaMeshCallbacks_t;

/*!
 * LoRaMAC layer initialization
 *
 * \param [IN] callabcks       Pointer to a structure defining the LoRaMAC
 *                             callback functions.
 */
void LoRaMeshInit(LoRaMeshCallbacks_t *callabcks);

#endif // __LORAMESH_H__
