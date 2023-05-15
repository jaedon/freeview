/* $Header$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 										         */
/* All rights reserved.																	           */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmbslTDA10025.h"

#include "htype.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_tspath.h"

#include "di_channel_priv.h"
#include "di_channel_attr.h"

#include "drv_channel_t.h"
#include "drv_channel_tda10025.h"
#include "drv_channel.h"

#include "drv_channel_tda18260.h"

#include "nexus_input_band.h"
// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define TDA10025_I2CADDR	(0x1E>>1)
#define MASTER_DEMOD		(0)
#define PRINT_BUFFER_SIZE	(1024)
#define MIN_WAIT_DELAY		(30)
#define CALIBRATION_TDA18260	(5.0)
#define DBMV_TO_DBUV		(60)

#define DEBUG_SIGNAL_STATUS
//#define DEBUG_DEMOD_I2C
// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
extern HINT32 show_di_ch;
// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static HBOOL s_DemodInitialized = FALSE;
// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

#define __TDA10025_C_PRIVATE_FUNCITONS__

/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_I2CRead                                   */
/* Object              :                                                      */
/* Input Parameters    : tmUnitSelect_t tUnit                                 */
/*                          UInt32 AddrSize,                                  */
/*                          UInt8* pAddr,                                     */
/*                          UInt32 ReadLen,                                   */
/*                          UInt8* pData                                      */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t DRV_DEMOD_I2CRead(tmUnitSelect_t tUnit,    UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT8 ucSubAddr = 0;
	HUINT16 usSubAddr = 0;
	HUINT8	ucDevAddr = TDA10025_I2CADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;
	HUINT8 aucReadBuf[2] = {0,0};
#if defined(DEBUG_DEMOD_I2C)
	int i;
#endif
	CH_UNUSED(tUnit);

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 3 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_DEMOD_I2CRead()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}

	ucDevAddr = TDA10025_I2CADDR;
	ucI2CBus = I2C_CHANNEL_CHANNEL;

