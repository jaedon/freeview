/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	drv_channel_mxl241sf.h
// Original Author: Jae-Deok, Cho
// File Description: Maxlinear MXL241SF Driver
// Remarks:
//	2011/04/28 - Initial Version

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_CHANNEL_MXL241SF_H__
#define	__DRV_CHANNEL_MXL241SF_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "di_channel.h"
// End Including Headers


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition
#define		MASTER_MXL241SF				0
#define		SLAVE_MXL241SF				1
#define		MASTER_MXL241SF_I2C_ADDR	((0xC0)>>1)
#define		SLAVE_MXL241SF_I2C_ADDR		((0xC6)>>1)

#define		TUNER_LOCK					1
#define		TUNER_UNLOCK				0

#define		DEMOD_LOCK					1
#define		DEMOD_UNLOCK				0
// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef struct
{
	HUINT32		ulStrength;
	HUINT32		ulQauality;
	HFLOAT32	fSnr;
	HUINT64		ulBer;
	HUINT64		ulUncodedBer;
	HUINT64		ulCer;
	HUINT64		ulCorrected;
	HUINT64		ulUncorrected;
	HUINT64		ulUnerrored;
	HFLOAT32	fPowerLevel;
}MxL241SF_SignalQuality_t;

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
int MXL241SF_Initialize(int nChannelId);
int MXL241SF_InitDevice( int nChannelId );
int MXL241SF_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );
int MXL241SF_GetStatus(int nChannelId, HUINT32 frequency);

HBOOL MXL241SF_CheckLock( int nChannelId );
int MXL241SF_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int MXL241SF_SetPowerState( int nChannelId, CH_Power_e etPower );
int MXL241SF_EnableTsOutput( int nChannelId );
int MXL241SF_DisableTsOutput( int nChannelId );
int MXL241SF_UpdateTunedParam( int nChannelId, CH_CAB_TuneParam_t * pstTuneParam );
int MXL241SF_ResetGpio(int nChannelId);
void MXL241SF_DumpRegister( int nChannelId);

void MXL241SF_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc);
void MXL241SF_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws);
int MXL241SF_CheckTsmfLock(int nChannelId, HUINT32 frequency);
void MXL241SF_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID);
void MXL241SF_QAMStop(int nChannelId);


// End global function prototypes

#endif /* !__DRV_CHANNEL_MXL241SF_H__ */
