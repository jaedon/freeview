/**
	@file
	@brief

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

#include <stdarg.h>
#include <signal.h>

#include <hlib.h>
#include <ondk.h>
#include <oapi.h>
#include <hapi.h>

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#include "./ib/MHEGEngine.h"


/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___
#define ___09_Debugging_APIs___

#define COMMAND_NAME						"APP_MhegIB"
#define DATA_STREAM_MHEG_IB				"mhegibdata"
#define DEBUG_MHEG_IB_PACKET_DUMP_FILE	"MhegIb_packet.dat"

#define	SignalToString(eSignal)		(\
								  ((eSignal) == eHSIG_AppsReady) 		? DxNAMEOF(eHSIG_AppsReady)		\
								: ((eSignal) == eHSIG_SystemBootupState)? DxNAMEOF(eHSIG_SystemBootupState)		\
								: ((eSignal) == eHSIG_SystemState)		? DxNAMEOF(eHSIG_SystemState)		\
								: ((eSignal) == eHSIG_ShowTeleText) 	? DxNAMEOF(eHSIG_ShowTeleText)		\
								: ((eSignal) == eHSIG_ShowSubtitle) 	? DxNAMEOF(eHSIG_ShowSubtitle)	\
								: ((eSignal) == eHSIG_ReadyMainApplication) ? DxNAMEOF(eHSIG_ReadyMainApplication)	\
								: ((eSignal) == eHSIG_StartFullBrowser) ? DxNAMEOF(eHSIG_StartFullBrowser)	\
								: NULL	\
								)

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___
static HBOOL	s_bTeletextDisplay = FALSE;
static HBOOL	s_bSubtitleDisplay = FALSE;

/********************************************************************
* Static Functions
* Naming rule : "local"_"MhegIb" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___
#if defined(CONFIG_ENHANCED_SECURITY)
static	void dropUserPrevilegeAndChroot(void)
{
	int i;
	struct passwd *u_info;
	struct group  *g_info;
	struct rlimit rlim;

#if (0)
	/* 1. Close open file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rlim))
	{
		printf("getrlimit() for RLIMIT_NOFILE failed with error %s\n",
				strerror(errno));
		exit(-1);
	}

	for (i = 0; i < rlim.rlim_max; i++)
	{
		while (close(i) != 0 && errno == EINTR);
	}


	/* 2. Chroot */
	if (chroot("/hmx/octo") == -1)
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : error chroot.....\n");
		exit(-1);
	}
	else
	{
		printf("\n\t\t =============> APP_ChrootAndDropUserPrevilege : succ chroot.....\n");
	}

	if (chdir("/"))
	{
		printf("chdir() failed...\n");
		exit(-1);
	}
#endif


	/* 3. Drop root privileges	to a non-privileged uesr */
	/* get GID and UID */
	u_info = getpwnam("settop");
	g_info = getgrnam("settop");

	if ( (u_info != NULL) && (g_info != NULL) )
	{
		uid_t real, eff, saved;
		gid_t gid = (gid_t)g_info->gr_gid;
		uid_t uid = (uid_t)u_info->pw_uid;

		if (setresgid(gid, gid, gid) != 0)
		{
			HxLOG_Print("Failed changing GID to %d with error %s\n",
						gid, strerror(errno));
			exit(-1);
		}

		if (setresuid(uid, uid, uid) != 0)
		{
			HxLOG_Print("Failed changing UID to %d with error %s\n",
						uid, strerror(errno));
			exit(-1);
		}

		if (getresuid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading UID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != uid || eff != uid || saved != uid)
		{
			HxLOG_Print("UID sanity check failed\n");
			exit(-1);
		}

		if (getresgid(&real, &eff, &saved) != 0)
		{
			HxLOG_Print("Failed reading GID with error %s\n", strerror(errno));
			exit(-1);
		}

		if (real != gid || eff != gid || saved != gid)
		{
			HxLOG_Print("GID sanity check failed\n");
			exit(-1);
		}

		HxLOG_Print("Dropped root priviliges. Now running as UID %u GID %u\n", uid, gid);
	}
	else
	{
		exit(-1);
	}
}
#endif

