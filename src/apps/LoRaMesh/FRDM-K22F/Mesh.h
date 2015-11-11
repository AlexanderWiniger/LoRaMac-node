/**
 * \file mesh.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 11.11.2015
 * \brief Mesh LoRa network implementation
 *
 */
#ifndef __MESH_H__
#define __MESH_H__

/*!
 * LoRaMAC mote MAC commands
 */
typedef enum
{
    MOTE_MAC_UPLINK_SLOT_INFO_REQ    = 0x80,
    MOTE_MAC_MULTICAST_GRP_INFO_REQ  = 0x81,
    MOTE_MAC_ROUTE_DISCOVERY_ANS     = 0x82,
    MOTE_MAC_RX_PARAM_SETUP_ANS      = 0x83,
    MOTE_MAC_DEV_STATUS_ANS          = 0x06,
}LoRaMacExtMoteCmd_t;

#endif // __MESH_H__