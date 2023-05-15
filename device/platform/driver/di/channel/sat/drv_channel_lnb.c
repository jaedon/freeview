// File Name	     :		drv_channel_lnb.c
// Original Author: 	Kim Sung Yong
// File Description:
// Module:				DDI
// Remarks:				Jinhong Kim, modified

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files ****************************/
/*******************************************************************/
/* include  common utils */
#include "vkernel.h"

/* include  drv headers */
#include "drv_i2c.h"
#include "drv_channel_lnb.h"
#include "drv_channel_s.h"

#include "di_channel_attr.h"

/* include  di headers */
#include "di_channel_priv.h"

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
#include "di_system.h"
#include "drv_channel_la8297.h"
#include "drv_channel_ldt1803.h"
#elif defined (CONFIG_PLATFORM_LA8286)
#include "drv_channel_la8286.h"
#elif defined (CONFIG_PLATFORM_LA8296)
#include "drv_channel_la8296.h"
#elif defined (CONFIG_PLATFORM_LA8297)
#include "drv_channel_la8297.h"
#elif defined (CONFIG_PLATFORM_LA8305)
#include "drv_channel_la8305.h"
#elif defined (CONFIG_PLATFORM_LA8304)
#include "drv_channel_la8304.h"
#elif defined (CONFIG_PLATFORM_LDT1803)
#include "drv_channel_ldt1803.h"
#endif


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/




/********************************************************************************
 *							Grobal & Extern  variables									*
 ********************************************************************************/


/********************************************************************************
 *							Static variables									*
 ********************************************************************************/
static HBOOL s_bLnbOutput[CONFIG_NUM_OF_SAT_CHANNEL] = {TRUE,};
extern HINT32 show_di_ch;

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
HBOOL g_A8297_NOT_DT1803_ = TRUE; 
#endif


/********************************************************************************
 *							Global functions 					                         				*
 ********************************************************************************/

int DRV_CH_LNB_Init(int nUnitId)
{
	int nRet = DI_CH_OK;
	int nRfInputId = 0;
#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	HUINT8 revision=0;
#endif
	
	nRfInputId = GetRfInputId(nUnitId);

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	nRet = DI_SYSTEM_GetSystemInfo (DI_SYS_HW_REVISION, &revision, 1);
		CH_DI_Print(0, ( "\n JAMES....revision = %x\n", revision));
	if ( (revision & 0x1c) == 0x0c)
	{
		nRet = LDT1803_Init( nRfInputId);
		g_A8297_NOT_DT1803_ = FALSE;
	}
	else
	{
		nRet = LA8297_Init( nRfInputId);
		g_A8297_NOT_DT1803_ = TRUE;
	}
	
#elif defined(CONFIG_PLATFORM_LA8286)
	nRet = LA8286_Init( nRfInputId);
#elif defined (CONFIG_PLATFORM_LA8297)
	nRet = LA8297_Init( nRfInputId);
#elif defined (CONFIG_PLATFORM_LA8305)
	nRet = LA8305_Init( nRfInputId);
#elif defined (CONFIG_PLATFORM_LA8304)
	nRet = LA8304_Init( nRfInputId);	
#elif defined (CONFIG_PLATFORM_LDT1803)
	nRet = LDT1803_Init( nRfInputId);
#else
	nRet = LA8296_Init( nRfInputId);
#endif
	return nRet;
}


HBOOL DRV_CH_LNB_GetOutput(int nUnitId)
{
	return s_bLnbOutput[nUnitId];
}



int DRV_CH_LNB_SetOutput(int nUnitId, HBOOL bEnable)
{
	int nRet = DI_CH_OK;
	int nRfInputId = 0;

	nRfInputId = GetRfInputId(nUnitId);

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	if (g_A8297_NOT_DT1803_ == TRUE)
		nRet = LA8297_SetOutput( nRfInputId, bEnable );
	else
		nRet = LDT1803_SetOutput( nRfInputId, bEnable );
	
#elif defined(CONFIG_PLATFORM_LA8286)
	nRet = LA8286_SetOutput( nRfInputId, bEnable );
#elif defined (CONFIG_PLATFORM_LA8297)
	nRet = LA8297_SetOutput( nRfInputId, bEnable );
#elif defined (CONFIG_PLATFORM_LA8305)
	nRet = LA8305_SetOutput( nRfInputId, bEnable );
#elif defined (CONFIG_PLATFORM_LA8304)
	nRet = LA8304_SetOutput( nRfInputId, bEnable );	
#elif defined (CONFIG_PLATFORM_LDT1803)
	nRet = LDT1803_SetOutput( nRfInputId, bEnable );
#else
	nRet = LA8296_SetOutput( nRfInputId, bEnable );
#endif

	s_bLnbOutput[nUnitId] = bEnable;

	return nRet;
}



