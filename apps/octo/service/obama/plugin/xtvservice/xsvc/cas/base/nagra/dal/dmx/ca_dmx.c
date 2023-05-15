/**************************************************************************************************/
#define ________CA_DMX_Private_Include____________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "util.h"
#include "pal_sef.h"

/**************************************************************************************************/
#define ________CA_DMX_Golbal_Value________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_DMX_Private_Define_______________________________________________________
/**************************************************************************************************/
//#define DEBUG_SECTION_AND_FILTER

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define DmxLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA DMX](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define DmxLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA DMX](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define DmxLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA DMX](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define DmxLogInfo(message)		//((HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	DmxLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	DmxLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	DmxLogWarning((__VA_ARGS__))
#endif

#if defined (HxLOG_Info)
#undef	HxLOG_Info
#define HxLOG_Info(...)		DmxLogInfo((__VA_ARGS__))
#endif

#endif






#if defined(CONFIG_MW_MM_PVR)
#define MAX_DMX_FILTER	  					40
#else
#define MAX_DMX_FILTER	  					16
#endif

#define RETRY_SECTION_FILTER				1

#define MAX_LENGTH_OF_FILTER_PATTERN		8

#define WAIT_FOR_SEM(x) 					VK_SEM_Get(x);
#define RELEASE_SEM(x)						VK_SEM_Release(x);

#if defined(VALID_PID)
#undef VALID_PID
#define VALID_PID(x)						(x > 0 && x < 0x1fff)
#endif
#if defined(INVALID_PID)
#undef INVALID_PID
#define INVALID_PID(x)						!VALID_PID(x)
#endif

#define DMX_MACRO_GET_TABLEID_N_RESULT(a, b, c)			\
{														\
	c = (a & 0x0000ffff); 								\
	b = (a & 0xffff0000) >> 16;							\
}

#define DMX_MACRO_GET_FILTERID_N_TSUNIQID(a, b, c)		\
{														\
	c = a & 0x0000ffff; 								\
	b = (a & 0xffff0000) >> 16;							\
}


/**************************************************************************************************/
#define ________CA_DSC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef enum
{
	eFilterStopped,
	eFilterRunning
} TFilterState_t;

typedef struct SDmxFilterId
{
	TPid								usPid;
	TUnsignedInt8 						ucFilterDepth;
	TUnsignedInt8 						ucMatch[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8 						ucEqualMask[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8 						ucNotEqualMask[MAX_LENGTH_OF_FILTER_PATTERN];
	TOsTime								uTimeout;
	TDmxCrcMode							eCrcMode;
	TDmxLoopMode						eLoopMode;
	TDmxFilterQueryBufferCallback		pfnQueryBufferCallback;
	TDmxFilterReceivedSectionCallback	pfnReceivedSectionCallback;
	const TUnsignedInt8					*pucPrivateData;
	TUnsignedInt16						usFilterId;
	TFilterState_t						eFilterState;
	TUnsignedInt8						*pucDmxBuffer;
	TUnsignedInt32 						hAction;
	TBoolean							fEmmStop;
} TSDmxFilterId;


/**************************************************************************************************/
#define ________CA_DMX_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TBoolean					s_bDmxFilterUsedFlag[MAX_DMX_FILTER];
static TDmxFilterId 			s_pstDmxFilterInfo[MAX_DMX_FILTER];

static volatile TUnsignedInt32	s_ulDmxFilterSemId = 0;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
static TBoolean					s_bEcmTrace = FALSE;
static TBoolean					s_bEmmTrace = TRUE;
#elif defined(CONFIG_DEBUG)
static TBoolean					s_bEcmTrace = FALSE;
static TBoolean					s_bEmmTrace = FALSE;
#endif


/**************************************************************************************************/
#define ________CA_DMX_Private_Static_Prototype_____________________________________________
/**************************************************************************************************/
static TDmxStatus 	local_caDmx_AllocateFilterResource(TDmxFilterId *pxFilterId);
static TDmxStatus 	local_caDmx_FreeFilterResource(TDmxFilterId xFilterId);
static TDmxStatus 	local_caDmx_FindFilterResource(TDmxFilterId xFilterId);
static TDmxStatus 	local_caDmx_FindFilterResourceByFilterId(TUnsignedInt16 usFilterId, TDmxFilterId *pxFilterId);
static TBoolean 	local_caDmx_CheckIdenticalFiltereFilterRunning(TDmxFilterId xFilterId);
static void 		local_caDmx_CallbackSection(TUnsignedInt16 usFilterId, TUnsignedInt8 *pucSection, TUnsignedInt32 ulSecSize,
														TBoolean bCrcError, TBoolean bTimeOutError, TBoolean bFilterError);


/**************************************************************************************************/
#define ________CA_DMX_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
static TDmxStatus local_caDmx_AllocateFilterResource(TDmxFilterId *pxFilterId)
{
	TUnsignedInt8 i;
	TDmxStatus 	eStatus = DMX_ERROR_NO_MORE_RESOURCES;

	if (pxFilterId == NULL)
	{
		return DMX_ERROR;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)

	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE )
		{
			s_bDmxFilterUsedFlag[i] = TRUE;
			HxSTD_memset(s_pstDmxFilterInfo[i], 0x00, sizeof(TSDmxFilterId));
			*pxFilterId = s_pstDmxFilterInfo[i];

			eStatus = DMX_NO_ERROR;
			HxLOG_Print("%d, pxFilterId:0x%x, *pxFilterId:0x%x\n",
									i, (unsigned int)pxFilterId, (unsigned int)*pxFilterId);
			break;
		}
	}
	RELEASE_SEM(s_ulDmxFilterSemId)
	return eStatus;
}


static TDmxStatus local_caDmx_FreeFilterResource(TDmxFilterId xFilterId)
{
	TUnsignedInt8 i;
	TDmxStatus 	eStatus = DMX_ERROR_UNKNOWN_ID;

	WAIT_FOR_SEM(s_ulDmxFilterSemId)

	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE)
		{
			continue;
		}

		if (s_pstDmxFilterInfo[i] == xFilterId)
		{
			s_bDmxFilterUsedFlag[i] = FALSE;
			HxSTD_memset(s_pstDmxFilterInfo[i], 0, sizeof(TSDmxFilterId));
			eStatus = DMX_NO_ERROR;
			break;
		}
	}

	RELEASE_SEM(s_ulDmxFilterSemId)
	return eStatus;
}


