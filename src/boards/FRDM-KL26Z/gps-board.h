/**
 * \file gps-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 05.11.2015
 * \brief Target board driver for GPS receiver
 *
 */

#ifndef __GPS_BOARD_H_
#define __GPS_BOARD_H_

/*!
 * \brief Low level Initialisation of the UART and IRQ for the GPS
 */
void GpsMcuInit(void);

#endif /* _GPS_BOARD_H_ */