#if defined(DEBUG_DEMOD_I2C)
		CH_DI_Print(0, ("\n<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, ("<<< Read : Data, ReadLen[%d]\n", ReadLen));
		for(i=0; i<ReadLen; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif

	if( AddrSize == 2 )
	{
		//usSubAddr = (pAddr[0]<<8)|pAddr[1];
		usSubAddr = (pAddr[1]<<8)|pAddr[0];
		nRet = DRV_I2c_ReadA16(ucI2CBus,ucDevAddr,usSubAddr,aucReadBuf,ReadLen);
	}
	else if( AddrSize == 1 )
	{
		ucSubAddr = pAddr[0];
		nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucSubAddr, aucReadBuf, ReadLen );
	}
	else
	{

	}

	if( ReadLen == 2 )
	{
		pData[0] = aucReadBuf[1];
		pData[1] = aucReadBuf[0];
	}
	else if( ReadLen == 1 )
	{
		pData[0] = aucReadBuf[0];
	}
	else
	{
		CH_DI_Print(0, ("&&&&&&& Error : DRV_DEMOD_I2CRead()... ReadLen[%d] \n", ReadLen));
	}

	if ( nRet != 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_DEMOD_I2CRead()..ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
		return TM_ERR_READ;
	}
	else
	{
#if defined(DEBUG_DEMOD_I2C)
		CH_DI_Print(0, ("\n<<<<<<< Read : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, ("<<<<<<< Read : Data, ReadLen[%d]\n", ReadLen));
		for(i=0; i<ReadLen; i++)
		{
			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}
	return err;
}

/*---------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_I2CWrite                                 */
/* Object              :                                                     */
/* Input Parameters    : tmUnitSelect_t tUnit                                */
/*                          UInt32 AddrSize,                                 */
/*                          UInt8* pAddr,                                    */
/*                          UInt32 WriteLen,                                 */
/*                          UInt8* pData                                     */
/* Output Parameters   : tmErrorCode_t.                                      */
/*---------------------------------------------------------------------------*/
tmErrorCode_t DRV_DEMOD_I2CWrite (tmUnitSelect_t tUnit,     UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	int nRet = 0;
	HUINT8 ucSubAddr = 0;
	HUINT16 usSubAddr = 0;
	HUINT8	ucDevAddr = TDA10025_I2CADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;
	HUINT8 aucWriteBuf[4] = {0,0,0,0};
#if defined(DEBUG_DEMOD_I2C)
	int i;
#endif
	CH_UNUSED(tUnit);

	if( pAddr == NULL )
	{
		return DI_CH_PARAM_ERR;
	}
	if( pData == NULL )
	{
		return DI_CH_PARAM_ERR;
	}

	if( AddrSize >= 3 || AddrSize == 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_DEMOD_I2CWrite()... AddrSize[%d] \n", AddrSize));
		return DI_CH_PARAM_ERR;
	}

	ucDevAddr = TDA10025_I2CADDR;
	ucI2CBus = I2C_CHANNEL_CHANNEL;

#if defined(DEBUG_DEMOD_I2C)
		CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif

	if( AddrSize == 2 )
	{
		if(s_DemodInitialized == FALSE)
		{
			//usSubAddr = (pAddr[0]<<8)|pAddr[1];
			usSubAddr = (pAddr[1]<<8)|pAddr[0];
			if( WriteLen == 2 )
			{
				aucWriteBuf[0] = pData[1];
				aucWriteBuf[1] = pData[0];
				nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, usSubAddr, aucWriteBuf, WriteLen);
			}
			else if( WriteLen == 1 )
			{
				CH_DI_Print(0, (">>> DRV_DEMOD_I2CWrite()... WriteLen[%d] \n", WriteLen));
				nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, usSubAddr, pData, WriteLen);
			}
			else
			{
				CH_DI_Print(0, ("&&&&&&& Error : DRV_DEMOD_I2CWrite()... WriteLen[%d] \n", WriteLen));
			}

		}
		else
		{

#if 0//defined(DEBUG_DEMOD_I2C)
		CH_DI_Print(0, ("\n>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
			usSubAddr = (pAddr[1]<<8)|pAddr[0];
			if( WriteLen == 2 )
			{
				aucWriteBuf[0] = pData[1];
				aucWriteBuf[1] = pData[0];
				nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, usSubAddr, aucWriteBuf, WriteLen);
			}
			else if( WriteLen == 1 )
			{
				CH_DI_Print(0, (">>> DRV_DEMOD_I2CWrite()... WriteLen[%d] \n", WriteLen));
				nRet = DRV_I2c_WriteA16(ucI2CBus, ucDevAddr, usSubAddr, pData, WriteLen);
			}
			else
			{
				CH_DI_Print(0, ("&&&&&&& Error : DRV_DEMOD_I2CWrite()... WriteLen[%d] \n", WriteLen));
			}

		}
	}
	else
	{
		ucSubAddr = pAddr[0];
		nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucSubAddr, pData, WriteLen);
	}

	if ( nRet != 0 )
	{
		CH_DI_Print(0, (">>> Error : DRV_DEMOD_I2CWrite()..ucI2CBus(%d) ucDevAddr(%x) ucSubAddr(%x)\n", ucI2CBus, ucDevAddr, ucSubAddr));
	    return TM_ERR_WRITE;
	}
	else
	{
#if defined(DEBUG_DEMOD_I2C)
		CH_DI_Print(0, ("\n>>>>>>> Write : Sub-address, AddrSize[%d]\n", AddrSize));
		for(i=0; i<AddrSize; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pAddr[i]));
		}
		CH_DI_Print(0, (">>>>>>> Write : Data, WriteLen[%d]\n", WriteLen));
		for(i=0; i<WriteLen; i++)
		{

			CH_DI_Print(0, ("[%d]=[0x%x]\n", i, pData[i]));
		}
#endif
	}
   return err;

}

