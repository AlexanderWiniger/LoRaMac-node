/**
 * \file LoRaNet.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa stack board specific configuration file
 */

#ifndef __LORANET_CONFIG_H_
#define __LORANET_CONFIG_H_

#include "board.h"

/* Configuration for Rx and Tx queues */
#ifndef LORANET_CONFIG_MSG_QUEUE_RX_LENGTH
#define LORANET_CONFIG_MSG_QUEUE_RX_LENGTH          (6)
/*!< Number items in the Rx message queue. The higher, the more items can be buffered. */
#endif

#ifndef LORANET_CONFIG_MSG_QUEUE_TX_LENGTH
#define LORANET_CONFIG_MSG_QUEUE_TX_LENGTH          (6)
/*!< Number items in the Tx message queue. The higher, the more items can be buffered. */
#endif

#ifndef LORANET_CONFIG_MSG_QUEUE_PUT_BLOCK_TIME_MS
#define LORANET_CONFIG_MSG_QUEUE_PUT_BLOCK_TIME_MS   (200/portTICK_RATE_MS)
/*!< Blocking time for putting items into the message queue before timeout. Use portMAX_DELAY for blocking. */
#endif

#endif /* __LORANET_CONFIG_H_ */
