/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	template.h
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	_DRV_CHANNEL_H_
#define	_DRV_CHANNEL_H_

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "di_channel.h"
#include "drv_channel_a.h"
#if defined(CONFIG_FRONTEND_TER)
#include "drv_channel_t.h"
#endif
#if defined(CONFIG_FRONTEND_SAT)
#include "drv_channel_s.h"
#endif
#if defined(CONFIG_FRONTEND_CAB)
#include "drv_channel_c.h"
#endif
// End Including Headers


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition
#define I2C_SPEED_400k	400000
#define I2C_SPEED_100k	100000

#if defined(CONFIG_PLATFORM_TM241SF)
#define CONST_CH_Q_TIMEOUT		900	 	/* 900 ms for correct BER calculation */
#else
#define CONST_CH_Q_TIMEOUT		2000 	/*2 sec */
#endif

//#define CH_TAGGED_API //to check dead lock

#if defined(MODEL_LF7700)

#define DI_CH_A_TEST
#define CH_A_TIMEOUT_SYNC_LOCK_INPUT	1100 	/*1100 mS */
#define CH_A_TIMEOUT_SYNC_LOCK_SEARCH	280 	/*280 mS */
#define CH_A_TIMEOUT_SYNC_LOCK_SECAM	800 	/*800 mS */
#define CH_A_TIMEOUT_SYNC_LOCK			800		/*800 mS */
#define CH_A_TIMEOUT_AFT_LOCK			350 	/*350 mS */

#else
#define CH_A_TIMEOUT_SYNC_LOCK_INPUT	500 	/*800 mS */
#define CH_A_TIMEOUT_SYNC_LOCK			300 	/*300 mS */
#define CH_A_TIMEOUT_SYNC_LOCK_SECAM	400 	/*400 mS */
#define CH_A_TIMEOUT_AFT_LOCK			200 	/*200 mS */

#endif

#define CH_A_TIMEOUT_IDLE   			500 	/* the min request time is 375mS after aft tune */ //1000	/*1 sec  */

// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef enum
{
	CH_GPIO_LOW,
	CH_GPIO_HIGH
} CH_GpioStatus_e;

//FQD1116AME IF selection for HC1 model ( TER : Low, CAB : Direct )
typedef enum
{
	DIRECT_IF,
	LOW_IF,
	BANDWIDTH_7M_IF = 7,
	BANDWIDTH_8M_IF = 8
} CH_SelectIf_e;

// End typedef

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
void DRV_CH_CallNotifyCallback ( int nChannelID, HUINT32 unRequestId, DI_CH_SignalInfo_e etSigalInfo );

#if !defined(CH_TAGGED_API)
void DRV_CH_Lock( int nChannelId );
void DRV_CH_UnLock( int nChannelId );
#else
void DRV_CH_LockTagged( int nChannelId, const char *pcFilename, int nLineNum );
#define DRV_CH_Lock(nChannelId) DRV_CH_LockTagged(nChannelId, __FILE__, __LINE__)

void DRV_CH_UnLockTagged( int nChannelId, const char *pcFilename, int nLineNum );
#define DRV_CH_UnLock(nChannelId) DRV_CH_UnLockTagged(nChannelId, __FILE__, __LINE__)
#endif

void DRV_CH_LockI2c( int nChannelId );
void DRV_CH_UnLockI2c( int nChannelId );
void DRV_CH_LockReentrant( void );
void DRV_CH_UnLockReentrant( void );
int DRV_CH_CheckTsPath ( int nChannelId );
void DRV_CH_CheckStatus( void );
void DRV_CH_ConfigTunerPll( int nChannelId, HUINT32 ulFreqHz, HUINT8 FreqOffset, HUINT8 ucBW );

int DRV_CH_GetDeviceStauts(int nChannelId, DI_CH_DeviceStatus * pstDeviceStatus);

// End global function prototypes

#endif /* !_DRV_CHANNEL_H_ */