static HERROR local_MhegIb_GetMhegData(eCiAmmiMsgType_t enMsgType, HUINT8 *p, HUINT32 unLen, uCiAmmiContent_t *puContent)
{
	HxLOG_Print("msg type[%d] len[%d]\n", enMsgType, unLen);

	switch(enMsgType)
	{
	case eCiAmmiMsgType_reqStart:
	{
		sCiAmmiTypeReqStart_t	*pstTypeReqStart = &puContent->typeReqStart;

		HxSTD_MemCopy(&pstTypeReqStart->usSessionId, p, sizeof(pstTypeReqStart->usSessionId));				p += sizeof(pstTypeReqStart->usSessionId);
		HxSTD_MemCopy(&pstTypeReqStart->ucAppDomainIdLen, p, sizeof(pstTypeReqStart->ucAppDomainIdLen));	p += sizeof(pstTypeReqStart->ucAppDomainIdLen);
		pstTypeReqStart->pucAppDomainId = p;															p += pstTypeReqStart->ucAppDomainIdLen;
		HxSTD_MemCopy(&pstTypeReqStart->ucInitialObjectLen, p, sizeof(pstTypeReqStart->ucInitialObjectLen));	p += sizeof(pstTypeReqStart->ucInitialObjectLen);
		pstTypeReqStart->pucInitialObject = p;															p += pstTypeReqStart->ucInitialObjectLen;
	}
		break;

	case eCiAmmiMsgType_FileAck:
	{
		sCiAmmiTypeFileAck_t		*pstTypeFileAck = &puContent->typeFileAck;

		HxSTD_MemCopy(&pstTypeFileAck->usSessionId, p, sizeof(pstTypeFileAck->usSessionId));	p += sizeof(pstTypeFileAck->usSessionId);
		HxSTD_MemCopy(&pstTypeFileAck->ucFileOK, p, sizeof(pstTypeFileAck->ucFileOK));			p += sizeof(pstTypeFileAck->ucFileOK);
		HxSTD_MemCopy(&pstTypeFileAck->enReqType, p, sizeof(pstTypeFileAck->enReqType));		p += sizeof(pstTypeFileAck->enReqType);
		HxSTD_MemCopy(&pstTypeFileAck->ulBytesLen, p, sizeof(pstTypeFileAck->ulBytesLen));		p += sizeof(pstTypeFileAck->ulBytesLen);
		pstTypeFileAck->pucBytes = p;														p += pstTypeFileAck->ulBytesLen;
	}
		break;

	case eCiAmmiMsgType_ReqAppAbort:
	{
		sCiAmmiTypeReqAppAbort_t	*pstTypeReqAppAbort = &puContent->typeReqAppAbort;

		HxSTD_MemCopy(&pstTypeReqAppAbort->usSessionId, p, sizeof(pstTypeReqAppAbort->usSessionId));			p += sizeof(pstTypeReqAppAbort->usSessionId);
		HxSTD_MemCopy(&pstTypeReqAppAbort->enAbortReqCode, p, sizeof(pstTypeReqAppAbort->enAbortReqCode));	p += sizeof(pstTypeReqAppAbort->enAbortReqCode);
	}
		break;

	case eCiAmmiMsgType_AppAbortAck:
	{
		sCiAmmiTypeAppAbortAck_t	*psTypeAppAbortAck = &puContent->typeAppAbortAck;

		HxSTD_MemCopy(&psTypeAppAbortAck->usSessionId, p, sizeof(psTypeAppAbortAck->usSessionId));			p += sizeof(psTypeAppAbortAck->usSessionId);
	}
		break;

	case eCiAmmiMsgType_MAX:
	default:
		HxLOG_Error("unknown eCiAmmiMsgType_t(%d) msg type \n", enMsgType);
		return ERR_FAIL;
	}


	return ERR_OK;
}

