/**************************************************************************************************/
#define ________CA_DSC_Private_Include____________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

//#include "pal_dsc.h"
//#include <pal_pipe.h>
//#include "mwc_fs.h"
#include "isosvc.h"

#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_DSC_Golbal_Value________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_DSC_Private_Define_______________________________________________________
/**************************************************************************************************/





#define DEFAULT_DSC_ID						0
#define MAX_DSC_POOL						60
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
#define START_DSC_POOL						1
#endif

#define WAIT_FOR_SEM(x) 					VK_SEM_Get(x);
#define RELEASE_SEM(x)						VK_SEM_Release(x);

#if defined(VALID_PID)
#undef VALID_PID
#define VALID_PID(x)							(x >= 0 && x < 0x1fff)
#endif
#if defined(INVALID_PID)
#undef INVALID_PID
#define INVALID_PID(x)						!(VALID_PID(x))
#endif
#define VALID_KEY_LENGTH(x) 				(x == 8)
#define INVALID_KEY_LENGTH(x)				!(VALID_KEY_LENGTH(x))

#define VALID_PROTECT_KEY_LENGTH(x) 		(x == 16)
#define INVALID_PROTECT_KEY_LENGTH(x)		!(VALID_PROTECT_KEY_LENGTH(x))

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
#define CONFIG_INIT_ENTROPY 				0
#else
#define CONFIG_INIT_ENTROPY 				1
#endif


/**************************************************************************************************/
#define ________CA_DSC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
#if (CONFIG_DSC_METADATA == 1)
typedef enum tagMetaTagType
{
	eTagCableCard 	= 0x00,
	eTagCopyCtlInfo = 0x01,
	eMetaTagTyp_Max
} MetaTagType_t;
#endif

typedef struct SDscChannelId
{
	TMpegStreamType		ucStreamType;
	TPid				usElemPid;
	TUnsignedInt32		hAction;
	TUnsignedInt32		hDscHandle;
	TSignedInt32 		nDeviceId;
	TUnsignedInt8		ucSetProtecedKeys;
} SDscChannelId_t;

#if 0//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define OsyExtLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA OSY EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define OsyExtLogInfo(message)		//((HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	HLIB_CMD_Printf((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	HLIB_CMD_Printf((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	HLIB_CMD_Printf((__VA_ARGS__))
#endif

#if defined (HxLOG_Info)
#undef	HxLOG_Info
#define HxLOG_Info(...)		OsyExtLogInfo((__VA_ARGS__))
#endif

#endif

/**************************************************************************************************/
#define ________CA_DSC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TBoolean				s_bDscUsedFlag[MAX_DSC_POOL];
static TDscChannelId		s_pstDscChannelId[MAX_DSC_POOL];

static volatile TUnsignedInt32	s_ulDscChannelSemId = 0;


/**************************************************************************************************/
#define ________CA_DSC_Private_Static_Prototype_____________________________________________
/**************************************************************************************************/
static void 		local_caDsc_InitialEntropy(const TUnsignedInt8 *key1, TUnsignedInt8 *key2);
static TDscStatus 	local_caDsc_AllocateChannelResource(TDscChannelId *ppstDscChannelId);
static TDscStatus 	local_caDsc_FreeChannelResource(TDscChannelId pstDscChannelId);
static TDscStatus 	local_caDsc_FindChannelResource(TDscChannelId pstDscChannelId);
static TDscStatus	local_caDsc_SetProtectedKeys(TDscChannelId pstDscChannelId, TBoolean ucSetKeys);
static TDscStatus 	local_caDsc_GetProtectedKeys(TDscChannelId pstDscChannelId, TBoolean *pucSetKeys);
static TDscStatus 	local_caDsc_SetKeys(TDscChannelId xDscChannelId,
												TSize xOddKeyLength, const TDscKey *pxOddKey,
												TSize xEvenKeyLength, const TDscKey *pxEvenKey);

#if (CONFIG_DSC_METADATA == 1)
static TDscStatus 	local_caDsc_GetDescriptor(TUnsignedInt8 *pxInputData, TUnsignedInt32 xInputDataLength,
													TUnsignedInt8 xTag, TUnsignedInt8 **ppxOutputData, TUnsignedInt8 *pxOutputDataLength);
#endif


/**************************************************************************************************/
#define ________CA_DSC_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
static void local_caDsc_InitialEntropy(const TUnsignedInt8 *key1, TUnsignedInt8 *key2)
{
	*(key2+0) = *(key1+0);
	*(key2+1) = *(key1+1);
	*(key2+2) = *(key1+2);
	*(key2+3) = *(key1+0) + *(key1+1) + *(key1+2);
	*(key2+4) = *(key1+4);
	*(key2+5) = *(key1+5);
	*(key2+6) = *(key1+6);
	*(key2+7) = *(key1+4) + *(key1+5) + *(key1+6);
}


static TDscStatus local_caDsc_AllocateChannelResource(TDscChannelId *ppstDscChannelId)
{
	TUnsignedInt8 i;
	TDscStatus 	eStatus;

	if (ppstDscChannelId == NULL)
		return DSC_ERROR;

	WAIT_FOR_SEM(s_ulDscChannelSemId)
	eStatus = DSC_ERROR_NO_MORE_RESOURCES;
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	for (i=START_DSC_POOL; i<MAX_DSC_POOL; i++)
#else
	for (i=0; i<MAX_DSC_POOL; i++)
#endif
	{
		if (s_bDscUsedFlag[i] == FALSE)
		{
			s_bDscUsedFlag[i] = TRUE;

			s_pstDscChannelId[i]->ucStreamType = 0;
			s_pstDscChannelId[i]->usElemPid = 0;
			s_pstDscChannelId[i]->hAction = 0;
			s_pstDscChannelId[i]->hDscHandle = 0;
			s_pstDscChannelId[i]->nDeviceId = 0xFF;
			s_pstDscChannelId[i]->ucSetProtecedKeys = 0;

			*ppstDscChannelId = s_pstDscChannelId[i];
			eStatus = DSC_NO_ERROR;
			break;
		}
	}
	RELEASE_SEM(s_ulDscChannelSemId)
	return eStatus;
}


static TDscStatus local_caDsc_FreeChannelResource(TDscChannelId	pstDscChannelId)
{
	TUnsignedInt8 i;
	TDscStatus 	eStatus = DSC_ERROR_UNKNOWN_ID;

	WAIT_FOR_SEM(s_ulDscChannelSemId)
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	for (i=START_DSC_POOL; i<MAX_DSC_POOL; i++)
#else
	for (i=0; i<MAX_DSC_POOL; i++)
#endif
	{
		if (s_bDscUsedFlag[i] == FALSE)
			continue;
		if (s_pstDscChannelId[i] == pstDscChannelId)
		{
			s_bDscUsedFlag[i] = FALSE;

			s_pstDscChannelId[i]->ucStreamType = 0;
			s_pstDscChannelId[i]->usElemPid = 0;
			s_pstDscChannelId[i]->hAction = 0;
			s_pstDscChannelId[i]->hDscHandle = 0;
			s_pstDscChannelId[i]->nDeviceId = 0xFF;
			s_pstDscChannelId[i]->ucSetProtecedKeys = 0;

			eStatus = DSC_NO_ERROR;
			break;
		}
	}

	RELEASE_SEM(s_ulDscChannelSemId)
	return eStatus;
}


static TDscStatus local_caDsc_FindChannelResource(TDscChannelId pstDscChannelId)
{
	TUnsignedInt8 i;
	TDscStatus 	eStatus = DSC_ERROR_UNKNOWN_ID;

	WAIT_FOR_SEM(s_ulDscChannelSemId)

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	for (i=START_DSC_POOL; i<MAX_DSC_POOL; i++)
#else
	for (i=0; i<MAX_DSC_POOL; i++)
#endif
	{
		if (s_bDscUsedFlag[i] == FALSE)
			continue;
		if (s_pstDscChannelId[i] == pstDscChannelId)
		{
			eStatus = DSC_NO_ERROR;
			break;
		}
	}

	RELEASE_SEM(s_ulDscChannelSemId)
	return eStatus;
}


static TDscStatus local_caDsc_SetProtectedKeys(TDscChannelId pstDscChannelId, TBoolean ucSetKeys)
{
	TUnsignedInt8 i;
	TDscStatus	eStatus = DSC_ERROR_UNKNOWN_ID;

	WAIT_FOR_SEM(s_ulDscChannelSemId)

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	for (i=START_DSC_POOL; i<MAX_DSC_POOL; i++)
#else
	for (i=0; i<MAX_DSC_POOL; i++)
#endif
	{
		if (s_bDscUsedFlag[i] == FALSE)
			continue;
		if (s_pstDscChannelId[i] == pstDscChannelId)
		{
			eStatus = DSC_NO_ERROR;
			s_pstDscChannelId[i]->ucSetProtecedKeys = ucSetKeys;
			break;
		}
	}

	RELEASE_SEM(s_ulDscChannelSemId)
	return eStatus;
}


static TDscStatus local_caDsc_GetProtectedKeys(TDscChannelId pstDscChannelId, TBoolean *pucSetKeys)
{
	TUnsignedInt8 i;
	TDscStatus	eStatus = DSC_ERROR_UNKNOWN_ID;

	if (pucSetKeys == NULL)
		return eStatus;

	WAIT_FOR_SEM(s_ulDscChannelSemId)

#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	for (i=START_DSC_POOL; i<MAX_DSC_POOL; i++)
#else
	for (i=0; i<MAX_DSC_POOL; i++)
#endif
	{
		if (s_bDscUsedFlag[i] == FALSE)
			continue;
		if (s_pstDscChannelId[i] == pstDscChannelId)
		{
			eStatus = DSC_NO_ERROR;
			*pucSetKeys = s_pstDscChannelId[i]->ucSetProtecedKeys;
			break;
		}
	}

	RELEASE_SEM(s_ulDscChannelSemId)
	return eStatus;
}