/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_Wait                                      */
/* Object              :                                                      */
/* Input Parameters    : tmUnitSelect_t tUnit                                 */
/*                          UInt32 tms                                        */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t DRV_DEMOD_Wait(tmUnitSelect_t tUnit, UInt32 tms)
{
	/* Variable declarations */
	tmErrorCode_t err = TM_OK;
	CH_UNUSED(tUnit);
	VK_TASK_Sleep(tms);

	return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_Print                                     */
/* Object              :                                                      */
/* Input Parameters    : UInt32 level, const char* format, ...                */
/*                                                                            */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t             DRV_DEMOD_Print(UInt32 level, const char* format, ...)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

    static char     buf[PRINT_BUFFER_SIZE] = "\0";
    va_list         ap;

	//DEBUGLVL_VERBOSE , DEBUGLVL_MAX, DEBUGLVL_TERSE
	if( level < DEBUGLVL_VERBOSE )//( level <= DEBUGLVL_VERBOSE )
	{
		va_start(ap, format);
		(void)vsnprintf(buf, PRINT_BUFFER_SIZE, format, ap);
		va_end(ap);

		DI_UART_Print(buf);
		DI_UART_Print("\n");
	}

	return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_MutexInit                                 */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle *ppMutexHandle             */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  DRV_DEMOD_MutexInit(ptmbslFrontEndMutexHandle *ppMutexHandle)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

	CH_UNUSED(ppMutexHandle);

   return err;
}


/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_MutexDeInit                               */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex                     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  DRV_DEMOD_MutexDeInit( ptmbslFrontEndMutexHandle pMutex)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

	CH_UNUSED(pMutex);

   return err;
}



/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_MutexAcquire                              */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  DRV_DEMOD_MutexAcquire(ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

	CH_UNUSED(pMutex);
	CH_UNUSED(timeOut);

   return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : DRV_DEMOD_MutexRelease                              */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex                     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  DRV_DEMOD_MutexRelease(ptmbslFrontEndMutexHandle pMutex)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

	CH_UNUSED(pMutex);

   return err;
}



static int ConvertTdaModulation( CH_CAB_Modulation_e etModulation )
{
	switch (etModulation)
	{
		case DI_CH_CMOD_AUTO:		return tmFrontEndModulationQam;//tmFrontEndModulationAuto;
		case DI_CH_CMOD_16QAM:		return tmFrontEndModulationQam16;
		case DI_CH_CMOD_32QAM:		return tmFrontEndModulationQam32;
		case DI_CH_CMOD_64QAM:		return tmFrontEndModulationQam64;
		case DI_CH_CMOD_128QAM:		return tmFrontEndModulationQam128;
		case DI_CH_CMOD_256QAM:		return tmFrontEndModulationQam256;
		default:
			CH_DI_Print(0, (" Error : ConvertTdaModulation etModulation(%d)\n", etModulation));
			return tmFrontEndModulationAuto;
	}
}