static void local_MhegIb_CbDataStreamer(HINT32 size, void *pvData)
{
	HINT32				len;
	HUINT8				*p = (HUINT8*)pvData;
	HCHAR				*dataType;
	HERROR				hErr;
	eCiAmmiMsgType_t	enMsgType;
	uCiAmmiContent_t		uContent;

	#if DEBUG_PACKET_DUMP
	{
		static FILE *packetFile = NULL;

		if (packetFile == NULL)
			packetFile = fopen(DEBUG_MHEG_IB_PACKET_DUMP_FILE, "wb");

		fwrite(&size, 4, 1, packetFile);
		fwrite(pvData, size, 1, packetFile);
		fflush(packetFile);
		fclose(packetFile);
	}
	#endif

	len = HxMACRO_Get32Bit(p);	p += 4;
	dataType = p;					p += len;

	len = size - (len + 4);

	if (HLIB_STD_StrCaseCmp(dataType, DATA_STREAM_MHEG_IB) == 0)
	{
		HxSTD_MemCopy(&enMsgType, p, sizeof(eCiAmmiMsgType_t));	p += sizeof(eCiAmmiMsgType_t);

		hErr = local_MhegIb_GetMhegData(enMsgType, p, len, &uContent);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("get mheg data fail \n");
			goto Return;
		}

		MHEGIB_CiAppMmi_SendData(enMsgType, &uContent);
	}
	else
	{
		HxLOG_Error("local_MhegIb_CbDataStreamer() data error (%s)\n", dataType);
	}

Return:

	/*	pvData 는 free하면 안된다.!!!!  ^^ 계속 재사용되는 buffer임.
		HLIB_STD_MemFree(pvData);
	*/

	return;
}

static HERROR local_MhegIb_InitOapi(void)
{
	if(OAPI_InitDataStreamer(local_MhegIb_CbDataStreamer) != ERR_OK)
	{
		HxLOG_Error("[OAPI_InitDataStreamer] Err");
		return ERR_FAIL;
	}

	if(OAPI_ConnectDataStreamer(DATA_STREAM_MHEG_IB) != ERR_OK)
	{

		HxLOG_Error("[OAPI_ConnectDataStreamer] Err");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static void local_MhegIb_ApkliveMediaNotifier ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	HxLOG_Print("local_MhegIb_ApkliveMediaNotifier Event [%d]\n", eEvent);

	switch (eEvent)
	{
		case	eOxMP_EVENT_LIVE_SvcChanged:
			HxLOG_Debug("Encounter... eMctrlEvent_LIVE_SvcChanged Event [%d]!!!\n", eEvent);
			MHEGEngine_quitByChannelChange();
			break;

		case	eOxMP_EVENT_LIVE_SvcStopped:
		case	eOxMP_EVENT_STARTED:
		case	eOxMP_EVENT_SI_PMT_CHANGED:
			HxLOG_Debug("<---------------- Encounter... LIVE_SVC_Changed Event [%d]!!!\n", eEvent);
			break;

		default:
			HxLOG_Debug("MHEG IB doesn't need other media control event [%d]\n", eEvent);
			break;
	}
}

static HERROR local_MhegIb_InitAPK(void)
{
	APK_MEDIA_PLAY_Init();
	APK_MEDIA_RECORD_Init();

	//  Register MediaControl Callback
	{
		HERROR	hErr;
		HUINT32 	ulViewId;

		hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		if( ERR_OK == hErr )
		{
			hErr = APK_MEDIA_PLAY_RegisterNotifier (ulViewId, local_MhegIb_ApkliveMediaNotifier, NULL, NULL);
			if( ERR_OK != hErr )
				HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
		}
	}

	APK_EXE_Init();

	return ERR_OK;
}

static HERROR	local_MhegIb_onApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}

static HERROR	local_MhegIb_onSystemBootupStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}

static HERROR	local_MhegIb_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}

static HERROR	local_MhegIb_onSubtitleState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	s_bSubtitleDisplay = HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE;

	if(HxOBJECT_INT(apArgv[0]))
	{
		MHEGEngine_quitByGui();
	}
	else
	{
		//1 To do something
	}

	return ERR_OK;
}