static TDmxStatus local_caDmx_FindFilterResource(TDmxFilterId xFilterId)
{
	TUnsignedInt8 i;
	TDmxStatus 	eStatus = DMX_ERROR_UNKNOWN_ID;

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE)
		{
			continue;
		}

		if (s_pstDmxFilterInfo[i] == xFilterId)
		{
			eStatus = DMX_NO_ERROR;
			break;
		}
	}

	RELEASE_SEM(s_ulDmxFilterSemId)
	return eStatus;
}


static TDmxStatus local_caDmx_FindFilterResourceByFilterId(TUnsignedInt16 usFilterId, TDmxFilterId *pxFilterId)
{
	TUnsignedInt8 i;
	TDmxStatus 	eStatus = DMX_ERROR_UNKNOWN_ID;

	if (pxFilterId == NULL)
	{
		return DMX_ERROR;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE)
			continue;

		if ( (s_pstDmxFilterInfo[i]->usFilterId == usFilterId)
			&& (s_pstDmxFilterInfo[i]->eFilterState == eFilterRunning) )
		{
			*pxFilterId = s_pstDmxFilterInfo[i];
			eStatus = DMX_NO_ERROR;
			break;
		}
	}
    RELEASE_SEM(s_ulDmxFilterSemId)
	return eStatus;
}


static TBoolean local_caDmx_CheckIdenticalFiltereFilterRunning(TDmxFilterId xFilterId)
{
	TUnsignedInt8	i, ucDepth;
	TBoolean		bFound = FALSE;

	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE)
		{
			continue;
		}

		if (s_pstDmxFilterInfo[i] != xFilterId)
		{
			continue;
		}

		if ( (s_pstDmxFilterInfo[i]->usPid == xFilterId->usPid)
			&& (s_pstDmxFilterInfo[i]->hAction == xFilterId->hAction)
			&& (s_pstDmxFilterInfo[i]->eFilterState == eFilterRunning) )
		{
			ucDepth = (s_pstDmxFilterInfo[i]->ucFilterDepth > xFilterId->ucFilterDepth) ? xFilterId->ucFilterDepth : s_pstDmxFilterInfo[i]->ucFilterDepth;
			if (!HxSTD_memcmp( s_pstDmxFilterInfo[i]->ucMatch, xFilterId->ucMatch, ucDepth))
			{
				bFound = TRUE;
				break;
			}
		}
	}

	return bFound;
}


static void local_caDmx_CallbackSection(TUnsignedInt16 usFilterId, TUnsignedInt8 *pucSection, TUnsignedInt32 ulSecSize,
												TBoolean bCrcError, TBoolean bTimeOutError, TBoolean bFilterError)
{
	TBoolean 			bFound;
	TDmxFilterId		pxFilterId;
	TDmxBufferStatus	eBufStatus;
	TUnsignedInt32		ulBufSize = 0;
	TDmxSectionStatus	eSecStatus;

	bFound = local_caDmx_FindFilterResourceByFilterId(usFilterId, &pxFilterId);
	if (bFound != DMX_NO_ERROR)
	{
		HxLOG_Error("Invalid load ID(0x%x)\n", usFilterId);
		return;
	}

	if (bCrcError)
	{
		eSecStatus = DMX_SECTION_CRC_ERROR;
	}
	else if (bTimeOutError)
	{
		eSecStatus = DMX_SECTION_TIMEOUT_ERROR;
	}
	else if (bFilterError)
	{
		eSecStatus = DMX_SECTION_ERROR;
	}
	else
	{
		eSecStatus = DMX_SECTION_AVAILABLE;
	}

	pxFilterId->pucDmxBuffer = NULL;
	if (*(pxFilterId->pfnQueryBufferCallback) && pxFilterId->eFilterState == eFilterRunning)
	{
		if ( (pxFilterId->fEmmStop == TRUE) && (pxFilterId->eLoopMode == DMX_LOOP_CONTINUOUS) )
		{
			HxLOG_Print("Ignore EMM section...\n");
			return ;
		}
		eBufStatus = (*(pxFilterId->pfnQueryBufferCallback))(pxFilterId, &(pxFilterId->pucDmxBuffer), (TSize*)&ulBufSize, (void *)(pxFilterId->pucPrivateData));
	}
	else
	{
		return;
	}

	HxLOG_Print("bquery(buffer status %d)(buffer size %d)(buffer address 0x%x)\n", eBufStatus, (int)ulBufSize, (unsigned int)pxFilterId->pucDmxBuffer);
	if (eBufStatus == DMX_BUFFER_ERROR)
	{
		// 3. Callback Function Tests, 1. Query Buffer Callback Error Return Test 
		HxLOG_Error("Don't get buffer from Nagra\n");
		eSecStatus = DMX_SECTION_BUFFER_ERROR;
	}
	else if (ulSecSize > ulBufSize && ulBufSize != 0)
	{
		HxLOG_Error("Invalid section length!!(0x%x)(0x%x)(table_id = 0x%x)\n", (unsigned int)ulSecSize, (unsigned int)ulBufSize, pucSection[0]);
		//eSecStatus = DMX_SECTION_ERROR;
	}

	if (eSecStatus == DMX_SECTION_AVAILABLE)
	{
		if (pxFilterId->pucDmxBuffer == NULL)
		{
			// 3. Callback Function Tests, 2 Query Buffer Callback Allocate Buffer Test 
			//                                            3. Query Buffer Callback Buffer Size Test
			HxLOG_Error("Don't get buffer from Nagra(buffer is null)\n");
			eSecStatus = DMX_SECTION_BUFFER_ERROR;
			(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, NULL, 0, (void *)(pxFilterId->pucPrivateData));
			return;
		}

		if ((pxFilterId->pucDmxBuffer != NULL) && (pucSection != NULL))
		{
			if (ulSecSize > ulBufSize && ulBufSize != 0)
			{				
				// DMX 3.6. Received Section Callback Filter Section Bigger than Allocated Buffer Test
				HxLOG_Error(">> copy (unsigned int)ulBufSize(%d), pxFilterId->fEmmStop(%d) \n", (unsigned int)ulBufSize, pxFilterId->fEmmStop);			
				HxSTD_memcpy(pxFilterId->pucDmxBuffer, pucSection, (unsigned int)ulBufSize);
				(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, pxFilterId->pucDmxBuffer, (unsigned int)ulBufSize, (void *)(pxFilterId->pucPrivateData));
				return ;
			}
			else
			{
				HxSTD_memcpy(pxFilterId->pucDmxBuffer, pucSection, ulSecSize);
			}
#if defined(CONFIG_DEBUG) || defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
			if (pxFilterId->pucDmxBuffer[0] == 0x80 || pxFilterId->pucDmxBuffer[0] == 0x81)
			{
				if(s_bEcmTrace != FALSE)
				{
					Hx_Print(C_CODE_F_BLUE);
					Hx_Print("\nReceived ECM Section (%d) - hAction(0x%x), ID(0x%x)\n", ulSecSize, pxFilterId->hAction, pxFilterId->usFilterId);
					HLIB_LOG_Dump(pxFilterId->pucDmxBuffer, ulSecSize, 0, TRUE);
					Hx_Print(C_CODE_RESET);
				}
			}
			else
			{
				if(s_bEmmTrace != FALSE)
				{
					Hx_Print(C_CODE_F_BLUE);
					Hx_Print("\nReceived EMM Section (%d) - hAction(0x%x), ID(0x%x)\n", ulSecSize, pxFilterId->hAction, pxFilterId->usFilterId);
					//HLIB_LOG_Dump(pxFilterId->pucDmxBuffer, ulSecSize, 0, TRUE);
					Hx_Print(C_CODE_RESET);
				}
			}
#endif
			if ( *(pxFilterId->pfnReceivedSectionCallback) && (pxFilterId->eFilterState == eFilterRunning) )
			{
				if ( (pxFilterId->fEmmStop == TRUE) && (pxFilterId->eLoopMode == DMX_LOOP_CONTINUOUS) )
				{
					HxLOG_Print("ignore EMM section (0x%x)\n", pxFilterId->pucDmxBuffer[0]);
					return ;
				}
				(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, pxFilterId->pucDmxBuffer, ulSecSize, (void *)(pxFilterId->pucPrivateData));
			}
			else
			{
				if (pxFilterId->pucDmxBuffer != NULL)
				{
					OxCAS_Free(pxFilterId->pucDmxBuffer);
					pxFilterId->pucDmxBuffer = NULL;
				}
				return ;
			}

			if (eBufStatus == DMX_BUFFER_ERROR)
			{
				if (pxFilterId->pucDmxBuffer != NULL)
				{
					OxCAS_Free(pxFilterId->pucDmxBuffer);
					pxFilterId->pucDmxBuffer = NULL;
				}
			}
		}
	}
	else if (eSecStatus == DMX_SECTION_ERROR || eSecStatus == DMX_SECTION_CRC_ERROR || eSecStatus == DMX_SECTION_TIMEOUT_ERROR)
	{
		if (eBufStatus != DMX_BUFFER_ERROR)
		{
			(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, pxFilterId->pucDmxBuffer, 0, (void *)(pxFilterId->pucPrivateData));
		}
		else
		{
			if (eSecStatus == DMX_SECTION_CRC_ERROR)
			{
				HxLOG_Error("CRC error!!\n");
			}
			else if (eSecStatus == DMX_SECTION_TIMEOUT_ERROR)
			{
				HxLOG_Error("Time out error!!\n");
			}

			if (pxFilterId->pucDmxBuffer != NULL)
			{
				OxCAS_Free( pxFilterId->pucDmxBuffer );
				pxFilterId->pucDmxBuffer = NULL;
			}
			(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, NULL, 0, (void *)(pxFilterId->pucPrivateData));
		}
	}
	else if (eSecStatus == DMX_SECTION_BUFFER_ERROR)
	{
		(*(pxFilterId->pfnReceivedSectionCallback))(pxFilterId, eSecStatus, NULL, 0, (void *)(pxFilterId->pucPrivateData));
	}
	
	return;
}


