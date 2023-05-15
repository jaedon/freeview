/**
	@file     	pal_pipe.c
	@brief    	pipe resources:

	Description: 		\n
	Module: 		pal/pipe			 	\n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/* ======================================== */
#define ___HEADER_FILES___
/* ======================================== */
#include <octo_common.h>
#include <rlib.h>

#include <di_demux.h>
#include <pal_pipe.h>

#if defined(CONFIG_MW_CH_EMULATION)
#include <pal_channel.h>
#include "../channel/_pal_channel_emu.h"
#endif
#if defined(CONFIG_MW_CI_PLUS)
#include <di_ci.h>
#endif

/* ======================================== */
#define ___LOCAL_DEFINES___
/* ======================================== */
#define palPipe_ACTIONID_MASK				0xFF000000
#define palPipe_SESSIONVER_MASK				0x00FFFFFF
#define palPipe_ACTIONID_BIT_OFFSET			24

#define palPipe_CRITICAL_ENTER				VK_SEM_Get (s_ullPalPipe_SemId)
#define palPipe_CRITICAL_LEAVE				VK_SEM_Release (s_ullPalPipe_SemId)

/* ======================================== */
#define ___LOCAL_TYPEDEFS___
/* ======================================== */
typedef struct
{
	void					*pvUserData;
	HUINT32					 ulUserDataSize;
	PalPipe_NotifyCb_t		 pfCallback;
} palPipe_Notifier_t;

typedef struct
{
	Handle_t				 hLatestActHandle;

	// _Latest 들은 실제 최근까지 DI에 세팅한 값들을 의미한다.

 	HINT32					 nDemuxId;
	PalPipe_Input_e			 eInputType;
	HINT32					 nInputDevId;

 	PalPipe_Output_e		 eOutputType;
	HINT32					 nOutputDevId;

 	HINT32					 nCiId;

	// Connect시 실행될 Function List:
	HxList_t				*pstOnConnectList;				// palPipe_Notifier_t
} palPipe_Instance_t;


typedef struct
{
	HUINT32					 ulDemuxId;

	HBOOL					 bSetPath;

	PalPipe_Input_e			 eInputType;
	HINT32					 nInputDevId;

 	PalPipe_Output_e		 eOutputType;
	HINT32					 nOutputDevId;

 	HINT32					 nCiId;

	HUINT32					 ulAllocatedFlag;
} palPipe_DemuxStatus_t;


/* ======================================== */
#define ___LOCAL_VARIABLES___
/* ======================================== */
STATIC HULONG					 s_ullPalPipe_SemId;
STATIC palPipe_Instance_t		*s_pstPalPipe_InstArray = NULL;
STATIC Handle_t					 s_hPalPipe_RlibHandle = RxHANDLE_NULL;
STATIC HUINT32					 s_ulPalPipe_NumDemux = 0;
STATIC palPipe_DemuxStatus_t	*s_pstPalPipe_DmxStatusArray = NULL;

/* ======================================== */
#define ___LOCAL_FUNCTION_DECLARATIONS___
/* ======================================== */

/* ======================================== */
#define ___LOCAL_FUNCTION___
/* ======================================== */
STATIC void pal_pipe_PrintDump (palPipe_Instance_t *pstInst)
{
	HxLOG_Print("[%s:%d]pstInst(0x%x)\r\n", __FUNCTION__, __LINE__, pstInst);
	HxLOG_HexIntDump(pstInst->hLatestActHandle);
	HxLOG_DecIntDump(pstInst->nCiId);
	HxLOG_DecIntDump(pstInst->nDemuxId);
	HxLOG_DecIntDump(pstInst->eInputType);
	HxLOG_DecIntDump(pstInst->nInputDevId);
	HxLOG_DecIntDump(pstInst->eOutputType);
	HxLOG_DecIntDump(pstInst->nOutputDevId);
}

INLINE STATIC Handle_t pal_pipe_makeActionHandle (HUINT32 ulActionId, HUINT32 ulSessionVer)
{
	if (ulActionId < eActionId_MAX)
	{
		return (Handle_t)((ulActionId << palPipe_ACTIONID_BIT_OFFSET) | (ulSessionVer & palPipe_SESSIONVER_MASK));
	}

	return HANDLE_NULL;
}

INLINE STATIC palPipe_Instance_t *pal_pipe_getInstance (HUINT32 ulActionId)
{
	if ((ulActionId < eActionId_MAX) && (NULL != s_pstPalPipe_InstArray))
	{
		return &(s_pstPalPipe_InstArray[ulActionId]);
	}

	return NULL;
}

STATIC void pal_pipe_freeNotifier (void *pvData)
{
	palPipe_Notifier_t		*pstNotifier = (palPipe_Notifier_t *)pvData;

	if (NULL != pstNotifier)
	{
		if (NULL != pstNotifier->pvUserData)
		{
			HLIB_STD_MemFree(pstNotifier->pvUserData);
		}

		HLIB_STD_MemFree(pstNotifier);
	}
}

INLINE STATIC HERROR pal_pipe_clearInstance (palPipe_Instance_t *pstInst)
{
	Handle_t			 hAction = pstInst->hLatestActHandle;

	pstInst->pstOnConnectList = HLIB_LIST_RemoveAllFunc(pstInst->pstOnConnectList, pal_pipe_freeNotifier);

	HxSTD_memset (pstInst, 0, sizeof(palPipe_Instance_t));

	pstInst->nDemuxId		= -1;
	pstInst->nInputDevId	= -1;
	pstInst->nOutputDevId	= -1;
	pstInst->nCiId			= -1;

	pstInst->hLatestActHandle	= hAction;
	return ERR_OK;
}