static int CheckLockedStatus( int nChannelId, HUINT32 ulWorkingId )
{
	int nRet = DI_CH_OK;
	int i;
	int nDelay = 0;
	int nRetryCount = 0;
	int nUnitId = 0;
	CH_Status_e etCurrentState = CH_STATUS_IDLE;
	tmbslFrontEndState_t etLockStatus = tmbslFrontEndStateUnknown;
	TDA10025_LockInd_t stLockInd;
	UInt32 ulGetSR = 0;
	Int32 nIFOffset = 0;
	CH_UNUSED(ulWorkingId);
	nUnitId = GetDeviceUnitId( nChannelId );

	if( 1 )
	{
		nDelay = 200;//500;//200;
		nRetryCount = 5;//30;//25;
	}
	else
	{
		nDelay = 100;
		nRetryCount = 10;
	}

	CH_DI_Print(3, ("nDelay [%d]:[%d] nRetryCount\n", nDelay, nRetryCount));

	VK_TASK_Sleep(MIN_WAIT_DELAY);

	for( i=0; i<nRetryCount; i++ )
	{
		etCurrentState =	GetTuneStatus(nChannelId);
		if( ( etCurrentState == CH_STATUS_IDLE) || (etCurrentState == CH_STATUS_CANCLE) )
		{
			CH_DI_Print(1, ("Cancel locking status...\n"));
			return DI_CH_CANCEL_TUNE;
		}
#if 1
		nRet = tmbslTDA10025_GetLockInd(nUnitId, &stLockInd);
		if(nRet != DI_CH_OK)
		{
			CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetLockInd()... nRet[%d] \n", nRet));
		}
		else
		{
			CH_DI_Print(6, ("&&&&&&&&&&& tmbslTDA10025_GetLockInd(%d) : START &&&&&&&&&&&\n", nUnitId));
			CH_DI_Print(6, (" stLockInd.bCAGCSat        (%d)\n", stLockInd.bCAGCSat));
			CH_DI_Print(6, (" stLockInd.bDAGCSat        (%d)\n", stLockInd.bDAGCSat));
			CH_DI_Print(6, (" stLockInd.bAagc           (%d)\n", stLockInd.bAagc));
			CH_DI_Print(6, (" stLockInd.bStl            (%d)\n", stLockInd.bStl));
			CH_DI_Print(6, (" stLockInd.bCtl            (%d)\n", stLockInd.bCtl));
			CH_DI_Print(6, (" stLockInd.bSctl           (%d)\n", stLockInd.bSctl));
			CH_DI_Print(6, (" stLockInd.bDemod          (%d)\n", stLockInd.bDemod));
			CH_DI_Print(6, (" stLockInd.bFEC            (%d)\n", stLockInd.bFEC));
			CH_DI_Print(6, (" stLockInd.uDemodLockTime  (%d)\n", stLockInd.uDemodLockTime));
			CH_DI_Print(6, (" stLockInd.uFecLockTime    (%d)\n", stLockInd.uFecLockTime));
			CH_DI_Print(6, ("&&&&&&&&&&& tmbslTDA10025_GetLockInd(%d) : END   &&&&&&&&&&&\n", nUnitId));

			nRet = tmbslTDA10025_GetSR( nUnitId,&ulGetSR);
			if(nRet != DI_CH_OK)
			{
				CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetSR()... nRet[%d] \n", nRet));
			}
			else
			{
				CH_DI_Print(6, ("&&&&&&&&&&& tmbslTDA10025_GetSR() : ulGetSR(%d) \n", ulGetSR));
			}

#if 1
	nRet = tmbslTDA10025_GetIFOffset(nUnitId, &nIFOffset);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetIFOffset()... nRet[%d] \n", nRet));
	}
	else
	{
		CH_DI_Print(6, ("&&&&&&&&&&& tmbslTDA10025_GetIFOffset() : nIFOffset(%d) \n", nIFOffset));
	}
#endif

		}
#endif
		nRet = tmbslTDA10025_GetLockStatus(nUnitId, &etLockStatus);
		if(nRet != DI_CH_OK)
		{
			CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetLockStatus()... nRet[%d] \n", nRet));
		}

		CH_DI_Print(6, ("nChannelId[%d] etLockStatus : %d [%d]\n", nChannelId, etLockStatus, i));

		if( etLockStatus == tmbslFrontEndStateLocked )
		{
		#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
			g_ulLowLevelLockedTime[nChannelId] = VK_TIMER_GetSystemTick();
			CH_DI_Print(0, ("[CheckTuneState] [%d] Lock Time( %d )ms  \n" ,nChannelId, g_ulLowLevelLockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
		#endif
			CH_DI_Print(1, (">>>>>>>>>>>>>>>>>ACQ_LOCKED_AND_TRACKING [%d ms] \n", (i * nDelay)));
			return DI_CH_OK;
		}
		else
		{
			VK_TASK_Sleep(nDelay);
		}
	}

#ifdef CH_PRINT_LOW_LEVEL_ZAPPING_TIME
	g_ulLowLevelUnlockedTime[nChannelId] = VK_TIMER_GetSystemTick();
	CH_DI_Print(0, ("[DB7335_CheckTuneState] [%d] UnLock Time( %d )ms  \n", nChannelId, g_ulLowLevelUnlockedTime[nChannelId] - g_ulRequestTune[nChannelId] ));
#endif

	return DI_CH_ERR;

}



void ResetHwTda10025( int nUintId )
{

	if (nUintId == 0)
	{
		DRV_GPIO_Write(GPIO_ID_RST_QAM, 1);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_RST_QAM, 0);

		VK_TASK_Sleep(20);

		/* CAM ReSeT Signal INACTIVE */
		DRV_GPIO_Write(GPIO_ID_RST_QAM, 1);
		CH_DI_Print(3, (" GPIO_ID_RST_QAM reset !!\n"));
	}
}