/**************************************************************************************************/
#define ________CA_DMX_Public_Functions_Body________________________________________________
/**************************************************************************************************/
TDmxStatus dmxFilterOpen(TDmxFilterId *pxFilterId, TTransportSessionId xTransportSessionId, TSize xFilterDepth, const void *pxPrivateData)
{
	TDmxStatus eStatus;

	if (pxFilterId == NULL || xFilterDepth ==0)
		return DMX_ERROR;

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	/* 	수정 원인 : 0. Allocate Filter Tests, dmxFilterOpen() Transport Session ID Tes 인해 수정함. 
		xTransportSessionId == TRANSPORT_SESSION_ID_INVALID 인경우 DMX_ERROR를 return하도록 수정함.
		2.call dmxFilterOpen with xFilterDepth=DEPTH, pxPrivateData=NULL, xTransportSessionId=TRANSPORT_SESSION_ID_INVALID. The expected return shall be DMX_ERROR.
	*/
	if(xTransportSessionId == TRANSPORT_SESSION_ID_INVALID)
	{
		return DMX_ERROR;
	}
#endif

	eStatus = local_caDmx_AllocateFilterResource(pxFilterId);
	HxLOG_Print("FilterId:0x%x, xTransSessionId:0x%x, xFilterDepth:%d, dmxStatus:0x%x\n",
								(unsigned int)*pxFilterId, (unsigned int)xTransportSessionId, xFilterDepth, eStatus);
	if (eStatus == DMX_NO_ERROR)
	{
		WAIT_FOR_SEM(s_ulDmxFilterSemId)
		(*pxFilterId)->ucFilterDepth 	= ((TUnsignedInt8)xFilterDepth > MAX_LENGTH_OF_FILTER_PATTERN) ? MAX_LENGTH_OF_FILTER_PATTERN : xFilterDepth;
		(*pxFilterId)->pucPrivateData 	= pxPrivateData;
		(*pxFilterId)->eFilterState 	= eFilterStopped;
		(*pxFilterId)->pucDmxBuffer 	= NULL;
		(*pxFilterId)->hAction 			= xTransportSessionId;
		RELEASE_SEM(s_ulDmxFilterSemId)

#if (0)
		if (xFilterDepth > MAX_LENGTH_OF_FILTER_PATTERN)
		{
			HxLOG_Error("Too long filter value\n");
			return DMX_WARNING_SOFTWARE_FILTERING;
		}
		else
		{
			return DMX_NO_ERROR;
		}
#endif
	}
	else
	{
		HxLOG_Error("Could not allocate the filter resource\n");
	}
	return eStatus;
}


TDmxStatus dmxFilterClose(TDmxFilterId xFilterId)
{
	TDmxStatus eStatus;

	HxLOG_Print("xFilterId:0x%x\n\n", (unsigned int)xFilterId);
	eStatus = dmxFilterStop(xFilterId);
	if (eStatus != DMX_NO_ERROR)
	{
		HxLOG_Error("error in dmxFilterStop : xFilterId(0x%x), status(0x%x)\n", (unsigned int)xFilterId, eStatus);
	}

	eStatus = local_caDmx_FreeFilterResource(xFilterId);
	if (eStatus != DMX_NO_ERROR)
	{
		HxLOG_Error("Failed to close filter : xFilterId(0x%x), status(0x%x)\n", (unsigned int)xFilterId, eStatus);
	}
	return eStatus;
}