INLINE STATIC palPipe_DemuxStatus_t *pal_pipe_getDemuxStatus (HUINT32 ulDemuxId)
{
	if ((ulDemuxId < s_ulPalPipe_NumDemux) && (NULL != s_pstPalPipe_DmxStatusArray))
	{
		return &(s_pstPalPipe_DmxStatusArray[ulDemuxId]);
	}

	return NULL;
}

INLINE STATIC HERROR pal_pipe_clearDemuxStatus (HUINT32 ulDemuxId)
{
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

	if (NULL == pstDmxStatus)
	{
		HxLOG_Error("pal_pipe_getDemuxStatus err: ulDemuxId(%d)\n", ulDemuxId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstDmxStatus, 0, sizeof(palPipe_DemuxStatus_t));
	pstDmxStatus->ulDemuxId = ulDemuxId;
	pstDmxStatus->nInputDevId = -1;
	pstDmxStatus->nOutputDevId = -1;
	pstDmxStatus->nCiId = -1;

	return ERR_OK;
}

STATIC HERROR pal_pipe_linkDemux (HUINT32 ulActionId, HUINT32 ulDemuxId)
{
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

	if (NULL == pstDmxStatus)
	{
		HxLOG_Error("pal_pipe_getDemuxStatus err: ulDemuxId(%d)\n", ulDemuxId);
		return ERR_FAIL;
	}

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error("ulActionID wrong (%d)\n", ulActionId);
		return ERR_FAIL;
	}

	pstDmxStatus->ulAllocatedFlag |= (0x0001 << ulActionId);
	return ERR_OK;
}

STATIC HERROR pal_pipe_unlinkDemux (HUINT32 ulActionId, HUINT32 ulDemuxId)
{
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

	if (NULL == pstDmxStatus)
	{
		HxLOG_Error("pal_pipe_getDemuxStatus err: ulDemuxId(%d)\n", ulDemuxId);
		return ERR_FAIL;
	}

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error("ulActionID wrong (%d)\n", ulActionId);
		return ERR_FAIL;
	}

	pstDmxStatus->ulAllocatedFlag &= ~(0x0001 << ulActionId);
	return ERR_OK;
}

STATIC HINT32 pal_pipe_countDemuxLink (HUINT32 ulDemuxId)
{
	HINT32					 nLinkNum;
	HUINT32					 ulActionId;
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

	if (NULL == pstDmxStatus)
	{
		HxLOG_Error("pal_pipe_getDemuxStatus err: ulDemuxId(%d)\n", ulDemuxId);
		return 0;
	}

	for (ulActionId = 0, nLinkNum = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		if (pstDmxStatus->ulAllocatedFlag & (0x0001 << ulActionId))
		{
			nLinkNum++;
		}
	}

	return nLinkNum;
}

STATIC HERROR pal_pipe_getDemuxRequestedArgs (HUINT32 ulDemuxId, PalPipe_Input_e *peInputType, HINT32 *pnInputId, PalPipe_Output_e *peOutputType, HINT32 *pnOutputId, HINT32 *pnCiId)
{
	HUINT32					 ulActionId;
	HINT32					 nInputId = -1;
	HINT32					 nOutputId = -1;
	HINT32					 nCiId = -1;
	PalPipe_Input_e			 eInput = ePAL_PIPE_INPUT_NONE;
	PalPipe_Output_e		 eOutput = ePAL_PIPE_OUTPUT_NONE;
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

	if (NULL == pstDmxStatus)
	{
		HxLOG_Error("pal_pipe_getDemuxStatus err: ulDemuxId(%d)\n", ulDemuxId);
		return ERR_FAIL;
	}

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		if (pstDmxStatus->ulAllocatedFlag & (0x0001 << ulActionId))
		{
			palPipe_Instance_t		*pstInst = pal_pipe_getInstance(ulActionId);

			if (NULL != pstInst)
			{
				if (ePAL_PIPE_INPUT_NONE != pstInst->eInputType)
				{
					if ((ePAL_PIPE_INPUT_NONE != eInput) && (pstInst->eInputType != eInput))
					{
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) InputType(%d) !!!!\n", ulDemuxId, pstInst->eInputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) InputType(%d) !!!!\n", ulDemuxId, pstInst->eInputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) InputType(%d) !!!!\n", ulDemuxId, pstInst->eInputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) InputType(%d) !!!!\n", ulDemuxId, pstInst->eInputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) InputType(%d) !!!!\n", ulDemuxId, pstInst->eInputType);
					}

					eInput = pstInst->eInputType;
					nInputId = pstInst->nInputDevId;
				}

				if (ePAL_PIPE_OUTPUT_NONE != pstInst->eOutputType)
				{
					if ((ePAL_PIPE_OUTPUT_NONE != eOutput) && (pstInst->eOutputType != eOutput))
					{
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) OutputType(%d) !!!!\n", ulDemuxId, pstInst->eOutputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) OutputType(%d) !!!!\n", ulDemuxId, pstInst->eOutputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) OutputType(%d) !!!!\n", ulDemuxId, pstInst->eOutputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) OutputType(%d) !!!!\n", ulDemuxId, pstInst->eOutputType);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) OutputType(%d) !!!!\n", ulDemuxId, pstInst->eOutputType);
					}

					eOutput = pstInst->eOutputType;
					nOutputId = pstInst->nOutputDevId;
				}

				if (pstInst->nCiId >= 0)
				{
					if ((nCiId >= 0) && (nCiId != pstInst->nCiId))
					{
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) nCiId(%d) !!!!\n", ulDemuxId, pstInst->nCiId);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) nCiId(%d) !!!!\n", ulDemuxId, pstInst->nCiId);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) nCiId(%d) !!!!\n", ulDemuxId, pstInst->nCiId);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) nCiId(%d) !!!!\n", ulDemuxId, pstInst->nCiId);
						HxLOG_Critical("!!!! Something Wrong DemuxId(%d) nCiId(%d) !!!!\n", ulDemuxId, pstInst->nCiId);
					}

					nCiId = pstInst->nCiId;
				}
			}
		}
	}

	if (NULL != peInputType)				{ *peInputType = eInput; }
	if (NULL != peOutputType)				{ *peOutputType = eOutput; }
	if (NULL != pnInputId)					{ *pnInputId = nInputId; }
	if (NULL != pnOutputId)					{ *pnOutputId = nOutputId; }
	if (NULL != pnCiId)						{ *pnCiId = nCiId; }

	return ERR_OK;
}