static HERROR	local_MhegIb_onTeletextState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	s_bTeletextDisplay =  HxOBJECT_INT(apArgv[0]) ? TRUE : FALSE;
	if (HxOBJECT_INT(apArgv[0]))
		MHEGEngine_quitByGui();

	return ERR_OK;
}

static HERROR	local_MhegIb_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	return ERR_OK;
}

static HERROR	local_MhegIb_onStartFullBrowser(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	MHEGEngine_quitByGui();

	return ERR_OK;
}

static HERROR local_MhegIb_InitHapi(void)
{
	HUINT32 i, ulCnt;

	if(HAPI_Init() != ERR_OK)
	{
		HxLOG_Error("[HAPI_Init] Err");
	}

	if(HAPI_SetWindowStyle(eAWINDOW_MHEGIB, eHWS_WindowOwnControl | eHWS_WindowInputAlways, FALSE) != ERR_OK)
	{
		HxLOG_Error("[HAPI_SetWindowStyle] Err");
	}

	HAPI_ConnectSystemSignal("hama:onReadyProcess", "i",NULL, local_MhegIb_onApplicationReady);

	HAPI_ConnectSignal("signal:onBootupStateChanged",   NULL, local_MhegIb_onSystemBootupStateChanged);
	HAPI_ConnectSignal("signal:onSystemStateChanged",   NULL, local_MhegIb_onSystemStateChanged);
	HAPI_ConnectSignal("signal:onSubtitleState",        NULL, local_MhegIb_onSubtitleState);
	HAPI_ConnectSignal("signal:onTeletextState",        NULL, local_MhegIb_onTeletextState);
	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, local_MhegIb_onMainApplicationReady);
	HAPI_ConnectSignal("signal:onStartFullBrowser",     NULL, local_MhegIb_onStartFullBrowser);

	return ERR_OK;
}

/********************************************************************
* Module Internal Functions
* Naming rule :
********************************************************************/
#define ___07_Module_Internal_APIs___
HBOOL MHEGIB_MAIN_CheckEngineStartCondition(void)
{
	HxLOG_Debug("[MHEGIB_MAIN_CheckEngineStartCondition] s_bTeletextDisplay : [%d],s_bSubtitleDisplay : [%d] \n", s_bTeletextDisplay, s_bSubtitleDisplay);

	if((s_bTeletextDisplay != FALSE)/* || (s_bSubtitleDisplay != FALSE) */)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************
* Global Public Functions
* Naming rule : "Module Prefix" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

/********************************************************************
* For Debugging
********************************************************************/

/**************************************************************/
#define __MAIN____________
/**************************************************************/
int main(int argc, char **argv)
{
	//int	nErr;

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	VK_Init();

#if defined(CONFIG_DEBUG)
	HLIB_DBG_Init();
#endif

	/* Initialize HAPI */
	if(local_MhegIb_InitHapi() != ERR_OK)
	{
		HxLOG_Error("[local_MhegIb_InitHapi] Err");
	}

	/* Initialize Window & ONDK */
	MHEGIB_CreateWindow(argc, argv);

	/* Initialize APK */
	if(local_MhegIb_InitAPK() != ERR_OK)
	{
		HxLOG_Error("[local_MhegIb_InitAPK] Err");
	}

	/* Initialize OAPI */
	if(local_MhegIb_InitOapi() != ERR_OK)
	{
		HxLOG_Error("[local_MhegIb_InitOapi] Err");
	}

	/* initialize mheg internal browser */
	MHEGIB_Init();

	/* initialize Code page*/
	if(HLIB_CP_Init() != ERR_OK)
	{
		HxLOG_Error("[HLIB_CP_Init] Err");
	}

	HLIB_EXTRA_DropRootPrivileges("settop");

	while (1)
	{
		/* APK MSG wait */
		if (APK_EVENT_Dispatch() == 0)
			HLIB_STD_TaskSleep(100);
	}
	return 0;
}