static TDscStatus local_caDsc_SetKeys(TDscChannelId xDscChannelId,
								TSize xOddKeyLength, const TDscKey *pxOddKey,
								TSize xEvenKeyLength, const TDscKey *pxEvenKey)
{
	TUnsignedInt32	ulErrCode;
	TUnsignedInt8 	ucCw[8] = {0,0,0,0,0,0,0,0};

	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	if ( ((xOddKeyLength > 0) && (INVALID_KEY_LENGTH(xOddKeyLength)))
		|| ((xEvenKeyLength > 0) && (INVALID_KEY_LENGTH(xEvenKeyLength))) )
	{
		HxLOG_Info("Invalid key length\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

#if defined(CONFIG_DEBUG)
{
	TUnsignedInt8 i;

	HxLOG_Print("(0x%X)(even %d)(odd %d)\n", (unsigned int)xDscChannelId, xEvenKeyLength, xOddKeyLength);

	HxLOG_Print("\n\npxOddKey info\n");
	if (pxOddKey != NULL)
	{
		for (i=0; i<xOddKeyLength; i++)
		{
			HxLOG_Print("0x%X ", pxOddKey[i]);
		}
	}
	HxLOG_Print("\npxOddKey info end\n");

	HxLOG_Print("\n\npxEvenKey info\n");
	if (pxEvenKey != NULL)
	{
		for (i=0; i<xEvenKeyLength; i++)
		{
			HxLOG_Print("0x%X ", pxEvenKey[i]);
		}
	}
	HxLOG_Print("\npxEvenKey info end\n");
}
#endif

	if (xOddKeyLength > 0)
	{
		if (pxOddKey == NULL)
		{
			HxLOG_Info("pxOddKey is NULL \n");
			return DSC_ERROR;
		}

		HxSTD_memcpy(ucCw, pxOddKey, 8);

		WAIT_FOR_SEM(s_ulDscChannelSemId)
		ulErrCode = svc_cas_DevDscSetKey(xDscChannelId->hDscHandle, eCAS_DSC_PARITY_ODD, (unsigned char*)ucCw);
		if (ulErrCode)
		{
			HxLOG_Info("svc_cas_DevDscSetKey Return Err(%x)\n", (unsigned int)ulErrCode);
		}
		RELEASE_SEM(s_ulDscChannelSemId)
	}

	if (xEvenKeyLength > 0)
	{
		if (pxEvenKey == NULL)
		{
			HxLOG_Info("pxEvenKey is NULL \n");
			return DSC_ERROR;
		}

		HxSTD_memcpy(ucCw, pxEvenKey, 8);

		WAIT_FOR_SEM(s_ulDscChannelSemId)
		ulErrCode = svc_cas_DevDscSetKey(xDscChannelId->hDscHandle, eCAS_DSC_PARITY_EVEN, (unsigned char*)ucCw);
		if (ulErrCode)
		{
			HxLOG_Info("svc_cas_DevDscSetKey Return Err(%x)\n", (unsigned int)ulErrCode);
		}
		RELEASE_SEM(s_ulDscChannelSemId)
	}

	return DSC_NO_ERROR;
}


#if (CONFIG_DSC_METADATA == 1)
static TDscStatus local_caDsc_GetDescriptor(TUnsignedInt8 *pxInputData, TUnsignedInt32 xInputDataLength,
											TUnsignedInt8 xTag, TUnsignedInt8 **ppxOutputData, TUnsignedInt8 *pxOutputDataLength)
{
	TUnsignedInt8 ucTag;
	TUnsignedInt8 ucHeadLen;
	TUnsignedInt8 ucDataLen;
	TUnsignedInt8 *pucInputData = pxInputData;

	if ( (pxInputData == NULL) || (xInputDataLength <= 0)
		|| (ppxOutputData == NULL) || (pxOutputDataLength == NULL) )
	{
		HxLOG_Info("target is null...\n");
		return DSC_ERROR;
	}

	*ppxOutputData = NULL;
	*pxOutputDataLength = 0;

	while ((TUnsignedInt32)(pucInputData - pxInputData) < xInputDataLength)
	{
		ucTag = *pucInputData;
		if ((*pucInputData & 0x80) == 0)
		{
			// Format 1
			ucDataLen = *pucInputData & 0x0F;
			ucHeadLen = 1;
			ucTag = (*pucInputData >> 4) & 0x0F;
		}
		else if ((*pucInputData & 0xC0) == 0x80)
		{
			// Format 2
			ucDataLen = *(pucInputData+1);
			ucHeadLen = 2;
		}
		else if ((*pucInputData & 0xF0) == 0xC0)
		{
			// Format 3
			ucDataLen = 4;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xD0)
		{
			// Format 4
			ucDataLen = 3;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xE0)
		{
			// Format 5
			ucDataLen = 2;
			ucHeadLen = 1;
		}
		else if ((*pucInputData & 0xF0) == 0xF0)
		{
			// Format 6
			ucDataLen = 1;
			ucHeadLen = 1;
		}

		if (ucTag == xTag)
		{
			// Yes, this is the descriptor that I'm looking for.
			*pxOutputDataLength = ucDataLen;
			*ppxOutputData = pucInputData + ucHeadLen;
			return DSC_NO_ERROR;
		}
		else
		{
			// This is not the right descriptor, go to the next one.
			pucInputData = pucInputData + ucHeadLen + ucDataLen;
		}
	} // while
	return DSC_ERROR;
}
#endif


/**************************************************************************************************/
#define ________CA_DSC_Public_Functions_Body________________________________________________
/**************************************************************************************************/
TDscStatus dscOpen(TDscChannelId *pxDscChannelId, TTransportSessionId xTransportSessionId, TMpegStreamType  xMpegStreamType)
{
	TDscStatus 			eStatus;
	TUnsignedInt32		ulDesHandle;
	TUnsignedInt32		ulErrCode;
	TSignedInt32		nDeviceId = DEFAULT_DSC_ID;
	SvcCas_DscDataType_e 	eDesDataType;

	HxLOG_Print("pxDscChannelId:0x%x, xTransportSessionId:0x%x, xMpegStreamType:0x%x\n",
							(unsigned int)pxDscChannelId, (unsigned int)xTransportSessionId, xMpegStreamType);

	if (pxDscChannelId == NULL)
	{
		HxLOG_Info("Error NULL channel ID \n");
		return DSC_ERROR;
	}

	if (xMpegStreamType > MPEG_TYPE_AUXILIARY)
	{
		if ( (xMpegStreamType != MPEG_TYPE_VIDEO_4)
			&& (xMpegStreamType != MPEG_TYPE_AUDIO_4)
			&& (xMpegStreamType != MPEG_TYPE_DSCMCC_U_N)
			)
		{
			HxLOG_Info("Error : Invalid MPEG TYPE \n");
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
			*pxDscChannelId = s_pstDscChannelId[0];
#endif
			return DSC_NO_ERROR;
		}
	}

	switch (xMpegStreamType)
	{
		case MPEG_TYPE_VIDEO:
		case MPEG_TYPE_VIDEO_13818:
		case MPEG_TYPE_VIDEO_4:
			eDesDataType = eCAS_DSC_DATA_TYPE_VIDEO_PES;
			break;

		case MPEG_TYPE_AUDIO:
		case MPEG_TYPE_AUDIO_13818:
		case MPEG_TYPE_PRIVATE_DATA:	// AC3
		case MPEG_TYPE_AUDIO_4:
			eDesDataType = eCAS_DSC_DATA_TYPE_AUDIO_PES;
			break;

#if (0)
		case MPEG_TYPE_PRIVATE_DATA:
			eDesDataType = eCAS_DSC_DATA_TYPE_PES_PES;
			break;
#endif

		default:
			eDesDataType = eCAS_DSC_DATA_TYPE_PSI_PES;
			break;
	}

	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId((unsigned int)xTransportSessionId), eCAS_RM_ITEM_DEMUX, (int*)&nDeviceId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Info("svc_cas_SubSvcRmGetResourceId error:0x%x, ts_id:0x%x\n", (unsigned int)ulErrCode, (unsigned int)xTransportSessionId);
		return DSC_ERROR;
	}

	ulErrCode = svc_cas_DevDscOpen((unsigned int)nDeviceId, eCAS_DSC_TYPE_DVB, eDesDataType, (unsigned int*)&ulDesHandle);
	if (ulErrCode == ERR_OK)
	{
		HxLOG_Print("device id:%d, stream type:0x%x, dsc handle:0x%x\n", (int)nDeviceId, eDesDataType, (unsigned int)ulDesHandle);
		eStatus = local_caDsc_AllocateChannelResource(pxDscChannelId);
		if (eStatus == DSC_NO_ERROR)
		{
			WAIT_FOR_SEM(s_ulDscChannelSemId)

			(*pxDscChannelId)->ucStreamType = xMpegStreamType;
			(*pxDscChannelId)->hDscHandle = ulDesHandle;
			(*pxDscChannelId)->hAction = xTransportSessionId;
			(*pxDscChannelId)->nDeviceId = nDeviceId;

			RELEASE_SEM(s_ulDscChannelSemId)
			HxLOG_Print("xDscChannelId:0x%x\n", (unsigned int)*pxDscChannelId);
			return DSC_NO_ERROR;
		}
	}
	return DSC_ERROR_NO_MORE_RESOURCES;
}


TDscStatus dscClose(TDscChannelId xDscChannelId)
{
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	HxLOG_Print("xDscChannelId:0x%x\n", (unsigned int)xDscChannelId);
#if (CONFIG_FOR_DAL_TEST_DEF == 1)
	if(xDscChannelId == s_pstDscChannelId[0])
	{
		HxLOG_Info("xDscChannelId 0(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_NO_ERROR;
	}
#endif
	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDscChannelSemId)
	if(svc_cas_DevDscClose(xDscChannelId->hDscHandle) != ERR_OK)
	{
		HxLOG_Error("[svc_cas_DevDscClose] fail!! \n");
	}	
	RELEASE_SEM(s_ulDscChannelSemId)

	return local_caDsc_FreeChannelResource(xDscChannelId);
}


TDscStatus dscSetPid(TDscChannelId xDscChannelId, TPid xPid)
{
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	HxLOG_Print("xDscChannelId:0x%x, xPid:0x%x\n", (unsigned int)xDscChannelId, xPid);

	if (INVALID_PID(xPid))
	{
		HxLOG_Info("Invalid PID(0x%x)\n", xPid);
		return DSC_ERROR_BAD_PID;
	}

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDscChannelSemId)
	if (svc_cas_DevDscSetPid(xDscChannelId->hDscHandle, xPid) != ERR_OK)
	{
		HxLOG_Info("Error DI_DSC_Control\n");
	}
 	xDscChannelId->usElemPid = xPid;
	RELEASE_SEM(s_ulDscChannelSemId)
	return DSC_NO_ERROR;
}


