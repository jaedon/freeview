/**
	@file     	xmgr_recorder_ertelecom.c
	@brief    	recording control application for SES.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_OP_ERTELECOM)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_svc.h>

#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_rec.h>
#include <svc_fs.h>

#include <bus.h>

#include <mgr_recorder.h>
#include <_xmgr_recorder.h>

#include <uapi.h>

#include "../base/local_include/_xmgr_recorder_base.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HERROR xmgr_recorder_GetTsrLimitForUsbFlash_Ertel (MgrRec_TsrLimitForUsbFlash_t *pstUsbFlash)
{
	if (NULL == pstUsbFlash)
		return ERR_FAIL;

	if (TRUE == pstUsbFlash->bIsUsbFlashRecord)
	{
		HUINT64 llFreeSpace = 0;
		HUINT64 unTsrSoftLimitTimeSecond;
		HUINT32	ulLimitTimeSecond = 0;

		//	-	Broadcom TS file 생성시 atom size
		const HINT32 USB_FLASH_TSR_DATA_BIT_RATE = ((HUINT64)13944*192*8); /* 20mpbs stream :  (20*1024*1024)/8/188 = 13943.8 = 13944 TS packet (->188byte + 4byte) */

		//	-	Index file 생성시 atom size
		const HINT32 USB_FLASH_TSR_INDEX_BIT_RATE = ((HUINT64)186414); /* ((160*1024*1024) / 7200) * 8 */
#if 0
		{
			HERROR hErr;
			SvcFs_DevIdx_e eDevIdx = (SvcFs_DevIdx_e)SVC_FS_GetDefaultPvrIdx();
			hErr = UAPI_FS_GetPvrReservedSize(eDevIdx, &llFreeSpace);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("UAPI_FS_GetSize : Error\n");
				return hErr;
			}

			// kb to b
			llFreeSpace *= SIZE_1K;
		}
#endif
		{
			HERROR hErr;
			SvcFs_DevIdx_e eDevIdx = (SvcFs_DevIdx_e)SVC_FS_GetDefaultPvrIdx();
			hErr = SVC_FS_GetPvrReservedSize(eDevIdx, &llFreeSpace);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("Device(%d) is not available for PVR\n", eDevIdx);
				return hErr;
			}
            // kb to b
			llFreeSpace *= SIZE_1K;
		}

		//	-	전체 Time / Data / Index 연산 방법
		//llFreeSpace = xxx; /*total xxx bytes for TSR */
		ulLimitTimeSecond = (HUINT32)((llFreeSpace*8) / (USB_FLASH_TSR_DATA_BIT_RATE + USB_FLASH_TSR_INDEX_BIT_RATE));

		pstUsbFlash->ulLimitTimeSecond	= (DEFAULT_DURATION_TSR_RECORD<ulLimitTimeSecond)?DEFAULT_DURATION_TSR_RECORD:ulLimitTimeSecond;

		//Hard Limitation은 위 값으로 Soft Limitation은 Hard Limitation을 기준으로 10% margin
		unTsrSoftLimitTimeSecond = pstUsbFlash->ulLimitTimeSecond + (pstUsbFlash->ulLimitTimeSecond / 10);

		pstUsbFlash->ullLimitDataSize	= ((HUINT64)unTsrSoftLimitTimeSecond * USB_FLASH_TSR_DATA_BIT_RATE / 8);
		pstUsbFlash->ullLimitIndexSize	= ((HUINT64)unTsrSoftLimitTimeSecond * USB_FLASH_TSR_INDEX_BIT_RATE / 8);
	}

	return ERR_OK;
}

HERROR	xmgr_recorder_SetRecSetup_Ertelecom(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup)
{
	HERROR hErr;

	hErr = xmgr_recorder_GetTsrLimitForUsbFlash_Ertel (&(pstRecSetup->stUsbFlash));
	if ((ERR_OK == hErr) && (TRUE == pstRecSetup->stUsbFlash.bIsUsbFlashRecord))
	{
		(void)xmgr_recorder_SetRecorderParam_Base (eMgrRec_DEFAULT_DURATION_TSR_RECORD, pstRecSetup->stUsbFlash.ulLimitTimeSecond);
	}
    else
    {
        (void)xmgr_recorder_SetRecorderParam_Base (eMgrRec_DEFAULT_DURATION_TSR_RECORD, DEFAULT_DURATION_TSR_RECORD);
    }

	if( (nRecIdx >= NUM_MAX_REC_ACTION) || (NULL == pstRecSetup) )
	{
		HxLOG_Error("incorrect params(nRecIdx:%d)(pstRecSetup:0x%x)\n", nRecIdx, pstRecSetup);
		return ERR_FAIL;
	}

	/* copy setup info to context */
	{
		xmgrRec_Context_t *pstContext = xmgr_rec_GetContext(nRecIdx);

		if( NULL == pstContext )
		{
			HxLOG_Error("can't find context by nRecIdx(%d)\n", nRecIdx);
			return ERR_FAIL;
		}

		if( eMgrRecState_IDLE != pstContext->eRecState )
		{
			HxLOG_Error("already alloc rec-context by nRecIdx(%d)\n", nRecIdx);
			return ERR_FAIL;
		}

		xmgr_rec_ClearContext(pstContext);
		HxSTD_MemCopy(&pstContext->stSetup, pstRecSetup, sizeof(MgrRec_RecSetup_t));
	}
	
	return ERR_OK;
}
#endif