int InitTda10025(int nUnitId)
{
	tmErrorCode_t               err = TM_OK;
	TDA10025_AdditionnalData_t sTDA10025AdditionnalData;
	/* First TDA10025 Instance 0 setup */
	tmbslFrontEndDependency_t sSrvFunc;

	CH_DI_Print(3, ("+++ InitTda10025()...nUnitId(%d)\n",nUnitId));

	sSrvFunc.sIo.Write = DRV_DEMOD_I2CWrite;
	sSrvFunc.sIo.Read = DRV_DEMOD_I2CRead;
	sSrvFunc.sTime.Get = Null;
	sSrvFunc.sTime.Wait = DRV_DEMOD_Wait;
	sSrvFunc.sDebug.Print = DRV_DEMOD_Print;
	sSrvFunc.sMutex.Init = DRV_DEMOD_MutexInit;
	sSrvFunc.sMutex.DeInit = DRV_DEMOD_MutexDeInit;
	sSrvFunc.sMutex.Acquire = DRV_DEMOD_MutexAcquire;
	sSrvFunc.sMutex.Release = DRV_DEMOD_MutexRelease;
	sSrvFunc.dwAdditionalDataSize = sizeof(TDA10025_AdditionnalData_t);
	sSrvFunc.pAdditionalData = (void*)&sTDA10025AdditionnalData;

	if( nUnitId == MASTER_DEMOD )
	{
		/* Initialise Additional data for instance 0 and IPA */
		sTDA10025AdditionnalData.tUnitCommon = 0;
		sTDA10025AdditionnalData.eChannelSel = TDA10025_ChannelSelectionIpA;
	}
	else
	{
		/* Initialise Additional data for instance 1 and IPB */
		sTDA10025AdditionnalData.tUnitCommon = 0;
		sTDA10025AdditionnalData.eChannelSel = TDA10025_ChannelSelectionIpB;
	}
	/* Open 0 */
	err = tmbslTDA10025_Open(nUnitId, &sSrvFunc);
	if(err != TM_OK)
		return err;


	/* TDA10025 Hardware initialization */
	err = tmbslTDA10025_HwInit(nUnitId);
	if(err != TM_OK)
		return err;

	if(nUnitId == 1)
	{
		s_DemodInitialized = TRUE;
	}
	CH_DI_Print(3, ("--- InitTda10025()...nUnitId(%d)\n",nUnitId));

	return err;
}

#define __TDA10025_C_PUBLIC_FUNCITONS__



void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &TDA10025_InitDevice;
	CAB_SetTune 			= &TDA10025_SetTune;
	//CAB_CheckTuneState 		= &TDA10025_CheckTuneState;
	CAB_CheckLock 			= &TDA10025_CheckLock;
	CAB_GetStrengthQuality 	= &TDA10025_GetStrengthQuality;
	CAB_SetPowerState 		= &TDA10025_SetPowerState;
	CAB_EnableTsOutput 		= &TDA10025_EnableTsOutput;
	CAB_DisableTsOutput 	= &TDA10025_DisableTsOutput;
	//CAB_UpdateTunedParam	= &TDA10025_UpdateTunedParam;
	//CAB_GetChannelStatus	= &TDA10025_GetChannelStatus;
	CAB_ResetGpio 			= &TDA10025_ResetGpio;

	return;
}



int TDA10025_InitDevice( int nChannelId )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	ResetHwTda10025(nUnitId);

	nRet = InitTda10025(nUnitId);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : InitTda10025()... nRet[%x] \n", nRet));
	}

	//tmbslTDA10025_SetBERWindow(tmUnitSelect_t tUnit,TDA10025_BERWindow_t eBERWindow); //TODO_ILEE

	nRet = TDA18260_Initialize(nUnitId);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_Initialize()... nRet[%x] \n", nRet));
	}
	return nRet;
}



