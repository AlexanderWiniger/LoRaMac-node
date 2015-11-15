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
typedef struct LoRaMeshEventInfo_s {
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
typedef struct LoRaMeshCallbacks_s {
    /*!
     * MAC layer event callback prototype.
     *
     * \param [IN] flags Bit field indicating the MAC events occurred
     * \param [IN] info  Details about MAC events occurred
     */
    void (*MeshEvent)(LoRaMeshEventFlags_t *flags, LoRaMeshEventInfo_t *info);
} LoRaMeshCallbacks_t;

/*!
 * LoRaMesh slot info structure
 */
typedef struct LoRaMeshSlotInfo_s {
    uint32_t Frequency;
    uint8_t Periodicity;
    uint8_t Duration;
} LoRaMeshSlotInfo_t;

/*!
 * LoRaMesh child node info structure
 */
typedef struct LoRaMeshChildNodeInfo_s {
    uint32_t Address;
    uint8_t NwkSKey[16];
    uint8_t AppSKey[16];
    uint32_t UpLinkCounter;
    LoRaMeshSlotInfo_t UplinkSlot;
} LoRaMeshChildNodeInfo_t;

/*!
 * LoRaMesh altitude data structure.
 */
typedef struct LoRaMeshAltitude_s {
    uint16_t GPS;
    uint16_t Barometric;
} LoRaMeshAltitude_t;

/*!
 * LoRaMesh track data structure.
 */
typedef struct LoRaMeshVectorTrack_s {
    uint16_t GroundSpeed;
    uint16_t Track;
} LoRaMeshVectorTrack_t;

/*!
 * LoRaMesh position data structure.
 */
typedef struct LoRaMeshPosition_s {
    uint32_t Latitude;
    uint32_t Longitude;
} LoRaMeshPosition_t;

/*!
 * LoRaMesh pilot data structure.
 */
typedef struct LoRaMeshPilotData_s {
    uint32_t Time;
    LoRaMeshPosition_t Position;
    LoRaMeshAltitude_t Altitude;
    LoRaMeshVectorTrack_t VectorTrack;
    uint16_t WindSpeed;
} LoRaMeshPilotData_t;

/*!
 * LoRaMAC layer initialization
 *
 * \param [IN] callabcks       Pointer to a structure defining the LoRaMAC
 *                             callback functions.
 */
void LoRaMeshInit(LoRaMeshCallbacks_t *callabcks);

/*!
 * \brief Add child node at the tail of the list.
 *
 * \param childNode Child Node to append
 */
void AddChildNode(LoRaMeshChildNodeInfo_t* childNode);

/*!
 * \brief Remove child node from the list.
 *
 * \param devAddr Device address of child node to remove.
 * \return LoRaMeshChildNodeInfo_t Removed child node.
 */
LoRaMeshChildNodeInfo_t* RemoveChildNode(uint32_t devAddr);

/*!
 * \brief Remove child node at specified position from the list.
 *
 * \param devAddr Device address of child node to find.
 * \return LoRaMeshChildNodeInfo_t* Returns pointer to the found child node or NULL if not found.
 */
LoRaMeshChildNodeInfo_t* FindChildNode(uint32_t devAddr);

/*!
 * \brief Print out child node list.
 *
 * \param reverseOrder Print out the list in reversed order.
 */
void PrintChildNodeList(bool reverseOrder);

#endif // __LORAMESH_H__
