/**
 * \file LoRaMesh_App.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa mesh application
 */

#ifndef __LORAMESH_APP_H_
#define __LORAMESH_APP_H_

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "Shell.h"

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
typedef struct {
    uint16_t GroundSpeed;
    uint16_t Track;
} VectorTrack_t;

typedef struct {
    uint16_t Barometric;
    uint16_t GPS;
} Altitude_t;

typedef union {
    uint8_t Value;
    struct {
        uint8_t WindSpeed :1;
        uint8_t VectorTrack :1;
        uint8_t AltitudeBar :1;
        uint8_t AltitudeGPS :1;
        uint8_t reserved :3;
    } Bits;
} DataEntryInfo_t;

typedef struct DataEntry_s {
    uint32_t DevAddr;
    uint32_t Timestamp;
    DataEntryInfo_t EntryInfo;
    uint32_t LatitudeBinary;
    uint32_t LongitudeBinary;
    Altitude_t Altitude;
    VectorTrack_t VectorTrack;
    uint16_t WindSpeed;
    struct DataEntry_s *next;
} DataEntry_t;

/*******************************************************************************
 * MODULE FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * Shell command parser.
 */
byte LoRaMesh_AppParseCommand( const unsigned char *cmd, bool *handled, Shell_ConstStdIO_t *io );
/*!
 * Initializes the LoRa mesh application
 */
void LoRaMesh_AppInit( void );
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LORAMESH_APP_H_ */