int TDA10025_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	HUINT32 ulFreqHz = 0;
	HUINT32 ulIfFreqHz = 5000000;
	HUINT32 ulSymbolRate = 0;
	tmFrontEndModulation_t etModulationType = 0;
	//UInt32 ulGetSR = 0;
	//int nIFOffset = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	ulFreqHz = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;

	/* Set Tuner PLL lock */
#if 0
	if (nUnitId == 0)
	{
		nRet = TDA18260_SetTune(1, ulFreqHz, 8);
	}
	else
	{
		nRet = TDA18260_SetTune(0, ulFreqHz, 8);
	}
#else
	nRet = TDA18260_SetTune(nUnitId, ulFreqHz, 8);
#endif
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_SetTune()... nRet[%d] \n", nRet));
	}
#if 0
	nRet = TDA18260_SetTune(nUnitId+1, ulFreqHz, 8);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : TDA18260_SetTune()... nRet[%d] \n", nRet));
	}
#endif
	VK_TASK_Sleep(500);

#if 0
	/* TDA10025 Hardware initialization */
	nRet = tmbslTDA10025_HwInit(nUnitId);
	nRet = tmbslTDA10025_SetPowerState(nUnitId, tmPowerOn);
#endif

	/* Set channel FEC mode with IF from Tuner */
	nRet = tmbslTDA10025_SetFECMode(nUnitId, tmFrontEndFECModeAnnexA);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetFECMode()... nRet[%d] \n", nRet));
	}

	/* Set channel decoder IF with IF from Tuner */
	//ulIfFreqHz = TDA18260_GetIfFreq(1);
	ulIfFreqHz = 5000000;

	CH_DI_Print(3, ("&&&&&&&&&&& TDA18260_SetTune() : ulIfFreqHz(%d) \n", ulIfFreqHz));

	nRet = tmbslTDA10025_SetIF(nUnitId, ulIfFreqHz);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetIF()... nRet[%d] \n", nRet));
	}
#if 0
	nRet = tmbslTDA10025_GetIFOffset(nUnitId, &nIFOffset);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetIFOffset()... nRet[%d] \n", nRet));
	}
	else
	{
		CH_DI_Print(0, ("&&&&&&&&&&& tmbslTDA10025_GetIFOffset() : nIFOffset(%d) \n", nIFOffset));
	}
#endif
	/* Set channel decoder Spectrum Inversion */
	nRet = tmbslTDA10025_SetSI(nUnitId, tmFrontEndSpecInvAuto);//tmFrontEndSpecInvOff, tmFrontEndSpecInvOn, tmFrontEndSpecInvAuto
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetSI()... nRet[%d] \n", nRet));
	}

	/* Set channel decoder Modulation */
	etModulationType = ConvertTdaModulation(pstCabTuneParam->etModulation);//TODO_ILEE : TDA10025_ERR_NOT_SUPPORTED for tmFrontEndModulationAuto
	//etModulationType = tmFrontEndModulationQam64;

	nRet = tmbslTDA10025_SetMod(nUnitId, etModulationType);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetMod()... nRet[%d] \n", nRet));
	}

	/* Set channel decoder Symbol Rate */
	ulSymbolRate = pstCabTuneParam->ulSymbolRate*KHZ_TO_HZ;// /KHZ_TO_HZ;//*KHZ_TO_HZ;

	CH_DI_Print(3, ("&&&&&&&&&&& tmbslTDA10025_SetSR() : ulSymbolRate(%d) \n", ulSymbolRate));

	nRet = tmbslTDA10025_SetSR(nUnitId, ulSymbolRate);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetSR()... nRet[%d] \n", nRet));
	}
#if 0
	nRet = tmbslTDA10025_GetSR( nUnitId,&ulGetSR);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetSR()... nRet[%d] \n", nRet));
	}
	else
	{
		CH_DI_Print(0, ("&&&&&&&&&&& tmbslTDA10025_GetSR() : ulGetSR(%d) \n", ulGetSR));
	}
#endif
	/* Start Lock acquisition */
	nRet = tmbslTDA10025_StartLock(nUnitId);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_StartLock()... nRet[%d] \n", nRet));
	}

	/* Wait for channel decoder lock procedure to complete or timeout after 5s */
	nRet = CheckLockedStatus( nChannelId, ulWorkingId );

	return nRet;
}



