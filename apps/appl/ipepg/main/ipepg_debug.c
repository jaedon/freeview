/**************************************************************
 *	@file		ipepg_debug.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/03/04
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/


typedef struct tagIpEpgDownloadDbg
{
	HBOOL				bStarted;
	HBOOL				bEnded;
	HINT32				lCountOfService;
	HINT32				lCountOfChLogSendToDama;
	HINT32				lCountOfDownload;
	HINT32				lCountOfDownloadEvent;
	HUINT32 			ulDownloadEventTime;
	HUINT32 			ulPartOfTime;
	HUINT32 			ulTotalTime;
	HUINT32 			ulStartTime;
	HUINT32 			ulEndTime;

	HUINT32				ulSema;
} IPEPG_DOWNLOAD_DBG_t;



static IPEPG_DOWNLOAD_DBG_t * s_ipepg_download_debug_info;


static void ipepg_initDownloadDebugInfo(void)
{
	if(s_ipepg_download_debug_info == NULL)
	{
		s_ipepg_download_debug_info = (IPEPG_DOWNLOAD_DBG_t *)IPEPG_MemAlloc(sizeof(IPEPG_DOWNLOAD_DBG_t));
		HxSTD_MemSet( (void*)s_ipepg_download_debug_info, 0, sizeof(IPEPG_DOWNLOAD_DBG_t));

		HxSEMT_Create(&(s_ipepg_download_debug_info->ulSema), "IpDownDbgSema", HxSEMT_FIFO);
	}
}
static IPEPG_DOWNLOAD_DBG_t * ipepg_getDownloadDebugInfo(void)
{
	return s_ipepg_download_debug_info;
}

static HINT32 ipepg_resetDownloadDebugInfo(void *szArgs)
{
	IPEPG_DOWNLOAD_DBG_t * pInfo = NULL;

	pInfo = ipepg_getDownloadDebugInfo();

	if(pInfo == NULL)
	{
		return 0;
	}

	HxSEMT_Get(pInfo->ulSema);
	pInfo->bStarted = pInfo->bEnded = pInfo->lCountOfService = pInfo->lCountOfChLogSendToDama = 0;
	pInfo->lCountOfDownload = pInfo->lCountOfDownloadEvent = pInfo->ulDownloadEventTime = 0;
	pInfo->ulPartOfTime = pInfo->ulTotalTime = pInfo->ulStartTime = pInfo->ulEndTime = 0;
	HxSEMT_Release(pInfo->ulSema);
	return 0;
}


static HINT32 siIpEpgDownloadTestValue = 0;
static HINT32 ipepg_setDownloadTest(void *szArgs)
{
	HCHAR			*szArg_kind = NULL;
	HUINT32 		kind = 0;
	HLIB_CMD_Printf("[%s] +++ \n" , __FUNCTION__ );

	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind == NULL )
	{
		return 0;
	}

	kind =	HLIB_STD_StrToUINT32(szArg_kind);
	if(kind != 1  && kind != 0)
	{
		return 0;
	}

	// 1 : ipepg download fail situation, 0: ipepg download success situation
	siIpEpgDownloadTestValue = kind;
	HLIB_CMD_Printf("[%s] value[%d] \n" , __FUNCTION__ , siIpEpgDownloadTestValue);
	return 0;
}
HINT32 IPEPG_GetNetworkTestValue(void)
{
	HxLOG_Print("[%s] value[%d] \n" , __FUNCTION__ , siIpEpgDownloadTestValue);
	return siIpEpgDownloadTestValue;
}


HUINT32 IPEPG_GetDownloadDebugInfo(IPEPG_DOWNLOAD_DBG_e e_paramType)
{
	HUINT32 ulRetVal = 0;
	IPEPG_DOWNLOAD_DBG_t * pInfo = NULL;

	pInfo = ipepg_getDownloadDebugInfo();
	if(	pInfo == NULL)
	{
		return HxCMD_OK;
	}


	HxSEMT_Get(pInfo->ulSema);
	switch(e_paramType)
	{
		case	eIPEPG_DNLD_DBG_ENDED:
				ulRetVal = (HUINT32)pInfo->bEnded;
				break;

		case	eIPEPG_DNLD_DBG_STARTED:
				ulRetVal = (HUINT32)pInfo->bStarted;
				break;

		case	eIPEPG_DNLD_DBG_SERVICE_COUNT:
				ulRetVal = (HUINT32)pInfo->lCountOfService;
				break;

		case	eIPEPG_DNLD_DBG_CHLOGSENDTODAMA_COUNT:
				ulRetVal = (HUINT32)pInfo->lCountOfChLogSendToDama;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOAD_COUNT:
				ulRetVal = (HUINT32)pInfo->lCountOfDownload;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOADEVENT_COUNT:
				ulRetVal = (HUINT32)pInfo->lCountOfDownloadEvent;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOADEVENT_TIME:
				ulRetVal = (HUINT32)pInfo->ulDownloadEventTime;
				break;

		case	eIPEPG_DNLD_DBG_PARTOFTIME:
				ulRetVal = (HUINT32)pInfo->ulPartOfTime;
				break;

		case	eIPEPG_DNLD_DBG_TOTALTIME:
				ulRetVal = (HUINT32)pInfo->ulTotalTime;
				break;

		case	eIPEPG_DNLD_DBG_STARTTIME:
				ulRetVal = (HUINT32)pInfo->ulStartTime;
				break;
		case	eIPEPG_DNLD_DBG_ENDTIME :
				ulRetVal = (HUINT32)pInfo->ulEndTime;
				break;
		default :
			break;
	}
	HxSEMT_Release(pInfo->ulSema);

	return HxCMD_OK;
}

void IPEPG_SetDownloadDebugInfo(IPEPG_DOWNLOAD_DBG_e e_paramType, void * arg)
{
	IPEPG_DOWNLOAD_DBG_t * pInfo = NULL;
	HUINT32 ulDebugTime = 0;

	pInfo = ipepg_getDownloadDebugInfo();
	if(	pInfo == NULL)
	{
		return ;
	}

	VK_CLOCK_GetTime((unsigned long*)&ulDebugTime);

	HxSEMT_Get(pInfo->ulSema);
	switch(e_paramType)
	{
		case	eIPEPG_DNLD_DBG_ENDED:
				pInfo->bEnded 	= (HBOOL)arg;
				break;

		case	eIPEPG_DNLD_DBG_STARTED:
				pInfo->bStarted = (HBOOL)arg;
				break;

		case	eIPEPG_DNLD_DBG_SERVICE_COUNT:
				pInfo->lCountOfService = (HINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_CHLOGSENDTODAMA_COUNT:
				pInfo->lCountOfChLogSendToDama = (HINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOAD_COUNT:
				pInfo->lCountOfDownload = (HINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOADEVENT_COUNT:
				pInfo->lCountOfDownloadEvent = (HINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_DOWNLOADEVENT_TIME:
				pInfo->ulDownloadEventTime = (HUINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_PARTOFTIME:
				pInfo->ulPartOfTime = (HUINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_TOTALTIME:
				pInfo->ulTotalTime= (HUINT32)arg;
				break;

		case	eIPEPG_DNLD_DBG_STARTTIME:
				pInfo->ulStartTime = (arg==NULL) ? ulDebugTime : (HUINT32)arg;
				break;
		case 	eIPEPG_DNLD_DBG_ENDTIME :
				pInfo->ulEndTime = (arg==NULL) ? ulDebugTime : (HUINT32)arg;
				break;
		default :
			break;
	}
	HxSEMT_Release(pInfo->ulSema);
}


#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
#define TMP_BUFF_SIZE	1024

static FILE 		*s_fp_save_xml = NULL;
static HBOOL		s_bForward = TRUE;		// TRUE is foward, FALSE is backward
static HUINT32		s_ulSeqNum = 0;
static HUINT32		s_ulFolderSeqNum = 0;
static HUINT32 		s_ulSystemTime = 0;
static HUINT8 		s_targetFolder[TMP_BUFF_SIZE] ={};


static HBOOL ipepg_debug_makeXmlDir(void)
{
	VK_memset(s_targetFolder, 0, TMP_BUFF_SIZE);
	s_ulFolderSeqNum++;
	HxSTD_snprintf(s_targetFolder,TMP_BUFF_SIZE, "mkdir %s_%d_%s", IPEPG_DEBUG_DOWNLOAD_XML_PATH, s_ulFolderSeqNum, s_bForward ? "F":"B");

	if(FALSE == HLIB_FILE_Exist(s_targetFolder))
	{
		VK_SYSTEM_Command(s_targetFolder);
	}
	else
	{
		HxLOG_Error("[%s] already exist samefolder (%s) \n",__FUNCTION__ ,s_targetFolder);
	}

	VK_memset(s_targetFolder, 0, TMP_BUFF_SIZE);
	HxSTD_snprintf(s_targetFolder,TMP_BUFF_SIZE, "%s_%d_%s", IPEPG_DEBUG_DOWNLOAD_XML_PATH, s_ulFolderSeqNum, s_bForward ? "F":"B");
	return TRUE;
}


void IPEPG_DEBUG_SaveXML_Init(HBOOL bForwardEPG, HUINT32 ulSystemTime, HBOOL bDbSnapshot)
{
	s_bForward = bForwardEPG;
	s_ulSeqNum = 0;
	s_ulSystemTime = ulSystemTime;
	ipepg_debug_makeXmlDir();

	// DB Snapshot
	if(TRUE == bDbSnapshot)
	{
		HUINT8 		cmd[TMP_BUFF_SIZE] ={};

		VK_memset(cmd, 0, TMP_BUFF_SIZE);
		HxSTD_snprintf(cmd,TMP_BUFF_SIZE, "cp -rf /tmp/eDxEPG_TYPE_IP.db %s/", s_targetFolder);
		VK_SYSTEM_Command(cmd);
	}
}


void IPEPG_DEBUG_SaveXML(const DxIpService_t *pIpSvcInfo, const HUINT8 * pUrl, void * pdata, HINT32 lSizeData)
{
	HUINT8 	tempBuffer[TMP_BUFF_SIZE] ={};
	HINT32 lRet = 0;

	if( NULL != s_fp_save_xml )
	{
		fclose(s_fp_save_xml);
	}
	VK_memset(tempBuffer, 0, TMP_BUFF_SIZE);
	HxSTD_snprintf(tempBuffer, 1024, "%s/%d_%d_%d_%d.xml", s_targetFolder, s_ulSystemTime,s_ulSeqNum, pIpSvcInfo->ulSvcUid,pIpSvcInfo->usSvcId);
	s_ulSeqNum++;

	s_fp_save_xml = fopen(tempBuffer, "wb");
	if(s_fp_save_xml)
	{
		if(NULL != pdata)
		{
			lRet = fwrite( pdata, 1, lSizeData, s_fp_save_xml);
			if( lRet != lSizeData)
			{
				HxLOG_Error("[%s] Can't write all data  (%d/%d) \n",__FUNCTION__ ,lRet, lSizeData);
			}
		}

		// write URL
		lRet = fwrite( "<!--", 1, 4, s_fp_save_xml );
		if(NULL == pdata)
		{
			lRet = fwrite( "\n downloadFail \n", 1, HxSTD_StrLen("\n downloadFail \n"), s_fp_save_xml );
		}
		lRet = fwrite( pUrl, 1, HxSTD_StrLen(pUrl), s_fp_save_xml );
		lRet = fwrite( "-->\n", 1, 4, s_fp_save_xml );

		fclose( s_fp_save_xml );
		s_fp_save_xml = NULL;
	}
	else
	{
		HxLOG_Error("[%s] Can't make a file (%s) \n",__FUNCTION__ ,tempBuffer);
	}
}


// XML에서 갱신된 데이터, 즉 event를 DELETE 하기 위한 정보가 존재하는 데이터
void IPEPG_DEBUG_SaveModifyEventXML(const DxIpService_t *pIpSvcInfo, const HUINT8 * pUrl, void * pdata, HINT32 lSizeData)
{
	HUINT8 	tempBuffer[TMP_BUFF_SIZE] ={};
	HINT32 lRet = 0;

	if( NULL != s_fp_save_xml )
	{
		fclose(s_fp_save_xml);
	}
	VK_memset(tempBuffer, 0, TMP_BUFF_SIZE);
	HxSTD_snprintf(tempBuffer, 1024, "%s/%d_%d_%d_%d_delete.xml", s_targetFolder, s_ulSystemTime,s_ulSeqNum, pIpSvcInfo->ulSvcUid,pIpSvcInfo->usSvcId);


	s_fp_save_xml = fopen(tempBuffer, "wb");
	if(s_fp_save_xml)
	{
		if(NULL != pdata)
		{
			lRet = fwrite( pdata, 1, lSizeData, s_fp_save_xml);
			if( lRet != lSizeData)
			{
				HxLOG_Error("[%s] Can't write all data  (%d/%d) \n",__FUNCTION__ ,lRet, lSizeData);
			}
		}
		// write URL
		lRet = fwrite( pUrl, 1, HxSTD_StrLen(pUrl), s_fp_save_xml );
		lRet = fwrite( "-->\n", 1, 4, s_fp_save_xml );

		fclose( s_fp_save_xml );
		s_fp_save_xml = NULL;
	}
	else
	{
		HxLOG_Error("[%s] Can't make a file (%s) \n",__FUNCTION__ ,tempBuffer);
	}
}
#endif



static void ipepgPrintDownloadDebugInfo(void)
{
	IPEPG_DOWNLOAD_DBG_t * pInfo = NULL;
	HUINT32 ulDebugTime = 0;
	VK_CLOCK_GetTime((unsigned long*)&ulDebugTime);


	pInfo = ipepg_getDownloadDebugInfo();
	if(	pInfo == NULL)
	{
		HLIB_CMD_Printf("\t[%s] DebugInfo is not initialized ==================\n" , __FUNCTION__);
		return ;
	}

	HLIB_CMD_Printf("\n");
	HLIB_CMD_Printf("\tDownload Debug Info ==================\n");
	HLIB_CMD_Printf("\t\t bStart:%d, bEnded:%d\n", pInfo->bStarted, pInfo->bEnded);
	HLIB_CMD_Printf("\t\t CountOfService:%d, lCountOfChLogSendToDama:%d\n", pInfo->lCountOfService, pInfo->lCountOfChLogSendToDama);
	HLIB_CMD_Printf("\t\t CountOfDownload:%d, CountOfDownloadEvent:%d\n", pInfo->lCountOfDownload, pInfo->lCountOfDownloadEvent);
	HLIB_CMD_Printf("\t\t ulPartOfTime:%d, ulTotalTime:%d\n", pInfo->ulPartOfTime, pInfo->ulTotalTime);
	HLIB_CMD_Printf("\t\t ulDownloadEventTime:%d\n", pInfo->ulDownloadEventTime);
	HLIB_CMD_Printf("\t\t ulStartTime:%d, ulEndTime:%d, WorkingTime:%d \n", pInfo->ulStartTime, pInfo->ulEndTime, (HUINT32)(ulDebugTime-pInfo->ulStartTime));


	if(pInfo->ulEndTime > 0 )
	{
		HLIB_CMD_Printf("\t\t Working duration is :%d\n", (HUINT32)(pInfo->ulEndTime-pInfo->ulStartTime));
	}
	HLIB_CMD_Printf("\n");
}

static void ipepgPrintSvcList(HUINT32	kind)
{
	HxHASH_t	*phSvcHash = NULL;

	phSvcHash = IPEPG_GetItemPoint(eIPEPG_ITEM_SVCLIST);
	if(phSvcHash != NULL)
	{
		HxHASH_TableIter_t	iter;
		IPEPG_SvcInfo_t * pService = NULL;

		HLIB_CMD_Printf("\tSvcList Count : %d\n", HLIB_HASH_Size(phSvcHash));

		if(kind != 0)
		{
			HLIB_HASH_IterInit(&iter, phSvcHash);
			while (HLIB_HASH_IterNext(&iter, NULL, (void**)&pService))
			{
				if(pService)
				{
					if(kind == 1)
						HLIB_CMD_Printf("\t\tSVCINFO UID[0x%08x] LCN[%04d] NAME[%s]\n",pService->ulUid, pService->ulLcn, pService->ucName);
					else
						HLIB_CMD_Printf("\t\tSVCINFO UID[0x%08x] LCN[%04d] ONID[0x%04x] TSID[0x%04x] SVCID[0x%04x] NAME[%s]\n",pService->ulUid, pService->ulLcn, pService->stTripleId.usOnId, pService->stTripleId.usTsId, pService->stTripleId.usSvcId, pService->ucName);
				}
			}
		}
		HLIB_HASH_Destroy(phSvcHash);
	}
	else
	{
		HLIB_CMD_Printf("Service list is NULL\n");
	}
}


static HINT32 ipepgPrintSvc(void *szArgs)
{
	HCHAR			*szArg_kind = NULL;
	HUINT32			kind = 0;

	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind != NULL )
	{
		kind =  HLIB_STD_StrToUINT32(szArg_kind);
	}

	if(kind > 2)
		kind = 0;

	ipepgPrintSvcList(kind);

	return HxCMD_OK;
}

static void ipepgPrintFvcSeriesEventList (void)
{
	HxList_t	*pEvtList = NULL;

	pEvtList = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_SERIES_EVENT_LIST);
	if (pEvtList != NULL)
	{
		HxList_t	*iter;
		IPEPG_FvcSeriesEvent_t * pEvent = NULL;

		HLIB_CMD_Printf("\tSeries Evt Count : %d\n", HLIB_LIST_Length(pEvtList));

		for (iter = HLIB_LIST_First(pEvtList); iter != NULL; iter = HLIB_LIST_Next(iter))
		{
			pEvent = HLIB_LIST_Data(iter);
			if (pEvent)
			{
				HLIB_CMD_Printf("\t\tSERIES EVT INFO SID[0x%x] StartTime[%d] Uid[%d] status[%d] \n"
					,pEvent->usSvcId, pEvent->ulStartTime, pEvent->uid, pEvent->ulHttpStatusCode);
			}
		}
	}
	else
	{
		HLIB_CMD_Printf("Service list is NULL\n");
	}

	if (pEvtList != NULL) { HLIB_LIST_RemoveAll(pEvtList);}
}

static HINT32 CMD_IpepgStart(void *szArgs)
{
	HCHAR			*szArg_kind = NULL;
	HUINT32 		kind = 0;
	HBOOL 			bRetVal = 0;
	HLIB_CMD_Printf("[%s] +++ \n" , __FUNCTION__ );

	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind == NULL )
	{
		return 0;
	}

	kind =	HLIB_STD_StrToUINT32(szArg_kind);
	if(kind != 1  && kind != 0)
	{
		return 0;
	}

	bRetVal = IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	if(kind == bRetVal)
	{
		HLIB_CMD_Printf("[%s] ipepg is already %s. \n" , __FUNCTION__, ((kind == 1) ? "started":"stopped") );
	}
	else
	{
		IPEPG_SetAPKDBItem(eIPEPG_ITEM_FLAG_IPEPG_ONOFF, (void *)kind );
		HAPI_BroadcastSignal("signal:onIpEPGStateChanged", 0, "i", kind ? 1 : 0);
		HLIB_CMD_Printf("[%s] ipepg is successfully %s. \n" , __FUNCTION__ , ((kind == 1) ? "started":"stopped") );
	}


	HLIB_CMD_Printf("[%s] GetItemValue(%d) --- \n" , __FUNCTION__, bRetVal );
	return 0;
}




static HINT32 CMD_RpStart(void *szArgs)
{

	HCHAR			*szArg_kind = NULL;
	HUINT32 		kind = 0;
	HUINT32 		bEnabled = 0;


	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind == NULL )
	{
		return 0;
	}

	kind =	HLIB_STD_StrToUINT32(szArg_kind);
	if(kind == 1)	bEnabled = 1;
	else			bEnabled = 0;


	IPEPG_SetAPKDBItem(eIPEPG_ITEM_FLAG_RP_ONOFF, (void *)bEnabled);
	HAPI_BroadcastSignal("signal:onRpStateChanged", 0, "i", bEnabled ? 1 : 0);
	return 0;
}

static HINT32 CMD_setStampTime(void *szArgs)
{
	HCHAR			*szArg_kind = NULL;
	HUINT32 		ulStampTime = 0;

	HLIB_CMD_Printf("\n\n===================   IPEPG SetTimeStamp =========================\n");

	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind == NULL )
	{
		return 0;
	}

	ulStampTime =	HLIB_STD_StrToUINT32(szArg_kind);
	IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME,	(void *)ulStampTime);
	HLIB_CMD_Printf("\tIPEPG SetTimeStamp value (%d) \n", ulStampTime);

	return 0;
}


static HINT32 IpepgCheckStatus(void *szArgs)
{
#define RESOLVE_PATH	CONFIG_PRODUCT_DATA_DIR "/resolv.conf"

	HUINT8		*pucMaccAddr = NULL;
	HUINT8		*pServerURL = NULL;
	HUINT32		ulSystemId = 0, ulSystemVersion = 0;

	HLIB_CMD_Printf("\n\n===================   IPEPG STATUS =========================\n");


	ulSystemId = IPEPG_GetItemValue(eIPEPG_ITEM_SYS_ID);
	ulSystemVersion = IPEPG_GetItemValue(eIPEPG_ITEM_SYS_VER);
	pucMaccAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);


	if (access(RESOLVE_PATH, R_OK | W_OK) != 0)
	{
		HLIB_CMD_Printf("\tResolve.conf does not Exist\n");
	}
	else
	{
		HLIB_CMD_Printf("\tResolve.conf Exist\n");
		system("cat " RESOLVE_PATH);
	}


	HLIB_CMD_Printf(
		"\t                    HDD : %s\n"
		"\t                Network : %s\n"
		"\t              ServerURL : %s\n"
		"\t             MACAddress : %s\n"
		"\t               SystemID : %02X%02X.%02X%02X SWVer : %X.%X.%X\n"
		,IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS)? "Mounted":"UnMounted"
		,IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK)? "Connected":"DisConnected"
		,pServerURL
		,pucMaccAddr
		,(ulSystemId>>24)&0x000000FF, (ulSystemId>>16)&0x000000FF, (ulSystemId>>8)&0x000000FF, (ulSystemId)&0x000000FF
		,(ulSystemVersion>>16)&0x000000FF, (ulSystemVersion>>8)&0x000000FF, (ulSystemVersion)&0x000000FF);


	IPEPG_MemFree(pServerURL);
	IPEPG_MemFree(pucMaccAddr);

#if defined(CONFIG_OP_UK_DTT)
	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_BASE_URL);
	HLIB_CMD_Printf(
		"\t         FVC Server URL : %s\n", pServerURL);
	IPEPG_MemFree(pServerURL);
	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_AUTH_HASH);
	HLIB_CMD_Printf(
		"\t         FVC Auth Hash   : %s\n", pServerURL);
	IPEPG_MemFree(pServerURL);
	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_AUTH_TIME_STAMP);
	HLIB_CMD_Printf(
		"\t         FVC Auth Time Stamp : %s\n", pServerURL);
	IPEPG_MemFree(pServerURL);

	HLIB_CMD_Printf(
		"\t         FVC Cached Series Evt List \n");

	ipepgPrintFvcSeriesEventList();
#endif

	HLIB_CMD_Printf("\tIP EPG is %s\n", IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF)?"On":"Off");

	if(IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF))
	{
		HxDATETIME_t stLastUpdateTime, stNextUpdateTime, stCurrentTime, stStampTime;
		HUINT32 pulUnixTime;
		VK_CLOCK_GetTime((unsigned long*)&pulUnixTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(pulUnixTime, &stCurrentTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(IPEPG_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME), &stLastUpdateTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(IPEPG_GetItemValue(eIPEPG_ITEM_UPDATE_TIME), &stNextUpdateTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(IPEPG_GetItemValue(eIPEPG_ITEM_STAMP_TIME), &stStampTime);

		HLIB_CMD_Printf(
			"IPEPG ::\n"
			"\t 	   ulCurrent Time :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t 	 ulLastUpdateTime :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t 	 ulNextUpdateTime :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t 	      ulStampTime :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t 	  bServiceUpdated : %s\n"
			"\t 	  bDownLoadServiceCount : %d\n"
			, stCurrentTime.stDate.usYear, stCurrentTime.stDate.ucMonth, stCurrentTime.stDate.ucDay , stCurrentTime.stTime.ucHour, stCurrentTime.stTime.ucMinute, stCurrentTime.stTime.ucSecond
			, pulUnixTime
			, stLastUpdateTime.stDate.usYear, stLastUpdateTime.stDate.ucMonth, stLastUpdateTime.stDate.ucDay , stLastUpdateTime.stTime.ucHour, stLastUpdateTime.stTime.ucMinute, stLastUpdateTime.stTime.ucSecond
			, IPEPG_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME)
			, stNextUpdateTime.stDate.usYear, stNextUpdateTime.stDate.ucMonth, stNextUpdateTime.stDate.ucDay , stNextUpdateTime.stTime.ucHour, stNextUpdateTime.stTime.ucMinute, stNextUpdateTime.stTime.ucSecond
			, IPEPG_GetItemValue(eIPEPG_ITEM_UPDATE_TIME)
			, stStampTime.stDate.usYear, stStampTime.stDate.ucMonth, stStampTime.stDate.ucDay , stStampTime.stTime.ucHour, stStampTime.stTime.ucMinute, stStampTime.stTime.ucSecond
			, IPEPG_GetItemValue(eIPEPG_ITEM_STAMP_TIME)
			, IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED)?"True":"False"
			, IPEPG_GetItemValue(eIPEPG_ITEM_SVC_COUNT)
		);

		ipepgPrintSvcList(0);
	}


	HLIB_CMD_Printf("\n\tRP is %s\n", IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF)?"On":"Off");
	if(IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF))
	{
		HLIB_CMD_Printf(	"\t    ulRpUpdateInterval : %d min\n"
					"\t 	  ulRpPeriodStart : %d : %d \n"
					"\t 		ulRpPeriodEnd : %d : %d \n"
					, IPEPG_GetItemValue(eIPEPG_ITEM_RP_UPDATE_INTERVAL)/60
					, IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_START)/3600, (IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_START)/60)%60
					, IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_END)/3600, (IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_END)/60)%60);
	}

	ipepgPrintDownloadDebugInfo();

#undef RESOLVE_PATH

	return HxCMD_OK;
}


void	ipepg_command_Init(void)
{
	ipepg_initDownloadDebugInfo();

#define	cmdHandle		"ipepgcmd"
	HLIB_CMD_RegisterWord(cmdHandle, IpepgCheckStatus,
					 (HINT8 *)"status",
					 (HINT8 *)"print ipepg status",
					 (HINT8 *)"status");

	HLIB_CMD_RegisterWord(cmdHandle, ipepgPrintSvc,
					 (HINT8 *)"svclist",
					 (HINT8 *)"print svclist: svclist 0 : Only Number, 1: name uid lcn 2: all",
					 (HINT8 *)"svclist 0 : Only Number, 1: name uid lcn 2: all");

	HLIB_CMD_RegisterWord(cmdHandle, CMD_IpepgStart,
					 (HINT8 *)"ipepgstart",
					 (HINT8 *)"ipepg start from schedule (ipepgstart 0: stop, ipepgstart 1: start)",
					 (HINT8 *)"ipepgstart 0: stop, ipepgstart 1: start");


	HLIB_CMD_RegisterWord(cmdHandle, CMD_RpStart,
					 (HINT8 *)"rpstart",
					 (HINT8 *)"rp start from schedule (rpstart 0: stop, rpstart 1: start)",
					 (HINT8 *)"rpstart 0: stop, rpstart 1: start");


	HLIB_CMD_RegisterWord(cmdHandle, ipepg_resetDownloadDebugInfo,
					 (HINT8 *)"rststatus",
					 (HINT8 *)"reset downlod debug info",
					 (HINT8 *)"reset downlod debug info");

	HLIB_CMD_RegisterWord(cmdHandle, ipepg_setDownloadTest,
					 (HINT8 *)"ipepgtest",
					 (HINT8 *)"ipepgtest [0|1] ( 1 : ipepg download fail situation, 0: ipepg download success situation ",
					 (HINT8 *)"ipepg download test");
/*
	HLIB_CMD_RegisterWord(cmdHandle, CMD_setStampTime,
					 (HINT8 *)"setstamp",
					 (HINT8 *)"setstamp [stame time value] (0 is reset)",
					 (HINT8 *)"setstamp [stame time value] (0 is reset)");
*/
}