TDscStatus dscSetKeys(TDscChannelId xDscChannelId,
						TSize xOddKeyLength, const TDscKey *pxOddKey,
						TSize xEvenKeyLength, const TDscKey *pxEvenKey)
{
	TUnsignedInt32	ulErrCode;
	TUnsignedInt8 	ucCw[8] = {0,0,0,0,0,0,0,0};
	TUnsignedInt8	ucSetKeys = FALSE;

	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	if ( ((xOddKeyLength == 0) && (pxOddKey != NULL))
		|| ((xEvenKeyLength == 0) && (pxEvenKey != NULL)) )
	{
		HxLOG_Info("Invalid key length = 0\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

	if ( ((xOddKeyLength > 0) && (INVALID_KEY_LENGTH(xOddKeyLength)))
		|| ((xEvenKeyLength > 0) && (INVALID_KEY_LENGTH(xEvenKeyLength))) )
	{
		HxLOG_Info("Invalid key length\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

#if defined(CONFIG_DEBUG)
{
	TUnsignedInt8 i;

	HxLOG_Print("(0x%x)(even %d)(odd %d)\n", (unsigned int)xDscChannelId, xEvenKeyLength, xOddKeyLength);

	HxLOG_Print("\n\npxOddKey info\n");
	if (pxOddKey != NULL)
	{
		for (i=0; i<xOddKeyLength; i++)
		{
			HxLOG_Print("0x%x ", pxOddKey[i]);
		}
	}
	HxLOG_Print("\npxOddKey info end\n");

	HxLOG_Print("\n\npxEvenKey info\n");
	if (pxEvenKey != NULL)
	{
		for (i=0; i<xEvenKeyLength; i++)
		{
			HxLOG_Print("0x%x ", pxEvenKey[i]);
		}
	}
	HxLOG_Print("\npxEvenKey info end\n");
}
#endif

	if (local_caDsc_GetProtectedKeys(xDscChannelId, &ucSetKeys) == DSC_NO_ERROR)
	{
		if (ucSetKeys == TRUE)
		{
			if (svc_cas_DevDscClearProtectionKey(xDscChannelId->hDscHandle) != ERR_OK)
			{
				HxLOG_Info("error in svc_cas_DevDscClearProtectionKey\n");
				return DSC_ERROR;
			}

			if (local_caDsc_SetProtectedKeys(xDscChannelId, FALSE) != DSC_NO_ERROR)
			{
				HxLOG_Info("local_caDsc_SetProtectedKeys : Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
			}
		}
	}

	if (xOddKeyLength > 0)
	{
		if (pxOddKey == NULL)
		{
			HxLOG_Info("pxOddKey is NULL \n");
			return DSC_ERROR;
		}
#if (CONFIG_INIT_ENTROPY)
		local_caDsc_InitialEntropy(pxOddKey, ucCw);
#else
		HxSTD_memcpy(ucCw, pxOddKey, 8);
#endif

		WAIT_FOR_SEM(s_ulDscChannelSemId)
		ulErrCode = svc_cas_DevDscSetKey(xDscChannelId->hDscHandle, eCAS_DSC_PARITY_ODD, (unsigned char*)ucCw);
		if (ulErrCode)
		{
			HxLOG_Info("svc_cas_DevDscSetKey Return Err(%x)\n", (unsigned int)ulErrCode);
		}
		RELEASE_SEM(s_ulDscChannelSemId)
	}

	if (xEvenKeyLength > 0)
	{
		if (pxEvenKey == NULL)
		{
			HxLOG_Info("pxEvenKey is NULL \n");
			return DSC_ERROR;
		}
#if (CONFIG_INIT_ENTROPY)
		local_caDsc_InitialEntropy(pxEvenKey, ucCw);
#else
		HxSTD_memcpy(ucCw, pxEvenKey, 8);
#endif
		WAIT_FOR_SEM(s_ulDscChannelSemId)
		ulErrCode = svc_cas_DevDscSetKey(xDscChannelId->hDscHandle, eCAS_DSC_PARITY_EVEN, (unsigned char*)ucCw);
		if (ulErrCode)
		{
			HxLOG_Info("svc_cas_DevDscSetKey Return Err(%x)\n", (unsigned int)ulErrCode);
		}
		RELEASE_SEM(s_ulDscChannelSemId)
	}
	return DSC_NO_ERROR;
}


TDscStatus dscSet0LevelProtectedKeys(TDscChannelId xDscChannelId,
        										TSize xOddProtectedKeyLength, const TUnsignedInt8 *pxOddProtectedKey,
        										TSize xEvenProtectedKeyLength, const TUnsignedInt8 *pxEvenProtectedKey)
{
#if (1) // fix DAL Test error. DSC-NO.4  	--> Warning  This case is only available if __MKL_L0 is enabled
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	if ( ((xOddProtectedKeyLength > 0) && (INVALID_KEY_LENGTH(xOddProtectedKeyLength)))
		|| ((xEvenProtectedKeyLength > 0) && (INVALID_KEY_LENGTH(xEvenProtectedKeyLength))) )
	{
		HxLOG_Info("Invalid key length (5)\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}
	else
	{
		HxLOG_Info("Invalid key length (5)\n");
		return DSC_ERROR_NOT_IMPLEMENTED;
	}

	if (local_caDsc_SetKeys(xDscChannelId, xOddProtectedKeyLength, pxOddProtectedKey,
							xEvenProtectedKeyLength, pxEvenProtectedKey) != DSC_NO_ERROR)
	{
		HxLOG_Info("error in dscSetKeys\n");
		return DSC_ERROR;
	}
#endif

	return DSC_NO_ERROR;
}


TDscStatus dscSet1LevelProtectedKeys(TDscChannelId xDscChannelId,
											TSize xL1ProtectingKeyLength, const TUnsignedInt8 *pxL1ProtectingKey,
											TSize xOddProtectedKeyLength, const TUnsignedInt8 *pxOddProtectedKey,
											TSize xEvenProtectedKeyLength, const TUnsignedInt8 *pxEvenProtectedKey)
{
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	HxLOG_Print("length (%d, %d, %d)\n", xL1ProtectingKeyLength, xOddProtectedKeyLength, xEvenProtectedKeyLength);

	if ( (pxL1ProtectingKey == NULL)
		|| (xL1ProtectingKeyLength == 0) )
		return DSC_ERROR_BAD_KEY_LENGTH;

	if ( (xL1ProtectingKeyLength > 0) && (INVALID_PROTECT_KEY_LENGTH(xL1ProtectingKeyLength)) )
	{
		HxLOG_Info("Invalid key length (4)\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

	if (xOddProtectedKeyLength == 0 || xEvenProtectedKeyLength == 0)
	{
		HxLOG_Info("Invalid key length (5)\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

	if ( ((xOddProtectedKeyLength > 0) && (INVALID_KEY_LENGTH(xOddProtectedKeyLength)))
		|| ((xEvenProtectedKeyLength > 0) && (INVALID_KEY_LENGTH(xEvenProtectedKeyLength))) )
	{
		HxLOG_Info("Invalid key length (6)\n");
		return DSC_ERROR_BAD_KEY_LENGTH;
	}

#if (CONFIG_PROTECT_CW == 1)
	if ( (xL1ProtectingKeyLength > 0) && (pxL1ProtectingKey != NULL) )
	{
		if (svc_cas_DevDscSetProtectionKey(xDscChannelId->hDscHandle, pxL1ProtectingKey) != ERR_OK)
		{
			HxLOG_Info("error in svc_cas_DevDscSetProtectionKey\n");
			return DSC_ERROR;
		}
	}

	if (xL1ProtectingKeyLength > 0)
	{
		TUnsignedInt16 i;
		TBoolean bAllnotZero = TRUE;

		for (i=0; i<xL1ProtectingKeyLength; i++)
		{
			if (pxL1ProtectingKey[i] != 0)
			{
				bAllnotZero = FALSE;
				break;
			}
		}

		if (bAllnotZero == TRUE)
		{
			if (svc_cas_DevDscClearProtectionKey(xDscChannelId->hDscHandle) != ERR_OK)
			{
				HxLOG_Info("error in svc_cas_DevDscClearProtectionKey\n");
				return DSC_ERROR;
			}
		}
	}

	if (local_caDsc_SetKeys(xDscChannelId, xOddProtectedKeyLength, pxOddProtectedKey,
							xEvenProtectedKeyLength, pxEvenProtectedKey) != DSC_NO_ERROR)
	{
		HxLOG_Info("error in dscSetKeys\n");
		return DSC_ERROR;
	}

	if (local_caDsc_SetProtectedKeys(xDscChannelId, TRUE) != DSC_NO_ERROR)
	{
		HxLOG_Info("local_caDsc_SetProtectedKeys : Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
	}
#endif
	return DSC_NO_ERROR;
}


TDscStatus dscSetMetadata(  TDscChannelId xDscChannelId, TSize xMetadataLength, const TUnsignedInt8 *pxMetadata)
{
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	// fix DAL Test error. dsc NO.8
	if ( (xMetadataLength > 0) && (pxMetadata == NULL) )
	{
		return DSC_ERROR;
	}

#if (CONFIG_DSC_METADATA == 1)
{
	TUnsignedInt8		i;
	TUnsignedInt8 		ucLength;
	TUnsignedInt8 		*pucMetaData;

	static TUnsignedInt8 	s_ucCopyData[2];

	if (local_caDsc_GetDescriptor(pxMetadata, xMetadataLength, (TUnsignedInt8)eTagCopyCtlInfo, &pucMetaData, &ucLength) == ERR_OK)
	{
		HxSTD_memset(s_ucCopyData, 0x00, 2);
		for (i=0; i<ucLength && i<2; i++)
		{
			s_ucCopyData[i] = pucMetaData[i];
			HxLOG_Print("cop data(%d, 0x%x)\n", i, s_ucCopyData[i]);
		}

		if (CA_NA_CopReceivedData(xDscChannelId->hAction, s_ucCopyData) != ERR_OK)
		{
			HxLOG_Info("error in CA_NA_CopReceivedData...(0x%x)\n", xDscChannelId);
			return DSC_ERROR;
		}
	}
}
#endif
	return DSC_NO_ERROR;
}


TDscStatus dscSetDecryptionEmi(TDscChannelId    xDscChannelId, TUnsignedInt16   xDecryptionEmi)
{
	if (xDscChannelId == NULL)
		return DSC_ERROR_UNKNOWN_ID;

	if (local_caDsc_FindChannelResource(xDscChannelId) != DSC_NO_ERROR)
	{
		HxLOG_Info("Unknown ID(0x%x)\n", (unsigned int)xDscChannelId);
		return DSC_ERROR_UNKNOWN_ID;
	}

	switch(xDecryptionEmi)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	case 0x4021:
#endif
	case 0x4023:		// AES-128 in CBC mode. Apple Live Streaming.
		break;
#endif
	default:
		// not Nagravision Standard
		HxLOG_Info("wrong EMI!! \n");
		return DSC_ERROR_NOT_IMPLEMENTED;
	}

	return DSC_NO_ERROR;
}


/**************************************************************************************************/
#define ________CA_DSC_Externel_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR CA_NA_DscInitialization(void)
{
	TUnsignedInt8 	i;
	TUnsignedInt32 	nSmid;

	if (VK_SEM_Create(&nSmid, "ca_dsc_sem", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Info("semaphore create error\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulDscChannelSemId = nSmid;
	}

	for (i=0; i<MAX_DSC_POOL; i++)
	{
		s_bDscUsedFlag[i] = FALSE;
		s_pstDscChannelId[i] = (TDscChannelId)OxCAS_Malloc(sizeof(SDscChannelId_t));
		HxSTD_memset(s_pstDscChannelId[i], 0x00, sizeof(SDscChannelId_t));
	}

	return ERR_OK;
}

/**************************************************************************************************/
#define ________CA_DSC_CSC_Pretest_ForCak6________________________________________________
/**************************************************************************************************/
#if (CONFIG_MW_CAS_NAGRA_PREDSC_TEST == 1)
#include <util.h>
#include <pal_sys.h>

#define USB_SUPPORT						0

#if (USB_SUPPORT == 1)
#include <pal_pvr.h>
#endif

#define MAX_PREDSC_DSC_POOL			60
#if defined(CONFIG_PROD_HD5400S)
#define MAX_CSCVECTOR_NUM				7
#define MAX_CSCDATA_NUM				7
#else
#define MAX_CSCVECTOR_NUM				5
#define MAX_CSCDATA_NUM				5
#endif
#define MAX_CSCVECTOR_SIZE				28
#define MAX_CSCDATA_SIZE				151
#define MAX_CSCVECTOR_KEY_SIZE		8
#define MAX_CSC_RECORD_SIZE			256


// USB가 지원되는 모델의 경우, Nagra에서 전달받은  CSC Data와 CSC Vector를  USB 메모리에 복사해서 Box에 꽂아서 테스트 하면 됨
// 경로는 USB 메모리의 최상위 디렉토리에  다음과 같이 나그라에서 전달받은 디렉토리 그대로 전체를 복사하면 됨
// => CSC Data/
// => Test Vectors/
#define PATH_CSC_DATA					"/media/drive1/CSC Data/"				// 모델별로 USB 메모리 Mount 위치가 다르면 변경해서 써야함
#define PATH_CSC_VECTOR				"/media/drive1/Test Vectors/"			// 모델별로 USB 메모리 Mount 위치가 다르면 변경해서 써야함

typedef enum
{
	CSC_OFFSET_NUID					= 4,
	CSC_OFFSET_KEY_LEN				= 16,
	CSC_OFFSET_KEY					= 17,
	CSC_OFFSET_MAX
} NaCscRecordOffset_t;

typedef enum
{
	CSCVECTOR_OFFSET_NUID			= 4,
	CSCVECTOR_OFFSET_EVEN_KEY		= 10,
	CSCVECTOR_OFFSET_ODD_KEY		= 18,
	CSCVECTOR_OFFSET_MAX
} NaCscVectorOffset_t;

typedef struct NaCscRecordInfo
{
	TBoolean		bValid;
	TUnsignedInt8	ulKeySize;
	TUnsignedInt8	aucCscRecord[MAX_CSC_RECORD_SIZE];
	TUnsignedInt8	aucCscKey[MAX_CSC_RECORD_SIZE];
} NA_CscRecord_Info_t;


typedef struct NaCscVectorInfo
{
	TBoolean		bValid;
	TUnsignedInt8	aucOddProtectedKey[MAX_CSCVECTOR_KEY_SIZE];
	TUnsignedInt8	aucEvenProtectedKey[MAX_CSCVECTOR_KEY_SIZE];
} NA_CscVector_Info_t;

typedef struct DscChannelInfo
{
	TBoolean				bUsed;
	TUnsignedInt32		ulActionId;
	TUnsignedInt32		ulDscIndex;
	TSignedInt32 			nDeviceId;
	SvcCas_DscDataType_e	eDscDataType;
	TUnsignedInt16		usElemPid;
} DSC_ChannelInfo_t;


static NA_CscRecord_Info_t	s_cscRecordInfo = {FALSE, 0, {0, }, {0,} };
static NA_CscVector_Info_t	s_cscVectorInfo = {FALSE, {0, }, {0,} };
static DSC_ChannelInfo_t	s_aDscChannel[MAX_PREDSC_DSC_POOL] = {{0,}, };

static TUnsignedInt8		s_aucCSCData[MAX_CSCDATA_NUM][MAX_CSCDATA_SIZE] =
{
	#if defined(CONFIG_PROD_HD5400S)
	{
		// 5411_89318734-89318734_04_00.D10
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x87, 0x34, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0xE8, 0x6C, 0x1A, 0x7E, 0xDE, 0xF5, 0x07, 0x9F, 0x74, 0xC7, 0xD5, 0xEA, 0xF2, 0xCF, 0x07,
		0x9A, 0x90, 0x0F, 0xB9, 0xD5, 0x02, 0x63, 0x73, 0x28, 0xE5, 0x2D, 0xF2, 0x43, 0x29, 0x6E, 0xE9,
		0x9E, 0x28, 0x06, 0x1C, 0xF6, 0xAE, 0xF7, 0x3B, 0x03, 0xC6, 0x73, 0x39, 0x12, 0x1F, 0x49, 0x7E,
		0x1E, 0x6A, 0xB3, 0x79, 0x28, 0xAE, 0x20, 0x4D, 0x0F, 0x1C, 0xB1, 0x5C, 0xF3, 0xBA, 0x7F, 0x5E,
		0x59, 0x24, 0xA2, 0xF4, 0x9E, 0xAF, 0xC8, 0xD2, 0xB6, 0x90, 0x01, 0x22, 0x6B, 0xDE, 0x10, 0x9E,
		0xC9, 0x33, 0x9A, 0xBE, 0xCD, 0x13, 0x5B, 0x58, 0x95, 0x2E, 0xFE, 0x15, 0xB0, 0x6D, 0xF6, 0x2F,
		0x3E, 0xB4, 0x94, 0x44, 0xBA, 0x42, 0xB2, 0x1F, 0xC1, 0x45, 0xA4, 0xFD, 0xD8, 0xFC, 0x16, 0x24,
		0x13, 0x7C, 0x40, 0xB9, 0xCF, 0xB0, 0xBD, 0x6D, 0xF8, 0x4A, 0x2E, 0xDB, 0x49, 0x94, 0x9C, 0x5A,
		0x1E, 0x30, 0xE1, 0xA8, 0x25, 0x1B, 0x4E
	},

	{
		// 5411_89318754-89318754_04_00.D10
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x87, 0x54, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0x13, 0xD8, 0x8F, 0xB6, 0x26, 0xFF, 0xFC, 0xD8, 0xA6, 0xA3, 0xF5, 0x07, 0xC3, 0x01, 0x2C,
		0xE4, 0xF6, 0x3B, 0xC8, 0x25, 0xC1, 0x66, 0x76, 0x38, 0xBB, 0x44, 0x86, 0xF0, 0x4F, 0xE8, 0xDA,
		0xA0, 0xB5, 0xA7, 0x2F, 0xC7, 0xDF, 0x06, 0x93, 0x22, 0x94, 0xE7, 0x0F, 0x72, 0xA5, 0x09, 0x5B,
		0x1A, 0xDF, 0x18, 0xA4, 0x0D, 0xDB, 0xB1, 0x71, 0x49, 0xC4, 0xD3, 0xAD, 0x9D, 0x18, 0x9C, 0x1F,
		0xCF, 0xD9, 0x3D, 0xBC, 0xA9, 0x07, 0x1E, 0xBB, 0xEC, 0x89, 0x25, 0x0F, 0xA0, 0x07, 0x97, 0x5C,
		0xF1, 0x28, 0x6E, 0x8E, 0x3F, 0x1E, 0xEB, 0x17, 0x19, 0xA0, 0x07, 0x67, 0xCE, 0xBD, 0x39, 0xDD,
		0x96, 0x04, 0x95, 0xF9, 0x2F, 0xD7, 0x5E, 0x08, 0xE5, 0xBD, 0xD1, 0xD3, 0x6C, 0xC8, 0x11, 0x57,
		0x5A, 0xAE, 0xEE, 0xAD, 0xF6, 0x7B, 0xBF, 0xBE, 0x91, 0x64, 0xF9, 0xAA, 0xD8, 0x52, 0xA5, 0x17,
		0xB0, 0x16, 0x2E, 0x06, 0x5B, 0xBE, 0x38
	},
	// 5411_893181D2_893181D2_04_00.D10
	{
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x81, 0xD2, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0xC9, 0xDB, 0x08, 0x8F, 0x06, 0xE1, 0x74, 0xD5, 0x5C, 0xFF, 0x33, 0x3A, 0xB6, 0xCA, 0x78,
		0x6C, 0xDF, 0xD0, 0x94, 0x8A, 0xA6, 0x2C, 0x15, 0x93, 0xC8, 0xDB, 0x8F, 0xEE, 0xF7, 0x85, 0x81,
		0xFE, 0x5C, 0x5E, 0x82, 0x21, 0xDE, 0xC4, 0x3B, 0x89, 0xCA, 0xD5, 0x9F, 0x4D, 0x08, 0x72, 0x98,
		0xB6, 0x81, 0x57, 0x2E, 0x55, 0xF9, 0xB3, 0x2F, 0x73, 0x85, 0xEF, 0xD0, 0xEA, 0x92, 0x49, 0xEC,
		0x8C, 0x3A, 0xA8, 0x30, 0xDD, 0x31, 0xE6, 0x24, 0x2D, 0xBC, 0x7D, 0x18, 0xD2, 0x28, 0xC0, 0x33,
		0x00, 0x13, 0x36, 0x53, 0xF4, 0xFD, 0x69, 0x8D, 0xCF, 0x0E, 0x2B, 0xB2, 0x60, 0x53, 0x72, 0x49,
		0x69, 0xA6, 0x28, 0x58, 0x28, 0x62, 0x5B, 0x64, 0x91, 0x89, 0xE7, 0x31, 0xA6, 0x5B, 0x4C, 0xAE,
		0x2F, 0x8A, 0xE1, 0x65, 0x13, 0xBF, 0x9F, 0x28, 0xDF, 0x2F, 0x6A, 0x42, 0xF7, 0xC3, 0xDA, 0x6E,
		0x9D, 0xE3, 0x29, 0x7E, 0x3E, 0x5A, 0x2E
	},

	// 5411_893181DA_893181DA_04_00.D10
	{
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x81, 0xDA, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0x7C, 0xD6, 0xE8, 0xFB, 0xAD, 0xEB, 0x76, 0x1B, 0xC1, 0x11, 0xEA, 0xC4, 0x04, 0x23, 0x57,
		0xBA, 0xC3, 0x85, 0x98, 0x4C, 0x4F, 0xA2, 0xE1, 0xB8, 0x20, 0xAA, 0x65, 0x80, 0x28, 0x5A, 0xE1,
		0xE9, 0x46, 0x1B, 0x7E, 0x21, 0x2E, 0x58, 0xDF, 0x15, 0xCC, 0xC9, 0xCD, 0x84, 0xDB, 0x3F, 0x18,
		0x8C, 0x94, 0xAF, 0x4E, 0x1C, 0x28, 0x50, 0x10, 0x4F, 0xD2, 0x1D, 0x97, 0x18, 0xB3, 0xB1, 0x22,
		0xF3, 0x49, 0x8A, 0xC3, 0xC6, 0xDF, 0x9D, 0x42, 0x25, 0x19, 0x56, 0xA0, 0x78, 0x78, 0x58, 0x6E,
		0x58, 0x61, 0xCF, 0x40, 0x48, 0xB5, 0x32, 0xD7, 0x09, 0x55, 0xDF, 0x26, 0xEE, 0xDD, 0x8F, 0x16,
		0x7C, 0x74, 0xBE, 0x40, 0xBE, 0x03, 0xA7, 0x97, 0x3F, 0x98, 0xAD, 0x9F, 0xA1, 0xD6, 0x2E, 0xBA,
		0x34, 0xD0, 0x01, 0x6E, 0x01, 0x95, 0x14, 0xB0, 0xDB, 0x36, 0xAA, 0xA9, 0x03, 0x83, 0x0D, 0x6A,
		0x39, 0xD0, 0x09, 0xBD, 0x3E, 0x95, 0x5A
	},

	//5411_893181DB_893181DB_04_00.D10
	{
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x81, 0xDB, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0x51, 0x4A, 0x6F, 0xF6, 0x8D, 0x5E, 0x9B, 0x01, 0x35, 0x3C, 0x2A, 0xDF, 0xA8, 0x4D, 0x0C,
		0xFF, 0x53, 0xE0, 0xE1, 0x5D, 0x53, 0xD2, 0x6C, 0xD8, 0xFE, 0x55, 0x38, 0xBD, 0x0C, 0x29, 0x87,
		0x4A, 0xD1, 0x41, 0x86, 0x8E, 0x65, 0x91, 0x2B, 0x27, 0x1F, 0xA7, 0x46, 0xC3, 0x94, 0x1F, 0x2E,
		0x8A, 0x76, 0x64, 0xCA, 0x7C, 0x47, 0xD0, 0x62, 0xA6, 0xA6, 0x13, 0x59, 0x9B, 0x89, 0xC4, 0xF5,
		0x0B, 0x47, 0xE1, 0x62, 0x5D, 0x20, 0xAE, 0xD0, 0x70, 0x1A, 0x08, 0x10, 0xAB, 0xEB, 0x50, 0x53,
		0x4E, 0x72, 0xE1, 0x59, 0x81, 0x66, 0x8F, 0x01, 0xE2, 0x80, 0x79, 0x24, 0xEA, 0x0F, 0x60, 0xCC,
		0x84, 0x98, 0xB9, 0xA6, 0x78, 0x6D, 0xBE, 0xE9, 0x60, 0xBF, 0x88, 0xE2, 0x36, 0x8A, 0xC7, 0xA9,
		0x77, 0x7D, 0x6C, 0x04, 0x55, 0x9A, 0xD1, 0x04, 0x0D, 0x86, 0x50, 0x6D, 0xF0, 0x13, 0x02, 0x9E,
		0x86, 0x4D, 0xD4, 0xE0, 0x5C, 0x8C, 0x87
	},

	// 5411_89318100_89318100_04_00.D10
	{
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x81, 0x00, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0x73, 0x46, 0x84, 0x6E, 0x73, 0x5A, 0x20, 0xC2, 0xC2, 0xD1, 0xDD, 0x59, 0x50, 0x75, 0xEC,
		0xBA, 0x00, 0x3E, 0xC7, 0xB9, 0x82, 0x31, 0x32, 0x4B, 0x5B, 0x1D, 0xB9, 0x4E, 0x97, 0x55, 0x88,
		0x17, 0xAE, 0x59, 0x55, 0x46, 0xF7, 0xA5, 0xEE, 0xD0, 0xBD, 0x3D, 0x38, 0x03, 0x00, 0xD6, 0x36,
		0x43, 0x1D, 0x3A, 0x23, 0x65, 0x75, 0x82, 0x18, 0xEA, 0xFE, 0x21, 0x87, 0x40, 0x9B, 0x44, 0x19,
		0x5C, 0xDD, 0x9F, 0x51, 0x94, 0x31, 0xC4, 0x0F, 0x8B, 0x89, 0x84, 0x80, 0xC6, 0x13, 0x28, 0xD5,
		0xA5, 0x23, 0x41, 0xB9, 0x0A, 0x83, 0x58, 0x3E, 0xA1, 0xD1, 0x21, 0x93, 0xE8, 0xE7, 0x70, 0x05,
		0x98, 0xB3, 0xD5, 0x7E, 0x9A, 0x4E, 0x8D, 0x46, 0xE2, 0xD9, 0x77, 0x32, 0x92, 0xF6, 0xF9, 0xD3,
		0xF1, 0x91, 0x96, 0xB9, 0x3A, 0x5A, 0xE5, 0xD5, 0xD2, 0xCB, 0x95, 0x12, 0x62, 0x11, 0x90, 0x0C,
		0xB8, 0xD9, 0xC5, 0xBF, 0x3B, 0xB7, 0x10
	},

	// 5411_89318117_89318117_04_00.D10
	{
		0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x81, 0x17, 0x00, 0x01, 0x54, 0x11, 0x00, 0x00, 0x01, 0x81,
		0x10, 0x36, 0x43, 0xD4, 0x92, 0xF1, 0x0C, 0x3D, 0x3D, 0x58, 0xD1, 0xB8, 0xDB, 0xAF, 0xC4, 0xDA,
		0x33, 0x74, 0xF3, 0xA4, 0x8D, 0xD2, 0xAB, 0x7B, 0xBB, 0xD6, 0x2C, 0xFC, 0xE6, 0x50, 0x57, 0x69,
		0x99, 0x00, 0x8B, 0xD6, 0x9A, 0x6F, 0xE4, 0x57, 0xE1, 0x51, 0x09, 0x3A, 0x22, 0x48, 0x5B, 0x83,
		0x84, 0x03, 0x11, 0xAA, 0x77, 0x20, 0x4F, 0xB6, 0xD4, 0x2A, 0x33, 0xB0, 0x70, 0xF4, 0xBD, 0xDA,
		0x11, 0x38, 0xF0, 0x3F, 0x32, 0x12, 0xF4, 0x6C, 0x2E, 0x4A, 0x59, 0xC5, 0xC7, 0x51, 0xAD, 0x54,
		0x40, 0xD0, 0x85, 0xE2, 0xEA, 0x91, 0x0C, 0x7D, 0xFA, 0x3E, 0x2C, 0xBF, 0x45, 0x87, 0x54, 0x41,
		0x26, 0x6F, 0x3B, 0xD9, 0xDE, 0x57, 0x6D, 0x53, 0x3F, 0xFD, 0x8C, 0xE8, 0xEF, 0xA1, 0xDD, 0x1D,
		0x24, 0xCC, 0x23, 0xFB, 0x96, 0x08, 0x85, 0xBC, 0x6E, 0xFA, 0xB5, 0x96, 0x9A, 0x97, 0x42, 0xC4,
		0x7A, 0xD7, 0x1B, 0xC6, 0x7D, 0x93, 0xB9
	}
	#elif defined(CONFIG_HD5500T)
	{
	// 8D11_89C7E76D-89C7E76D_01_00.DS10
	0x00, 0x00, 0x00, 0x97, 0x89, 0xC7, 0xE7, 0x6D, 0x00, 0x01, 0x8D, 0x11, 0x00, 0x00, 0x01, 0x81,
	0x10, 0x77, 0x25, 0x0B, 0xD2, 0xAC, 0x32, 0xDD, 0x39, 0x23, 0xD2, 0xBD, 0x96, 0xE1, 0x4F, 0x34,
	0xE7, 0xB5, 0xF8, 0x7D, 0xCE, 0x2C, 0x51, 0xC5, 0xF2, 0x71, 0x09, 0x5D, 0x1A, 0x22, 0x42, 0x15,
	0x7E, 0x8A, 0x42, 0x28, 0xB4, 0x51, 0xEC, 0x78, 0x75, 0xFD, 0x58, 0x5C, 0xD8, 0x4C, 0x1E, 0x72,
	0x1D, 0x95, 0x50, 0xE8, 0x4B, 0x81, 0x98, 0xAB, 0x44, 0xEF, 0x5D, 0x77, 0x73, 0xE5, 0x0E, 0x81,
	0xAA, 0xE7, 0x7A, 0x46, 0xEA, 0xD0, 0xB9, 0xBE, 0xAD, 0x4F, 0x41, 0x6D, 0xF4, 0x1A, 0x66, 0x8E,
	0x04, 0x99, 0x7E, 0xF1, 0xE6, 0xC5, 0xCA, 0x41, 0x3D, 0xB7, 0x4B, 0xB3, 0xA5, 0x5F, 0x7A, 0x49,
	0xFD, 0x5D, 0x6E, 0x03, 0x83, 0xDD, 0xE7, 0xC3, 0xE2, 0xE6, 0xF2, 0x43, 0x2D, 0xE0, 0x7A, 0xF7,
	0x58, 0x3A, 0x81, 0x86, 0x27, 0x54, 0xCA, 0x17, 0x6E, 0xCC, 0x0D, 0x77, 0xEF, 0xA2, 0x0E, 0x89,
	0x85, 0xF4, 0x06, 0xF9, 0xFB, 0x92, 0x63
	},

	{
	// 8D11_89314A15-89314A15_ManufacturingTest.X10
	0x00, 0x00, 0x00, 0x97, 0x89, 0x31, 0x4a, 0x15, 0x00, 0x01, 0x8d, 0x11, 0x00, 0x00, 0x01, 0x81,
	0x10, 0xfd, 0xab, 0xb8, 0xb9, 0xdf, 0xcc, 0x68, 0x80, 0x9e, 0x21, 0xc3, 0x76, 0x67, 0x49, 0x0f,
	0xe1, 0xbb, 0x7c, 0xdd, 0xf3, 0x93, 0xae, 0x86, 0x47, 0x9e, 0xc8, 0xb7, 0x13, 0x44, 0x20, 0xf7,
	0x38, 0xfe, 0x95, 0x77, 0x2d, 0x2c, 0xf8, 0x5d, 0xfe, 0x47, 0xd8, 0xaf, 0xeb, 0xc6, 0xd8, 0x25,
	0x58, 0x60, 0x5f, 0x69, 0x7d, 0x24, 0x87, 0x3e, 0xbe, 0xc3, 0xc3, 0x50, 0x88, 0x48, 0xcc, 0x77,
	0xbd, 0x3d, 0x3d, 0xef, 0xf5, 0x9f, 0x49, 0xfc, 0x6a, 0x53, 0x2e, 0x2b, 0x94, 0xff, 0xc7, 0x91,
	0x58, 0x67, 0xab, 0x8c, 0xc6, 0xc4, 0xf9, 0x54, 0x74, 0xc4, 0xb4, 0xf3, 0x5d, 0xfd, 0x89, 0x1e,
	0x58, 0xc2, 0x93, 0xf8, 0xcd, 0x34, 0x2b, 0x23, 0x20, 0x48, 0x5c, 0x1e, 0xd6, 0x02, 0x28, 0x5c,
	0x9e, 0xe6, 0xe6, 0x13, 0xfb, 0x5f, 0x1c, 0x4c, 0xbd, 0x69, 0x84, 0xed, 0x32, 0x89, 0xfc, 0xbc,
	0xf8, 0x16, 0xc9, 0xd7, 0x1b, 0x7b, 0x31
	},

	{ 0x00,


	},

	{ 0x00,


	},

	{ 0x00,


	}
	#endif
};


static TUnsignedInt8		s_aucVector[MAX_CSCVECTOR_NUM][MAX_CSCVECTOR_SIZE] =
#if (1)
{
#if defined(CONFIG_PROD_HD5400S)
	// 5411_89318734-89318734_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1C, 0x89, 0x31, 0x87, 0x34, 0x01, 0x00, 0xF4, 0x46, 0x77, 0x94, 0x61, 0x27,
	0x56, 0x77, 0x85, 0x19, 0xC8, 0xE8, 0x4E, 0xDB, 0x91, 0xDC, 0xA5, 0x88
	},

	// 5411_89318754-89318754_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1C, 0x89, 0x31, 0x87, 0x54, 0x01, 0x00, 0x4D, 0xBE, 0x8F, 0xA2, 0x77, 0xCE,
	0x21, 0x6B, 0x5B, 0x9F, 0xAB, 0x72, 0xB9, 0xA4, 0xF9, 0x7A, 0x18, 0x56
	},

	// 5411_893181D2_893181D2_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x81, 0xD2, 0x01, 0x00, 0x79, 0x68, 0xA3, 0x84, 0x94, 0xF4,
	0x0E, 0xF1, 0xA9, 0xBB, 0x34, 0xAD, 0x78, 0x0A, 0x07, 0x18, 0x5F, 0xCD
	},

	// 5411_893181DA_893181DA_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x81, 0xDA, 0x01, 0x00, 0xDD, 0xF2, 0xAD, 0x83, 0xFA, 0x8E,
	0xFB, 0x5F, 0xD4, 0x1A, 0xB2, 0xD5, 0xC8, 0x4D, 0x1E, 0x30, 0x7B, 0x60
	},

	//5411_893181DB_893181DB_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x81, 0xDB, 0x01, 0x00, 0x43, 0xCD, 0x42, 0xFA, 0xD4, 0x6A,
	0x60, 0xDB, 0xA7, 0xF8, 0xA9, 0xC6, 0xC4, 0x4B, 0xFA, 0x75, 0x6C, 0x7D
	},

	// 5411_89318100_89318100_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x81, 0x00, 0x01, 0x00, 0x14, 0x3A, 0x84, 0xF6, 0x1B, 0x70,
	0xDE, 0x40, 0x2C, 0xC5, 0xA6, 0xD2, 0x51, 0xF5, 0xDD, 0x0F, 0x22, 0x8D
	},

	// 5411_89318117_89318117_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x81, 0x17, 0x01, 0x00, 0xF8, 0xCB, 0x21, 0xBA, 0x4E, 0x49,
	0xB9, 0xC9, 0xBB, 0x89, 0xDB, 0x6E, 0x83, 0xA0, 0xF1, 0x6A, 0x82, 0xF1
	}
#elif defined(CONFIG_HD5500T)
#if 1
	// 8D11_89C7E76D-89C7E76D_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1C, 0x89, 0xC7, 0xE7, 0x6D, 0x01, 0x00, 0x53, 0x80, 0x77, 0x8C, 0x9C, 0xE6,
	0xA9, 0xE5, 0x5A, 0x6B, 0xA9, 0xC0, 0x7E, 0xA0, 0xCF, 0x98, 0xC3, 0xD0
	},
#else
	// 8D11_89BD2B2D-89BD2B2D_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0xbd, 0x2b, 0x2d, 0x01, 0x00, 0xc9, 0xe8, 0xcf, 0x5e, 0xa7, 0x1b,
	0x25, 0xeb, 0xe1, 0x18, 0xf0, 0x2a, 0x6e, 0x0c, 0x62, 0x1d, 0x0f, 0xce
	},
#endif
	// 8D11_89BD2DDE-89BD2DDE_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0xbd, 0x2d, 0xde, 0x01, 0x00, 0x1e, 0x85, 0x17, 0x6e, 0x1f, 0xfc,
	0x7c, 0x66, 0x5b, 0x72, 0xf3, 0x8f, 0x46, 0xac, 0x6c, 0x6a, 0xc8, 0xef
	},

	//8D11_89BD32BF-89BD32BF_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0xbd, 0x32, 0xbf, 0x01, 0x00, 0xe9, 0x44, 0x4f, 0xfe, 0x8c, 0xed,
	0x3d, 0x4b, 0x0e, 0x0c, 0x5f, 0x0d, 0x03, 0xe4, 0x4c, 0x80, 0x61, 0xb3
	},

	// 8D11_89314A15-89314A15_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x4a, 0x15, 0x01, 0x00, 0xcd, 0xcb, 0x18, 0xdb, 0xd0, 0x44,
	0x0b, 0x7c, 0x5b, 0xaa, 0x09, 0x6c, 0xfa, 0x9f, 0x25, 0x4d, 0xdb, 0x61
	},

	// 8D11_89314B78-89314B78_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x89, 0x31, 0x4b, 0x78, 0x01, 0x00, 0x83, 0xe6, 0x7f, 0xd5, 0xb2, 0xc0,
	0x80, 0x92, 0x6f, 0x34, 0x21, 0xf4, 0xbc, 0x9b, 0xeb, 0xcb, 0x35, 0x5d
	}
#else
	// 6E11_83AE1982-83AE1982_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x83, 0xae, 0x19, 0x82, 0x01, 0x00, 0x2c, 0xac, 0x3b, 0xfe, 0x73, 0xd1,
	0x51, 0x84, 0x8e, 0xd2, 0xfd, 0xca, 0x52, 0x79, 0x24, 0x60, 0x91, 0x81
	},

	// 6E11_83AE243A-83AE243A_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x83, 0xae, 0x24, 0x3a, 0x01, 0x00, 0x0f, 0x14, 0xbe, 0x6b, 0x2a, 0xe3,
	0xfa, 0x03, 0x7c, 0xc3, 0xf6, 0xdc, 0x63, 0x29, 0x78, 0xe4, 0x40, 0xce
	},

	//6E11_83AE55F0-83AE55F0_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x83, 0xae, 0x55, 0xf0, 0x01, 0x00, 0x9a, 0x5d, 0x08, 0x6d, 0xdb, 0x86,
	0x27, 0xc1, 0x80, 0xb4, 0xd4, 0x5f, 0xd4, 0xeb, 0xf5, 0xb7, 0x88, 0x28
	},

	// 6E11_83AE1999-83AE1999_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x83, 0xae, 0x19, 0x99, 0x01, 0x00, 0x91, 0xde, 0x85, 0xb6, 0xd6, 0xd8,
	0x57, 0x25, 0xf9, 0x45, 0xe1, 0x50, 0x4d, 0xfa, 0x50, 0x46, 0x80, 0x2e
	},

	// 6E11_83AE1989-83AE1989_ManufacturingTest.X10
	{
	0x00, 0x00, 0x00, 0x1c, 0x83, 0xae, 0x19, 0x89, 0x01, 0x00, 0xb4, 0x8c, 0x5a, 0xe8, 0x18, 0x25,
	0x79, 0x2f, 0xc0, 0xf4, 0xf2, 0x71, 0x6a, 0x53, 0xc3, 0x8d, 0xcc, 0xdb
	}