HBOOL TDA10025_CheckLock( int nChannelId)
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	HBOOL bLocked = FALSE;
	tmbslFrontEndState_t etLockStatus = tmbslFrontEndStateUnknown;
#if defined(DEBUG_SIGNAL_STATUS)
	DI_CH_SignalQuality_t stSignalInfo;
#endif

	nUnitId = GetDeviceUnitId( nChannelId );

	nRet = tmbslTDA10025_GetLockStatus(nUnitId, &etLockStatus);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_GetLockStatus()... nRet[%d] \n", nRet));
	}
	else
	{
		CH_DI_Print(6, ("TDA10025_CheckLock[%d] etLockStatus =(%d)\n", nUnitId, etLockStatus));
	}

	if( etLockStatus == tmbslFrontEndStateLocked )
	{
		bLocked = TRUE;
#if defined(DEBUG_SIGNAL_STATUS)
		TDA10025_GetStrengthQuality( nChannelId, &stSignalInfo);//test only
#endif
	}

	return bLocked;
}



int TDA10025_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	float fPower = 0.0;
	tmUnitSelect_t tUnit = 0;
	TDA10025_BERWindow_t eBERWindow = 0;
	tmFrontEndFracNb32_t stBER;
	UInt32 ulUncors = 0;
	UInt32 ulSignalQuality = 0;
	tmFrontEndFracNb32_t stEsno;
	float fBer = 0.0, fInversBer = 0.0;
	float fBerFactor = 0.0;

	nUnitId = GetDeviceUnitId( nChannelId );
	tUnit = (tmUnitSelect_t)nUnitId;

	TDA18260_GetSignalStrength( nUnitId,&fPower );
	TDA18260_GetAgcGain( nUnitId);

	//tmbslTDA10025_ClearUncor(tUnit);
	tmbslTDA10025_SetBERWindow( tUnit,TDA10025_BERWindow1E7);//TDA10025_BERWindow1E5);

	tmbslTDA10025_GetBERWindow(tUnit, &eBERWindow);
	tmbslTDA10025_GetBER(tUnit, &stBER, &ulUncors);
	tmbslTDA10025_GetSignalQuality(tUnit,&ulSignalQuality);
	tmbslTDA10025_GetChannelEsno(tUnit, &stEsno);

	if(stBER.lInteger == 0)
	{
		fBer = 0.0;
		fInversBer = 0.0;
	}
	else
	{
		if(eBERWindow == TDA10025_BERWindow1E5)
		{
			fBerFactor = 1.0e-5;
		}
		else if(eBERWindow == TDA10025_BERWindow1E6)
		{
			fBerFactor = 1.0e-6;
		}
		else if(eBERWindow == TDA10025_BERWindow1E7)
		{
			fBerFactor = 1.0e-7;
		}
		else if(eBERWindow == TDA10025_BERWindow1E8)
		{
			fBerFactor = 1.0e-8;
		}
		else
		{
			CH_DI_Print(0, ("Error : eBERWindow [%d] \n", eBERWindow));
		}

		//fInversBer = ((float)stBER.uDivider/(float)stBER.lInteger);//*fBerFactor;
		fBer = ((float)stBER.lInteger/(float)stBER.uDivider);//*fBerFactor;

	}

	CH_DI_Print(3, ("=================================================================\n", nRet));
	CH_DI_Print(3, ("           TDA10025_GetStrengthQuality. nUnitId [%d] \n", nUnitId));
	CH_DI_Print(3, ("=================================================================\n", nRet));
	CH_DI_Print(3, (" fPower(%f)dBuV \n", fPower+CALIBRATION_TDA18260+DBMV_TO_DBUV));
	CH_DI_Print(3, (" eBERWindow(%d) : 1=E5,2=E6,3=E7,4=E8 \n", eBERWindow));
	CH_DI_Print(3, (" ulUncors(%d) \n", ulUncors));
	CH_DI_Print(3, (" stBER.lInteger(%d):(%d)uDivider \n", stBER.lInteger, stBER.uDivider));
	CH_DI_Print(3, (" fBer (%3.2e) \n", fBer));
	//CH_DI_Print(0, (" fInversBer (%3.2e) \n", fInversBer));
	CH_DI_Print(3, (" ulSignalQuality(%d) \n", ulSignalQuality));
	CH_DI_Print(3, (" stEsno.lInteger(%d):(%d)uDivider \n", stEsno.lInteger, stEsno.uDivider));
	CH_DI_Print(3, (" Esno(%f) \n", (float)(stEsno.lInteger/(int)stEsno.uDivider)));
	CH_DI_Print(3, ("=================================================================\n", nRet));

	fPower += CALIBRATION_TDA18260+DBMV_TO_DBUV;

	pstSignalInfo->ulStrength = ((fPower/75*100)>100)?100:(fPower/75*100);
	pstSignalInfo->ulQuality = (HUINT32)ulSignalQuality;
	//pstSignalInfo->ulAgc;
	pstSignalInfo->fBer = fBer;
	pstSignalInfo->fSnr = (float)(stEsno.lInteger/(int)stEsno.uDivider);
	//pstSignalInfo->bEmergencyflag;
	pstSignalInfo->ulCorrectedNo = 0;
	pstSignalInfo->ulUnCorrectedNo = (HUINT32)ulUncors;
	pstSignalInfo->ulUnErroredNo = 0;
	pstSignalInfo->ulInbandBer = fBer;
	//pstSignalInfo->ulUnCorrectedFlag;
	//pstSignalInfo->ulRfAgc;
	//pstSignalInfo->ulIfAgc;
	pstSignalInfo->fSignalInputPower = fPower;
	return nRet;
}