STATIC DI_DEMUX_INPUTDEVICETYPE pal_pipe_convertPalInput2DiInput (PalPipe_Input_e eInputType)
{
	switch (eInputType)
	{
	case ePAL_PIPE_INPUT_NONE:
		return DI_DEMUX_INPUT_NONE;

	case ePAL_PIPE_INPUT_TUNER:
		return DI_DEMUX_INPUT_TUNER;

	case ePAL_PIPE_INPUT_RECODER:
		return DI_DEMUX_INPUT_RECORDER;

	case ePAL_PIPE_INPUT_SCRAMBLED_RECODER:
		return DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER;

	case ePAL_PIPE_INPUT_PLAYER:
		return DI_DEMUX_INPUT_PLAYER;

	case ePAL_PIPE_INPUT_SCRAMBLED_PLAYER:
		return DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER;

	case ePAL_PIPE_INPUT_MEDIA_PLAYER:
		return DI_DEMUX_INPUT_MEDIA_PLAYER;

	case ePAL_PIPE_INPUT_EXTERNAL:
		return DI_DEMUX_INPUT_EXTERNAL;

	default:
		break;
	}

	return DI_DEMUX_INPUT_CURRENT;
}

STATIC DI_DEMUX_OUTPUTDEVICETYPE pal_pipe_convertPalOutput2DiOutput (PalPipe_Output_e eOutputType)
{
	switch (eOutputType)
	{
	case ePAL_PIPE_OUTPUT_NONE:
		return DI_DEMUX_OUTPUT_NONE;

	case ePAL_PIPE_OUTPUT_RECORDER:
		return DI_DEMUX_OUTPUT_RECORDER;

	case ePAL_PIPE_OUTPUT_DECORDER:
		return DI_DEMUX_OUTPUT_DECODER;

	case ePAL_PIPE_OUTPUT_EXTERNAL:
		return DI_DEMUX_OUTPUT_EXTERNAL;

	case ePAL_PIPE_OUTPUT_TRANSCODER:
		return DI_DEMUX_OUTPUT_TRANSCODER;

	default:
		break;
	}

	return DI_DEMUX_OUTPUT_CURRENT;
}

STATIC DI_ERR_CODE pal_pipe_callDiSetPath(HUINT32 ulDemuxId,
								   DI_DEMUX_INPUTDEVICETYPE etProducer, HUINT32 ulInputDevice_id,
								   DI_DEMUX_OUTPUTDEVICETYPE etConsumer, HUINT32 ulConsumerGroupId,
								   HINT32 nCI_Id)
{
#if defined(CONFIG_MW_CH_EMULATION)
	if ((DI_DEMUX_INPUT_TUNER == etProducer) && (PAL_CH_EMU_CheckIpTuneReady() == TRUE))
	{
		HINT32				 nPlayerId = PAL_CH_EMU_GetFirstPlayerIdForFakeTuner();

		if (nPlayerId >= 0)
		{
			return DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_PLAYER, (HUINT32)nPlayerId, etConsumer, ulConsumerGroupId, nCI_Id);
		}
	}
#endif
	return DI_DEMUX_SetPath(ulDemuxId, etProducer, ulInputDevice_id, etConsumer, ulConsumerGroupId, nCI_Id);
}