#endif
};
#else
{
	{ 0x00,



	},

	{ 0x00,


	},

	{ 0x00,


	},

	{ 0x00,


	},

	{ 0x00,


	}
};
#endif


static HERROR local_preDsc_allocDsc(DSC_ChannelInfo_t **ppDscChannel)
{
	HERROR				hError = ERR_FAIL;
	TSignedInt16		usIndex = 0;
	DSC_ChannelInfo_t	*pFreeDscChannel = NULL;

	HxLOG_Error("[local_preDsc_allocDsc] +!! \n");

	if (ppDscChannel == NULL)
	{
		HxLOG_Error("[local_preDsc_allocDsc] param fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	*ppDscChannel = NULL;

	for (usIndex = 0; usIndex < MAX_PREDSC_DSC_POOL; usIndex++)
	{
		if (!s_aDscChannel[usIndex].bUsed)
		{
			pFreeDscChannel = &s_aDscChannel[usIndex];
			HxSTD_memset(pFreeDscChannel, 0x00, sizeof(DSC_ChannelInfo_t));
			break;
		}
	}

	*ppDscChannel = pFreeDscChannel;

	hError = ERR_OK;
	HxLOG_Error("[local_preDsc_allocDsc] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_openDsc(Handle_t hAction, SvcCas_DscDataType_e eDscDataType, DSC_ChannelInfo_t **ppDscChannel)
{
	HERROR				hError = ERR_FAIL;
	DSC_ChannelInfo_t	*pNewDscChannel = NULL;
	TSignedInt32		nDeviceId = 0;
	TUnsignedInt32		ulDscIndex = 0;

	HxLOG_Error("[local_preDsc_openDsc] +!! \n");

	if (ppDscChannel == NULL)
	{
		HxLOG_Error("[local_preDsc_openDsc] param fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	*ppDscChannel = NULL;

	hError = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, (int*)&nDeviceId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_preDsc_openDsc] svc_cas_SubSvcRmGetResourceId fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = svc_cas_DevDscOpen((unsigned int)nDeviceId, eCAS_DSC_TYPE_DVB, eDscDataType, &ulDscIndex);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_preDsc_openDsc] svc_cas_DevDscOpen fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = local_preDsc_allocDsc(&pNewDscChannel);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_preDsc_openDsc] local_preDsc_allocDsc fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}
	HxLOG_Info("[local_preDsc_openDsc] device id:%d, stream type:0x%x, dsc handle:0x%x\n",
										(int)nDeviceId,
										eDscDataType,
										(unsigned int)ulDscIndex));

	pNewDscChannel->eDscDataType = eDscDataType;
	pNewDscChannel->ulDscIndex = ulDscIndex;
	pNewDscChannel->ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	pNewDscChannel->nDeviceId = nDeviceId;
	pNewDscChannel->bUsed = TRUE;

	*ppDscChannel = pNewDscChannel;

	hError = ERR_OK;
	HxLOG_Error("[local_preDsc_openDsc] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_setDscPid(DSC_ChannelInfo_t *pDscChannel, TUnsignedInt16 usPid)
{
	HERROR	hError = ERR_FAIL;

	HxLOG_Error("[local_preDsc_setDscPid] +!! \n");

	if (pDscChannel == NULL)
	{
		HxLOG_Error("[local_preDsc_setDscPid] param fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = svc_cas_DevDscSetPid(pDscChannel->ulDscIndex, usPid);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[local_preDsc_setDscPid] svc_cas_DevDscSetPid fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = ERR_OK;

	HxLOG_Error("[local_preDsc_setDscPid] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_setDscKey(	DSC_ChannelInfo_t *pDscChannel,
															TUnsignedInt32 ulOddKeyLength,
															TUnsignedInt8 *pucOddKey,
															TUnsignedInt32 ulEvenKeyLength,
															TUnsignedInt8 *pucEvenKey)
{
	HERROR hError = ERR_FAIL;

	HxLOG_Error("[local_preDsc_setDscKey] +!! \n");

	if (pDscChannel == NULL)
	{
		HxLOG_Error("[local_preDsc_setDscKey] param fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	if (ulOddKeyLength > 0)
	{
		if (pucOddKey == NULL)
		{
			HxLOG_Error("[local_preDsc_setDscKey] pucOddKey == NULL!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = svc_cas_DevDscSetKey(pDscChannel->ulDscIndex, eCAS_DSC_PARITY_ODD, pucOddKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[local_preDsc_setDscPid] svc_cas_DevDscSetKey fail!! \n");
		}
	}

	if (ulEvenKeyLength > 0)
	{
		if (pucEvenKey == NULL)
		{
			HxLOG_Error("[local_preDsc_setDscKey] pucEvenKey == NULL!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = svc_cas_DevDscSetKey(pDscChannel->ulDscIndex, eCAS_DSC_PARITY_EVEN, pucEvenKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[local_preDsc_setDscPid] svc_cas_DevDscSetKey fail!! \n");
		}
	}

	hError = ERR_OK;

	HxLOG_Error("[local_preDsc_setDscKey] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_setDsc1LevelProtectedKeys(DSC_ChannelInfo_t *pDscChannel,
															TUnsignedInt32 ulL1ProtectingKeyLength,
															TUnsignedInt8 *pucL1ProtectingKey,
															TUnsignedInt32 ulOddKeyLength,
															TUnsignedInt8 *pucOddKey,
															TUnsignedInt32 ulEvenKeyLength,
															TUnsignedInt8 *pucEvenKey)
{
	HERROR	hError = ERR_FAIL;

	HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] +!! \n");

	if (pDscChannel == NULL)
	{
		HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] param fail!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	if (ulL1ProtectingKeyLength > 0)
	{
		if (pucL1ProtectingKey == NULL)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] pucL1ProtectingKey == NULL!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = svc_cas_DevDscSetProtectionKey(pDscChannel->ulDscIndex, pucL1ProtectingKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] svc_cas_DevDscSetProtectionKey fail!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}

	if (ulOddKeyLength > 0)
	{
		if (pucOddKey == NULL)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] pucOddKey == NULL!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = svc_cas_DevDscSetKey(pDscChannel->ulDscIndex, eCAS_DSC_PARITY_ODD, pucOddKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] svc_cas_DevDscOpen fail!! \n");
		}
	}

	if (ulEvenKeyLength > 0)
	{
		if (pucEvenKey == NULL)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] pucEvenKey == NULL!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = svc_cas_DevDscSetKey(pDscChannel->ulDscIndex, eCAS_DSC_PARITY_EVEN, pucEvenKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] svc_cas_DevDscOpen fail!! \n");
		}
	}

	hError = ERR_OK;

	HxLOG_Error("[local_preDsc_setDsc1LevelProtectedKeys] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_closeAllDsc(void)
{
	HERROR 			hError = ERR_FAIL;
	TSignedInt16 	usIndex = 0;

	HxLOG_Error("[local_preDsc_closeAllDsc] +!! \n");

	for (usIndex = 0; usIndex < MAX_PREDSC_DSC_POOL; usIndex++)
	{
		if (s_aDscChannel[usIndex].bUsed)
		{
			HxLOG_Error("[local_preDsc_closeAllDsc] svc_cas_DevDscClose call!! \n");
			hError = svc_cas_DevDscClose(s_aDscChannel[usIndex].ulDscIndex);
			if (hError != ERR_OK)
			{
				HxLOG_Error("[local_preDsc_closeAllDsc] svc_cas_DevDscClose fail!! \n");
			}
			s_aDscChannel[usIndex].bUsed = FALSE;
		}
	}

	hError = ERR_OK;
	HxLOG_Error("[local_preDsc_closeAllDsc] -!! \n");

EXIT_FUNC :

	return hError;
}


static HERROR local_preDsc_LoadCSCKey(TUnsignedInt8 *pucPathName, TUnsignedInt8 *pucCscKey, TUnsignedInt32 *pulKeySize)
{
	HERROR 			ulRet = ERR_OK;

	TUnsignedInt32	i;
	TUnsignedInt32	ulListNum = 0;
	TUnsignedInt32 	ulSysNuid = 0;
	TUnsignedInt32 	ulCscNuid = 0;
	TUnsignedInt32 	ulKeySize = 0;

	void			**list_data;
	TUnsignedInt8	aucFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	TUnsignedInt8	aucFileNameWithPath[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	TUnsignedInt32	ulFileHandle, ulReadSize = 0, ulDataSize = 0;
	TUnsignedInt8	aucCscData[MAX_CSC_RECORD_SIZE];

	if ( (pucPathName == NULL) || (pucCscKey == NULL) || (pulKeySize==NULL) )
		return ERR_FAIL;

	HxLOG_Info("Path Name : %s\n", pucPathName);

	ulRet = CARM_FILE_StartScanDir (pucPathName, NULL, &ulListNum, &list_data);
	if ( (ulRet != ERR_OK) || (ulListNum == 0) )
	{
		HxLOG_Error("scan fail or no item. ulRet=0x%X, item_num(%d)\n", ulRet, ulListNum);
		return ERR_FAIL;
	}

	ulRet = svc_cas_DevPortSysGetChipId(&ulSysNuid);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysGetChipId fail : 0x%X \n", ulRet);
		return ERR_FAIL;
	}

	HxLOG_Info("----- items (%d) -----\n", ulListNum);
	for (i = 0; i < ulListNum; i++)
	{
		HxSTD_memset(aucFileName, 0x00, PAL_PVR_MAX_FILE_NAME_LENGTH);
		ulRet = CARM_FILE_GetScanDir (i, list_data, aucFileName );
		if (ulRet != ERR_OK) continue;

		HxSTD_memset(aucFileNameWithPath, 0x00, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
		snprintf(aucFileNameWithPath, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s", pucPathName, aucFileName);
		HxLOG_Info(" %03d: %s\n", i, aucFileName);

		ulRet = CARM_PVR_OpenFile(CARM_FS_MODE_READ, aucFileNameWithPath, &ulFileHandle);
		if (ulRet != ERR_OK)
		{
			ulRet = svc_cas_DevFsOpen(0, (char*)aucFileNameWithPath, CARM_FS_MODE_READ, &ulFileHandle);
			if (ulRet != ERR_OK)
			{
				HxLOG_Error("fail to read the file (%s). err = 0x%x\n", aucFileName, ulRet);
				continue;
			}
		}

		/* Only for regular files, below calls are valid, so directory ignored */
		if (svc_cas_DevFsIsFileExist(aucFileNameWithPath) != TRUE)
		{
			CARM_PVR_CloseFile(ulFileHandle);
			continue;
		}

		ulDataSize = MAX_CSC_RECORD_SIZE;
		ulReadSize = 0;
		HxSTD_memset( aucCscData, 0, MAX_CSC_RECORD_SIZE );
		ulRet = CARM_PVR_ReadFile(ulFileHandle, (TUnsignedInt8 *)aucCscData, ulDataSize, &ulReadSize);
		if (ulReadSize == 0)
		{
			HxLOG_Error("fail to read (%s) read_size(%d) err = 0x%x\n", aucFileName, ulReadSize, ulRet);
			CARM_PVR_CloseFile(ulFileHandle);
			continue;
		}

		ulCscNuid = (TUnsignedInt32)get32bit( &(aucCscData[CSC_OFFSET_NUID]) );
		HxLOG_Info("%d) ulCscNuid(%x) ulSysNuid(%x)!! \n", i, ulCscNuid, ulSysNuid);
		if (ulCscNuid == ulSysNuid)
		{
			ulKeySize = (TUnsignedInt32)aucCscData[CSC_OFFSET_KEY_LEN];
			HxSTD_memcpy(pucCscKey, &aucCscData[CSC_OFFSET_KEY], ulKeySize);
			*pulKeySize = ulKeySize;

			HxLOG_Info("Found CSC Data. NUID:0x%X, ulKeySize:%d\n", ulSysNuid, ulKeySize);

			CARM_PVR_CloseFile(ulFileHandle);
			ulRet = CARM_FILE_ReleaseScanDir (ulListNum, list_data);
			return ERR_OK;
		}

		CARM_PVR_CloseFile(ulFileHandle);
	}

	ulRet = CARM_FILE_ReleaseScanDir (ulListNum, list_data);
	return ERR_FAIL;
}


static HERROR local_preDsc_LoadCSCVector(TUnsignedInt8 *pucPathName, TUnsignedInt8 *pucEvenKey, TUnsignedInt8 *pucOddKey)
{
	HERROR 			ulRet = ERR_OK;
	TUnsignedInt32	i;
	TUnsignedInt32	ulListNum = 0;
	TUnsignedInt32 	ulSysNuid = 0;
	TUnsignedInt32 	ulCscNuid = 0;

	void			**list_data;
	TUnsignedInt8	aucFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	TUnsignedInt8	aucFileNameWithPath[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	TUnsignedInt32	ulFileHandle, ulReadSize = 0, ulDataSize = 0;
	TUnsignedInt8	aucCscVector[MAX_CSCVECTOR_SIZE];

	if( (pucPathName == NULL) || (pucEvenKey == NULL) || (pucOddKey==NULL))
		return ERR_FAIL;

	HxLOG_Info("Path Name : %s\n", pucPathName);

	ulRet = CARM_FILE_StartScanDir (pucPathName, NULL, &ulListNum, &list_data);
	if ((ulRet != ERR_OK) || (ulListNum == 0))
	{
		HxLOG_Error("scan fail or no item. ulRet=0x%X, item_num(%d)\n", ulRet, ulListNum);
		return ERR_FAIL;
	}

	ulRet = svc_cas_DevPortSysGetChipId(&ulSysNuid);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysGetChipId fail : 0x%X \n", ulRet);
		return ERR_FAIL;
	}

	HxLOG_Info("----- items (%d) -----\n", ulListNum);
	for (i = 0; i < ulListNum; i++)
	{
		HxSTD_memset(aucFileName, 0x00, PAL_PVR_MAX_FILE_NAME_LENGTH);
		ulRet = CARM_FILE_GetScanDir (i, list_data, aucFileName );
		if (ulRet != ERR_OK) continue;

		HxSTD_memset(aucFileNameWithPath, 0x00, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
		snprintf(aucFileNameWithPath, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s", pucPathName, aucFileName);
		HxLOG_Info(" %03d: %s\n", i, aucFileName);

		ulRet = CARM_PVR_OpenFile(CARM_FS_MODE_READ, aucFileNameWithPath, &ulFileHandle);
		if (ulRet != ERR_OK)
		{
			ulRet = svc_cas_DevFsOpen(0, (char*)aucFileNameWithPath, CARM_FS_MODE_READ, &ulFileHandle);
			if (ulRet != ERR_OK)
			{
				HxLOG_Error("fail to read the file (%s). err = 0x%x\n", aucFileName, ulRet);
				continue;
			}
		}

		/* Only for regular files, below calls are valid, so directory ignored */
		if (svc_cas_DevFsIsFileExist(aucFileNameWithPath) != TRUE)
		{
			CARM_PVR_CloseFile(ulFileHandle);
			continue;
		}

		ulDataSize = MAX_CSCVECTOR_SIZE;
		ulReadSize = 0;
		HxSTD_memset(aucCscVector, 0, MAX_CSCVECTOR_SIZE );
		ulRet = CARM_PVR_ReadFile(ulFileHandle, (TUnsignedInt8 *)aucCscVector, ulDataSize, &ulReadSize);
		if (ulReadSize == 0)
		{
			HxLOG_Error("fail to read (%s) read_size(%d) err = 0x%x\n", aucFileName, ulReadSize, ulRet);
			CARM_PVR_CloseFile(ulFileHandle);
			continue;
		}

		ulCscNuid = (TUnsignedInt32)get32bit( &(aucCscVector[CSCVECTOR_OFFSET_NUID]) );
		HxLOG_Info("%d) ulCscNuid(%x) ulSysNuid(%x)!! \n", i, ulCscNuid, ulSysNuid);
		if (ulCscNuid == ulSysNuid)
		{
			HxSTD_memcpy(pucEvenKey, &aucCscVector[CSCVECTOR_OFFSET_EVEN_KEY], MAX_CSCVECTOR_KEY_SIZE);
			HxSTD_memcpy(pucOddKey, &aucCscVector[CSCVECTOR_OFFSET_ODD_KEY], MAX_CSCVECTOR_KEY_SIZE);

			HxLOG_Info("Found CSC Vector. NUID:0x%X, ulKeySize:%d\n", ulSysNuid, MAX_CSCVECTOR_KEY_SIZE);

			CARM_PVR_CloseFile(ulFileHandle);
			ulRet = CARM_FILE_ReleaseScanDir (ulListNum, list_data);
			return ERR_OK;
		}

		CARM_PVR_CloseFile(ulFileHandle);
	}

	ulRet = CARM_FILE_ReleaseScanDir (ulListNum, list_data);
	return ERR_FAIL;
}


HERROR CA_NA_PreDsc_SetPidForPreDescramble(
							Handle_t hAction,
							TUnsignedInt16 usVideoPid,
							TUnsignedInt16 usAudioPid
							)
{
	HERROR 				hError = ERR_FAIL;
	DSC_ChannelInfo_t	*pDscChannel = NULL;

	HxLOG_Info("hAction(%x), vid(%x), aud(%x)\n", hAction, usVideoPid, usAudioPid);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("hAction == HANDLE_NULL!! \n");
		hError = ERR_FAIL;
		goto EXIT_FUNC;
	}

	hError = local_preDsc_closeAllDsc();
	if (hError != ERR_OK)
	{
		HxLOG_Error("local_preDsc_closeAllDsc fail!! \n");
	}

/*********************************************************************
  *
  * Nagra CscDataUseTest010100.pdf
  *  3.4. Reference Test Streams - Table 2: Test Stream Services
  *
  ********************************************************************/
#if (0)
/*********************************************************************
  *
  * 1. clear text CW
  *
  ********************************************************************/
	if (usVideoPid == 0x65 && usAudioPid == 0xc9)
	{
#if (0)
		TUnsignedInt8		aucOddKey[8] = {0xAB, 0xBC, 0xCB, 0x32, 0xBA, 0xAB, 0xBC, 0x21};
		TUnsignedInt8		aucEvenKey[8] = {0x54, 0x43, 0x34, 0xCB, 0x45, 0x54, 0x43, 0xDC};
#else
		TUnsignedInt8		aucOddKey[8] = {0x54, 0x43, 0x34, 0xCB, 0x45, 0x54, 0x43, 0xDC};
		TUnsignedInt8		aucEvenKey[8] = {0xAB, 0xBC, 0xCB, 0x32, 0xBA, 0xAB, 0xBC, 0x21};
#endif
		HxLOG_Info("service clear text CW!! \n");

/*********************************************************************
  *
  * 1.1. set video dsc
  *
  ********************************************************************/
		pDscChannel = NULL;
		hError = local_preDsc_openDsc(hAction, ePAL_DSC_DATATYPE_VIDEOPES, &pDscChannel);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_openDsc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscPid(pDscChannel, usVideoPid);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscPid fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscKey(	pDscChannel,
											8,
											aucOddKey,
											8,
											aucEvenKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscKey fail!! n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

/*********************************************************************
  *
  * 1.2. set audio dsc
  *
  ********************************************************************/
		pDscChannel = NULL;
		hError = local_preDsc_openDsc(hAction, ePAL_DSC_DATATYPE_AUDIOPES, &pDscChannel);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_openDsc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscPid(pDscChannel, usAudioPid);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscPid fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscKey(	pDscChannel,
											8,
											aucOddKey,
											8,
											aucEvenKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscKey fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}
#else
/*********************************************************************
  *
  * 2. protected CW
  *
  ********************************************************************/
	if ( (usVideoPid == 0x65) && (usAudioPid == 0xc9) )
	{
		HxLOG_Info("service protected CW!! \n");

#if (USB_SUPPORT == 1)
		if (!s_cscRecordInfo.bValid)
		{
			hError = local_preDsc_LoadCSCKey(PATH_CSC_DATA, s_cscRecordInfo.aucCscKey, &s_cscRecordInfo.ulKeySize);
			if (hError != ERR_OK)
			{
				HxLOG_Error("local_preDsc_LoadCSCData not found !!\n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
			s_cscRecordInfo.bValid = TRUE;
		}

		if (!s_cscVectorInfo.bValid)
		{
			hError = local_preDsc_LoadCSCVector(PATH_CSC_VECTOR, s_cscVectorInfo.aucEvenProtectedKey, s_cscVectorInfo.aucOddProtectedKey);
			if (hError != ERR_OK)
			{
				HxLOG_Error("local_preDsc_LoadCSCVector not found !!\n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
			s_cscVectorInfo.bValid = TRUE;
		}
#else
		TBoolean		bFound = FALSE;
		TUnsignedInt32	ulFoundIndex = 0;
		TUnsignedInt32	ulIndex = 0;
		TUnsignedInt32	ulSysNuid = 0;
		TUnsignedInt32	ulCscNuid = 0;
		TUnsignedInt32	ulVectorNuid = 0;
		TUnsignedInt8	aucLevel1ProtectingKey[16] = {0, };

		hError = svc_cas_DevPortSysGetChipId(&ulSysNuid);
		if (hError != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevPortSysGetChipId fail!! \n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
		HxLOG_Info("ulSysNuid(%x)!! \n", ulSysNuid);

		if (!s_cscRecordInfo.bValid)
		{
#if 0
			hError = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)s_cscRecordInfo.aucCscRecord, MAX_CSC_RECORD_SIZE);
			if (hError != ERR_OK)
			{
				HxLOG_Error("[CA_NA_PreDsc_SetPidForPreDescramble] svc_cas_DevSysGetField fail!! \n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
#else
			for (ulIndex = 0; ulIndex < MAX_CSCDATA_NUM; ulIndex++)
			{
				ulCscNuid = (TUnsignedInt32)get32bit( &(s_aucCSCData[ulIndex][CSC_OFFSET_NUID]) );
				HxLOG_Info("ulCscNuid(%x) ulSysNuid(%x)!! \n", ulCscNuid, ulSysNuid);

				if (ulCscNuid == ulSysNuid)
				{
					ulFoundIndex = ulIndex;
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				HxLOG_Error(" !bFound !! \n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
			HxSTD_memcpy(s_cscRecordInfo.aucCscRecord, &s_aucCSCData[ulFoundIndex], MAX_CSCDATA_SIZE);
#endif
			ulCscNuid = (TUnsignedInt32)get32bit( &(s_cscRecordInfo.aucCscRecord[CSC_OFFSET_NUID]) );
			HxLOG_Info("ulCscNuid(%x) ulSysNuid(%x)!! \n", ulCscNuid, ulSysNuid);

			if (ulCscNuid != ulSysNuid)
			{
				HxLOG_Error("ulCscNuid != ulSysNuid !! \n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}
			s_cscRecordInfo.ulKeySize = (TUnsignedInt32)s_cscRecordInfo.aucCscRecord[CSC_OFFSET_KEY_LEN];

			HxLOG_Info("ulKeySize(%d)!! \n", s_cscRecordInfo.ulKeySize);

			HxSTD_memcpy(s_cscRecordInfo.aucCscKey, &s_cscRecordInfo.aucCscRecord[CSC_OFFSET_KEY], s_cscRecordInfo.ulKeySize);
			s_cscRecordInfo.bValid = TRUE;
		}

		if (!s_cscVectorInfo.bValid)
		{
			bFound = FALSE;
			for (ulIndex = 0; ulIndex < MAX_CSCVECTOR_NUM; ulIndex++)
			{
				ulVectorNuid = (TUnsignedInt32)get32bit( &(s_aucVector[ulIndex][CSCVECTOR_OFFSET_NUID]) );
				HxLOG_Info("ulVectorNuid(%x) ulSysNuid(%x)!! \n", ulVectorNuid, ulSysNuid);

				if (ulVectorNuid == ulSysNuid)
				{
					ulFoundIndex = ulIndex;
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				HxLOG_Error(" !bFound !! \n");
				hError = ERR_FAIL;
				goto EXIT_FUNC;
			}

			HxSTD_memcpy(s_cscVectorInfo.aucOddProtectedKey,&s_aucVector[ulFoundIndex][CSCVECTOR_OFFSET_ODD_KEY], 8);
			HxSTD_memcpy(s_cscVectorInfo.aucEvenProtectedKey,&s_aucVector[ulFoundIndex][CSCVECTOR_OFFSET_EVEN_KEY], 8);
			s_cscVectorInfo.bValid = TRUE;
		}
#endif

/*********************************************************************
  *
  * 2.1. set video dsc
  *
  ********************************************************************/
		pDscChannel = NULL;
		hError = local_preDsc_openDsc(hAction, ePAL_DSC_DATATYPE_VIDEOPES, &pDscChannel);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_openDsc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscPid(pDscChannel, usVideoPid);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscPid fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDsc1LevelProtectedKeys(pDscChannel,
														s_cscRecordInfo.ulKeySize,
														s_cscRecordInfo.aucCscKey,
														8,
														s_cscVectorInfo.aucOddProtectedKey,
														8,
														s_cscVectorInfo.aucEvenProtectedKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscKey fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

/*********************************************************************
  *
  * 2.2. set audio dsc
  *
  ********************************************************************/
		pDscChannel = NULL;
		hError = local_preDsc_openDsc(hAction, ePAL_DSC_DATATYPE_AUDIOPES, &pDscChannel);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_openDsc fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDscPid(pDscChannel, usAudioPid);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscPid fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}

		hError = local_preDsc_setDsc1LevelProtectedKeys(pDscChannel,
														s_cscRecordInfo.ulKeySize,
														s_cscRecordInfo.aucCscKey,
														8,
														s_cscVectorInfo.aucOddProtectedKey,
														8,
														s_cscVectorInfo.aucEvenProtectedKey);
		if (hError != ERR_OK)
		{
			HxLOG_Error("local_preDsc_setDscKey fail!!\n");
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}
#endif
	else
	{
		HxLOG_Info("FTA!! \n");
	}

	hError = ERR_OK;

EXIT_FUNC :

	return hError;
}
#endif


