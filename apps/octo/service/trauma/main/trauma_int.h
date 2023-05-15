
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#ifndef	__TRAUMA_INT__
#define	__TRAUMA_INT__
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>
#include <apk.h>
#include <trapi_int.h>


#include <vkernel.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

#define TRAUMA_MSG_ERIN_START			0x00010000
#define TRAUMA_MSG_STATISTICS_START		0x00020000
#define TRAUMA_MSG_APPKIT_START			0x00040000
#define TRAUMA_MSG_TVPORTAL_START		0x00080000

#define TRAUMA_TVPORTAL_URL_MAX_LEN		(512)
#define TRAUMA_TASK_SLEEP 				(1000)

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef enum
{
	eTRAUMA_MSG_START,
	eTRAUMA_MSG_ERIN 				= TRAUMA_MSG_ERIN_START,
	eTRAUMA_MSG_ERIN_RECEIVE,
	eTRAUMA_MSG_ERIN_CHECK_TIMEOUT,

	eTRAUMA_MSG_STATISTICS 			= TRAUMA_MSG_STATISTICS_START,
	eTRAUMA_MSG_STATISTICS_REPORT,
	eTRAUMA_MSG_STATISTICS_SET_ACCEPT_PRIVACY,
	eTRAUMA_MSG_STATISTICS_LOG,
	eTRAUMA_MSG_STATISTICS_CODE_SETINFO,
	eTRAUMA_MSG_STATISTICS_CODE_SSTART,
	eTRAUMA_MSG_STATISTICS_CODE_SSTAND,
	eTRAUMA_MSG_STATISTICS_CODE_SALIVE,
	eTRAUMA_MSG_STATISTICS_CODE_KI,
	eTRAUMA_MSG_STATISTICS_CODE_VOL,
	eTRAUMA_MSG_STATISTICS_CODE_SUSB,
	eTRAUMA_MSG_STATISTICS_CODE_SHDMI,
	eTRAUMA_MSG_STATISTICS_CODE_ERSYS,
	eTRAUMA_MSG_STATISTICS_CODE_ERCAS,
	eTRAUMA_MSG_STATISTICS_CODE_GES,
	eTRAUMA_MSG_STATISTICS_CODE_GEE,
	eTRAUMA_MSG_STATISTICS_CODE_CKOTA,
	eTRAUMA_MSG_STATISTICS_CODE_DLS,
	eTRAUMA_MSG_STATISTICS_CODE_DLE,
	eTRAUMA_MSG_STATISTICS_CODE_ELWS,
	eTRAUMA_MSG_STATISTICS_CODE_ERECS,
	eTRAUMA_MSG_STATISTICS_CODE_ERECE,
	eTRAUMA_MSG_STATISTICS_CODE_EREMIND,
	eTRAUMA_MSG_STATISTICS_CODE_ELSS,
	eTRAUMA_MSG_STATISTICS_CODE_ELSE,
	eTRAUMA_MSG_STATISTICS_CHECK_LOGFILTER,
	eTRAUMA_MSG_STATISTICS_REFRESH_LOGFILTER,
	/*******************************************************************************/
	//only used to TV-portal HTTP thread
	eTRAUMA_MSG_TVPORTAL_START		= TRAUMA_MSG_TVPORTAL_START,
	eTRAUMA_MSG_TVPORTAL_LOGOUT,
	eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGIN,
	eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGFILTER,
	eTRAUMA_MSG_TVPORTAL_HTTPGET_LOGIN,
	eTRAUMA_MSG_TVPORTAL_HTTPGET_GETLOGFILTER,
	eTRAUMA_MSG_TVPORTAL_HTTPGET_ACCEPT_PRIVACY,
	eTRAUMA_MSG_TVPORTAL_HTTPPUT_HCR,
	eTRAUMA_MSG_TVPORTAL_HTTPPUT_STATISTICS_UPLOAD,

	eTRAUMA_MSG_END
}TRAUMA_Msg_e;