// Release 후 처음 DI_DEMUX_SetPath를 하는 경우
STATIC HERROR pal_pipe_setPathPhase1 (palPipe_Instance_t *pstInst)
{
	HINT32						 nInputDevId = -1;
	HINT32						 nOutputDevId = -1;
	HINT32						 nCiId = -1;
	PalPipe_Input_e				 eInputType = ePAL_PIPE_INPUT_NONE;
	PalPipe_Output_e			 eOutputType = ePAL_PIPE_OUTPUT_NONE;
	palPipe_DemuxStatus_t		*pstDmxStatus = pal_pipe_getDemuxStatus((HUINT32)pstInst->nDemuxId);
	HUINT32						 ulDiInputId;
	HUINT32						 ulDiOutputId;
	DI_DEMUX_INPUTDEVICETYPE	 eDiInputType;
	DI_DEMUX_OUTPUTDEVICETYPE	 eDiOutputType;
	DI_ERR_CODE					 eDiErr;
	HERROR						 hErr;

	if (NULL == pstDmxStatus)
	{
		return ERR_FAIL;
	}

	hErr = pal_pipe_getDemuxRequestedArgs(pstDmxStatus->ulDemuxId, &eInputType, &nInputDevId, &eOutputType, &nOutputDevId, &nCiId);
	if ((ERR_OK != hErr) || (ePAL_PIPE_INPUT_NONE == eInputType) || (nInputDevId < 0))
	{
		HxLOG_Error("pal_pipe_getDemuxRequestedArgs or InputType None\n");
		return ERR_FAIL;
	}

	// DEMUX와 Source 가 한번도 Setting 되지 않았다 : Release 후 첫 DI_DEMUX_SetPath를 시도하게 됨
	if ((pstDmxStatus->eInputType != eInputType) ||
		(pstDmxStatus->nInputDevId != nInputDevId) ||
		(pstDmxStatus->eOutputType != eOutputType) ||
		(pstDmxStatus->nOutputDevId != nOutputDevId) ||
		(pstDmxStatus->nCiId != nCiId))
	{
		eDiInputType = pal_pipe_convertPalInput2DiInput(eInputType);
		ulDiInputId = (HUINT32)nInputDevId;

		if ((ePAL_PIPE_OUTPUT_NONE == eOutputType) || (nOutputDevId < 0))
		{
			eDiOutputType = DI_DEMUX_OUTPUT_NONE;
			ulDiOutputId = 0;
		}
		else
		{
			eDiOutputType = pal_pipe_convertPalOutput2DiOutput(eOutputType);
			ulDiOutputId = (HUINT32)nOutputDevId;
		}

		eDiErr = pal_pipe_callDiSetPath((HUINT32)pstInst->nDemuxId, eDiInputType, ulDiInputId, eDiOutputType, ulDiOutputId, pstInst->nCiId);
		if (DI_ERR_OK == eDiErr)
		{
			pstDmxStatus->bSetPath = TRUE;
			pstDmxStatus->eInputType = eInputType;
			pstDmxStatus->nInputDevId = nInputDevId;
			pstDmxStatus->eOutputType = eOutputType;
			pstDmxStatus->nOutputDevId = nOutputDevId;
			pstDmxStatus->nCiId = nCiId;
		}
	}
	else
	{
		eDiErr = DI_ERR_OK;
	}

	if (DI_ERR_OK == eDiErr)
	{
		HUINT32			 ulActionId = PAL_PIPE_GetActionId(pstInst->hLatestActHandle);
		HxList_t		*pstItem;

		// SetPath 된 Notifier를 불러주고 제거한다.
		for (pstItem = pstInst->pstOnConnectList; NULL != pstItem; pstItem = pstItem->next)
		{
			palPipe_Notifier_t		*pstNoti = HLIB_LIST_Data(pstItem);

			if ((NULL != pstNoti) &&
				(NULL != pstNoti->pfCallback))
			{
				pstNoti->pfCallback(ulActionId, pstNoti->pvUserData, pstNoti->ulUserDataSize);
			}
		}

		pstInst->pstOnConnectList = HLIB_LIST_RemoveAllFunc(pstInst->pstOnConnectList, pal_pipe_freeNotifier);

#if defined(CONFIG_MW_CI_PLUS)
		if (pstInst->nCiId >= 0)
		{
			// Call Pass Through
			eDiErr = DI_CI_SetTsPath(pstInst->nCiId, DI_CI_PASSTHROUGH);
			if (eDiErr != DI_ERR_OK)
			{
				HxLOG_Error("[ERROR] DI_CI_SetTsPath : err %d\n", eDiErr);
				return ERR_FAIL;
			}
		}
#endif
	}

	return ERR_OK;
}

