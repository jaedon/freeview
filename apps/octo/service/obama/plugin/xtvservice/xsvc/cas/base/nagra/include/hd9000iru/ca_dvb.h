/**
** @file ca_dvb.h
**
** @brief
**   This file defines some constants and typesin relation with DVB standards.
**
** @ingroup GLB
**
** @author Sebastien Robyr
**
** COPYRIGHT:
**   2004 Nagravision S.A.
**
** HISTORY:
**   $Log: ca_dvb.h,v $
**   Revision 1.2  2004/05/04 15:42:04  tonelli
**   PRMS ID: NONE
**   - Change TDvdS... in TDvbS...
**
**   Revision 1.1  2004/05/04 06:26:13  robyr
**   PRMS ID: NONE
**   - First issue.
**
*/

#ifndef CA_DVB_H
#define CA_DVB_H

#include "ca_defs.h"

/******************************************************************************/
/*                                                                            */
/*                           CONSTANTS DEFINITIONS                            */
/*                                                                            */
/******************************************************************************/

/* None */

/******************************************************************************/
/*                                                                            */
/*                              TYPES DEFINITIONS                             */
/*                                                                            */
/******************************************************************************/


/**
 *  @brief
 *    DVB Original Network Identifier (ONid).
*/
typedef TUnsignedInt16 TDvbOriginalNetworkId;

/**
 *  @brief
 *    DVB Transport Stream Identifier (TSid).
*/
typedef TUnsignedInt16 TDvbTransportStreamId;

/**
 *  @brief
 *    DVB Service Identifier (Service_id).
*/
typedef TUnsignedInt16 TDvbServiceId;


/**
 *  @brief
 *    DVB Triplet, that uniquely identifies a program on a DBV network.
*/
typedef struct
{
  TDvbOriginalNetworkId  originalNetworkId;
  TDvbTransportStreamId  transportStreamId;
  TDvbServiceId          serviceId;
} TDvbTriplet;

#endif /* CA_DVB_H */

/* End of file */