TDmxStatus dmxFilterSetPatterns(TDmxFilterId xFilterId, const TUnsignedInt8* pxMatch, const TUnsignedInt8* pxEqualMask, const TUnsignedInt8* pxNotEqualMask)
{
	TUnsignedInt8 i;

	if (pxMatch == NULL || pxEqualMask == NULL || pxNotEqualMask == NULL)
	{
		return DMX_ERROR_BAD_PARAMETER;
	}

	HxLOG_Info("xFilterId:0x%x\n", (unsigned int)xFilterId);
#if defined(DEBUG_SECTION_AND_FILTER)
	if (xFilterId != NULL)
	{
		HxLOG_Info("	xMatch: 	  ");
		for (i=0; i<xFilterId->ucFilterDepth; i++)
		{
			HxLOG_Info("%02X ", pxMatch[i]);
		}
		HxLOG_Info("\n	EqualMask:	  ");
		for (i=0; i<xFilterId->ucFilterDepth; i++)
		{
			HxLOG_Info("%02X ", pxEqualMask[i]);
		}
		HxLOG_Info("\n	NotEqualMask: ");
		for (i=0; i<xFilterId->ucFilterDepth; i++)
		{
			HxLOG_Info("%02X ", pxNotEqualMask[i]);
		}
		HxLOG_Info("\n");
	}
	else
	{
		// for remove prevent warring
		HxLOG_Print("xFilterId is null...\n");
	}
#endif

	if (local_caDmx_FindFilterResource(xFilterId) != DMX_NO_ERROR)
	{
		HxLOG_Error("Unable to find filter(0x%x)\n", (unsigned int)xFilterId);
		return DMX_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	if (xFilterId)
	{
		if (xFilterId->eFilterState == eFilterRunning)
		{
			HxLOG_Error("Filter is eFilterRunning!!(0x%x)\n", (unsigned int)xFilterId);
			RELEASE_SEM(s_ulDmxFilterSemId)
			return DMX_ERROR_FILTER_RUNNING;
		}

		for (i=0; i<xFilterId->ucFilterDepth; i++)
		{
			if (pxEqualMask[i] & pxNotEqualMask[i])
			{
				HxLOG_Error("EqualMask and NotEqualMask masks overlap!(0x%x)\n", (unsigned int)xFilterId);
				RELEASE_SEM(s_ulDmxFilterSemId)
				return DMX_ERROR_BAD_PARAMETER;
			}
		}

		HxSTD_memset(xFilterId->ucMatch, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);
		HxSTD_memcpy(xFilterId->ucMatch, pxMatch, xFilterId->ucFilterDepth);
		HxSTD_memset(xFilterId->ucEqualMask, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);
		HxSTD_memcpy(xFilterId->ucEqualMask, pxEqualMask, xFilterId->ucFilterDepth);
		HxSTD_memset(xFilterId->ucNotEqualMask, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);
		HxSTD_memcpy(xFilterId->ucNotEqualMask, pxNotEqualMask, xFilterId->ucFilterDepth);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) && !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		// According to SES HE's pperation problem,  Nagra suggest that force global EMM table id form 0x84 to 0x82.
		// cak test 에서는 기존처럼 0x84를 사용해야 한다. 주의하자. 안그러면 EMM update 안될 것이며 BDC test fail 난다.x82.
		if( pxMatch[0] == 0x84 )
		{
			HxSTD_memset(&xFilterId->ucMatch, 0, MAX_LENGTH_OF_FILTER_PATTERN);
			HxSTD_memset(&xFilterId->ucEqualMask, 0, MAX_LENGTH_OF_FILTER_PATTERN);
			xFilterId->ucMatch[0] = 0x82;
			xFilterId->ucEqualMask[0] = 0xFF;
			HxLOG_Print("\r\n[dmxFilterSetPatterns] GLOBAL EMM : 0x84 -> 0x82\r\n");
		}
#endif
	}

	RELEASE_SEM(s_ulDmxFilterSemId)
	return DMX_NO_ERROR;
}


