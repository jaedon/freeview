/**************************************************************
 *	@file		cpe_peer.c
 *	TR-069 Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/20
 *	@author			humax
 *	@brief			peer function that will be call by iCube library
 **************************************************************/
#include <hlib.h>
#include <hapi.h>
#include <papi.h>

#include <CWMP_STB_API.h>
#include <CWMP_CPE_API.h>
#include "rema_int.h"
#include "rema_cwmp.h"

#include "rema_DM.h"


#if 0
#define HxLog_Print printf
#define HxLog_Debug printf
#define HxLog_Message printf
#define HxLog_info printf
#endif
CWMP_STB_RESULT Cwmp_Stb_open()
{
	HERROR hErr;
	RemLOG_Print("Cwmp_Stb_open ###################\n\n");

	hErr = rema_cwmp_Init();
	hErr = rema_DM_Init();

	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_close ()
{
	RemLOG_Print("Cwmp_Stb_close ###################\n\n");

	rema_DM_Deinit();
	rema_cwmp_DeInit();

	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_getParam (char *key, char **output, int nextlevel)
{
	RemLOG_Print("Cwmp_Stb_getParam ###################\n\n");

	REM_ENTER;

	if(output == NULL)
	{
		RemLOG_Print("Cwmp_Stb_getParam Null pointer!!\n\n");
		return CWMP_STB_RES_FAIL;
	}

	RemLOG_Print("Cwmp_Stb_getParam key:[%s], nextlevel:[%d]\n", key, nextlevel);

	*output = rema_cwmp_GetParam(key);
	if(*output==NULL)
	{
		RemLOG_Print("couldn't get parameter value!\n");
		REM_LEAVE(CWMP_STB_RES_FAIL);
	}

	RemLOG_Print("################ Cwmp_Stb_getParam output:[%s]\n", *output);

	return CWMP_STB_RES_SUCCESS;
	REM_LEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_setParam (char *input)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("Cwmp_Stb_setParam################### \n\n");

	REM_ENTER;

	hErr = rema_cwmp_SetParam(input);
	if(hErr != ERR_OK)
	{
		RemLOG_Print("couldn't set parameter value!\n");
		REM_LEAVE(CWMP_STB_RES_FAIL);
	}
	return CWMP_STB_RES_SUCCESS;
	REM_LEAVE(CWMP_STB_RES_SUCCESS);
}
CWMP_STB_RESULT Cwmp_Stb_setParamByCPE (char *input)
{
		return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_addObject (char *input)
{
		return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_deleteObject (const char *key)
{
		return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_primeUpload(CWMP_STB_FILE_TYPE file_type,int nIndex,char **src_path,char **upload_id)
{
	return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_finishedUpload(char *upload_id, CWMP_STB_FILE_TYPE file_type, char *pszFaultCode)
{

	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_primeDownload(CWMP_STB_FILE_TYPE file_type,	char *target_file_name,	 int file_size, char **download_path, char **download_id)
{
		return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_applyDownload (char *download_id)
{
	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_checkDownload (char *download_id)
{
		return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_failedDownload (char *download_id,char *pszFaultCode)
{
		return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_CheckAutoXfer(int *nCheck)
{
		return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_GetAutoXferOp(AutoXferOp *pOp)
{
		return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_notifyEvent(CWMP_STB_EVENT event_code,	char *additional_info)
{
	switch(event_code)
	{
		case CWMP_STB_EVT_REBOOT:
			RemLOG_Print("[%s]CWMP_STB_EVT_REBOOT \n\n",__FUNCTION__);
			Cwmp_Cpe_close();
			PAPI_RequestReboot();
			break;

		case CWMP_STB_EVT_FACTORYRESET:
			RemLOG_Print("[%s]CWMP_STB_EVT_FACTORYRESET \n\n",__FUNCTION__);
			break;

		case CWMP_STB_EVT_FACTORYRESET_STB:
			RemLOG_Print("[%s]CWMP_STB_EVT_FACTORYRESET_STB \n\n",__FUNCTION__);
			break;

		case CWMP_STB_EVT_CANNOT_ACCESS_TO5MIN_FROMOPEN:
			RemLOG_Print("[%s]CWMP_STB_EVT_CANNOT_ACCESS_TO5MIN_FROMOPEN \n\n",__FUNCTION__);
			break;

		default:
			//HxLog_Debug(" STB_API: %s was called, but not implemented! %d, %s\n",__FUNCTION__, event_code, additional_info);
			break;
	}
	
	return CWMP_STB_RES_SUCCESS;
}

int total =0;
//정상적으로 open 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_open(void)
{
		return CWMP_STB_RES_SUCCESS;
}

//정상적으로 close 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_close(void)
{
		return CWMP_STB_RES_SUCCESS;
}

//정상적으로 close 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_write(const void *buf, size_t nbyte)
{
		return CWMP_STB_RES_SUCCESS;
}

void Cwmp_Stb_notifyTransferStates(const char *pszID, const CWMP_STB_FILE_TYPE eType,
			const unsigned int unDownloadedSize, const unsigned int unTotalSize)
{

}

CWMP_STB_RESULT Cwmp_Stb_StartDiagnostics(char * pDianosicsObject)
{
	return CWMP_STB_RES_SUCCESS;
}
CWMP_STB_RESULT Cwmp_Stb_StopDiagnostics(char * pDianosicsObject)
{
	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_PrintFault(const char * pszMsg, ECWMP_FAULTCODE_FROM eFromEq)
{
		return CWMP_STB_RES_SUCCESS;
}

void Cwmp_Stb_NotifySessionStart(void)
{
	//TR069_NotifyCwmpSessionStart();
}

void Cwmp_Stb_NotifySendTransferComplete(void)
{
	//TR069_NotifyCwmpSendTransferComplete();
}

void Cwmp_Stb_NotifySendHTTPEmpty(void)
{
	//TR069_NotifyCwmpSendHTTPEmpty();
}

void Cwmp_Stb_NotifySendDownloadResponse(void)
{
	//TR069_NotifyCwmpSendDownloadResponse();
}

void Cwmp_Stb_NotifySessionEnd(void)
{
	Cwmp_Cpe_Add_Event(eCWMP_CPE_EVENT_NOTIFY_UPDATED_PARAM);
}


