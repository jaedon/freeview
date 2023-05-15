/*
	Viaccess Promgram Information API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.75
*/

#ifndef VA_PI_API_H
#define VA_PI_API_H

#include "va_pi.h"
#include "va_ctrl.h"
#include "va_main.h"
#include "va_def.h"
#include "va_ui_adapt.h"
#include <hlib.h>

#define VA_PI_MAX_ACS_INSTANCE		VA_MAIN_ACS_NUM
#define VA_PI_MAX_STREAM				8
#define VA_PI_MAX_ES_TYPE				12	// 4 : octo에서는 좀 더 늘어났지? ==> recording은 모든 audio를 저장해야하고, audio는 여러개 있을 수 있다..

#define VA_SI_SECTION_SIZE				1024
#define VA_PI_MAX_SECTIONS				4

#define VA_PI_GetAcsId(a)				((a >> 8) & 0xFF)
#define VA_PI_GetStreamId(a)			(a & 0xFF)
//#define VA_PI_GetEsId(a)					(a & 0xFF)

#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
#define VA_PI_IfNotValidAcsId(a,b) do { if (a >= VA_PI_MAX_ACS_INSTANCE) { 	HxLOG_Info("wrong dwAcsId (%d)\n", a); return (b); } } while (0)
#else
#define VA_PI_IfNotValidAcsId(a,b) do { if (a >= VA_PI_MAX_ACS_INSTANCE) { 	HxLOG_Error("wrong dwAcsId (%d)\n", a); 	return (b); } } while (0)
#endif
#define VA_PI_IfNotValidStreamId(a,b) do { 	if (a >= VA_PI_MAX_ES_TYPE) { HxLOG_Error("wrong stream id (%d)\n", a); return (b); } } while (0)

typedef enum
{
	VA_PI_ACS_CLOSE = 0,
	VA_PI_ACS_OPEN
} VA_PI_ACS_STATE_ENUM;

typedef enum
{
	VA_PI_PROGRAM_SWITCH_OFF = 0,
	VA_PI_PROGRAM_SWITCH_ON,		// first pmt delivered
	VA_PI_PROGRAM_UPDATE,			// next pmt delivered
	VA_PI_PROGRAM_READY
} VA_PI_PROGRAM_STATE_ENUM;

typedef struct {
	UINT32 				ulVaStreamHandle;		// dwVaStreamHandle, set by VA_PI_Adapt_PmtChange() through Viaccess function, VA_PI_AddStream(dwStbStreamHandle).
	UINT32 				ulStbStreamHandle;	// dwStbStreamHandle, set by VA_PI_Adapt_PmtChange() through Humax private function, local_va_pi_GetStreamHandle().
#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
	UINT32				ulStbDscrHandle;
#endif

	UINT16 				usPid;					// set by VA_PI_Adapt_PmtChange().
	EsType_t 			esType;				// set by VA_PI_Adapt_PmtChange().
	SvcCas_PmtElement_t	*pPmtElement;			// set by VA_PI_Adapt_PmtChange().
#ifdef __VIACCESS50__
	tVA_CTRL_StreamDescramblingEvent	nDesState;
	tVA_CTRL_StreamCaEnableEvent		nCaState;
#else
	tVA_PI_StreamEvent 	nState;				// set by VA_PI_NotifyStreamEvent().
#endif
} VA_PI_Stream;

typedef struct {
	UINT16 usType;
	UINT16 usSvcType;
	UINT16 usPid;
	UINT8 *pucDesc;
	UINT32 ulDescLength;
} VA_PI_ES_Desc;

typedef struct
{
	UINT32			ulEsNum;
	VA_PI_Stream	astEs[VA_PI_MAX_ES_TYPE];
} VA_PI_StreamInfo_t;