TDmxStatus dmxFilterSetPid(TDmxFilterId	xFilterId, TPid xPid)
{
	HxLOG_Print("xFilterId:0x%x, xPid:0x%x\n", (unsigned int)xFilterId, xPid);

	if (INVALID_PID(xPid))
	{
		HxLOG_Error("Invalid PID(0x%x)\n", xPid);
#if defined(CONFIG_PROD_HGS1000S) && defined(CONFIG_MW_CAS_NAGRA_DALTEST)
		return DMX_ERROR; // 0x1fff 이면 BAD PID가 아니고 ERROR or NO_ERROR로 return 하라고 나와서 수정 필요함
#else
		return DMX_ERROR_BAD_PID;
#endif
	}

	if (local_caDmx_FindFilterResource(xFilterId) != DMX_NO_ERROR)
	{
		HxLOG_Error("Unable to find filter(0x%x)\n", (unsigned int)xFilterId);
		return DMX_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	if (xFilterId->eFilterState == eFilterRunning)
	{
		HxLOG_Error("Filter is eFilterRunning!!(0x%x)\n", (unsigned int)xFilterId);
		RELEASE_SEM(s_ulDmxFilterSemId)
		return DMX_ERROR_FILTER_RUNNING;
	}

	xFilterId->usPid = xPid;
	RELEASE_SEM(s_ulDmxFilterSemId)
	return DMX_NO_ERROR;
}


TDmxStatus dmxFilterStart(TDmxFilterId xFilterId,
							TOsTime xTimeout,
							TDmxLoopMode xLoopMode,
							TDmxCrcMode xCrcMode,
							TDmxFilterQueryBufferCallback xQueryBufferCallback,
							TDmxFilterReceivedSectionCallback xReceivedSectionCallback)
{
	TUnsignedInt8 	i;
	TUnsignedInt8	ucCrcCheck = 0x00;
	TUnsignedInt8	ucFvalue[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8	ucFmask[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt32	ulCount = 0;
	TBoolean 		bFound = FALSE;
	TSignedInt32	nErrCode;

	if ( (xLoopMode < DMX_LOOP_CONTINUOUS || xLoopMode > DMX_LOOP_TOGGLE)
		|| (xCrcMode < DMX_CRC_CHECK || xCrcMode > DMX_CRC_IGNORE)
		|| (xQueryBufferCallback == NULL) || (xReceivedSectionCallback == NULL) )
	{
		return DMX_ERROR;
	}

	HxLOG_Print("xFitlerId:0x%x, xTimeout:%d, xLoopMode:%d, xCrcMode:0x%x\n",
							(unsigned int)xFilterId, (int)xTimeout, xLoopMode, xCrcMode);

	HxSTD_memset(ucFvalue, 0, MAX_LENGTH_OF_FILTER_PATTERN);
	HxSTD_memset(ucFmask, 0, MAX_LENGTH_OF_FILTER_PATTERN);
	if (xCrcMode == DMX_CRC_CHECK)
	{
		ucCrcCheck = 0x80;
	}

	if (local_caDmx_FindFilterResource(xFilterId) != DMX_NO_ERROR)
	{
		HxLOG_Error("Unable to find filter(0x%x)\n", (unsigned int)xFilterId);
		return DMX_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	if (xFilterId->eFilterState == eFilterRunning)
	{
		HxLOG_Error( "Filter is eFilterRunning!!(0x%x)\n", (unsigned int)xFilterId);
		RELEASE_SEM(s_ulDmxFilterSemId)
		return DMX_ERROR_FILTER_RUNNING;
	}

#if 0 //defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	/* 	수정 원인 : 2_2_5 dmxFilterStop() Stop Filters In Different Order Test -> TEST  임시 적용. 
		아래 local_caDmx_CheckIdenticalFiltereFilterRunning()함수를 막지 않으면 계속해서 fail이 되어 임시로 막음
	*/
#else
	if (local_caDmx_CheckIdenticalFiltereFilterRunning(xFilterId))
	{
		xFilterId->eFilterState = eFilterStopped;
		HxLOG_Error(  "Identical filter eFilterRunning(0x%x)\n", (unsigned int)xFilterId);
		RELEASE_SEM(s_ulDmxFilterSemId)
		return DMX_ERROR_FILTER_RUNNING;
	}
#endif

	ucFvalue[0] = xFilterId->ucMatch[0];
	ucFmask[0] = xFilterId->ucEqualMask[0];
	for (i = 1; (i<(MAX_LENGTH_OF_FILTER_PATTERN-2)) && ((i+2)<xFilterId->ucFilterDepth); i++)
	{
		ucFvalue[i] = xFilterId->ucMatch[i+2];
		ucFmask[i] = xFilterId->ucEqualMask[i+2];
	}

	for (i=0; i<MAX_LENGTH_OF_FILTER_PATTERN; i++)
	{
		if (ucFmask[i] )
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		for (i=0; i<MAX_LENGTH_OF_FILTER_PATTERN; i++)
		{
			if (xFilterId->ucNotEqualMask[i])
			{
				bFound = TRUE;
				break;
			}
		}
	}

	if (bFound == FALSE)
	{
		HxLOG_Error( "Invalid mask\n");
#if (0)
		RELEASE_SEM(s_ulDmxFilterSemId)
		return DMX_ERROR_FILTER_RUNNING;
#endif
	}

#if defined(DEBUG_SECTION_AND_FILTER)
	HxLOG_Info("[....dmxFilterStart....]");
	HxLOG_Info("(match ");
	for(i=0;i<MAX_LENGTH_OF_FILTER_PATTERN;i++)
		HxLOG_Info(".%x", ucFvalue[i]);
	HxLOG_Info(")(EqualMask ");
	for(i=0;i<MAX_LENGTH_OF_FILTER_PATTERN;i++)
		HxLOG_Info(".%x", ucFmask[i]);
	HxLOG_Info(")\n\n");
#endif

	while (1)
	{
		switch (xLoopMode)
		{
			case DMX_LOOP_CONTINUOUS:
				nErrCode = CA_NA_DmxStartFilter(xFilterId->hAction,
												xFilterId->usPid,
												xTimeout,
												(SI_EVENT_CONT_MODE | ucCrcCheck),
												ucFvalue,
												ucFmask,
												&(xFilterId->usFilterId));
				break;
			case DMX_LOOP_ONE_SHOT:
				nErrCode = CA_NA_DmxStartFilter(xFilterId->hAction,
												xFilterId->usPid,
												xTimeout,
												(SI_EVENT_ONETIME_MODE | ucCrcCheck),
												ucFvalue,
												ucFmask,
												&(xFilterId->usFilterId));
				break;
			default : // Toggle Mode
				if (ucFmask[0] & 0x01)
				{
					ucFmask[0] = ucFmask[0] & 0xFE;
				}
				nErrCode = CA_NA_DmxStartFilter(xFilterId->hAction,
												xFilterId->usPid,
												xTimeout,
												(SI_EVENT_ONETIME_MODE | ucCrcCheck),
												ucFvalue,
												ucFmask,
												&(xFilterId->usFilterId));
		}

		if (nErrCode != ERR_OK)
		{
			HxLOG_Error( "Failed to start filtering, %d\n", (int)ulCount);
			RELEASE_SEM(s_ulDmxFilterSemId)

			ulCount++;
			if (ulCount > 2)
			{
				HxLOG_Error("Retry Over\n");
				return DMX_ERROR;
			}

			VK_TASK_Sleep(50);
			WAIT_FOR_SEM(s_ulDmxFilterSemId)
			continue;
		}
		break;
	}

	xFilterId->eLoopMode 					= xLoopMode;
	xFilterId->eCrcMode 					= xCrcMode;
	xFilterId->pfnQueryBufferCallback 		= xQueryBufferCallback;
	xFilterId->pfnReceivedSectionCallback 	= xReceivedSectionCallback;
	xFilterId->eFilterState 				= eFilterRunning;
	xFilterId->uTimeout 					= xTimeout;
	RELEASE_SEM(s_ulDmxFilterSemId)
	return DMX_NO_ERROR;
}


TDmxStatus dmxFilterStop(TDmxFilterId xFilterId)
{
	HxLOG_Print("xFilterId:0x%x\n", (unsigned int)xFilterId);

	if (local_caDmx_FindFilterResource(xFilterId) != ERR_OK)
	{
		HxLOG_Error("Unknown ID-1(0x%x)\n", (unsigned int)xFilterId);
		return DMX_ERROR_UNKNOWN_ID;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	HxLOG_Print("Filter Status(0x%x), Dmx Filter ID(0x%x) \n", xFilterId->eFilterState, xFilterId->usFilterId);
	if (xFilterId->eFilterState == eFilterRunning)
	{
		if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
		{
			HxLOG_Error("Could not stop filter(0x%x)\n", (unsigned int)xFilterId);
		}
	}
	xFilterId->eFilterState = eFilterStopped;
	RELEASE_SEM(s_ulDmxFilterSemId)
	return DMX_NO_ERROR;
}


TDmxStatus dmxFilterReset(	TDmxFilterId xFilterId)
{
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	TUnsignedInt8	ucFvalue[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8	ucFmask[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8	ucCrcCheck = 0x00;
	TUnsignedInt8	i = 0;
#endif

	if (local_caDmx_FindFilterResource(xFilterId) != ERR_OK)
	{
		HxLOG_Error("Unknown ID-1\n");
		return DMX_ERROR_UNKNOWN_ID;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	/*
		Test Case :
			a. dmx -> 2. Manage Filter Tests -> 9. Reset One Shot Filter Test 
			b. dmx -> 4. Filter Type Tests -> 5. Multi Reset One Shot Filter Test 
		이슈 내용
			아래처럼 dallib에서 dmxFilterReset()을 호출시 filter callback을 호출해주어야한다.
			그러나 현 소스에서는 dmxFilterReset()시 무조건 CA_NA_DmxStopFilter()을 해주어 test result 를 만족하지 못한다.
			그래서 dmxFilterReset()시 filter stop 해준후 다시 filter  start를 해주도록 한다.
			8. all dmxFilterReset with xFilterId=FID. The expected return shall be DMX_NO_ERROR. 
			9.Wait until filter callback invoked.
	**/
	

	VK_memset(ucFvalue, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);
	VK_memset(ucFmask, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);

	ucFvalue[0] = xFilterId->ucMatch[0];
	ucFmask[0] = xFilterId->ucEqualMask[0];

	for (i=1; i<(MAX_LENGTH_OF_FILTER_PATTERN-2); i++)
	{
		ucFvalue[i] = xFilterId->ucMatch[i+2];
		ucFmask[i] = xFilterId->ucEqualMask[i+2];
	}

	if (xFilterId->eCrcMode == DMX_CRC_CHECK)
	{
		ucCrcCheck = 0x80;
	}

	WAIT_FOR_SEM(s_ulDmxFilterSemId)

	if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
	{
		HxLOG_Error("Could not stop filter(0x%x)\n", (unsigned int)xFilterId);
	}

	// 여기서 eFilterRunning을 다시 해준이유는 CA_NA_DmxCallbackTable()에서 (xFilterId->eLoopMode == DMX_LOOP_ONE_SHOT)인 경우 
	// xFilterId->eFilterState = eFilterStopped을 해주어서 강제로 eFilterRunning을 해준다. 그렇지 않으면 CA_NA_DmxStartFilter()이후 CA_NA_DmxCallbackTable()에서 
	// error를 return해주기 때문이다.	
	xFilterId->eFilterState = eFilterRunning;

	if (CA_NA_DmxStartFilter(xFilterId->hAction,
							xFilterId->usPid,
							xFilterId->uTimeout,
							(SI_EVENT_ONETIME_MODE | ucCrcCheck),
							ucFvalue,
							ucFmask,
							&(xFilterId->usFilterId)) != ERR_OK)
	{
		HxLOG_Error("CA_NA_DmxStartFilter() error filter(0x%x)\n", (unsigned int)xFilterId);
	}

	RELEASE_SEM(s_ulDmxFilterSemId)

	return DMX_NO_ERROR;
#else

	WAIT_FOR_SEM(s_ulDmxFilterSemId)
	if( xFilterId->eFilterState == eFilterRunning )
	{
		if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
		{
			HxLOG_Error("Could not stop filter(0x%x)\n", (unsigned int)xFilterId);
		}
	}
	xFilterId->eFilterState = eFilterStopped;
	RELEASE_SEM(s_ulDmxFilterSemId)
	return DMX_NO_ERROR;

#endif
}


/**************************************************************************************************/
#define ________CA_DMX_External_Functions_Body____________________________________________
/**************************************************************************************************/
HERROR CA_NA_DmxInitialization(void)
{
	TUnsignedInt8 	i;
	TSignedInt32 	nErrCode;
	TUnsignedInt32 	ulSmid;

	nErrCode = VK_SEM_Create((unsigned long*)&ulSmid, "ca_dmx_sem", VK_SUSPENDTYPE_PRIORITY);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("Exceeds node's maximum number of semaphore\n");
		return ERR_FAIL;
	}
	else
	{
		s_ulDmxFilterSemId = ulSmid;
	}

	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		s_bDmxFilterUsedFlag[i] = 0;
		s_pstDmxFilterInfo[i] = (TDmxFilterId)OxCAS_Malloc(sizeof(TSDmxFilterId));
		HxSTD_memset(s_pstDmxFilterInfo[i], 0, sizeof(TSDmxFilterId));
	}

	return ERR_OK;
}

void CA_NA_DmxCallbackTable(TUnsignedInt32 ulTableIdAndResult, TUnsignedInt32 ulFilterAndUniqId, TUnsignedInt32 ulParam, TUnsignedInt32 hAction)
{
	PalSef_RawData_t 	*pSiRawData = (PalSef_RawData_t*)ulParam;
	TDmxFilterId	xFilterId;

	TUnsignedInt8	*pucSiSec;
	TUnsignedInt8	ucFvalue[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8	ucFmask[MAX_LENGTH_OF_FILTER_PATTERN];
	TUnsignedInt8	ucEqualTest[MAX_LENGTH_OF_FILTER_PATTERN];

	TBoolean 		bCrcError = FALSE;
	TBoolean		bFilterError = FALSE;
	TBoolean 		bTimeOutError = FALSE;
	TBoolean 		bProperSec = FALSE, bFoundAny = FALSE;

	TUnsignedInt16 	i, j;
	TUnsignedInt16	usSecLen;
	TUnsignedInt16	usTableId;
	TUnsignedInt16 	usFilterId;
	TUnsignedInt16	usTsUniqId;
	TUnsignedInt16	usFilterResult;
	TUnsignedInt16  usSecNum;

	DMX_MACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usFilterResult);
	DMX_MACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqId);

	HxLOG_Print("Action(0x%x) TableId(0x%x), FilterId(0x%x), UniqueId(0x%x)\n", (unsigned int)hAction, usTableId, usFilterId, usTsUniqId);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid\n");
                goto FREE_EXIT;
	}

	if (usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid\n");
                goto FREE_EXIT;
 	}

	if (usFilterResult == eSiFilterResult_Timeout)
	{
		bTimeOutError = TRUE;
	}
	else if (usFilterResult == eSiFilterResult_Error)
	{
		bFilterError = TRUE;
	}

	HxLOG_Print("Filter result (0x%x)\n", usFilterResult);

	HxSTD_memset(ucFvalue, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);
	HxSTD_memset(ucFmask, 0x00, MAX_LENGTH_OF_FILTER_PATTERN);

	if (local_caDmx_FindFilterResourceByFilterId(usFilterId, &xFilterId) != DMX_NO_ERROR)
	{
		HxLOG_Error("Invalid Filter ID (0x%x)\n", usFilterId);
		goto FREE_EXIT;
	}

	ucFvalue[0] = xFilterId->ucMatch[0];
	ucFmask[0] = xFilterId->ucEqualMask[0];
	for (i=1; i<(MAX_LENGTH_OF_FILTER_PATTERN-2); i++)
	{
		ucFvalue[i] = xFilterId->ucMatch[i+2];
		ucFmask[i] = xFilterId->ucEqualMask[i+2];
	}

	if (!bCrcError && !bTimeOutError && !bFilterError)
	{
		HxSTD_memset(ucEqualTest, 0, MAX_LENGTH_OF_FILTER_PATTERN);
		for(i=0; i<MAX_LENGTH_OF_FILTER_PATTERN; i++)
		{
			ucEqualTest[i] = xFilterId->ucMatch[i] & xFilterId->ucNotEqualMask[i];
		}

		if (pSiRawData != NULL)
		{
			usSecNum = pSiRawData->num_sec;

			if (usSecNum != 1)
			{
				HxLOG_Error("number of sections %d(pid 0x%x)(filter 0x%x)\n", usSecNum, xFilterId->usPid, (unsigned int)xFilterId);
			}

			bFoundAny = FALSE;
			for (j = 0; j<usSecNum; j++)
			{
				pucSiSec = (TUnsignedInt8 *)pSiRawData->buf[j];
				usSecLen = (TUnsignedInt16)get12bit(pucSiSec + 1);
				// Total section length
				usSecLen = usSecLen + 3;
				bProperSec = TRUE;
				usTableId = (TUnsignedInt8)pucSiSec[0];

				for (i=0; i<MAX_LENGTH_OF_FILTER_PATTERN; i++)
				{
					if (ucEqualTest[i] != 0 && ucEqualTest[i] == pucSiSec[i])
					{
						bProperSec = FALSE;
					}
				}

				if (bProperSec == TRUE)
				{
					if (xFilterId->eFilterState != eFilterStopped)
					{
						local_caDmx_CallbackSection(usFilterId, pucSiSec, usSecLen, bCrcError, bTimeOutError, bFilterError);
					}
					else
					{
						HxLOG_Error("Already eFilterStopped-0...!!\n");
					}
					bFoundAny = TRUE;
				}
			}
		}

		if (xFilterId->eLoopMode == DMX_LOOP_ONE_SHOT)
		{
			if (xFilterId->eFilterState == eFilterRunning)
			{
				xFilterId->eFilterState = eFilterStopped;
				if (!bFoundAny)
				{
					TUnsignedInt8 ucCrcCheck = 0x00;
					if (xFilterId->eCrcMode == DMX_CRC_CHECK)
					{
						ucCrcCheck = 0x80;
					}

					if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
					{
						HxLOG_Error("loopMode oneshot, CA_NA_DmxStopFilter error\n");
					}

					if (CA_NA_DmxStartFilter(xFilterId->hAction,
											xFilterId->usPid,
											xFilterId->uTimeout,
											(SI_EVENT_ONETIME_MODE | ucCrcCheck),
											ucFvalue,
											ucFmask,
											&(xFilterId->usFilterId)) != ERR_OK)
					{
						HxLOG_Error("\n[dmxCallbackTable] loopMode oneshot, CA_NA_DmxStartFilter error\n");
                                                goto FREE_EXIT;
					}
					xFilterId->eFilterState = eFilterRunning;
				}
			}
			else
			{
				HxLOG_Error("Already eFilterStopped-1...!!\n");
			}
		}
		else if (xFilterId->eLoopMode == DMX_LOOP_CONTINUOUS)
		{
#if (CONFIG_ENABLE_FILTER_CONTINUE_MODE == 0)
			TUnsignedInt8 ucCrcCheck = 0x00;
			if (xFilterId->eCrcMode == DMX_CRC_CHECK)
			{
				ucCrcCheck = 0x80;
			}

			if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
			{
				HxLOG_Error("loopMode contiuous, CA_NA_DmxStopFilter error\n");
			}

			HxLOG_Print("loopMode continuous, crcCheck:0x%x\n", ucCrcCheck);
			if (CA_NA_DmxStartFilter(xFilterId->hAction
									xFilterId->usPid,
									xFilterId->uTimeout,
									(SI_EVENT_CONT_MODE | ucCrcCheck),
									ucFvalue,
									ucFmask,
									&(xFilterId->usFilterId)) != ERR_OK)
			{
				HxLOG_Error("\n[dmxCallbackTable] loopMode contiuous, CA_NA_DmxStartFilter error\n");
				goto FREE_EXIT;
			}

			HxLOG_Print("loopMode continuous end\n");
#endif
		}
		else if (xFilterId->eLoopMode == DMX_LOOP_TOGGLE)
		{
			if (xFilterId->eFilterState == eFilterRunning)
			{
				TUnsignedInt8 ucCrcCheck = 0x00;
				if (xFilterId->eCrcMode == DMX_CRC_CHECK)
				{
					ucCrcCheck = 0x80;
				}

				if (CA_NA_DmxStopFilter(xFilterId->usFilterId) != ERR_OK)
				{
					HxLOG_Error("loopMode toggle, CA_NA_DmxStopFilter error\n");
				}

				xFilterId->eFilterState = eFilterStopped;
				ucFvalue[0] = ((TUnsignedInt8)usTableId == ucFvalue[0]) ? (xFilterId->ucMatch[0] ^ 0x01) : (xFilterId->ucMatch[0]);
				ucFmask[0] = ucFmask[0] | 0x01;
				xFilterId->ucMatch[0] = ucFvalue[0];

				if (CA_NA_DmxStartFilter(xFilterId->hAction,
										xFilterId->usPid,
										(xFilterId->uTimeout),
										(SI_EVENT_ONETIME_MODE | ucCrcCheck),
										ucFvalue,
										ucFmask,
										&(xFilterId->usFilterId)) != ERR_OK)
				{
					HxLOG_Error("loopMode toggle, CA_NA_DmxStartFilter error\n");
					goto FREE_EXIT;
				}
				xFilterId->eFilterState = eFilterRunning;
			}
			else
			{
				HxLOG_Error("Already eFilterStopped-2...!!\n");
			}
		}
	}
	else	// timeout or crc error or filter error
	{
		if (xFilterId->eFilterState == eFilterRunning)
		{
#if (RETRY_SECTION_FILTER == 1)
			if (bCrcError || bTimeOutError)
#endif
			{
				local_caDmx_CallbackSection(usFilterId, NULL, 0, bCrcError, bTimeOutError, bFilterError);
			}
#if (RETRY_SECTION_FILTER == 1)
			// Filter를 걸지 못해서 다시 걸도록 요청한다.
			if (bFilterError)
			{
				TUnsignedInt8 ucEvtMode;
				TUnsignedInt8 ucCrcCheck = 0x00;

				if (xFilterId->eCrcMode == DMX_CRC_CHECK)
				{
					ucCrcCheck = 0x80;
				}

				HxLOG_Print("Restart Filter....\n");

				if (xFilterId->eLoopMode == DMX_LOOP_CONTINUOUS)
					ucEvtMode = (SI_EVENT_CONT_MODE | ucCrcCheck);
				else
					ucEvtMode = (SI_EVENT_ONETIME_MODE | ucCrcCheck);

				VK_TASK_Sleep(50);
				if (CA_NA_DmxStartFilter(xFilterId->hAction,
										xFilterId->usPid,
										(xFilterId->uTimeout),
										ucEvtMode,
										ucFvalue,
										ucFmask,
										&(xFilterId->usFilterId)) != ERR_OK)
				{
					HxLOG_Error("Could not start filter-0(0x%x)\n", (unsigned int)xFilterId);
					goto FREE_EXIT;
				}
			}
#endif
		}
		else
		{
			HxLOG_Error("Already eFilterStopped-2...!!\n");
		}
	}

FREE_EXIT:
        return;
}


HERROR CA_NA_DmxSetEmmProcess(TUnsignedInt32 hAction, TBoolean bEmmStop)
{
	TUnsignedInt8 i;

	HxLOG_Print("Action:0x%x, Stop:0x%x\n", (unsigned int)hAction, bEmmStop);

	WAIT_FOR_SEM(s_ulDmxFilterSemId)

	for (i=0; i<MAX_DMX_FILTER; i++)
	{
		if (s_bDmxFilterUsedFlag[i] == FALSE)
			continue;

		if ( (s_pstDmxFilterInfo[i]->hAction == hAction)
			&& (s_pstDmxFilterInfo[i]->eLoopMode == DMX_LOOP_CONTINUOUS) )
		{
			s_pstDmxFilterInfo[i]->fEmmStop = bEmmStop;
		}
	}

	RELEASE_SEM(s_ulDmxFilterSemId)
	return ERR_OK;
}


/**************************************************************************************************/
#define ________CA_DMX_Debug_Functions_Body____________________________________________
/**************************************************************************************************/

#if defined(CONFIG_DEBUG)
void CA_NA_DmxEcmSectionTrace(TBoolean bOnOff)
{
	s_bEcmTrace = !s_bEcmTrace;
	Hx_Print(HxANSI_COLOR_GREEN("[%s - %s]")"\n", __FUNCTION__, s_bEcmTrace ? "ON" : "OFF");
}

void CA_NA_DmxEmmSectionTrace(TBoolean bOnOff)
{
	s_bEmmTrace = !s_bEmmTrace;
	Hx_Print(HxANSI_COLOR_GREEN("[%s - %s]")"\n", __FUNCTION__, s_bEmmTrace ? "ON" : "OFF");
}

void CA_NA_DmxPrintInfo(void)
{
	HINT32 i;

	for(i = 0 ; i < MAX_DMX_FILTER ; i++)
	{
		if(s_bDmxFilterUsedFlag[i] != TRUE)
			continue;

		Hx_Print("\n============ DAL-DMX (%d) ============\n", i);
		Hx_Print("\tAction : 0x%x\n", s_pstDmxFilterInfo[i]->hAction);
		Hx_Print("\tPid : 0x%x\n", s_pstDmxFilterInfo[i]->usPid);
		Hx_Print("\tFilterDepth : %d\n", s_pstDmxFilterInfo[i]->ucFilterDepth);
		Hx_Print("\tMatch : %02x %02x %02x %02x %02x %02x %02x %02x\n",
			s_pstDmxFilterInfo[i]->ucMatch[0], s_pstDmxFilterInfo[i]->ucMatch[1],
			s_pstDmxFilterInfo[i]->ucMatch[2], s_pstDmxFilterInfo[i]->ucMatch[3],
			s_pstDmxFilterInfo[i]->ucMatch[4], s_pstDmxFilterInfo[i]->ucMatch[5],
			s_pstDmxFilterInfo[i]->ucMatch[6], s_pstDmxFilterInfo[i]->ucMatch[7]);
		Hx_Print("\tEqualMask : %02x %02x %02x %02x %02x %02x %02x %02x\n",
			s_pstDmxFilterInfo[i]->ucEqualMask[0], s_pstDmxFilterInfo[i]->ucEqualMask[1],
			s_pstDmxFilterInfo[i]->ucEqualMask[2], s_pstDmxFilterInfo[i]->ucEqualMask[3],
			s_pstDmxFilterInfo[i]->ucEqualMask[4], s_pstDmxFilterInfo[i]->ucEqualMask[5],
			s_pstDmxFilterInfo[i]->ucEqualMask[6], s_pstDmxFilterInfo[i]->ucEqualMask[7]);
		Hx_Print("\tNotEqualMask : %02x %02x %02x %02x %02x %02x %02x %02x\n",
			s_pstDmxFilterInfo[i]->ucNotEqualMask[0], s_pstDmxFilterInfo[i]->ucNotEqualMask[1],
			s_pstDmxFilterInfo[i]->ucNotEqualMask[2], s_pstDmxFilterInfo[i]->ucNotEqualMask[3],
			s_pstDmxFilterInfo[i]->ucNotEqualMask[4], s_pstDmxFilterInfo[i]->ucNotEqualMask[5],
			s_pstDmxFilterInfo[i]->ucNotEqualMask[6], s_pstDmxFilterInfo[i]->ucNotEqualMask[7]);
		Hx_Print("\tTimeout : %d\n", s_pstDmxFilterInfo[i]->uTimeout);
		Hx_Print("\tCrcMode : %d\n", s_pstDmxFilterInfo[i]->eCrcMode);
		Hx_Print("\tLoopMode : %d\n", s_pstDmxFilterInfo[i]->eLoopMode);
		Hx_Print("\tFilterId : 0x%x\n", s_pstDmxFilterInfo[i]->usFilterId);
		Hx_Print("\tFilterState : %d\n", s_pstDmxFilterInfo[i]->eFilterState);
		Hx_Print("\tEmmStop : %d\n", s_pstDmxFilterInfo[i]->fEmmStop);
	}
}
#endif

