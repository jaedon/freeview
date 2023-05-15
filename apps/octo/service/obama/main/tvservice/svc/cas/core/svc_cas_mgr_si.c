/**
	@file     _svc_cas_mgr_si.c
	@brief    _svc_cas_mgr_si.c (CAS Service)

	Description:
	Module:  \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>
#include <util.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_pal_port.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_si.h>

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
#include <namgr_main.h>
#endif

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___




/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

#define	ENTER_CRITICAL_CASRAW	VK_SEM_Get(svc_cas_SiGetSemId())
#define	LEAVE_CRITICAL_CASRAW	VK_SEM_Release(svc_cas_SiGetSemId())

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

STATIC HULONG					s_ulCasMgrRawSemId;
STATIC SvcCas_SiRawPool_h		s_phCasMgrSiRawPool;

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrSi" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

STATIC HULONG svc_cas_SiGetSemId(void)
{
	return s_ulCasMgrRawSemId;
}

STATIC SvcCas_SiRawPool_h svc_cas_SiGetRawPool(void)
{
	return s_phCasMgrSiRawPool;
}

STATIC HERROR svc_cas_SiSendCmd2CasMgrTask(SvcCas_Command_e eCmd, Handle_t hAction, HUINT32 ulFilterId, HUINT16 usUniqueId, SvcCas_SiTableState_e eSiTableState)
{
	SvcCas_Message_t stCasMsg;
	HUINT32 nResult = 0;
#if defined(CONFIG_DEBUG)
	HxLOG_Info("%s \n", svc_cas_MgrCmd_MakeStrCasCmd(eCmd));
#endif
	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stCasMsg.checkSiTbl, 0, sizeof(SvcCas_CheckSiMsg_t));

	stCasMsg.checkSiTbl.command = eCmd;
	stCasMsg.checkSiTbl.hAction = hAction;
	stCasMsg.checkSiTbl.ulFilterId = ulFilterId;
	stCasMsg.checkSiTbl.usUniqueId = usUniqueId;
	stCasMsg.checkSiTbl.eSiTableState = eSiTableState;

	// 필요하면, Table Id, Unique Id 등도 보내자.
	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &stCasMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____PMT________________________________________________________________________________________________________
/*
	이후 추가해야 할 Stream Type 발생 시 이 곳에 추가 함.
*/
STATIC HERROR svc_cas_SiGetEsTypeFromStreamType(HUINT8 ucStreamType, EsType_t *peEsType, HUINT16 usUniqueId, HUINT32 ulPmtIndex, HUINT32 ulSubIndex)
{
	HUINT32				hErr = 0;
	HUINT32				ulDesCnt = 0;
	SvcCas_SiRawPool_h	hSiRawPool = svc_cas_SiGetRawPool();

	HxLOG_Info("ucStreamType (%d), UniqueId (%x), PmtIdx (%d), SubIdx (%d) \n",
							ucStreamType, usUniqueId, ulPmtIndex, ulSubIndex);

	switch (ucStreamType)
	{

	/* VIDEO */
	case ISO_VIDEO_1 :
	case ISO_VIDEO_2 :
	case ISO_VIDEO_4 :							// AVC
		*peEsType = eEsType_Video;
		break;

	/* AUDIO */
	case ISO_AUDIO_1 :
	case ISO_AUDIO_2 :
	case ISO_AUDIO_AAC :
	case ISO_AUDIO_4 :							// HE-AAC
		*peEsType = eEsType_Audio;
		break;

	case ISO_PRIVDATA_1 :
	case ISO_PRIVDATA_2 :
		//AC3
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor (hSiRawPool, usUniqueId, ulPmtIndex, ulSubIndex, TAG_DOLBY_AC3, &ulDesCnt);
		if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
		{
			*peEsType = eEsType_Audio;
			break;
		}

		//E-AC3
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor (hSiRawPool, usUniqueId, ulPmtIndex, ulSubIndex, TAG_ENHANCED_AC3, &ulDesCnt);
		if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
		{
			*peEsType = eEsType_Audio;
			break;
		}

		// SubTitle
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor (hSiRawPool, usUniqueId, ulPmtIndex, ulSubIndex, TAG_SUBTITLING, &ulDesCnt);
		if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
		{
			*peEsType = eEsType_Subtitle;
			break;
		}

		// TTX
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor (hSiRawPool, usUniqueId, ulPmtIndex, ulSubIndex, TAG_TELETEXT, &ulDesCnt);
		if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
		{
			*peEsType = eEsType_Teletext;
			break;
		}

		// Application Signaling Descriptor
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor (hSiRawPool, usUniqueId, ulPmtIndex, ulSubIndex, TAG_APPLICATION_SIGNALLING, &ulDesCnt);
		if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
		{
			*peEsType = eEsType_Data;
			break;
		}

		HxLOG_Info("ucStreamType (%x) has no dsc\n", ucStreamType);
		*peEsType = eEsType_None;
		break;

   	case ISO_DSMCC:
    	case ISO_DSMCC_U_N:
	case ISO_DATA:
            *peEsType = eEsType_Data;
            break;


	default :
		HxLOG_Info("ucStreamType (%x) not supported\n", ucStreamType);
		*peEsType = eEsType_None;
		break;
	}

	return ERR_OK;
}

STATIC INLINE void svc_cas_SiGetTableIdAndResult(HUINT32 ulValue, HUINT16 *pusTableId, HUINT16 *pusResult)
{
	*pusResult = ulValue & 0x0000ffff;
	*pusTableId = (ulValue & 0xffff0000) >> 16;
	return;
}

STATIC INLINE void svc_cas_SiGetFilterAndTsUniqueId(HUINT32 ulValue, HUINT16 *pusFilterId, HUINT16 *pusTsUniqId)
{
	*pusTsUniqId = ulValue & 0x0000ffff;
	*pusFilterId = (ulValue & 0xffff0000) >> 16;
	return;
}

#define _____CAT________________________________________________________________________________________________________

