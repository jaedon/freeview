/**
@defgroup RF rf di layer
Define rf functions
*/
/*@{*/

/**
@file di_rf.h
@brief rf header file
@author bhchung

@remarks
	-# 2010.05.06 : first release
*/

#ifndef __DI_RF_H__
#define __DI_RF_H__

#include "htype.h"
#include "di_err.h" /* if di module is included, di_err.h is included first!Later this line will be removed.*/

/**
@brief RF's output tv standard
*/
typedef enum DI_RF_TV_Standard
{
	DI_RF_TV_STD_PAL_I = 0,
	DI_RF_TV_STD_PAL_G,
	DI_RF_TV_STD_PAL_B,
	DI_RF_TV_STD_PAL_D,
	DI_RF_TV_STD_PAL_K,
	DI_RF_TV_STD_PAL_M,
	DI_RF_TV_STD_NTSC,
	DI_RF_TV_STD_END
} DI_RF_TV_STANDARD;


/**
@ingroup RF
Set RF video format, channel number, offset.
@param[in] DI_RF_TV_STANDARD eTvStd
@param[in] HUINT32 unChannel
@param[in] HINT32 nOffset

@retval DI_ERR_OK
@retval DI_ERR_ERROR
*/
DI_ERR_CODE DI_RF_SetModulation (DI_RF_TV_STANDARD eTvStd, HUINT32 unChannel, HINT32 nOffset);

/**
@ingroup RF
Set RF Standby

@retval DI_ERR_OK
@retval DI_ERR_ERROR
*/
DI_ERR_CODE DI_RF_SetStandby (void);

#endif
/*@}*/