typedef struct{
	TRAUMA_Msg_e	msg;
	HINT32			param1;
	HINT32			param2;
	HINT32			param3;
	HINT32			param4;
}TRAUMA_MSG_t;

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
HCHAR* 	TRAUMA_GetLanMac(void);
HBOOL 	TRAUMA_GetWLanMac(HCHAR *pszMacAddress);
HCHAR* 	TRAUMA_GetIPaddress(void);
HERROR 	TRAUMA_GetSystemInfo(HCHAR **ppszSysId,HCHAR **ppSysVer);
HBOOL 	TRAUMA_GetRemoteInfo(HxDATETIME_t *pstStartTime,HxDATETIME_t *pstEndTime);
HBOOL	TRAUMA_GetPowerOnTimerInfo(HCHAR *pszChannel,HUINT32 *pulVolume,DxRsvRepeat_e *peRepeat);


void 	TRAUMA_RPC_Init(void);
HINT32 	TRAUMA_RPC_GetHandle(void);
void 	TRAUMA_RPC_Print(void);
void 	TRAUMA_MSG_Send(TRAUMA_MSG_t *pstMsg);
void 	TRAUMA_ERIN_Process(TRAUMA_MSG_t *pstMsgData);
HERROR	TRAUMA_ERIN_RPCRecv(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);

void 	TRAUMA_STATISTICS_Init(void);
void 	TRAUMA_STATISTICS_Start(void);
void 	TRAUMA_STATISTICS_Process(TRAUMA_MSG_t *pstMsgData);
HINT32 	TRAUMA_STATISTICS_MakeHeader(HCHAR *pszDesc);
void 	TRAUMA_STATISTICS_AddReport(HCHAR *pszReport);
HxList_t*	TRAUMA_STATISTICS_GetLogs(void);
void 	TRAUMA_TVPORTAL_Init(void);
void 	TRAUMA_TVPORTAL_Login(void);
void 	TRAUMA_TVPORTAL_LogOut(void);
void 	TRAUMA_TVPORTAL_MSG_Send(TRAUMA_MSG_t *pstMsgData);
void	TRAUMA_TVPORTAL_GetLogFilter(void);
void 	TRAUMA_TVPORTAL_StatisticsUpload(HCHAR *pszData);
HCHAR* 	TRAUMA_TVPORTAL_GetAuthTicket(void);
HBOOL 	TRAUMA_TVPORTAL_GetAcceptPrivacy(void);
void 	TRAUMA_TVPORTAL_SetAcceptPrivacy(HBOOL bAccept);
HBOOL 		TRAUMA_TVPORTAL_GetLogFilterActiveStatus(void);
HUINT32 	TRAUMA_TVPORTAL_GetLogFilterInterval(void);
HUINT32 	TRAUMA_TVPORTAL_GetLogFilterTTL(void);
HxList_t* 	TRAUMA_TVPORTAL_GetLogFilterAllowList(void);
HxList_t* 	TRAUMA_TVPORTAL_GetLogFilterDenyList(void);

void 	TRAUMA_HCR_Check(void);

HINT32 	TRAUMA_TOOL_DFBKeyConvertGogiKey(HINT32 nSysmbol);
HCHAR* 	TRAUMA_TOOL_BoolConvertString(HBOOL bBool);
HCHAR*	TRAUMA_TOOL_AspectRatioConvertString(DxAspectRatio_e eTVAspectRatio);
HCHAR*	TRAUMA_TOOL_DFCConvertString(DxDfcSelect_e eDfc);
HCHAR*	TRAUMA_TOOL_ResolutionConvertString(DxDfcSelect_e eDfc);
HCHAR* 	TRAUMA_TOOL_DigitalAudioFormatConvertString(DxDigitalAudioFormat_e	eSpdifOut);
HCHAR* 	TRAUMA_TOOL_SoundModeConvertString(DxStereoSelect_e	eStereoSelect);
HCHAR* 	TRAUMA_TOOL_RepeatConvertString(DxRsvRepeat_e	eRepeat);
HCHAR* 	TRAUMA_TOOL_AntennaTypeConvertString(DxAntennaType_e	eAntType);
HCHAR* 	TRAUMA_TOOL_SourceTypeConvertString(DxSwUpdate_Source_e	eSource);
HINT32 	TRAUMA_TOOL_GetErrorByStates (OxMediaPlay_LiveViewState_e eViewState, OxMediaPlay_LiveLockState_e eLockState);
TRAUMA_Msg_e TRAUMA_TOOL_GetCodeByScheduleType(APK_META_SCH_MsgType_e eMsgType, DxRsvType_e eRsvType);
void 	TRAUMA_CMD_Init(void);
/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

#endif //__TRAUMA_INT__