#if defined (CAS_SUPPORT_CAT)
STATIC void svc_cas_SiInitCatInfo(SvcCas_CatInfo_t *pstCatInfo)
{
	HUINT32 i;

	if( pstCatInfo == NULL )
		return;

	HxSTD_memset(pstCatInfo, 0x00, sizeof(SvcCas_CatInfo_t));

	for (i = 0; i < CAS_MAX_NUM_OF_CAPID; i++)
	{
		pstCatInfo->stDesc[i].usCaPid = PID_NULL;
	}

	return;
}
#endif

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrSi" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR svc_cas_SiInitModule(void)
{
	HINT32 nResult = 0;

	nResult = VK_SEM_Create(&s_ulCasMgrRawSemId, "CasRawMgr", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \n");
		return ERR_FAIL;
	}

	s_phCasMgrSiRawPool = svc_cas_SvcPortSiRawAllocPool(50, (HUINT8 *)"CasMgrRaw");
	if(s_phCasMgrSiRawPool == NULL)
	{
		HxLOG_Error("Failure in SI RawPool Creation \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SiUpdateTableState(SvcCas_Context_t *pstContext, SiTableType_e eTblType, SvcCas_SiTableState_e eTblState)
{
	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	switch(eTblType)
	{
	case eSI_TableType_PAT: pstContext->ePatState = eTblState; return ERR_OK;
	case eSI_TableType_PMT: pstContext->ePmtState = eTblState; return ERR_OK;
#if defined (CAS_SUPPORT_CAT)
	case eSI_TableType_CAT: pstContext->eCatState = eTblState; return ERR_OK;
#endif
#if defined (CAS_SUPPORT_SDT)
	case eSI_TableType_SDT_ACT:	pstContext->eSdtState = eTblState; return ERR_OK;
#endif
#if defined(CAS_SUPPORT_NIT)
	case eSI_TableType_NIT_ACT: pstContext->eNitState = eTblState; return ERR_OK;
#endif
#if defined(CAS_SUPPORT_EIT)
	case eSI_TableType_EIT_ACTPF: pstContext->eEitState = eTblState; return ERR_OK;
#endif
#if defined(CAS_SUPPORT_TDT)
	case eSI_TableType_TDT: pstContext->eTdtState = eTblState; return ERR_OK;
#endif
#if defined(CAS_SUPPORT_BAT)
	case eSI_TableType_BAT: pstContext->eBatState = eTblState; return ERR_OK;
#endif
	default:
		HxLOG_Error("unknown table type \n");
		return ERR_FAIL;
	}
}

#define _____PAT________________________________________________________________________________________________________

void svc_cas_SiParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usFilterResult = 0;
	HUINT16				 usTableId = 0;
	HUINT16				 usFilterId = 0;
	HUINT16				 usTsUniqId = 0;
	HINT32				 i = 0;
	HUINT32				 ulCount = 0;
	HERROR				 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool = NULL;

	// Get TableId, FilterId, UniqueId, Filter Result
	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usFilterResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usFilterResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;
	if(pstSiRawData == NULL)
	{
		HxLOG_Error("raw data is null.\n");
		return;
	}

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	/** 기존 내용을 지운다. */
	hErr = svc_cas_SvcPortSiRawFreeSection (hSiRawPool, usTsUniqId, eSI_TableType_PAT, FALSE, (HUINT32)0);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	for ( i=0, ulCount=0; ulCount < pstSiRawData->num_sec && i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstSiRawData->buf[i] != NULL)
		{
			HUINT16 usSecLen = 0;
			HUINT8 *pucTemp = NULL, *pucNewSec = NULL;

			pucTemp = pstSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxCAS_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Critical("\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (HUINT32)usSecLen + 3);

			hErr = svc_cas_SvcPortSiRawAddRawSection (hSiRawPool, usTsUniqId, eSI_TableType_PAT, pucNewSec);
			if( hErr != ERR_OK )
			{
				OxCAS_Free(pucNewSec);
			}
			ulCount++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKPAT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreePat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR	hErr;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	hErr = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_PAT, FALSE, (HUINT32)0);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartPatFilter(SvcCas_Context_t *pstContext)
{
	HERROR					hErr = 0;
	HUINT16					usUniqueId = 0;
	HUINT16					usTsId = 0;
	HUINT16					usFilterId = 0;
	HUINT32					ulDemuxId;
	HUINT32					ulActionId;
	SvcCas_FilterParam_t	stFilterParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usTsId);

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stFilterParam.usPid = PAT_PID;
	stFilterParam.ucMode = eCAS_SI_FILTER_MODE_Monitor;

	hErr = svc_cas_SvcPortSiRetrievePatSection(&stFilterParam, svc_cas_SiParsePat, svc_cas_SiFreePat, &usFilterId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("filter start \n");
		return ERR_FAIL;
	}

	pstContext->ulPatFilterId = usFilterId;

	HxLOG_Info("filter id (%d)\n", pstContext->ulPatFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopPatFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id (%d), unique id (0x%x), ts id (0x%x), pat filter id (%d) \n",
		pstContext->ulActionId, pstContext->usUniqueId, pstContext->usTsId, pstContext->ulPatFilterId);

	if(pstContext->ulPatFilterId == CAS_FILTER_ID_NULL)
	{
		/*
			연속 stop이 오는 경우  or
			최초 stop 시 이런 경우가 발생 가능.
		*/
		return ERR_OK;
	}

	/*
		pat에 action이 한개만 남은 경우에 filter stop.
	*/
	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulPatFilterId);

	pstContext->ulPatFilterId = CAS_FILTER_ID_NULL;

	return ERR_OK;
}

HERROR svc_cas_SiUpdatePatInfo(SvcCas_Context_t *pstContext)
{
	HUINT32				nResult = 0;
	HUINT16				usUniqueId = 0;
	HUINT16				usSvcId = 0;
	HUINT16				usPmtPid = 0;
	SvcCas_SiRawPool_h	hSiRawPool;

	if(pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usSvcId = pstContext->usSvcId;
	usUniqueId = pstContext->usUniqueId;

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	nResult = svc_cas_SvcPortSiParseGetPmtPidByServiceId( hSiRawPool, usUniqueId, usSvcId, &usPmtPid);
	if( nResult != ERR_OK )
	{
		HxLOG_Error("\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	pstContext->usPmtPid = usPmtPid;
	pstContext->bPatUpdated = TRUE;

	(void)svc_cas_SvcPortDbUpdatePmtPidInServiceInfo(pstContext->hSvc, usPmtPid);

	LEAVE_CRITICAL_CASRAW;

	return ERR_OK;
}

#define _____PMT________________________________________________________________________________________________________

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-EMM-04 test log -> 150

STATIC void local_HexPrint(HUINT16 ucgap, HUINT32 ulSize, HUINT8* p_data)
{
        HUINT32 i = 0;

#if defined(CONFIG_DEBUG)
        HxLOG_Print("[local_HexPrint] hex_data[%d] : \n", ulSize);
        HLIB_CMD_Printf(" 0x0000 : ");
        for (i = 0; i < ulSize; i++)
        {
                HLIB_CMD_Printf("%02X ", p_data[i]);

              	if (i % ucgap == (ucgap-1))
             	{
                HLIB_CMD_Printf("\n 0x%04x : ", i+1);
              	}
        }
        HLIB_CMD_Printf(" end : \n\n");
#else
	{
		HUINT8 ucTmp[64];
		HUINT8 ucTmp_Big[2048];

		HxSTD_memset(ucTmp, 0x00, 64);
		snprintf(ucTmp, 64, "[CAK-MER-PSI][SI] hex_data[PMT size = %d] : \n", ulSize);
		if (ulSize > 2048)
			return;

		MW_CA_NA_PrintForRelease(ucTmp);

		MW_CA_NA_PrintForRelease(" 0x0000 : ");

		HxSTD_memset(ucTmp_Big, 0x00, 2048);
		for (i = 0; i < ulSize; i++)
		{
			snprintf(ucTmp_Big, 2048, "%02X ", p_data[i]);
			MW_CA_NA_PrintForRelease(ucTmp_Big);

			if (i % ucgap == (ucgap-1))
			{
				HxSTD_memset(ucTmp_Big, 0x00, 2048);
				snprintf(ucTmp_Big, 2048, "\n 0x%04x : ", i+1);
				MW_CA_NA_PrintForRelease(ucTmp_Big);
			}
		}
		MW_CA_NA_PrintForRelease(" end : \n");
	}
#endif


        return;
}
#endif

// SiThd의Task Callback.
void svc_cas_SiParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usFilterResult = 0;
	HUINT16				 usTableId = 0;
	HUINT16				 usFilterId;
	HUINT16				 usTsUniqId = 0;
	HUINT16				 usExtValue = 0;
	HINT32				 i = 0;
	HUINT32				 ulCount = 0;
	HUINT32				 nResult = 0;
	HUINT16				 usSecLen = 0;
	HUINT8				*pucTemp = NULL;
	HUINT8				*pucNewSec = NULL;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	// Get TableId, FilterId, UniqueId, Filter Result
	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%x) TableId(%d), FilterId(%d), UniqueId(%x) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usFilterResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("ParsePMT result = %s\n", (usFilterResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;
	if(pstSiRawData == NULL)
	{
		HxLOG_Error("Input param is invalid \n");
		return;
	}


	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	if (pstSiRawData->num_sec > 0)
	{
		pucTemp = pstSiRawData->buf[0];
		usExtValue = get16bit (pucTemp + 3);

		nResult = svc_cas_SvcPortSiRawFreeSection(hSiRawPool, usTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)usExtValue);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}

	for ( i=0, ulCount=0; ulCount < pstSiRawData->num_sec && i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstSiRawData->buf[i] != NULL)
		{
			pucTemp = pstSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxCAS_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Critical("\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (HUINT32)usSecLen + 3);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-PSI-02 log
#if !defined(CONFIG_DEBUG)
			MW_CA_NA_PrintForRelease("[svc_cas_SiParsePmt] >>> PMT payload");
#endif
			local_HexPrint(32, usSecLen + 3, pucNewSec);
#endif
			nResult = svc_cas_SvcPortSiRawAddRawSection (hSiRawPool, usTsUniqId, eSI_TableType_PMT, pucNewSec);
			if( nResult != ERR_OK )
			{
				OxCAS_Free(pucNewSec);
			}
			ulCount++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	if(i != ulCount)
	{
		HxLOG_Critical("\n");
	}

	nResult = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKPMT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HUINT32 nResult = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	nResult = svc_cas_SvcPortSiRawFreeSection(svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_PMT, TRUE, ulExternalValue);
	if(ERR_OK!=nResult) HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartPmtFilter(SvcCas_Context_t *pstContext)
{
	HERROR 					hErr = 0;
	HUINT16 				usFilterId = 0;
	HUINT16 				usUniqueId = 0;
	HUINT16 				usSvcId = 0;
	HUINT32 				ulActionId;
	HUINT32 				ulDemuxId;
	SvcCas_FilterParam_t	stFilterParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	//2009.03.30 추가.
	if(pstContext->usPmtPid == PID_NULL)
	{
		HxLOG_Error("pmt pid is null\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usSvcId		= pstContext->usSvcId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId(%d), DemuxId(%d), UniqueId(0x%x), ServiceId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usSvcId);

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stFilterParam.ucMode 	= eCAS_SI_FILTER_MODE_Monitor;
	stFilterParam.bUseExtId	= TRUE;
	stFilterParam.usExtId	= usSvcId;
	stFilterParam.usPid		= pstContext->usPmtPid;

	hErr = svc_cas_SvcPortSiRetrievePmtSection(&stFilterParam, (void *)svc_cas_SiParsePmt, (void *)svc_cas_SiFreePmt, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		pstContext->ulPmtFilterId = CAS_FILTER_ID_NULL;
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstContext->ulPmtFilterId = (HUINT32)usFilterId;

	HxLOG_Info("Filter Id (%d) \n", pstContext->ulPmtFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopPmtFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if(pstContext->ulPmtFilterId == CAS_FILTER_ID_NULL)
	{
		HxLOG_Info("FilterId(%d) \n", pstContext->ulPmtFilterId);
		return ERR_OK;
	}

	(void)svc_cas_SvcPortSiStopOneSection(pstContext->ulPmtFilterId);

	pstContext->ulPmtFilterId = CAS_FILTER_ID_NULL;

	return ERR_OK;
}

HERROR svc_cas_SiFreePmtInfo(SvcCas_PmtInfo_t *pstPmtInfo)
{
	HUINT32 i = 0, j = 0;

	HxLOG_Info(" pstPmtInfo->ulNumOfElement (%d) \n", pstPmtInfo->ulNumOfElement);

	for(i = 0; i<pstPmtInfo->ulNumOfElement; i++)
	{
		HxLOG_Info(" pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor (%d) \n", pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor);
		if(pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor != 0)
		{
			for(j=0; j<pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor; j++)
			{
				if(pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].pucRaw != NULL)
				{
					OxCAS_Free(pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].pucRaw);
					pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].pucRaw = NULL;
				}
			}
		}
	}

	HxLOG_Info(" pstPmtInfo->ulNumOfProgramCaDescriptor (%d) \n", pstPmtInfo->ulNumOfProgramCaDescriptor);

	if(pstPmtInfo->ulNumOfProgramCaDescriptor != 0)
	{
		for(i = 0; i<pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			if(pstPmtInfo->stProgramCaDescriptor[i].pucRaw != NULL)
			{
				OxCAS_Free(pstPmtInfo->stProgramCaDescriptor[i].pucRaw);
				pstPmtInfo->stProgramCaDescriptor[i].pucRaw = NULL;
			}
		}
	}

	pstPmtInfo->ulNumOfProgramCaDescriptor = 0;
	HxSTD_memset(pstPmtInfo->stProgramCaDescriptor, 0x00, sizeof(SvcCas_CaDescriptor_t) * CAS_MAX_NUM_OF_CAPID);
	pstPmtInfo->ulNumOfElement = 0;
	HxSTD_memset(pstPmtInfo->stElementInfo, 0x00, sizeof(SvcCas_PmtElement_t) * CAS_MAX_NUM_OF_ES);

	pstPmtInfo->ulSectionState = FALSE;
	pstPmtInfo->usServiceId = 0;
	pstPmtInfo->usPcrPid = PID_NULL;
	pstPmtInfo->ucVerNum = 0;
	pstPmtInfo->bCurNextInd = FALSE;
	pstPmtInfo->ulCrc = 0;
	pstPmtInfo->bProgramCaFlag = FALSE;

	return ERR_OK;
}

HERROR svc_cas_SiUpdatePmtInfo(SvcCas_Context_t *pstContext, SvcCas_PmtInfo_t *pstPmtInfo)
{
	HERROR						  hErr = 0;
	HUINT16						  usUniqueId = 0;
	HUINT16						  usSvcId = 0;
	HUINT32						  ulPmtIndex = 0;
	HUINT32						  ulDesCnt = 0;
	HUINT32						  ulPmtEsCnt = 0;
	HUINT32						  ulIndex = 0, ulIndex2 = 0;
	HUINT8						 *pucDesRaw = NULL;
	HUINT32						  ulDesLen = 0;
	HUINT8						**pucPmtRaw = NULL;
	HUINT32						  ulSecCnt = 0;

	SvcCas_CaDes_t				  stCaMgrCaDes;
	SvcCas_PmtElement_t			 *pstElemInfo = NULL;
	SvcCas_CaDescriptor_t		 *pstCaDescInfo = NULL;
	SvcCas_PmtProgInfo_t		  stPmtProgInfo;
	svcCas_SiPmtElementInfo_t	  stPmtElemInfo;
	SvcCas_SiRawPool_h 			  hSiRawPool;
	SIxTextEncoding_e			  eTextEnc;

	if(pstContext == NULL || pstPmtInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usSvcId = pstContext->usSvcId;
	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info("hAction(0x%08x) svc id (0x%x), unique id(0x%x) \n", pstContext->hAction, usSvcId, usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucPmtRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_PMT, TRUE, usSvcId, &ulSecCnt);
	if(pucPmtRaw == (HUINT8 **)NULL || pucPmtRaw[0] == NULL)
	{
		HxLOG_Error("pmt raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	if(ulSecCnt == 0)
	{
		HxLOG_Error("pmt raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucPmtRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}
	else if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n\n");
	}

	{
		HUINT8		*pucRawSec;
		HINT32		 nSecBytes;
		HUINT16		 usSecLen;

		pucRawSec = pucPmtRaw[0];
		usSecLen = (HUINT16)get12bit(pucRawSec + 1);
		nSecBytes = (HINT32)usSecLen + 3;

		VK_memset32(pstContext->aucPmtRaw, 0x00, sizeof(HUINT8)*CAS_MAX_SECTION_LENGTH);
		VK_memcpy32(pstContext->aucPmtRaw, pucRawSec, nSecBytes);
	}

	hErr = svc_cas_SvcPortSiRawFreeRawArrays(pucPmtRaw);
	if(hErr !=  ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	hErr = svc_cas_SvcPortSiParseGetPmtIndexByServiceId(hSiRawPool, usUniqueId, usSvcId, &ulPmtIndex);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	hErr = svc_cas_SvcPortSiParseGetPmtProgramInfoByServiceId(hSiRawPool, usUniqueId, usSvcId, &stPmtProgInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	svc_cas_SiFreePmtInfo(pstPmtInfo);

	pstPmtInfo->usServiceId	= pstContext->usSvcId;
	pstPmtInfo->usPcrPid		= stPmtProgInfo.usPCRPid;
	pstPmtInfo->ucVerNum		= stPmtProgInfo.ucVer;
	pstPmtInfo->bCurNextInd	= stPmtProgInfo.bCurNextInd;
	pstPmtInfo->ulCrc			= stPmtProgInfo.ulCrc32;

	#ifdef CAS_PMT_PRINT
	HxLOG_Print("=====Pmt ProgInfo=====\n");
	HxLOG_Print("Version No. : 0x%x\n", pstPmtInfo->ucVerNum);
	HxLOG_Print("Curr/Next : 0x%x\n", pstPmtInfo->bCurNextInd);
	HxLOG_Print("PCR ID : 0x%x\n", pstPmtInfo->usPcrPid);
	#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-PSI-02 log

#if defined(CONFIG_DEBUG)
	HxLOG_Error("=====Pmt ProgInfo=====\n");
	HxLOG_Error("Version No. : 0x%x\n", pstPmtInfo->ucVerNum);
	HxLOG_Error("Curr/Next : 0x%x\n", pstPmtInfo->bCurNextInd);
	HxLOG_Error("PCR ID : 0x%x\n", pstPmtInfo->usPcrPid);
#else
{
	HUINT8 ucTmp[32];

	MW_CA_NA_PrintForRelease("\n=====Pmt ProgInfo=====\n");

	HxSTD_memset(ucTmp, 0x00, 32);
	snprintf(ucTmp, 32, "Version No. : 0x%x\n", pstPmtInfo->ucVerNum);
	MW_CA_NA_PrintForRelease(ucTmp);

	HxSTD_memset(ucTmp, 0x00, 32);
	snprintf(ucTmp, 32, "Curr/Next : 0x%x\n", pstPmtInfo->bCurNextInd);
	MW_CA_NA_PrintForRelease(ucTmp);

	HxSTD_memset(ucTmp, 0x00, 32);
	snprintf(ucTmp, 32, "PCR ID : 0x%x\n\n", pstPmtInfo->usPcrPid);
	MW_CA_NA_PrintForRelease(ucTmp);
}
#endif

#endif
	/*
		2009.05.19. current next indicator가 zero인 경우 처리하지 않도록 변경.
	*/
	if(pstPmtInfo->bCurNextInd == 0)
	{
		HxLOG_Info("current next indicator is zero.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_OK;
	}

	eTextEnc = svc_cas_MgrGetTextEncoding(pstContext->ulActionId);

	hErr = svc_cas_SvcPortSiParseCountPmtProgramDescriptor(hSiRawPool, usUniqueId, ulPmtIndex, TAG_CA, &ulDesCnt);
	if(hErr == ERR_OK)
	{
		HxLOG_Info("global ca descriptor number (%d)\n", ulDesCnt);
		pstPmtInfo->ulNumOfProgramCaDescriptor = ulDesCnt;
		if(ulDesCnt != 0)
		{
			pstPmtInfo->bProgramCaFlag = TRUE;
		}

		for(ulIndex = 0; ulIndex < ulDesCnt; ulIndex++)
		{
			pstCaDescInfo = &pstPmtInfo->stProgramCaDescriptor[ulIndex];

			hErr = svc_cas_SvcPortSiParseGetPmtProgramCaDescriptor(hSiRawPool, eTextEnc, usUniqueId, ulPmtIndex, ulIndex, &stCaMgrCaDes);
			if (hErr == ERR_OK)
			{
				pstCaDescInfo->usCaPid = stCaMgrCaDes.usCaPid;
				pstCaDescInfo->usCaSysId = stCaMgrCaDes.usCaSysId;

			#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
				if (pstCaDescInfo->usCaSysId >= NA_CAS_TIVU_RANGE_FROM && pstCaDescInfo->usCaSysId <= NA_CAS_TIVU_RANGE_TO)
				{
					if (!MW_CA_NA_CheckTivuService())
					{
						MW_CA_NA_SetTivuService(TRUE);
					}
				}
				else if (pstCaDescInfo->usCaSysId == NA_CAS_TIVU_TEST_CA_SYSTEM_ID)	// Test stream일 경우 필요
				{
					if (!MW_CA_NA_CheckTivuService())
					{
						MW_CA_NA_SetTivuService(TRUE);
					}
				}
			#endif

			}

			hErr = svc_cas_SvcPortSiParseGetPmtProgramDesRaw(hSiRawPool, usUniqueId, ulPmtIndex, TAG_CA, ulIndex, &ulDesLen, &pucDesRaw);
			if (hErr == ERR_OK && pucDesRaw != NULL)
			{
				pstCaDescInfo->pucRaw = (HUINT8 *)OxCAS_Malloc(sizeof(HUINT8) * ulDesLen);
				if(pstCaDescInfo->pucRaw == NULL)
				{
					HxLOG_Critical("\n");
					svc_cas_SvcPortSiParseFreeDescriptorRaw(pucDesRaw);
					LEAVE_CRITICAL_CASRAW;
					return ERR_FAIL;			// TODO: 당분간 확인 시 ...
				}
				HxSTD_memcpy(pstCaDescInfo->pucRaw, pucDesRaw, sizeof(HUINT8) * ulDesLen);
				pstCaDescInfo->ucRawLen = ulDesLen;
				svc_cas_SvcPortSiParseFreeDescriptorRaw(pucDesRaw);
				pucDesRaw = NULL;
				ulDesLen = 0;
			}

		}
	}

	hErr = svc_cas_SvcPortSiParseCountPmtElement(hSiRawPool, usUniqueId, ulPmtIndex, &ulPmtEsCnt);
	if ( hErr != ERR_OK || ulPmtEsCnt == 0)
	{
		HxLOG_Error("element_count (%d) \n", ulPmtEsCnt);
		LEAVE_CRITICAL_CASRAW;
		return ERR_OK;
	}

	HxLOG_Info("element_count (%d) \n", ulPmtEsCnt);

	pstPmtInfo->ulNumOfElement = ulPmtEsCnt;

	for(ulIndex = 0; ulIndex < ulPmtEsCnt; ulIndex++)
	{
		// Get Element Info.
		HxSTD_memset(&stPmtElemInfo, 0x00, sizeof(svcCas_SiPmtElementInfo_t));

		hErr = svc_cas_SvcPortSiParseGetPmtElementInfo(hSiRawPool, usUniqueId, ulPmtIndex, ulIndex, &stPmtElemInfo);
		if(hErr != ERR_OK)
		{
			continue;
		}

		pstElemInfo = &pstPmtInfo->stElementInfo[ulIndex];

		pstElemInfo->usPid = stPmtElemInfo.usElemPid;
		pstElemInfo->ucStreamType= stPmtElemInfo.ucStreamType;
		svc_cas_SiGetEsTypeFromStreamType(stPmtElemInfo.ucStreamType, &pstElemInfo->eEsType, usUniqueId, ulPmtIndex, ulIndex);

		// Get Ca Desc.
		hErr = svc_cas_SvcPortSiParseCountPmtElementDescriptor(hSiRawPool, usUniqueId, ulPmtIndex, ulIndex, TAG_CA, &ulDesCnt);
		if (hErr == ERR_OK)
		{
			HxLOG_Info("[%d]-th element level ca descriptor (%d) \n", ulIndex, ulDesCnt);
			pstElemInfo->ulNumOfCaDescriptor = ulDesCnt;

			if(ulDesCnt != 0)
			{
				pstElemInfo->bElementCaFlag = TRUE;
			}

			for (ulIndex2 = 0; ulIndex2 < ulDesCnt; ulIndex2++)
			{
				pstCaDescInfo = &pstElemInfo->stElementCaDescriptor[ulIndex2];

				hErr = svc_cas_SvcPortSiParseGetPmtElementCaDescriptor(hSiRawPool, eTextEnc, usUniqueId, ulPmtIndex, ulIndex, ulIndex2, &stCaMgrCaDes);
				if (hErr == ERR_OK)
				{
					pstCaDescInfo->usCaPid = stCaMgrCaDes.usCaPid;
					pstCaDescInfo->usCaSysId= stCaMgrCaDes.usCaSysId;

				#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
					if (pstCaDescInfo->usCaSysId >= NA_CAS_TIVU_RANGE_FROM && pstCaDescInfo->usCaSysId <= NA_CAS_TIVU_RANGE_TO)
					{
						if (!MW_CA_NA_CheckTivuService())
						{
							MW_CA_NA_SetTivuService(TRUE);
						}
					}
					else if (pstCaDescInfo->usCaSysId == NA_CAS_TIVU_TEST_CA_SYSTEM_ID)	// Test stream일 경우 필요
					{
						if (!MW_CA_NA_CheckTivuService())
						{
							MW_CA_NA_SetTivuService(TRUE);
						}
					}
				#endif
				}

				hErr = svc_cas_SvcPortSiParseGetPmtElementDesRaw(hSiRawPool, usUniqueId, ulPmtIndex, ulIndex, TAG_CA, ulIndex2, &ulDesLen, &pucDesRaw);
				if (hErr == ERR_OK && pucDesRaw != NULL)
				{
					pstCaDescInfo->pucRaw = (HUINT8 *)OxCAS_Malloc(sizeof(HUINT8) * ulDesLen);
					if(pstCaDescInfo->pucRaw == NULL)
					{
						HxLOG_Critical("\n");
						svc_cas_SvcPortSiParseFreeDescriptorRaw(pucDesRaw);
						LEAVE_CRITICAL_CASRAW;
						return ERR_FAIL;			// TODO: 당분간 확인 시 ...
					}
					HxSTD_memcpy(pstCaDescInfo->pucRaw, pucDesRaw, sizeof(HUINT8) * ulDesLen);
					pstCaDescInfo->ucRawLen = ulDesLen;
					svc_cas_SvcPortSiParseFreeDescriptorRaw(pucDesRaw);
					pucDesRaw = NULL;
					ulDesLen = 0;
				}
			}
		}
	}

	LEAVE_CRITICAL_CASRAW;

	pstContext->stPmtInfo.ulSectionState = TRUE;


#if 0
	{
		HUINT32 i = 0;

		if(pstContext->stPmtInfo.stProgramCaDescriptor[0].pucRaw != NULL)
		{
			HxLOG_Print("******** pmt raw bytes *********\n");

			for(i=0; i<pstContext->stPmtInfo.stProgramCaDescriptor[0].ucRawLen; i++)
			{
				HxLOG_Print("0x%02x ", pstContext->stPmtInfo.stProgramCaDescriptor[0].pucRaw[i]);
				if ((i%10) == 9)
					HxLOG_Print("\n");
			}
			HxLOG_Print("\n**************************\n");
		}

		svc_cas_DbgPrintPmtInfo(&pstContext->stPmtInfo);
	}
#endif

	return ERR_OK;
}

HERROR CasMgrSI_DefineChannelType(SvcCas_Context_t *pstContext, SvcCas_ChannelType_e *peChType)
{
	SvcCas_ChannelType_e eType;
	HUINT32 i = 0;

	if (pstContext == NULL)
	{
		*peChType = eCasChannel_Unknown;
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	eType = eCasChannel_FTA;

	if(pstContext->stPmtInfo.bProgramCaFlag == TRUE)
	{
		eType = eCasChannel_Scramble;
	}
	else
	{
		for(i=0; i<pstContext->stPmtInfo.ulNumOfElement; i++)
		{
			if(pstContext->stPmtInfo.stElementInfo[i].bElementCaFlag == TRUE)
			{
				eType = eCasChannel_Scramble;
				break;
			}
		}
	}

	*peChType = eType;

	return ERR_OK;
}

#define _____CAT________________________________________________________________________________________________________

#if defined (CAS_SUPPORT_CAT)
void svc_cas_SiParseCat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usFilterResult = 0;
	HUINT16				 usTableId = 0;
	HUINT16				 usFilterId = 0;
	HUINT16				 usTsUniqId = 0;
	HINT32				 i = 0, nCount = 0;
	HERROR				 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	// Get TableId, FilterId, UniqueId, Filter Result
	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usFilterResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usFilterResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	pstSiRawData = (PalSef_RawData_t*)ulParam;
	if(pstSiRawData == NULL)
	{
		HxLOG_Error("raw data is null.\n");
		return;
	}

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	/** 기존 내용을 지운다. */
	hErr = svc_cas_SvcPortSiRawFreeSection (hSiRawPool, usTsUniqId, eSI_TableType_CAT, FALSE, (HUINT32)0);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	for ( i=0, nCount=0; nCount < pstSiRawData->num_sec && i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstSiRawData->buf[i] != NULL)
		{
			HUINT16 usSecLen = 0;
			HUINT8 *pucTemp = NULL, *pucNewSec = NULL;

			pucTemp = pstSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxCAS_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Critical("\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (HUINT32)usSecLen + 3);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)	// cak test 중 CAK-MER-PSI-02 log
#if !defined(CONFIG_DEBUG)
			MW_CA_NA_PrintForRelease("[svc_cas_SiParseCat] >>> CAT payload");
#endif
			local_HexPrint(32, usSecLen + 3, pucNewSec);
#endif

			hErr = svc_cas_SvcPortSiRawAddRawSection (hSiRawPool, usTsUniqId, eSI_TableType_CAT, pucNewSec);
			if( hErr != ERR_OK )
			{
				OxCAS_Free(pucNewSec);
			}
			nCount++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKCAT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeCat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR hErr = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	hErr = svc_cas_SvcPortSiRawFreeSection(svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_CAT, FALSE, (HUINT32)0);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartCatFilter(SvcCas_Context_t *pstContext)
{
	HUINT16					usTsId = 0;
	HUINT16					usFilterId = 0;
	HUINT16					usUniqueId = 0;
	HUINT32 				ulActionId;
	HUINT32					ulDemuxId;
	HERROR					hErr = 0;
	SvcCas_FilterParam_t	stFilterParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usTsId);


	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stFilterParam.usPid = CAT_PID;
	stFilterParam.ucMode = eSiFilterMode_Monitor;

	hErr = svc_cas_SvcPortSiRetrieveCatSection( &stFilterParam, (void *)svc_cas_SiParseCat, (void *)svc_cas_SiFreeCat, &usFilterId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("filter start \n");
		return ERR_FAIL;
	}

	pstContext->ulCatFilterId = usFilterId;

	HxLOG_Info("filter id (%d)\n", pstContext->ulCatFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopCatFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id (%d), unique id (0x%x), ts id (0x%x), cat filter id (%d) \n",
		pstContext->ulActionId, pstContext->usUniqueId, pstContext->usTsId, pstContext->ulCatFilterId);

	if(pstContext->ulCatFilterId == CAS_FILTER_ID_NULL)
	{
		/*
			연속 stop이 오는 경우  or
			최초 stop 시 이런 경우가 발생 가능.
		*/
		return ERR_OK;
	}

	/*
		cat에 action이 한개만 남은 경우에 filter stop.
	*/
	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulCatFilterId);

	pstContext->ulCatFilterId = CAS_FILTER_ID_NULL;

	return ERR_OK;
}

HERROR svc_cas_SiUpdateCatInfo(SvcCas_Context_t *pstContext)
{
	SvcCas_CatInfo_t	 *pstCatInfo = NULL;
	HUINT8				**ppucCatRaw = NULL;
	HUINT32				  nResult = 0;
	HUINT16				  usUniqueId = 0;
	HUINT32				  ulSecCnt = 0;
	HUINT32				  i = 0, j = 0, ulDesCnt = 0, ulNumCaPid = 0;
	SvcCas_CaDes_t		  stCaMgrCaDes;
	SvcCas_SiRawPool_h	  hSiRawPool;
	SIxTextEncoding_e	  eTextEnc;

	if(pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	pstCatInfo = &pstContext->stCatInfo;

	svc_cas_SiInitCatInfo(pstCatInfo);

	ENTER_CRITICAL_CASRAW;
	hSiRawPool = svc_cas_SiGetRawPool();

	ppucCatRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_CAT, FALSE, 0, &ulSecCnt);
	if(ppucCatRaw == (HUINT8 **)NULL)
	{
		HxLOG_Error("cat raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	if(ulSecCnt == 0)
	{
		HxLOG_Error("cat raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(ppucCatRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}
	else if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n");
	}

	{
		HUINT8		*pucRawSec;
		HINT32		nSecBytes;
		HUINT16		usSecLen;

		pucRawSec = ppucCatRaw[0];
		usSecLen = (HUINT16)get12bit(pucRawSec + 1);
		nSecBytes = (HINT32)usSecLen + 3;

		VK_memset32(pstContext->aucCatRaw, 0x00, sizeof(HUINT8)*CAS_MAX_SECTION_LENGTH);
		VK_memcpy32(pstContext->aucCatRaw, pucRawSec, nSecBytes);
	}

	nResult = svc_cas_SvcPortSiRawFreeRawArrays(ppucCatRaw);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	//#define CAS_CAT_PRINT
	#ifdef CAS_CAT_PRINT
	{
		HUINT8 *pucData;
		HUINT32 i, ulLen;

		pucData = pstContext->aucCatRaw;

		HxLOG_Print("******** 1 cat raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Error("\n");
		}
		HxLOG_Print("\n**************************\n");


		pucData = pstContext->aucCatRaw[0];

		HxLOG_Print("******** 2 cat raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");
	}
	#endif

	nResult = svc_cas_SvcPortSiParseCountCatSection(hSiRawPool, usUniqueId, &ulSecCnt );
	if( nResult != ERR_OK )
	{
		HxLOG_Error(" CountCatSection failed \n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info("num of cat sec (%d) \n", ulSecCnt);

	eTextEnc = svc_cas_MgrGetTextEncoding(pstContext->ulActionId);

	for( i=0; i<ulSecCnt; i++ )
	{
		ulDesCnt = 0;
		nResult = svc_cas_SvcPortSiParseCountCatDescriptor( hSiRawPool, usUniqueId, i, TAG_CA, &ulDesCnt );
		if( nResult != ERR_OK ) continue;

		HxLOG_Info("num of cat ca desc (%d) \n", ulDesCnt);

		for( j=0; j<ulDesCnt; j++ )
		{
			nResult = svc_cas_SvcPortSiParseGetCatDescriptor(hSiRawPool, eTextEnc, usUniqueId, i, TAG_CA, j, &stCaMgrCaDes );
			if (nResult == ERR_OK)
			{
				pstCatInfo->stDesc[ulNumCaPid].usCaPid = stCaMgrCaDes.usCaPid;
				pstCatInfo->stDesc[ulNumCaPid].usCaSysId = stCaMgrCaDes.usCaSysId;

				/*
					이슈가 존재하여 당분간 출력.
				*/
				HxLOG_Info("\t\t\t %d-th, ca pid (0x%x), ca system id (0x%x)\n",
								ulNumCaPid, stCaMgrCaDes.usCaPid, stCaMgrCaDes.usCaSysId);

				ulNumCaPid++;
			}
		}
	}

	pstCatInfo->ulDescNum = ulNumCaPid;

	pstContext->bCatUpdated = TRUE;

	HxLOG_Info("num of ca desc (%d) \n", pstCatInfo->ulDescNum);

	LEAVE_CRITICAL_CASRAW;

	return ERR_OK;
}

#endif

#define _____SDT________________________________________________________________________________________________________

#if defined (CAS_SUPPORT_SDT)
HERROR svc_cas_SiUpdateSdtRawData(SvcCas_Context_t *pstContext)
{
	HUINT32				  nResult = 0;
	HUINT16				  usUniqueId = 0;
	HUINT8				**pucSdtRaw = NULL, *pucSdt = NULL;
	HUINT32				  ulSecCnt = 0, ulLen = 0;
	SvcCas_SiRawPool_h	  hSiRawPool;

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info(" unique id (%x) \n", usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucSdtRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_SDT_ACT, FALSE, 0, &ulSecCnt);
	if(pucSdtRaw == NULL)
	{
		HxLOG_Error("sdt raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info(" ulSecCnt (%x) \n", ulSecCnt);

	if(ulSecCnt == 0)
	{
		HxLOG_Error("sdt raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucSdtRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}
	else if(ulSecCnt > 1)
	{
		HxLOG_Error("SDT Section Count is more than 1...\n\n");
	}
	// Section Count가 1보다 커도 Sub CAS에서 사용하는 SDT는 가장 처음 데이터 하나만 사용하기 때문에 pstContext의 aucSdtRaw에는 첫번째 SDT RAW data만 저장하여 넘기도록 한다.
	{
		pucSdt = pucSdtRaw[0];
		ulLen = ( get12bit(pucSdt+1) + 3 );

		HxSTD_MemSet(pstContext->aucSdtRaw, 0x00, sizeof(HUINT8)*CAS_MAX_SECTION_LENGTH);
		HxSTD_MemCopy(pstContext->aucSdtRaw, pucSdt, ulLen);
	}

	nResult = svc_cas_SvcPortSiRawFreeRawArrays(pucSdtRaw);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	LEAVE_CRITICAL_CASRAW;
	return ERR_OK;
}

void svc_cas_SiParseSdt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8				 ucHead;
	HUINT16				 usResult, usTableId, usFilterId, usTsUniqId;
	HUINT16				 usSecLen;
	HUINT16				 usIdx, usSecNum;
	HUINT16				 usTsId;
	HUINT8				*pucRaw, *pucNewRaw;
	HERROR				 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	svc_cas_SiGetTableIdAndResult (ulTableIdAndResult, &usTableId, &usResult);
	svc_cas_SiGetFilterAndTsUniqueId (ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("unique id %x \n", usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");

		/* 일단 timeout과 error는 모두 timeout으로 처리한다. */
		hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKSDT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Timeout);
		return;
	}

	HxLOG_Info("ActionHandle(0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	ucHead = (HUINT8)((usTableId == SDT_ACTUAL_TABLE_ID) ? eCAS_FILTER_HEAD_Act : eCAS_FILTER_HEAD_Oth);

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	if (pstSiRawData->num_sec > 0)
	{
		HxLOG_Info(" number of section %d \n", pstSiRawData->num_sec);
		pucRaw = pstSiRawData->buf[0];
		usTsId = get16bit (pucRaw + 3);

		svc_cas_SvcPortSiRawFreeSection(hSiRawPool, usTsUniqId, eSI_TableType_SDT_ACT + ucHead, TRUE, (HUINT32)usTsId);
	}

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			pucRaw = pstSiRawData->buf[usIdx];
			usSecLen = (HUINT16)get12bit (pucRaw + 1);

			pucNewRaw = (HUINT8 *)OxCAS_Malloc ((HUINT32)usSecLen + 3);
			if (pucNewRaw == NULL)
			{
				HxLOG_Critical("\n\n\n");
				break;
			}

			HxSTD_memcpy (pucNewRaw, pucRaw, (HUINT32)usSecLen + 3);
			hErr = svc_cas_SvcPortSiRawAddRawSection(hSiRawPool, usTsUniqId, eSI_TableType_SDT_ACT + ucHead, pucNewRaw);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Add Section in RawPool at %d-th Section \n", usIdx);
				OxCAS_Free (pucNewRaw);
			}

			usSecNum++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	HxLOG_Info("SDT[UniqueId(0x%x)] is Received \n", usTsUniqId);

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKSDT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeSdt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HUINT32 nResult = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	nResult = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_SDT_ACT, TRUE, ulExternalValue);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartSdtFilter(SvcCas_Context_t *pstContext)
{
	HUINT16					usFilterId = 0;
	HUINT16					usUniqueId = 0;
	HUINT16					usTsId = 0;
	HUINT32					ulDemuxId;
	HUINT32 				ulActionId;
	HERROR					hErr = 0;
	SvcCas_FilterParam_t	stParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usTsId);

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stParam.usPid = SDT_PID;
	stParam.ucMode = eSiFilterMode_Monitor;
	stParam.ucHead = (HUINT8)eCAS_FILTER_HEAD_Act;
	stParam.bUseExtId = TRUE;
	stParam.usExtId = usTsId;

	hErr  = svc_cas_SvcPortSiRetrieveSdtSection(&stParam, svc_cas_SiParseSdt, svc_cas_SiFreeSdt, &usFilterId);
	if(hErr  != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstContext->ulSdtFilterId = (HUINT32)usFilterId;

	HxLOG_Info("Filter Id (%d) \n", pstContext->ulSdtFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopSdtFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulSdtFilterId);

	pstContext->ulSdtFilterId = CAS_FILTER_ID_NULL;

	return ERR_OK;
}

#endif	// CAS_SUPPORT_SDT

#define _____NIT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_NIT)
HERROR svc_cas_SiFreeNitRaw(SvcCas_Context_t *pstContext)
{
	int i = 0;

	for(i = 0; i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstContext->aucNitRaw[i] != NULL)
			OxCAS_Free(pstContext->aucNitRaw[i]);

		pstContext->aucNitRaw[i] = NULL;
	}

	return ERR_OK;
}

HERROR svc_cas_SiUpdateNitRawData(SvcCas_Context_t *pstContext)
{
	HUINT32				  nResult = 0;
	HUINT16				  usUniqueId = 0;
	HUINT8				**pucNitRaw = NULL, *pucNit = NULL;
	HUINT32				  ulSecCnt = 0, ulLen = 0;
	HUINT32				  ulIndex = 0;
	SvcCas_SiRawPool_h	  hSiRawPool;

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info(" unique id (%x) \n", usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucNitRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_NIT_ACT, FALSE, 0, &ulSecCnt);
	if(pucNitRaw == (HUINT8 **)NULL)
	{
		HxLOG_Error("nit raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info(" ulSecCnt (%x) \n", ulSecCnt);

	if(ulSecCnt == 0)
	{
		HxLOG_Error("nit raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucNitRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	for(ulIndex = 0; ulIndex < ulSecCnt; ulIndex++)
	{
		pucNit = pucNitRaw[ulIndex];
		ulLen = ( get12bit(pucNit+1) + 3 );

		pstContext->aucNitRaw[ulIndex] = (HUINT8 *)OxCAS_Malloc(ulLen);
		if (pstContext->aucNitRaw[ulIndex] == NULL)
		{
			break;
		}
		VK_memcpy32(pstContext->aucNitRaw[ulIndex], pucNit, sizeof(HUINT8)*ulLen);
	}

	if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n\n");
	}

	//#define CAS_NIT_PRINT
	#ifdef CAS_NIT_PRINT
	{
		HUINT8 *pucData;
		HUINT32 i;

		pucData = pucNitRaw[0];

		HxLOG_Print("******** 1 nit raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");


		pucData = pstContext->aucNitRaw[0];

		HxLOG_Print("******** 2 nit raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");
	}
	#endif

	nResult = svc_cas_SvcPortSiRawFreeRawArrays(pucNitRaw);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	LEAVE_CRITICAL_CASRAW;

	return ERR_OK;
}

void svc_cas_SiParseNit(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usFilterResult = 0;
	HUINT16				 usTableId = 0;
	HUINT16				 usFilterId = 0;
	HUINT16				 usTsUniqId = 0;
	HINT32				 i = 0, nCount = 0;
	HUINT32 			 nResult = 0;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	// Get TableId, FilterId, UniqueId, Filter Result
	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usFilterResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usFilterResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;
	if(pstSiRawData == NULL)
	{
		HxLOG_Error("raw data is null.\n");
		return;
	}

	ENTER_CRITICAL_CASRAW;
	hSiRawPool = svc_cas_SiGetRawPool();

	/** 기존 내용을 지운다. */
	nResult = svc_cas_SvcPortSiRawFreeSection (hSiRawPool, usTsUniqId, eSI_TableType_NIT_ACT, FALSE, (HUINT32)0);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	for ( i=0, nCount=0; nCount < pstSiRawData->num_sec && i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstSiRawData->buf[i] != NULL)
		{
			HUINT16		 usSecLen = 0;
			HUINT8		*pucTemp = NULL, *pucNewSec = NULL;

			pucTemp = pstSiRawData->buf[i];					//NIT_ACT => 0x40
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxCAS_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Critical("\n\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (HUINT32)usSecLen + 3);

			nResult = svc_cas_SvcPortSiRawAddRawSection (hSiRawPool, usTsUniqId, eSI_TableType_NIT_ACT, pucNewSec);
			if( nResult != ERR_OK )
			{
				OxCAS_Free(pucNewSec);
			}
			nCount++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	// Message Send to Cas Task.
	nResult = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKNIT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeNit(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HUINT32 nResult = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	nResult = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_NIT_ACT, FALSE, (HUINT32)0);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartNitFilter(SvcCas_Context_t *pstContext)
{
	HUINT16					usUniqueId = 0;
	HUINT16					usTsId = 0;
	HUINT16					usFilterId = 0;
	HUINT32					ulActionId;
	HUINT32					ulDemuxId;
	HERROR					hErr = 0;
	SvcCas_FilterParam_t	stRetriveParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usTsId);

	if(pstContext->ulNitFilterId != CAS_FILTER_ID_NULL)
	{
		// tp changed 가 아니라서 NIT filter stop이 안된 경우 다시 걸 필요가 없음
		return ERR_OK;
	}

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stRetriveParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stRetriveParam.usPid = NIT_PID;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;

	hErr = svc_cas_SvcPortSiRetrieveNitSection(&stRetriveParam, (void *)svc_cas_SiParseNit, (void *)svc_cas_SiFreeNit, &usFilterId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("filter start \n");
		return ERR_FAIL;
	}

	pstContext->ulNitFilterId = usFilterId;

	HxLOG_Info("filter id (%d)\n", pstContext->ulNitFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopNitFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id (%d), unique id (0x%x), ts id (0x%x), Nit filter id (%d) \n",
		pstContext->ulActionId, pstContext->usUniqueId, pstContext->usTsId, pstContext->ulCatFilterId);

	if(pstContext->ulNitFilterId == CAS_FILTER_ID_NULL)
	{
		/*
			연속 stop이 오는 경우  or
			최초 stop 시 이런 경우가 발생 가능.
		*/
		return ERR_OK;
	}

	/*
		nit에 action이 한개만 남은 경우에 filter stop.
	*/
	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulNitFilterId);

	pstContext->ulNitFilterId = CAS_FILTER_ID_NULL;
	return ERR_OK;
}
#endif

#define _____BAT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_BAT)
HERROR svc_cas_SiUpdateBatRawData(SvcCas_Context_t *pstContext)
{
	HUINT8				 *pucBat = NULL;
	HUINT8				**pucBatRaw = NULL;
	HUINT16				  usUniqueId = 0;
	HUINT32				  ulSecCnt = 0;
	HUINT32				  ulLen = 0;
	HUINT32				  ulIndex = 0;
	HERROR				  hErr;
	SvcCas_SiRawPool_h	  hSiRawPool;

	if(pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info(" unique id (%x) \n", usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucBatRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_BAT, FALSE, 0, &ulSecCnt);
	if(pucBatRaw == (HUINT8 **)NULL)
	{
		HxLOG_Error("bat raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info(" ulSecCnt (%x) \n", ulSecCnt);

	if(ulSecCnt == 0)
	{
		HxLOG_Error("bat raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucBatRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	for(ulIndex = 0; ulIndex < ulSecCnt; ulIndex++)
	{
		pucBat = pucBatRaw[ulIndex];
		ulLen = ( get12bit(pucBat+1) + 3 );

		pstContext->aucBatRaw[ulIndex] = (HUINT8 *)OxCAS_Malloc(ulLen);
		if (pstContext->aucBatRaw[ulIndex] == NULL)
		{
			break;
		}
		VK_memcpy32(pstContext->aucBatRaw[ulIndex], pucBat, sizeof(HUINT8)*ulLen);
	}

	if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n\n");
	}

	//#define CAS_BAT_PRINT
	#ifdef CAS_BAT_PRINT
	{
		HUINT8 *pucData;
		HUINT32 i;

		pucData = pucBatRaw[0];

		HxLOG_Print("******** 1 bat raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");


		pucData = pstContext->aucBatRaw[0];

		HxLOG_Print("******** 2 bat raw bytes *********\n");
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");
	}
	#endif

	hErr = svc_cas_SvcPortSiRawFreeRawArrays(pucBatRaw);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	LEAVE_CRITICAL_CASRAW;
	return ERR_OK;
}


void svc_cas_SiParseBat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usFilterResult = 0;
	HUINT16				 usTableId = 0;
	HUINT16				 usFilterId = 0;
	HUINT16				 usTsUniqId = 0;
	HINT32 				 i = 0, nCount = 0;
	HERROR				 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	// Get TableId, FilterId, UniqueId, Filter Result
	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usFilterResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usFilterResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	pstSiRawData = (PalSef_RawData_t*)ulParam;
	if(pstSiRawData == NULL)
	{
		HxLOG_Error("raw data is null.\n");
		return;
	}

	ENTER_CRITICAL_CASRAW;
	hSiRawPool = svc_cas_SiGetRawPool();

	/** 기존 내용을 지운다. */
	hErr = svc_cas_SvcPortSiRawFreeSection(hSiRawPool, usTsUniqId, eSI_TableType_BAT, FALSE, (HUINT32)0);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	for ( i=0, nCount=0; nCount < pstSiRawData->num_sec && i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstSiRawData->buf[i] != NULL)
		{
			HUINT16 usSecLen = 0;
			HUINT8 *pucTemp = NULL, *pucNewSec = NULL;

			pucTemp = pstSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxCAS_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Critical("\n\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (HUINT32)usSecLen + 3);

			hErr = svc_cas_SvcPortSiRawAddRawSection(hSiRawPool, usTsUniqId, eSI_TableType_BAT, pucNewSec);
			if( hErr != ERR_OK )
			{
				OxCAS_Free(pucNewSec);
			}
			nCount++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKBAT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeBatRaw(SvcCas_Context_t *pstContext)
{
	int i = 0;

	for(i = 0; i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstContext->aucBatRaw[i] != NULL)
			OxCAS_Free(pstContext->aucBatRaw[i]);

		pstContext->aucBatRaw[i] = NULL;
	}
}

void svc_cas_SiFreeBat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HUINT32 nResult = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	nResult = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_BAT, FALSE, (HUINT32)0);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartBatFilter(SvcCas_Context_t *pstContext)
{
	HUINT32					ulDemuxId;
	HUINT32					ulActionId;
	HUINT16					usUniqueId = 0;
	HUINT16					usTsId = 0;
	HUINT32					hErr = 0;
	HUINT16					usFilterId = 0;
	SvcCas_FilterParam_t	stRetriveParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId, usTsId);

	if(pstContext->ulBatFilterId != CAS_FILTER_ID_NULL)
	{
		// tp changed 가 아니라서 bat filter stop이 안된 경우 다시 걸 필요가 없음
		return ERR_OK;
	}

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stRetriveParam, pstContext->hAction, usUniqueId, ulDemuxId);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");

	stRetriveParam.usPid		= BAT_PID;
	stRetriveParam.ucMode		= eSiFilterMode_Monitor;
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	stRetriveParam.bUseExtId	= TRUE;
	stRetriveParam.usExtId		= (HUINT16)eBOUQEUT_ID_IRDETO_SOL;
#endif

	hErr = svc_cas_SvcPortSiRetrieveBatSection( &stRetriveParam, svc_cas_SiParseBat, svc_cas_SiFreeBat, &usFilterId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("filter start \n");
		return ERR_FAIL;
	}

	pstContext->ulBatFilterId = usFilterId;

	HxLOG_Info("filter id (%d)\n", pstContext->ulBatFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopBatFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id (%d), unique id (0x%x), ts id (0x%x), cat filter id (%d) \n",
		pstContext->ulActionId, pstContext->usUniqueId, pstContext->usTsId, pstContext->ulBatFilterId);

	if(pstContext->ulBatFilterId == CAS_FILTER_ID_NULL)
	{
		/*
			연속 stop이 오는 경우  or
			최초 stop 시 이런 경우가 발생 가능.
		*/
		return ERR_OK;
	}

	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulBatFilterId);

	pstContext->ulBatFilterId = CAS_FILTER_ID_NULL;
	return ERR_OK;
}

#endif

#define _____EIT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_EIT)
HERROR svc_cas_SiFreeEitRaw(SvcCas_Context_t *pstContext)
{
	HINT32 i = 0;

	for(i = 0; i < CAS_MAX_SECTION_NUM; i++)
	{
		if(pstContext->aucEitRaw[i] != NULL)
			OxCAS_Free(pstContext->aucEitRaw[i]);

		pstContext->aucEitRaw[i] = NULL;
	}

	return ERR_OK;
}

HERROR svc_cas_SiUpdateEitRawData(SvcCas_Context_t *pstContext)
{
	HERROR				  hErr = 0;
	HUINT16				  usUniqueId = 0;
	HUINT8				**pucEitRaw = NULL, *pucEit = NULL;
	HUINT32				  ulSecCnt = 0, ulLen = 0;
	HUINT32				  ulIndex = 0;
	SvcCas_SiRawPool_h	  hSiRawPool;

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info(" unique id (%x) \n", usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucEitRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_EIT_ACTPF, FALSE, 0, &ulSecCnt);
	if(pucEitRaw == NULL)
	{
		HxLOG_Error("eit raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info(" ulSecCnt (%x) \n", ulSecCnt);

	if(ulSecCnt == 0)
	{
		HxLOG_Error("eit raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucEitRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	for(ulIndex = 0; ulIndex < ulSecCnt; ulIndex++)
	{
		pucEit = pucEitRaw[ulIndex];
		ulLen = ( get12bit(pucEit+1) + 3 );
		HxLOG_Info("ulLen = %d, pstContext->usSvcId = 0x%x, pstContext->usOrgNetId = 0x%x, pstContext->usTsId = 0x%x\n", ulLen, pstContext->usSvcId, pstContext->usOrgNetId, pstContext->usTsId);

		#ifdef CAS_EIT_PRINT
		{
			HUINT16 usSvcId =0, usTsId = 0, usOnId = 0;

			usSvcId = (HUINT16)get16bit(pucEit+3);
			usTsId = (HUINT16)get16bit(pucEit+8);
			usOnId = (HUINT16)get16bit(pucEit+10);
			HxLOG_Print("usSvcId = 0x%x, usTsId = 0x%x, usOnId = 0x%x\n", usSvcId, usTsId, usOnId);
		}
		#endif

		pstContext->aucEitRaw[ulIndex] = (HUINT8 *)OxCAS_Malloc(ulLen);
		if (pstContext->aucEitRaw[ulIndex] == NULL)
		{
			break;
		}
		VK_memset32(pstContext->aucEitRaw[ulIndex], 0, ulLen);
		VK_memcpy32(pstContext->aucEitRaw[ulIndex], pucEit, sizeof(HUINT8)*ulLen);

		#ifdef CAS_EIT_PRINT
		{
			HUINT8 *pucData;
			HUINT32 i;

			pucData = pucEitRaw[ulIndex];

			HxLOG_Print("******** %d eit raw bytes *********\n", ulIndex);
			for (i=0; i<ulLen; i++)
			{
				HxLOG_Print("0x%02x ", pucData[i]);
				if ((i%10) == 9)
					HxLOG_Print("\n");
			}
			HxLOG_Print("\n**************************\n");
		}
		#endif
	}

	if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n\n");
	}

	hErr = svc_cas_SvcPortSiRawFreeRawArrays(pucEitRaw);
	if(ERR_OK != hErr)			HxLOG_Critical("Critical Eror!!!\n");

	LEAVE_CRITICAL_CASRAW;
	return ERR_OK;
}

void svc_cas_SiParseEit(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8			 	 ucHead;
	HUINT16				 usResult, usTableId, usFilterId, usTsUniqId;
	HUINT16				 usSecLen;
	HUINT16				 usIdx, usSecNum;
	HUINT16				 usTsId;
	HUINT8				*pucRaw, *pucNewRaw;
	HERROR			 	 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	HxLOG_Info("hAction(%d)\n", hAction);

	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	HxLOG_Info("ActionHandle(0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	ucHead = (usTableId == EIT_ACTUAL_TABLE_ID) ? eCAS_FILTER_HEAD_Act : eCAS_FILTER_HEAD_Oth;

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	if (pstSiRawData->num_sec > 0)
	{
		HxLOG_Info(" number of section %d \n", pstSiRawData->num_sec);
		pucRaw = pstSiRawData->buf[0];
		usTsId = get16bit (pucRaw + 3);

		svc_cas_SvcPortSiRawFreeSection(hSiRawPool, usTsUniqId, eSI_TableType_EIT_ACTPF + ucHead, FALSE, 0);
	}

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			pucRaw = pstSiRawData->buf[usIdx];
			usSecLen = (HUINT16)get12bit (pucRaw + 1);

			pucNewRaw = (HUINT8 *)OxCAS_Malloc ((HUINT32)usSecLen + 3);
			if (pucNewRaw == NULL)
			{
				HxLOG_Critical("\n\n\n");
				break;
			}

			HxSTD_memcpy (pucNewRaw, pucRaw, (HUINT32)usSecLen + 3);
			hErr = svc_cas_SvcPortSiRawAddRawSection (hSiRawPool, usTsUniqId, eSI_TableType_EIT_ACTPF + ucHead, pucNewRaw);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Add Section in RawPool at %d-th Section \n", usIdx);
				OxCAS_Free (pucNewRaw);
			}

			usSecNum++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	HxLOG_Info("EIT[UniqueId(0x%x)] is Received \n", usTsUniqId);

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKEIT, hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeEit(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR	hErr;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	hErr = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_EIT_ACTPF, TRUE, (HUINT32)ulExternalValue);
	if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}


HERROR svc_cas_SiStartEitFilter(SvcCas_Context_t *pstContext)
{
	HUINT16					usFilterId = 0;
	HUINT16					usUniqueId = 0, usTsId = 0, usSvcId = 0;
	HUINT32					ulDemuxId;
	HUINT32					ulActionId;
	HERROR					hErr;
	SvcCas_FilterParam_t	stRetriveParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	usTsId		= pstContext->usTsId;
	usSvcId		= pstContext->usSvcId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x), TsId(0x%x) \n", pstContext->hAction, ulDemuxId, usUniqueId, usTsId);

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stRetriveParam, ulActionId, usUniqueId, (HUINT32)ulDemuxId);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	stRetriveParam.usPid = EIT_PID;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;
	stRetriveParam.ucHead = (HUINT8)eCAS_FILTER_HEAD_Act;
	stRetriveParam.bUseExtId = TRUE;
	stRetriveParam.usExtId = usSvcId;

	hErr = svc_cas_SvcPortSiRetrieveBatSection(&stRetriveParam, (void*)svc_cas_SiParseEit, (void*)svc_cas_SiFreeEit, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstContext->ulEitFilterId = (HUINT32)usFilterId;

	HxLOG_Info("Filter Id (%d) \n", pstContext->ulEitFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopEitFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulEitFilterId);

	pstContext->ulEitFilterId = CAS_FILTER_ID_NULL;

	return ERR_OK;
}

#endif

#define _____TDT_TOT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_TDT)
HERROR svc_cas_SiUpdateTdtRawData(SvcCas_Context_t *pstContext)
{
	HUINT8				**pucTdtRaw = NULL;
	HUINT8				 *pucTdt = NULL;
	HUINT16				  usUniqueId = 0;
	HUINT32				  ulSecCnt = 0;
	HUINT32				  ulLen = 0;
	HERROR				  hErr;
	SvcCas_SiRawPool_h	  hSiRawPool;

	if (pstContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	usUniqueId = pstContext->usUniqueId;

	HxLOG_Info(" unique id (%x) \n", usUniqueId);

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	pucTdtRaw = svc_cas_SvcPortSiRawGetRawArrays(hSiRawPool, usUniqueId, eSI_TableType_TDT, FALSE, 0, &ulSecCnt);
	if(pucTdtRaw == (HUINT8 **)NULL)
	{
		HxLOG_Error("tdt raw pool allocation failed.\n");
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	HxLOG_Info(" ulSecCnt (%x) \n", ulSecCnt);

	if(ulSecCnt == 0)
	{
		HxLOG_Error("tdt raw buffer allocated but section count is zero.\n");
		svc_cas_SvcPortSiRawFreeRawArrays(pucTdtRaw);
		LEAVE_CRITICAL_CASRAW;
		return ERR_FAIL;
	}

	pucTdt = pucTdtRaw[0];
	ulLen = ( get12bit(pucTdt+1) + 3 );

	VK_memset32(pstContext->aucTdtRaw, 0, ulLen);
	VK_memcpy32(pstContext->aucTdtRaw, pucTdt, sizeof(HUINT8)*ulLen);

	#ifdef CAS_TDT_PRINT
	{
		HUINT8 *pucData;
		HUINT32 i;

		pucData = pucTdtRaw[0];

		HxLOG_Print("******** %d eit raw bytes *********\n", ulLen);
		for (i=0; i<ulLen; i++)
		{
			HxLOG_Print("0x%02x ", pucData[i]);
			if ((i%10) == 9)
				HxLOG_Print("\n");
		}
		HxLOG_Print("\n**************************\n");
	}
	#endif

	if(ulSecCnt > 1)
	{
		HxLOG_Critical("\n\n");
	}

	hErr = svc_cas_SvcPortSiRawFreeRawArrays(pucTdtRaw);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	LEAVE_CRITICAL_CASRAW;
	return ERR_OK;
}

void svc_cas_SiParseTdt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				 usResult, usTableId, usFilterId, usTsUniqId;
	HUINT16				 usSecLen;
	HUINT16				 usIdx, usSecNum;
	HUINT8				*pucRaw, *pucNewRaw;
	HERROR			 	 hErr;
	SvcCas_SiRawData_t	*pstSiRawData = NULL;
	SvcCas_SiRawPool_h	 hSiRawPool;

	HxLOG_Info("hAction(%d)\n", hAction);

	svc_cas_SiGetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	svc_cas_SiGetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid (HANDLE_NULL) \n");
		return;
	}

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("Input param is invalid (usFilterId) \n");
		return;
	}

	if(usResult != eCAS_SI_FILTER_RESULT_Complete)
	{
		HxLOG_Info("result = %s\n", (usResult == eCAS_SI_FILTER_RESULT_Timeout) ? "Time out" : "Error");
		return;
	}

	HxLOG_Info("ActionHandle(0x%x) TableId(%d), FilterId(%d), UniqueId(%d) \n", hAction, usTableId, usFilterId, usTsUniqId);

	pstSiRawData = (SvcCas_SiRawData_t *)ulParam;

	ENTER_CRITICAL_CASRAW;

	hSiRawPool = svc_cas_SiGetRawPool();

	if (pstSiRawData->num_sec > 0)
	{
		HxLOG_Info(" number of section %d \n", pstSiRawData->num_sec);
		pucRaw = pstSiRawData->buf[0];

		svc_cas_SvcPortSiRawFreeSection (hSiRawPool, usTsUniqId, eSI_TableType_TDT, FALSE, 0);
	}

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			pucRaw = pstSiRawData->buf[usIdx];
			usSecLen = (HUINT16)get12bit (pucRaw + 1);

			pucNewRaw = (HUINT8 *)OxCAS_Malloc ((HUINT32)usSecLen + 3);
			if (pucNewRaw == NULL)
			{
				HxLOG_Critical("\n\n\n");
				break;
			}

			HxSTD_memcpy (pucNewRaw, pucRaw, (HUINT32)usSecLen + 3);
			hErr = svc_cas_SvcPortSiRawAddRawSection(hSiRawPool, usTsUniqId, eSI_TableType_TDT, pucNewRaw);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Add Section in RawPool at %d-th Section \n", usIdx);
				OxCAS_Free (pucNewRaw);
			}

			usSecNum++;
		}
	}

	LEAVE_CRITICAL_CASRAW;

	HxLOG_Info("TDT[UniqueId(0x%x)] is Received \n", usTsUniqId);

	// Message Send to Cas Task.
	hErr = svc_cas_SiSendCmd2CasMgrTask(CMD_CAS_CHECKTDT,hAction, usFilterId, usTsUniqId, eCASSI_Table_Received);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

void svc_cas_SiFreeTdt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HUINT32 nResult = 0;

	HxLOG_Info("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	ENTER_CRITICAL_CASRAW;
	nResult = svc_cas_SvcPortSiRawFreeSection (svc_cas_SiGetRawPool(), (HUINT16)ulTsUniqId, eSI_TableType_TDT, TRUE, (HUINT32)ulExternalValue);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	LEAVE_CRITICAL_CASRAW;

	return;
}

HERROR svc_cas_SiStartTdtFilter(SvcCas_Context_t *pstContext)
{

	HUINT16					usFilterId = 0;
	HUINT16					usUniqueId = 0;
	HUINT32					ulDemuxId;
	HUINT32					ulActionId;
	HERROR					hErr;
	SvcCas_FilterParam_t	stRetriveParam;

	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	ulActionId	= pstContext->ulActionId;
	usUniqueId	= pstContext->usUniqueId;
	ulDemuxId	= pstContext->ulDemuxId;

	HxLOG_Info("ActionId (%d), DemuxId(%d), UniqueId(0x%x) \n", ulActionId, ulDemuxId, usUniqueId);

	hErr = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stRetriveParam, pstContext->hAction, usUniqueId, (HUINT32)ulDemuxId);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	stRetriveParam.usPid	= TDT_PID;
	stRetriveParam.ucMode	= eSiFilterMode_Monitor;
	stRetriveParam.bCrc	= FALSE;

	hErr = svc_cas_SvcPortSiRetrieveTdtSection(&stRetriveParam, svc_cas_SiParseTdt, svc_cas_SiFreeTdt, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstContext->ulTdtFilterId = (HUINT32)usFilterId;

	HxLOG_Info("Filter Id (%d) \n", pstContext->ulTdtFilterId);

	return ERR_OK;
}

HERROR svc_cas_SiStopTdtFilter(SvcCas_Context_t *pstContext)
{
	HxLOG_Info("+[%s]\n", __FUNCTION__);

	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	(void)svc_cas_SvcPortSiStopOneSection((HUINT16)pstContext->ulTdtFilterId);

	pstContext->ulTdtFilterId = CAS_FILTER_ID_NULL;
	return ERR_OK;
}

#endif

/********************************************************************
* For Debugging
********************************************************************/

#define ___08_Debugging_APIs___

#if defined(CONFIG_DEBUG)
void svc_cas_SiPrintRawTable(void)
{
	ENTER_CRITICAL_CASRAW;
	svc_cas_SvcPortSiRawPrintRawPool(s_phCasMgrSiRawPool);
	LEAVE_CRITICAL_CASRAW;

	return;
}
#endif


/*********************** End of File ******************************/