// SetPath를 한번 한 또  DI_DEMUX_SetPath를 하는 경우
STATIC HERROR pal_pipe_setPathPhase2 (palPipe_Instance_t *pstInst)
{
	HINT32						 nInputDevId = -1;
	HINT32						 nOutputDevId = -1;
	HINT32						 nCiId = -1;
	PalPipe_Input_e 			 eInputType = ePAL_PIPE_INPUT_NONE;
	PalPipe_Output_e			 eOutputType = ePAL_PIPE_OUTPUT_NONE;
	palPipe_DemuxStatus_t		*pstDmxStatus = pal_pipe_getDemuxStatus((HUINT32)pstInst->nDemuxId);
	HUINT32 					 ulDiInputId;
	HUINT32 					 ulDiOutputId;
	DI_DEMUX_INPUTDEVICETYPE	 eDiInputType;
	DI_DEMUX_OUTPUTDEVICETYPE	 eDiOutputType;
	DI_ERR_CODE 				 eDiErr;
	HERROR						 hErr;

	if (NULL == pstDmxStatus)
	{
		return ERR_FAIL;
	}

	hErr = pal_pipe_getDemuxRequestedArgs(pstDmxStatus->ulDemuxId, &eInputType, &nInputDevId, &eOutputType, &nOutputDevId, &nCiId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_pipe_getDemuxRequestedArgs or InputType None\n");
		return ERR_FAIL;
	}

	// DEMUX와 Source 가 한번도 Setting 되지 않았다 : Release 후 첫 DI_DEMUX_SetPath를 시도하게 됨
	if ((pstDmxStatus->eInputType != eInputType) ||
		(pstDmxStatus->nInputDevId != nInputDevId) ||
		(pstDmxStatus->eOutputType != eOutputType) ||
		(pstDmxStatus->nOutputDevId != nOutputDevId) ||
		(pstDmxStatus->nCiId != nCiId))
	{
		if ((ePAL_PIPE_INPUT_NONE == eInputType) || (nInputDevId < 0))
		{
			eDiInputType = DI_DEMUX_INPUT_NONE;
			ulDiInputId = 0;
		}
		else
		{
			eDiInputType = pal_pipe_convertPalInput2DiInput(eInputType);
			ulDiInputId = (HUINT32)nInputDevId;
		}

		if ((ePAL_PIPE_OUTPUT_NONE == eOutputType) || (nOutputDevId < 0))
		{
			eDiOutputType = DI_DEMUX_OUTPUT_NONE;
			ulDiOutputId = 0;
		}
		else
		{
			eDiOutputType = pal_pipe_convertPalOutput2DiOutput(eOutputType);
			ulDiOutputId = (HUINT32)nOutputDevId;
		}

		eDiErr = pal_pipe_callDiSetPath((HUINT32)pstInst->nDemuxId, eDiInputType, ulDiInputId, eDiOutputType, ulDiOutputId, pstInst->nCiId);
		if (DI_ERR_OK == eDiErr)
		{
			pstDmxStatus->bSetPath = TRUE;
			pstDmxStatus->eInputType = eInputType;
			pstDmxStatus->nInputDevId = nInputDevId;
			pstDmxStatus->eOutputType = eOutputType;
			pstDmxStatus->nOutputDevId = nOutputDevId;
			pstDmxStatus->nCiId = nCiId;
		}
	}
	else
	{
		eDiErr = DI_ERR_OK;
	}

	if (DI_ERR_OK == eDiErr)
	{
		HUINT32 		 ulActionId = PAL_PIPE_GetActionId(pstInst->hLatestActHandle);
		HxList_t		*pstItem;

		// SetPath 된 Notifier를 불러주고 제거한다.
		for (pstItem = pstInst->pstOnConnectList; NULL != pstItem; pstItem = pstItem->next)
		{
			palPipe_Notifier_t		*pstNoti = HLIB_LIST_Data(pstItem);

			if ((NULL != pstNoti) &&
				(NULL != pstNoti->pfCallback))
			{
				pstNoti->pfCallback(ulActionId, pstNoti->pvUserData, pstNoti->ulUserDataSize);
			}
		}

		pstInst->pstOnConnectList = HLIB_LIST_RemoveAllFunc(pstInst->pstOnConnectList, pal_pipe_freeNotifier);

#if defined(CONFIG_MW_CI_PLUS)
		if (pstInst->nCiId >= 0)
		{
			// Call Pass Through
			eDiErr = DI_CI_SetTsPath(pstInst->nCiId, DI_CI_PASSTHROUGH);
			if (eDiErr != DI_ERR_OK)
			{
				HxLOG_Error("[ERROR] DI_CI_SetTsPath : err %d\n", eDiErr);
				return ERR_FAIL;
			}
		}
#endif
	}

	return ERR_OK;
}

STATIC HERROR pal_pipe_setPath (palPipe_Instance_t *pstInst)
{
	palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus((HUINT32)pstInst->nDemuxId);

	if (NULL == pstDmxStatus)
	{
		return ERR_FAIL;
	}

 	// 1. Demux 와 Source를 Set 했는가 ?
	if (ePAL_PIPE_INPUT_NONE == pstDmxStatus->eInputType)
	{
		pal_pipe_setPathPhase1(pstInst);
	}
	else
	{
		pal_pipe_setPathPhase2(pstInst);
	}

 	return ERR_OK;
}

STATIC HBOOL pal_pipe_isPipeConnected (palPipe_Instance_t *pstInst)
{
	return ((pstInst->nDemuxId >= 0) && (pstInst->eInputType != ePAL_PIPE_INPUT_NONE) && (pstInst->nInputDevId >= 0)) ? TRUE : FALSE;
}

#if defined(CONFIG_DEBUG)
STATIC HCHAR *pal_pipe_enum2strInputType (PalPipe_Input_e eInputType)
{
	STATIC HCHAR		 s_szInputString[16];

	switch (eInputType)
	{
	case ePAL_PIPE_INPUT_NONE:
		return (HCHAR *)"NONE";

	case ePAL_PIPE_INPUT_TUNER:
		return (HCHAR *)"TUNER";

	case ePAL_PIPE_INPUT_RECODER:
		return (HCHAR *)"RECODER";

	case ePAL_PIPE_INPUT_SCRAMBLED_RECODER:
		return (HCHAR *)"SCRAMBLED_RECORDER";

	case ePAL_PIPE_INPUT_PLAYER:
		return (HCHAR *)"PLAYER";

	case ePAL_PIPE_INPUT_SCRAMBLED_PLAYER:
		return (HCHAR *)"SCRAMBLED_PLAYER";

	case ePAL_PIPE_INPUT_MEDIA_PLAYER:
		return (HCHAR *)"MEDIA_PLAYER";

	case ePAL_PIPE_INPUT_EXTERNAL:
		return (HCHAR *)"EXTERNAL";

	default:
		break;
	}

	HxSTD_PrintToStrN(s_szInputString, 16, "UNKNOWN(%d)", eInputType);
	return s_szInputString;
}

STATIC HCHAR *pal_pipe_enum2strOutputType (PalPipe_Output_e eOutputType)
{
	STATIC HCHAR		 s_szOutputString[16];

	switch (eOutputType)
	{
	case ePAL_PIPE_OUTPUT_NONE:
		return (HCHAR *)"NONE";

	case ePAL_PIPE_OUTPUT_RECORDER:
		return (HCHAR *)"RECORDER";

	case ePAL_PIPE_OUTPUT_DECORDER:
		return (HCHAR *)"DECODER";

	case ePAL_PIPE_OUTPUT_EXTERNAL:
		return (HCHAR *)"EXTERNAL";

	case ePAL_PIPE_OUTPUT_TRANSCODER:
		return (HCHAR *)"TRANSCODER";

	default:
		break;
	}

	HxSTD_PrintToStrN(s_szOutputString, 16, "UNKNOWN(%d)", eOutputType);
	return s_szOutputString;
}

