/**
 * \file LoRaMesh_AppConfig.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 04.12.2015
 * \version 1.0
 *
 * \brief LoRa mesh application configuration file
 */

#ifndef __LORAMESH_APPCONFIG_H_
#define __LORAMESH_APPCONFIG_H_

/*!
 * Indicates if the end-device is to be connected to a private or public network
 */
#define LORAWAN_PUBLIC_NETWORK              true

/*!
 * Join requests trials duty cycle.
 */
#define LORAWAN_OTAA_INTERVAL               10000  // 10 [s] value in us

/*!
 * Defines the application data transmission duty cycle
 */
#define LORAMESH_APP_TX_INTERVAL            4000  // 5 [s] value in us
#define LORAMESH_APP_TX_INTERVAL_RND        1000  // 1 [s] value in us

/*!
 * Mote device IEEE EUI
 *
 * \remark must be written as a little endian value (reverse order of normal reading)
 *
 * \remark In this application the value is automatically generated by calling
 *         BoardGetUniqueId function
 */
#define LORAWAN_DEVICE_EUI                  {                           \
                                                0x00, 0x00, 0x00, 0x00, \
                                                0x00, 0x00, 0x00, 0x00  \
                                            }

/*!
 * Application IEEE EUI
 *
 * \remark must be written as a little endian value (reverse order of normal reading)
 */
#define LORAWAN_APPLICATION_EUI             {                           \
                                                0x00, 0x00, 0x00, 0x00, \
                                                0x00, 0x00, 0x00, 0x00  \
                                            }

/*!
 * AES encryption/decryption cipher application key
 */
#define LORAWAN_APPLICATION_KEY             {                           \
                                                0x11, 0x22, 0x33, 0x44, \
                                                0x55, 0x66, 0x77, 0x88, \
                                                0x99, 0xAA, 0xBB, 0xCC, \
                                                0xDD, 0xEE, 0xFF, 0x00  \
                                            }

/*!
 * Current network ID
 */
#define LORAWAN_NETWORK_ID                  ( uint32_t )0

/*!
 * Device address on the network
 *
 * \remark must be written as a big endian value (normal reading order)
 *
 * \remark In this application the value is automatically generated using
 *         a pseudo random generator seeded with a value derived from
 *         BoardUniqueId value
 */
#define LORAWAN_DEVICE_ADDRESS              ( uint32_t )0x00000000

/*! AES encryption/decryption cipher network session key */
#define LORAWAN_NWKSKEY                     {                           \
                                                0x2B, 0x7E, 0x15, 0x16, \
                                                0x28, 0xAE, 0xD2, 0xA6, \
                                                0xAB, 0xF7, 0x15, 0x88, \
                                                0x09, 0xCF, 0x4F, 0x3C  \
                                            }

/*! AES encryption/decryption cipher application session key */
#define LORAWAN_APPSKEY                     {                           \
                                                0x2B, 0x7E, 0x15, 0x16, \
                                                0x28, 0xAE, 0xD2, 0xA6, \
                                                0xAB, 0xF7, 0x15, 0x88, \
                                                0x09, 0xCF, 0x4F, 0x3C  \
                                            }

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON            false

/*!
 * LoRaWAN Adaptative Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                      false

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_OFF               true

/*!
 * LoRaMesh default number of retries
 */
#define LORAMESH_NOF_RETRIES                2

/*!
 * LoRaWAN application port
 */
#define LORAMESH_APP_PORT                   2

/*!
 * User application data buffer size
 */
#define LORAMESH_APP_DATA_MAX_SIZE          64

/*!
 * User application data buffer size
 */
#define LORAMESH_APP_DATA_SIZE              12

#endif /* __LORAMESH_APPCONFIG_H_ */