#if 0
    typedef enum tmPowerState
    {
        tmPowerOn = 0,                      /* Device powered on      (D0 state) */
        tmPowerStandby,                     /* Device power standby   (D1 state) */
        tmPowerSuspend,                     /* Device power suspended (D2 state) */
        tmPowerOff,                         /* Device powered off     (D3 state) */
        tmPowerMax                          /* Device power max */
    }   tmPowerState_t, *ptmPowerState_t;
#endif

int TDA10025_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	tmPowerState_t etPowerState;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(3, (">>> TDA10025_SetPowerState()... nUnitId[%d]:[%d]etPower \n", nUnitId, etPower));

	if(etPower == CH_POWER_ON)
	{
		etPowerState = tmPowerOn;
	}
	else
	{
		//etPowerState = tmPowerStandby;
		etPowerState = tmPowerOn;//tmPowerOff;
	}

	nRet = tmbslTDA10025_SetPowerState(nUnitId, etPowerState);
	//nRet = tmbslTDA10025_SetPowerState(nUnitId+1, etPowerState);
	if(nRet != DI_CH_OK)
	{
		CH_DI_Print(0, (">>> Error : tmbslTDA10025_SetPowerState()... nRet[%d] \n", nRet));
	}

	return nRet;
}



int TDA10025_EnableTsOutput( int nChannelId )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	NEXUS_InputBand		inband;
	NEXUS_InputBandSettings inputBandSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	inband = DRV_TS_PATH_GetInbandID(nUnitId);
	NEXUS_InputBand_GetSettings(inband, &inputBandSettings);
	inputBandSettings.validActiveHigh = true;
	NEXUS_InputBand_SetSettings(inband, &inputBandSettings);

	return nRet;
}



int TDA10025_DisableTsOutput( int nChannelId )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;
	NEXUS_InputBand		inband;
	NEXUS_InputBandSettings inputBandSettings;

	nUnitId = GetDeviceUnitId( nChannelId );

	inband = DRV_TS_PATH_GetInbandID(nUnitId);
	NEXUS_InputBand_GetSettings(inband, &inputBandSettings);
	inputBandSettings.validActiveHigh = false;
	NEXUS_InputBand_SetSettings(inband, &inputBandSettings);

	return nRet;
}



int TDA10025_ResetGpio( int nChannelId )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	return nRet;
}



int TDA10025_Initialize( int nChannelId )
{
	int nRet = DI_CH_OK;
	int	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	return nRet;
}




/*********************** End of File ******************************/