STATIC void pal_pipe_printActionStatus (void)
{
	HUINT32		ulActionId;

	HLIB_CMD_Printf("===================== Pipe Status (Action) ============================ \n");
	HLIB_CMD_Printf(" Action Number: %d \n", eActionId_MAX);
	HLIB_CMD_Printf(" \n");

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		palPipe_Instance_t	*pstInst = pal_pipe_getInstance(ulActionId);

		if (NULL != pstInst)
		{
			HLIB_CMD_Printf(" ActionId(%02d) : DemuxId:%d Input[%s]:%d Output[%s]:%d CI:%d\n", ulActionId, pstInst->nDemuxId,
																	pal_pipe_enum2strInputType(pstInst->eInputType), pstInst->nInputDevId,
																	pal_pipe_enum2strOutputType(pstInst->eOutputType), pstInst->nOutputDevId,
																	pstInst->nCiId);
		}
	}

	HLIB_CMD_Printf("======================================================================= \n");
}

STATIC void pal_pipe_printDemuxStatus (void)
{
	HUINT32				 ulDemuxId;

	HLIB_CMD_Printf("====================== Pipe Status (Demux) ============================ \n");
	HLIB_CMD_Printf(" Demux: %d \n", s_ulPalPipe_NumDemux);
	HLIB_CMD_Printf(" \n");

	for (ulDemuxId = 0; ulDemuxId < s_ulPalPipe_NumDemux; ulDemuxId++)
	{
		palPipe_DemuxStatus_t	*pstDmxStatus = pal_pipe_getDemuxStatus(ulDemuxId);

		if (NULL != pstDmxStatus)
		{
			HUINT32				 ulActionId;
			HINT32				 nInputId = -1;
			HINT32				 nOutputId = -1;
			HINT32				 nCiId = -1;
			PalPipe_Input_e 	 eInput = ePAL_PIPE_INPUT_NONE;
			PalPipe_Output_e	 eOutput = ePAL_PIPE_OUTPUT_NONE;
			HCHAR				 szActionString[128], szNumber[16];

			(void)pal_pipe_getDemuxRequestedArgs(ulDemuxId, &eInput, &nInputId, &eOutput, &nOutputId, &nCiId);

			HxSTD_MemSet(szActionString, 0, 128);
			for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
			{
				if (pstDmxStatus->ulAllocatedFlag & (0x0001 << ulActionId))
				{
					HxSTD_PrintToStrN(szNumber, 16, "%d ", ulActionId);
					HxSTD_StrCat(szActionString, szNumber);
				}
			}

			if ('\0' == szActionString[0])
			{
				HxSTD_StrCpy(szActionString, "NOTHING");
			}


			HLIB_CMD_Printf(" Demux (%02d) : \n", ulDemuxId);


			HLIB_CMD_Printf("   Requested Input[%s]:%d Output[%s]:%d CI:%d\n",
											pal_pipe_enum2strInputType(eInput), nInputId,
											pal_pipe_enum2strOutputType(eOutput), nOutputId,
											nCiId);

			HLIB_CMD_Printf("   SetPath   Input[%s]:%d Output[%s]:%d CI:%d\n",
											pal_pipe_enum2strInputType(pstDmxStatus->eInputType), pstDmxStatus->nInputDevId,
											pal_pipe_enum2strOutputType(pstDmxStatus->eOutputType), pstDmxStatus->nOutputDevId,
											pstDmxStatus->nCiId);
			HLIB_CMD_Printf("   Related Actions = %s\n", szActionString);
		}
	}

	HLIB_CMD_Printf("======================================================================= \n");
}

STATIC int pal_pipe_dbgcmdActionStatus(void *szArgList)
{
	pal_pipe_printActionStatus();

	return HxCMD_OK;
}

STATIC int pal_pipe_dbgcmdDemuxStatus(void *szArgList)
{
	pal_pipe_printDemuxStatus();

	return HxCMD_OK;
}


#define	hCmdHandle		"pal_pipe"

STATIC void pal_pipe_registerCommand (void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_pipe_dbgcmdActionStatus,
						"action",
						"Print pipe action status",
						"action");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_pipe_dbgcmdDemuxStatus,
						"demux",
						"Print pipe demux status",
						"demux");
}
#endif


/* ======================================== */
#define ___MEMBER_FUNCTION___
/* ======================================== */