typedef struct
{
	UINT32						nAcsIdx;
	Handle_t 						hVaAction;
//사용안함	HBOOL						bSelected;			// 선택되어야만 ecm filter 사용하도록

	VA_PI_ACS_STATE_ENUM 		nAcsState;

#ifdef __VIACCESS50__
	tVA_CTRL_AcsMode			nAcsMode;
#else
	tVA_PI_AcsMode				nAcsMode;
#endif
	VA_PI_PROGRAM_STATE_ENUM 	nProgramState;

	SvcCas_PmtInfo_t 				*pstPmtInfo;		// set by VA_PI_Adapt_PmtChange() through svc_cas_MgrSubGetPmtInfo(), called by local_vaact_UpdatePmt() -> PMT 내의 ES 정보를 모두 들고 있음.
	SvcCas_SetPidInfo_t				stPidInfo;			// set by VA_PI_Adapt_SetPid(), called by local_vaact_SetPid() -> 현재 play 중인 ES 정보만 들고 있음.
	VA_PI_StreamInfo_t			stStreamInfo;		// set by local_va_pi_adapt_StreamChange() -> VA 와 핸들링 되고 있는 ES 정보를 들고 있음.
#ifdef __VIACCESS50__
	UINT32 						ulPmtLength;
	UINT8 						aucPmt[VA_SI_SECTION_SIZE];
#endif
	UINT16 						usUniqueTsId;
	HBOOL 						bCatReceived;
	UINT32 						ulCatLength;
	UINT8 						aucCat[VA_SI_SECTION_SIZE];

	VA_UI_INFO					stUiInfo;
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	HBOOL						bRecordForbidden;
	HBOOL						bTsrForbidden;
	HBOOL						bPlaybackStarted;
#endif
#ifdef __VIACCESS50__
	tVA_CTRL_StreamNotificationCallback	cbCtrlStreamNotiCallback;
#endif
}  VA_PI_AcsInstance;
// hjlee3, for Va_ctrl_api.c
extern VA_PI_AcsInstance *pstVaAcsInstance[VA_PI_MAX_ACS_INSTANCE];

VA_UI_INFO *VA_PI_GetUiInfoByAcsId(HUINT32 ulAcsIdx);
VA_ES_TYPE VA_PI_GetStreamStateForUi(HUINT32 ulAcsIdx);
int VA_PI_Adapt_GetStreamTypeFromStreamHandle(UINT32 lStbStreamHandle);
Handle_t VA_PI_Adapt_GetVaActionHandleByAcsId(HUINT32 ulAcsIdx);
UINT16 VA_PI_Adapt_GetPidFromStreamHandle(UINT32 lStbStreamHandle);
Handle_t VA_PI_Adapt_GetVaActionHandleByStreamHandle(UINT32 lStbStreamHandle);

void VA_PI_Adapt_Init();
void VA_PI_Adapt_StartProgram(Handle_t hVaAction, UINT16 usUniqueId);
void VA_PI_Adapt_StopProgram();
void VA_PI_Adapt_PmtChange(Handle_t hVaAction, SvcCas_PmtInfo_t *pstPmtInfo, HUINT8 *pmtRaw);
void VA_PI_Adapt_CatChange(Handle_t hVaAction, UINT16 usUniqueId, UINT8 *pucCat);
void VA_PI_Adapt_SetPid(Handle_t hVaAction, SvcCas_SetPidInfo_t *pstEsPidInfo);
void VA_PI_Adapt_Select(Handle_t hVaAction, HBOOL bSelect);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VA_PI_Adapt_IsProgramSwitchOn(Handle_t hVaAction);
HUINT32 VA_PI_Adapt_GetAcsIdByVaActionHandle(Handle_t hVaAction);
HINT32 VA_PI_Adapt_GetRecordModeAcs(HUINT32 ulAcsIdx);
HBOOL VA_PI_Adapt_IsLive(HUINT32 ulAcsIdx);
HBOOL VA_PI_Adapt_IsActive(UINT32 lStbStreamHandle);
HERROR VA_PI_Adapt_SetRecordForbidden(HUINT32 ulAcsId);
HINT32 VA_PI_Adapt_GetPlaybackAcsId();
HBOOL VA_PI_Adapt_IsPlayback(HUINT32 ulAcsIdx);
HBOOL VA_PI_Adapt_IsRecord(HUINT32 ulAcsIdx);
HBOOL VA_PI_Adapt_IsTimeshiftRecord(HUINT32 ulAcsIdx);
VA_PI_AcsInstance *VA_PI_Adapt_GetAcsInstById(HUINT32 ulAcsIdx);
#endif

#endif //VA_PI_API_H