int DRV_CH_LNB_SetPolar(int  nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage)
{
	int nRet = DI_CH_OK;
	int nRfInputId = 0;

	nRfInputId = GetRfInputId(nUnitId);

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	if (g_A8297_NOT_DT1803_ == TRUE)
		nRet = LA8297_SetPolar( nRfInputId, etPolar, etVoltage);
	else
		nRet = LDT1803_SetPolar( nRfInputId, etPolar, etVoltage);
	
#elif defined(CONFIG_PLATFORM_LA8286)
	nRet = LA8286_SetPolar( nRfInputId, etPolar, etVoltage);
#elif defined (CONFIG_PLATFORM_LA8297)
	nRet = LA8297_SetPolar( nRfInputId, etPolar, etVoltage);
#elif defined (CONFIG_PLATFORM_LA8305)
	nRet = LA8305_SetPolar( nRfInputId, etPolar, etVoltage);
#elif defined (CONFIG_PLATFORM_LA8304)
	nRet = LA8304_SetPolar( nRfInputId, etPolar, etVoltage);	
#elif defined (CONFIG_PLATFORM_LDT1803)
	nRet = LDT1803_SetPolar( nRfInputId, etPolar, etVoltage);
#else
	nRet = LA8296_SetPolar( nRfInputId, etPolar, etVoltage);
#endif
	return nRet;
}



int DRV_CH_LNB_CheckAntState( int nUnitId )
{
	int nRet = DI_CH_OK;
	int nRfInputId = 0;

	nRfInputId = GetRfInputId(nUnitId);

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	if (g_A8297_NOT_DT1803_ == TRUE)
		nRet = LA8297_CheckAntState( nRfInputId );
	else
		nRet = LDT1803_CheckAntState( nRfInputId );
	
#elif defined(CONFIG_PLATFORM_LA8286)
	nRet = LA8286_CheckAntState( nRfInputId );
#elif defined (CONFIG_PLATFORM_LA8297)
	nRet = LA8297_CheckAntState( nRfInputId );
#elif defined (CONFIG_PLATFORM_LA8305)
	nRet = LA8305_CheckAntState( nRfInputId );	
#elif defined (CONFIG_PLATFORM_LA8304)
	nRet = LA8304_CheckAntState( nRfInputId );
#elif defined (CONFIG_PLATFORM_LDT1803)
	nRet = LDT1803_CheckAntState( nRfInputId );
#else
	nRet = LA8296_CheckAntState( nRfInputId );
#endif
	return nRet;
}



int DRV_CH_LNB_ReadStatus(
		int	nUnitId,
		HUINT8 *pucData
		)
{
	int nRet = DI_CH_OK;
	int nRfInputId = 0;

	nRfInputId = GetRfInputId(nUnitId);

#if defined (CONFIG_PLATFORM_LA8297) && defined (CONFIG_PLATFORM_LDT1803)
	if (g_A8297_NOT_DT1803_ == TRUE)
		nRet = LA8297_ReadStatus( nRfInputId, pucData);
	else
		nRet = LDT1803_ReadStatus( nRfInputId, pucData);
	
#elif defined(CONFIG_PLATFORM_LA8286)
	nRet = LA8286_ReadStatus( nRfInputId, pucData);
#elif defined (CONFIG_PLATFORM_LA8297)
	nRet = LA8297_ReadStatus( nRfInputId, pucData);
#elif defined (CONFIG_PLATFORM_LA8305)
	nRet = LA8305_ReadStatus( nRfInputId, pucData);
#elif defined (CONFIG_PLATFORM_LA8304)
	nRet = LA8304_ReadStatus( nRfInputId, pucData);	
#elif defined (CONFIG_PLATFORM_LDT1803)
	nRet = LDT1803_ReadStatus( nRfInputId, pucData);
#else
	nRet = LA8296_ReadStatus( nRfInputId, pucData);
#endif
	return nRet;
}
/* EOF */