/* ======================================== */
#define ___API_FUNCTION___
/* ======================================== */
HERROR PAL_PIPE_Init (void)
{
	HUINT32					 ulCount, ulNumDmx = 0;
	palPipe_Instance_t		*pstInstArray = NULL;
	palPipe_DemuxStatus_t	*pstDmxArray = NULL;
	int						 nRet;

	nRet = VK_SEM_Create ((HULONG *)&s_ullPalPipe_SemId, "PipeSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != nRet)
	{
		HxLOG_Error ("VK_SEM_Create failed:\n");
		goto ERROR;
	}

	pstInstArray = (palPipe_Instance_t *)OxMW_Calloc (sizeof(palPipe_Instance_t) * eActionId_MAX);
	if (NULL == pstInstArray)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto ERROR;
	}

	for (ulCount = 0; ulCount < eActionId_MAX; ulCount++)
	{
		pstInstArray[ulCount].hLatestActHandle = pal_pipe_makeActionHandle (ulCount, 1);
		pal_pipe_clearInstance (&(pstInstArray[ulCount]));
	}

	nRet = DI_DEMUX_GetNumOfDemux(&ulNumDmx);
	if ((DI_ERR_OK != nRet) && (ulNumDmx == 0))
	{
		HxLOG_Error("DI_DEMUX_GetNumOfDemux Err:\n");
		goto ERROR;
	}

	pstDmxArray = (palPipe_DemuxStatus_t *)OxMW_Calloc(sizeof(palPipe_DemuxStatus_t) * ulNumDmx);
	if (NULL == pstDmxArray)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	for (ulCount = 0; ulCount < ulNumDmx; ulCount++)
	{
		pstDmxArray[ulCount].ulDemuxId = ulCount;
		pal_pipe_clearDemuxStatus(ulCount);
	}

	s_pstPalPipe_InstArray = pstInstArray;
	s_ulPalPipe_NumDemux = ulNumDmx;
	s_pstPalPipe_DmxStatusArray = pstDmxArray;

	s_hPalPipe_RlibHandle = RLIB_CreateSessionHandle("PAL_PIPE");

#if defined(CONFIG_DEBUG)
	pal_pipe_registerCommand();
#endif

	return ERR_OK;

ERROR:
	if (NULL != pstInstArray)			{ OxMW_Free(pstInstArray); }
	if (NULL != pstDmxArray)			{ OxMW_Free(pstDmxArray); }

	return ERR_FAIL;
}

Handle_t PAL_PIPE_GetRscSessionHandle (void)
{
	return s_hPalPipe_RlibHandle;
}


/*
	Action ID / Action Handle 관련 :
*/
INLINE HUINT32 PAL_PIPE_GetActionId (Handle_t hAction)
{
	return (HUINT32)((hAction & palPipe_ACTIONID_MASK) >> palPipe_ACTIONID_BIT_OFFSET);
}

INLINE HUINT32 PAL_PIPE_GetSessionVersion (Handle_t hAction)
{
	return (HUINT32)(hAction & palPipe_SESSIONVER_MASK);
}

Handle_t PAL_PIPE_GetActionHandle (HUINT32 ulActionId)
{
	palPipe_Instance_t			*pstInst = pal_pipe_getInstance (ulActionId);

	if (NULL != pstInst)
	{
		return pstInst->hLatestActHandle;
	}

	return HANDLE_NULL;
}

Handle_t PAL_PIPE_IncreaseActionVersion (HUINT32 ulActionId)
{
	palPipe_Instance_t			*pstInst = pal_pipe_getInstance (ulActionId);

	if (NULL != pstInst)
	{
		HUINT32			 ulSessionVer = PAL_PIPE_GetSessionVersion (pstInst->hLatestActHandle);

		ulSessionVer ++;
		if (ulSessionVer >= palPipe_SESSIONVER_MASK)
		{
			ulSessionVer = 1;
		}

		pstInst->hLatestActHandle = pal_pipe_makeActionHandle (ulActionId, ulSessionVer);
		return pstInst->hLatestActHandle;
	}

	return HANDLE_NULL;
}

