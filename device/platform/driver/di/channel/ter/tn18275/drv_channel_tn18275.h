/********************************************************************
 * $Workfile:   drv_channel_tn18275.h  $
 * $Modtime:   Feb 19 2011 17:03:30  $
 * Project    : Common
 * Author     : J.D.CHO
 * Description:
 *
 *                              Copyright (c) 2011 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _TDA18275_H_
#define _TDA18275_H_


/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */

int TDA18273_Initialize( int nUnitId );
int TDA18273_SetTune( int nUnitId, HUINT32 ulRfFreqHz, int bandWidth );
int TDA18273_GetPowerLevel(int nUnitId, float *pfPower);
void TDA18275_DumpRegister(int nUnitId);
int TDA18273_Close( int nUnitId );
HUINT32 TDA18275_GetIfFreq( int nUnitId, HUINT32 ulRfFreqHz, int nBandWidth );

#endif // _TDA18275_H_