HERROR PAL_PIPE_IsActionHandleLatest (Handle_t hAction)
{
	HUINT32						 ulActionId = PAL_PIPE_GetActionId (hAction);
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance (ulActionId);

	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	if (pstInst->hLatestActHandle != hAction)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*
	PIPE 연결 관련 :
*/
HERROR PAL_PIPE_SetDemux (HUINT32 ulActionId, HUINT32 ulDemuxId)
{
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance (ulActionId);
	HERROR						 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;
	pstInst->nDemuxId			= (HUINT32)ulDemuxId;
	pal_pipe_linkDemux(ulActionId, ulDemuxId);
	hErr = pal_pipe_setPath (pstInst);
	palPipe_CRITICAL_LEAVE;

	return hErr;
}

/* for DEMUX optimize */
HERROR PAL_PIPE_SetTunerResource (HUINT32 ulDemuxId, PalPipe_Input_e eType, HUINT32 ulRscId)
{

	DI_DEMUX_INPUTDEVICETYPE			 eDiInputType;

	palPipe_CRITICAL_ENTER;
	eDiInputType	= pal_pipe_convertPalInput2DiInput (eType);
	DI_DEMUX_SetInputSource(ulDemuxId, eDiInputType, ulRscId);
	palPipe_CRITICAL_LEAVE;

	return ERR_OK;
}

HERROR PAL_PIPE_SetInputResource (HUINT32 ulActionId, PalPipe_Input_e eType, HUINT32 ulRscId)
{
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance (ulActionId);
	HERROR						 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;
	pstInst->eInputType			= eType;
	pstInst->nInputDevId		= (HINT32)ulRscId;
	hErr = pal_pipe_setPath (pstInst);
	palPipe_CRITICAL_LEAVE;

	return hErr;
}

HERROR PAL_PIPE_SetOutputResource (HUINT32 ulActionId, PalPipe_Output_e eType, HUINT32 ulRscId)
{
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance (ulActionId);
	HERROR						 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;
	pstInst->eOutputType		= eType;
	pstInst->nOutputDevId		= (HINT32)ulRscId;
	hErr = pal_pipe_setPath (pstInst);
	palPipe_CRITICAL_LEAVE;

	return hErr;
}

HERROR PAL_PIPE_SetCi (HUINT32 ulActionId, HINT32 nCiId)
{
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance (ulActionId);
	HERROR						 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error ("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;
	pstInst->nCiId				= nCiId;
	hErr = pal_pipe_setPath (pstInst);
	palPipe_CRITICAL_LEAVE;

	return hErr;
}

HERROR PAL_PIPE_ResetPipe (HUINT32 ulActionId)
{
	HUINT32						 ulDmxId;
	palPipe_Instance_t			*pstInst = pal_pipe_getInstance(ulActionId);
	DI_ERR_CODE					 eDiErr = DI_ERR_OK;

	if (NULL == pstInst)
	{
		HxLOG_Error("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;

	ulDmxId = (HUINT32)pstInst->nDemuxId;
	pal_pipe_unlinkDemux(ulActionId, ulDmxId);

	pal_pipe_clearInstance(pstInst);
	if (pal_pipe_countDemuxLink(ulDmxId) <= 0)
	{
		eDiErr = DI_DEMUX_ReleasePath(ulDmxId);
		if (DI_ERR_OK == eDiErr)
		{
			pal_pipe_clearDemuxStatus(ulDmxId);
		}
	}

	palPipe_CRITICAL_LEAVE;

	return (DI_ERR_OK == eDiErr) ? ERR_OK : ERR_FAIL;
}

HBOOL PAL_PIPE_IsPipeConnected (HUINT32 ulActionId)
{
	HBOOL					 bRet = FALSE;
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance(ulActionId);

	if (NULL == pstInst)
	{
		HxLOG_Error("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return FALSE;
	}

	palPipe_CRITICAL_ENTER;
	bRet = pal_pipe_isPipeConnected(pstInst);
	palPipe_CRITICAL_LEAVE;

	return bRet;
}

HERROR PAL_PIPE_NotifyPipeConnected (HUINT32 ulActionId, PalPipe_NotifyCb_t pfCallback, void *pvUserData, HUINT32 ulUserDataSize)
{
	palPipe_Instance_t		*pstInst = pal_pipe_getInstance(ulActionId);
	palPipe_Notifier_t		*pstNoti = NULL;
	void					*pvDupData = NULL;
	HERROR					 hResult = ERR_FAIL;

	if (NULL == pstInst)
	{
		HxLOG_Error("Instance NULL: PipeId(%d) is wrong...\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pfCallback)
	{
		HxLOG_Error("Notifier NULL\n");
		return ERR_FAIL;
	}

	palPipe_CRITICAL_ENTER;
	if (pal_pipe_isPipeConnected(pstInst) == TRUE)
	{
		palPipe_CRITICAL_LEAVE;
		pfCallback(ulActionId, pvUserData, ulUserDataSize);
		return ERR_OK;
	}

	pstNoti = (palPipe_Notifier_t *)HLIB_STD_MemCalloc(sizeof(palPipe_Notifier_t));
	if (NULL == pstNoti)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	pstNoti->pfCallback = pfCallback;

	if ((NULL != pvUserData) && (ulUserDataSize > 0))
	{
		pvDupData = (void *)HLIB_STD_MemAlloc(ulUserDataSize);
		if (NULL == pvDupData)
		{
			HxLOG_Error("Memory allocation failed\n");
			goto END_FUNC;
		}

		HxSTD_MemCopy(pvDupData, pvUserData, ulUserDataSize);

		pstNoti->pvUserData = pvDupData;
		pstNoti->ulUserDataSize = ulUserDataSize;
	}

	pstInst->pstOnConnectList = HLIB_LIST_Append(pstInst->pstOnConnectList, (void *)pstNoti);

	pstNoti = NULL;
	pvDupData = NULL;
	hResult = ERR_OK;

END_FUNC:
	palPipe_CRITICAL_LEAVE;
	if (NULL != pstNoti)			{ HLIB_STD_MemFree(pstNoti); }
	if (NULL != pvDupData)			{ HLIB_STD_MemFree(pvDupData); }

	return hResult;

}

/*
	Resource ID 관련 :
*/
HERROR PAL_PIPE_GetResourceNumber (RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	return RLIB_GetResourceNumber(PAL_PIPE_GetRscSessionHandle(), eRscType, pulRscNum);
}

HERROR PAL_PIPE_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	return RLIB_GetResourceId(PAL_PIPE_GetRscSessionHandle(), ulActionId, eRscType, pulRscId);
}

HERROR PAL_PIPE_ChangeResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId)
{
	return RLIB_ChangeResourceId(PAL_PIPE_GetRscSessionHandle(), ulActionId, eRscType, ulRscId);
}

HERROR PAL_PIPE_GetResourceObject (HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj)
{
	return RLIB_GetResourceObject(PAL_PIPE_GetRscSessionHandle(), ulActionId, eRscType, pstRscObj);
}

/*
	RF-INPUT Function
*/
HERROR PAL_PIPE_GetRfInputAbility (HUINT32 ulRfInputId, HBOOL *pbAbility)
{
	return RLIB_GetRfInputAbility(PAL_PIPE_GetRscSessionHandle(), ulRfInputId, pbAbility);
}

HERROR PAL_PIPE_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility)
{
	return RLIB_SetRfInputAbility(PAL_PIPE_GetRscSessionHandle(), ulRfInputId, bAbility);
}

HERROR PAL_PIPE_GetRfInputCapacity (HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity)
{
	return RLIB_GetRfInfoCapacity(PAL_PIPE_GetRscSessionHandle(), ulRscId, pstCapacity);

}

HERROR PAL_PIPE_GetTunerCapacity (HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType)
{
	return RLIB_GetTunerCapacity(PAL_PIPE_GetRscSessionHandle(), ulRscId, peSupportedDeliType);
}

HERROR PAL_PIPE_GetResourceUsageType (RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType)
{
	return RLIB_GetResourceUsageType(PAL_PIPE_GetRscSessionHandle(), eRscType, ulRscId, peUsageType);
}

