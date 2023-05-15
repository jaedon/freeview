/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/********************************************************************
 * $Workfile:   _xsvc_cas_vmx_psi.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef	__INT_XSVC_VMXPSI_H__
#define	__INT_XSVC_VMXPSI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <stb_params.h>
#include "_svc_cas_mgr_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

// 사양 관리.
#define VMX_INVALID_SERVICE_IDX				(0xff)
#define VMX_INVALID_FILTER_VALUE			(0xff)

#if defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)
/*******************************************
  *		YSR-2000,  YS-1000 (TDC)
  ******************************************/
#define VMX_MAX_NUM_OF_SERVICE_IDX			(10)

#define	VMX_MAX_NUM_OF_ES					(5*9)		// 5 ES * 9 (live 1 + tsr 1 + rec 2 + streaming 2 + fcc 2 + pip 1)
#define VMX_MAX_NUM_OF_ECM					(5*9)		// TODO_VMX : 단지 3으로 해야하는지 ?		//(5*3)		// live = TSR 일것같음..ㅎㅎ
#define VMX_MAX_NUM_OF_EMM					(1)			//(2)
#define VMX_MAX_NUM_OF_SESSION				(10)		// 5 session * 1 sc : 정확하게는 smartcard에서 session number를 준다....

#define VMX_NUM_OF_EMM_FILTER				(10)
#define VMX_EMM_FILTER_LEN					(7)

#define VMX_ECM_FILTER_LEN					(8)
#else
/*******************************************
  *		Default
  ******************************************/
#define VMX_MAX_NUM_OF_SERVICE_IDX			(3)

#define	VMX_MAX_NUM_OF_ES					(5*4)		// 5 ES * 4 (live + TSR + REC1/2)
#define VMX_MAX_NUM_OF_ECM					(5*3)		// TODO_VMX : 단지 3으로 해야하는지 ?		//(5*3)		// live = TSR 일것같음..ㅎㅎ
#define VMX_MAX_NUM_OF_EMM					(1)			//(2)
#define VMX_MAX_NUM_OF_SESSION				(5*1)		// 5 session * 1 sc : 정확하게는 smartcard에서 session number를 준다....

#define VMX_NUM_OF_EMM_FILTER				(10)
#define VMX_EMM_FILTER_LEN					(7)

#define VMX_ECM_FILTER_LEN					(8)
#endif

#define VMX_INVALID_SESSION_NUM				(-1)

// Macro Tool
#define VALID_PID(x)								(x > 0 && x < 0x2000)
#define INVALID_PID(x)							!VALID_PID(x)
#define GET_ACTION_ID(c) 						((c & 0xFF000000) >> 24)
#define GET_ACTION_VERSION(c) 				(c & 0x00FFFFFF)
#define GET_SERVICE_ID(x)						(x & 0xffff)
#define GET_UNIQUE_ID(x) 						((x & 0xffff0000) >> 16)
#define MWCAS_UNUSED(X)

#define VMX_MAX_NUM_OF_ESPID				(12)

#define VMX_PID_ADD							(TRUE)
#define VMX_PID_DELETE						(FALSE)

#define VMX_MAX_EMM_ADDR					(8)
#define VMX_EMM_ADDR_LEN					(1+7)	// 맨앞에다가는 0x82를 넣을 것이다.
#define	VMX_EMM_MAX_GET_HANDLE			(5)		// 3번(CX_SC_MAX_RETRY_SEND_CMD 참조) 이상 될 수 없으니 여유를 생각해서...

#define VMXGETBIT8(addr,s,len) (unsigned char)(((addr)[0]>>(s))&((1<<len)-1))
#define VMXGETBIT16(addr,s,len) (unsigned short)((((((unsigned short)((addr)[0]))<<8)|\
							(unsigned short)((addr)[1]))>>(s))&((1<<len)-1))
#define VMXGETBIT32(addr,s,len) (unsigned long)(((((((unsigned long)((addr)[0]))<<24)|\
                             			(((unsigned long)((addr)[1]))<<16)|\
                            			(((unsigned long)((addr)[2]))<< 8)|\
			                          (unsigned long)((addr)[3]))>>(s)) << (32-len)) >> (32-len))


#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#define VMX_MAX_EMM_FILTER_NUM				(1)
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#define VMX_MAX_EMM_FILTER_NUM				(32)
#define VMX_ECM_TABLE_ID_80					(0x80)
#define VMX_ECM_TABLE_ID_81					(0x81)
#define VMX_EMM_TABLE_ID						(0x82)
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*********************************************************************************************
						enum
*********************************************************************************************/
typedef enum
{
	eVmx_Session_Init,	// reset 전
	eVmx_Session_Ready,		// reset 된 후
	eVmx_Session_Used,		// scrambled service 에 사용
} Vmx_SessionState_t;

typedef enum
{
	eVmx_Stream_NotUsed,
	eVmx_Stream_Used,
} Vmx_StreamState_t;

typedef enum
{
	eVmx_Ecm_Init,
	eVmx_Ecm_Running,
	eVmx_Ecm_Stopped
} Vmx_EcmState_t;

typedef enum
{
	eVmx_Ecm_Both = 0,
	eVmx_Ecm_Even = 0x80,
	eVmx_Ecm_Odd = 0x81
} Vmx_EcmMode_t;

 typedef enum
{
	eVmxBc_EcmFilterMode_None = 0,
   	eVmxBc_EcmFilterModeDisableFilter,
  	eVmxBc_EcmFilterMode_PageSearch,
  	eVmxBc_EcmFilterModePageLocked,
}  VmxBc_EcmFilterMode_t;

typedef enum
{
	eVmx_EcmPageSearch_None = 0,
	eVmx_EcmPageSearch_Ready,
	eVmx_EcmPageSearch_Complete
} Vmx_EcmPageSearch_e;

typedef enum
{
	eVmx_Emm_Init,
	eVmx_Emm_Running,
	eVmx_Emm_Stopped
} Vmx_EmmState_t;

typedef enum
{
	eVmxDvb_Emm_NoMatch = 0,
	eVmxDvb_Emm_Match
} VmxDvb_EmmMatch_e;

typedef enum
{
	eSvcVmxEsType_None,
	eSvcVmx_Video,
	eSvcVmx_Audio,
	eSvcVmx_Subtitle,
	eSvcVmx_Teletext,
	eSvcVmx_Data,
	eSvcVmx_Reserved,
	eSvcVmx_Unknown,
	eSvcVmx_Max
} Vmx_EsType_t;

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB) && defined(VMX_SVC_TEST)
typedef enum
{
	eVmx_PsiMsg_Init = 0,
	eVmx_PsiMsg_StopSvc,
	eVmx_PsiMsg_Error
} Vmx_Psi_Msg_e;
#endif

/*********************************************************************************************
						struct
*********************************************************************************************/

typedef struct {
	HUINT32 					ulPidNum;
	Vmx_EsType_t				eType[VMX_MAX_NUM_OF_ESPID];
	HUINT16						usPid[VMX_MAX_NUM_OF_ESPID];
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	HUINT16						usEcmPid[VMX_MAX_NUM_OF_ESPID];
#endif
} Vmx_EsPidInfo_t;

// information about session (각 session에 service (action)을 할당한다. 즉, session단위로 모든 처리를 한다)
typedef struct
{
	Handle_t 			hStbHandle;
	HINT32				nSessionIdx;
	Vmx_SessionState_t	eState;
	HBOOL				bSelected;			// 선택되어야만 ecm filter 사용하도록
	HUINT16 			usUniqueTsId;			// 각 session별로 CAT가 들어올 수도 있어서 같은 것들을 구분하려면 필요.
	SvcCas_PmtInfo_t 	*pstPmtInfo;
	Vmx_EsPidInfo_t 	stEsPidInfo;
} Vmx_Session_t;

// information about each elementary stream
typedef struct
{
	HUINT16			usPid;
	Vmx_StreamState_t	eState;
	Handle_t			hSession;
	Handle_t			hEcm;
	Handle_t			hDescrambler;
} Vmx_Stream_t;


typedef struct
{
	VmxBc_EcmFilterMode_t	eFilterMode;
	HUINT8		ucVmxFilterId; // serviceIndex
	Vmx_EcmPageSearch_e   ePageSearchMode;
	HUINT8  		ucTableId;
	HUINT8  		ucVersion;
	HUINT8 		ucPage;
//	HUINT8 		ucNumOfLastPage;
} Vmx_EcmFilter_t;

// information about ecm filter and corresponding CW
typedef struct
{
	HUINT16				usEcmPid;
	HUINT16				usRefCnt;
	Handle_t				hSession;
	Vmx_EcmState_t		eState;
	HUINT16				usUniqueId;		// filter 거는데 필요
	HUINT32				ulFilterId;		// filter 거는데 필요
	HUINT8				*pucEcmRaw;
	HUINT16				usLen;
	HUINT8				aucFilter[VMX_ECM_FILTER_LEN];
	HUINT8				aucMask[VMX_ECM_FILTER_LEN];
	Vmx_EcmFilter_t		 stFilter;	// 단지 betacrypt 용으로만 사용된다.
} Vmx_Ecm_t;

typedef struct
{
	HUINT8  address[4];	/* address for emm filter */
} VmxDvb_EmmFilterAddress_t;

typedef struct {
	Vmx_EmmState_t	eState;
	HUINT16			usUniqueId;		// filter 거는데 필요
	HUINT32			ulFilterId;		// filter 거는데 필요
	HUINT8			aucFilter[VMX_EMM_ADDR_LEN];
	HUINT8			aucMask[VMX_EMM_ADDR_LEN];
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
	HUINT8			ucFilterIndex;
	VmxDvb_EmmFilterAddress_t 	stEmmFilterAdd;	/* address for emm filter */
#endif
} Vmx_EmmAddr_t;

typedef struct {
	HUINT16				usUniqueTsId;		// ts id로 구분해야 할 것 같네요.
	Handle_t				hSession;			// filter 걸려니까 필요한 것 같음 (octo의 경우 filter걸때 action handle을 써야 해서..)
//	HINT32				nScSlot;
	HUINT8				*pucCatRaw;
	HUINT16				usEmmPid;
	HUINT16				usEmmAddrCnt;		// Betacrypt = 32, Hybrid = 1
	Vmx_EmmAddr_t		stEmmAddr[VMX_MAX_EMM_FILTER_NUM];
} Vmx_Emm_t;

/*********************************************************************************************
						function
*********************************************************************************************/
// main
HERROR xsvc_vmx_psi_Init(void);

// session
HERROR  xsvc_vmx_psi_session_Init(void);
Handle_t xsvc_vmx_psi_session_GetHandleByStbHandle(Handle_t hStbHandle);
Handle_t xsvc_vmx_psi_session_GetStbHandle(Handle_t hSession);
HUINT32 xsvc_vmx_psi_session_GetStbActionId(Handle_t hSession);
Vmx_Session_t *xsvc_vmx_psi_session_GetSessionFromhStbHandle(Handle_t hStbHandle);
SvcCas_PmtInfo_t * xsvc_vmx_psi_session_GetPmtInfo(Handle_t hSession);
HINT32 xsvc_vmx_psi_session_GetSessionIdx(Handle_t hSession);
HBOOL xsvc_vmx_psi_session_IsSelected(Handle_t hSession);
HERROR xsvc_vmx_psi_session_Select(Handle_t hStbHandle, HBOOL bSelect);
HERROR xsvc_vmx_psi_session_StartSvc(Handle_t hStbHandle);
HERROR xsvc_vmx_psi_session_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist);
HERROR xsvc_vmx_psi_session_SetPid(Handle_t hStbHandle, Vmx_EsPidInfo_t *pstEsPidInfo);
HERROR xsvc_vmx_psi_session_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t *pstPmtInfo);
HERROR xsvc_vmx_psi_session_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8 *pucCat);
HUINT32 xsvc_vmx_psi_session_GetPidNum(Handle_t hStbHandle);
HUINT16 * xsvc_vmx_psi_session_GetPidList(Handle_t hStbHandle);
Vmx_EsPidInfo_t * xsvc_vmx_psi_session_GetSessonInfoByStbHandle(Handle_t hStbHandle);

// stream
HERROR xsvc_vmx_psi_stream_InitCore(void);
HERROR  xsvc_vmx_psi_stream_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag);
//HBOOL   xsvc_vmx_psi_stream_IsVmxStream(Handle_t hSession);
HERROR  xsvc_vmx_psi_stream_ParsePmt(Handle_t hSession, HBOOL *pbDscrStartFlag, HBOOL * pbDscrStopFlag);
HERROR  xsvc_vmx_psi_stream_DeleteAll(Handle_t hStbHandle,Handle_t hSession, SvcCas_PmtInfo_t *pstPmtInfo);
HERROR  xsvc_vmx_psi_stream_IsScrambledSvc(Handle_t hSession, HBOOL *pbScrambled);
HERROR  xsvc_vmx_psi_stream_StartAllEcmFilter(Handle_t hSession);
#ifdef VMX_SVC_TEST
HERROR xsvc_vmx_psi_stream_CompareValidPidFromSession(Handle_t hSession, HUINT16 usPid);
#endif
HERROR  xsvc_vmx_psi_stream_StopAllEcmFilter(Handle_t hSession);
HERROR  xsvc_vmx_psi_stream_RestartAllEcmFilter(Handle_t hSession);
HERROR  xsvc_vmx_psi_stream_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm);
Handle_t xsvc_vmx_psi_stream_GetFirstSessionByEcm(Handle_t hEcm);
HUINT16  xsvc_vmx_psi_stream_GetEcmPidFromPmt(SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_PmtElement_t *pstEsInfo);
Handle_t xsvc_vmx_psi_stream_GetEcmHandleByEsPid(HUINT16 usPid);

//ecm
HUINT16 xsvc_vmx_psi_ecm_GetEcmLength(Handle_t hEcm);
HERROR xsvc_vmx_psi_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT32 hAction);
Handle_t  xsvc_vmx_psi_ecm_Add(Handle_t hSession, HUINT16 usEcmPid);
HERROR xsvc_vmx_psi_ecm_Delete(Handle_t hEcm);
HERROR xsvc_vmx_psi_ecm_StartFilter(Handle_t hEcm, Vmx_EcmMode_t eMode, Vmx_EcmFilter_t *pstEcmFilter);
HERROR xsvc_vmx_psi_ecm_StopFilter(Handle_t hEcm);
HUINT16 xsvc_vmx_psi_ecm_GetPid(Handle_t hEcm);
HERROR  xsvc_vmx_psi_ecm_RestartFilter(Handle_t hEcm, Vmx_EcmMode_t eMode, Vmx_EcmFilter_t *pstEcmFilter);
HERROR vmx_vr_ecm_GetCaSystemId(HUINT16* pusCaSystemId);
HERROR xsvc_vmx_psi_ecm_InitCore(void);
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
// ecm iptv hybrid
void vmx_vr_iptvh_ecm_SendEcmMsgToVm(void);
void vmx_vr_iptvh_emm_SendEmmMsgToVm(HUINT32 ulLen);
HBOOL vmx_vr_iptvh_ecm_IsQueueEmpty(void);
HBOOL  xsvc_vmx_core_dvb_iptv_ecm_IsQueueEmpty(void);
HUINT8 *  xsvc_vmx_core_dvb_iptv_ecm_QueueGet(Handle_t *pSessionHandle, Handle_t *pEcmHandle);
HERROR   xsvc_vmx_core_dvb_iptv_ecm_QueueClear(void);
HERROR vmx_vr_iptvh_ecm_Init(void);
HERROR vmx_vr_iptvh_ecm_OnEcmReceived(Vmx_Ecm_t *pEcm, HUINT8 *pucRawData);
HUINT8 * vmx_vr_iptvh_ecm_QueueGet(Handle_t *pSessionHandle, Handle_t *pEcmHandle);
// emm iptv hybrid
HERROR vmx_vr_iptvh_emm_Init(void);
HERROR vmx_vr_iptvh_emm_OnReceived(Vmx_Emm_t *pEmm, HUINT8 *pucRawData, HBOOL *pbMsg);
HUINT8 *vmx_vr_iptvh_emm_QueueGet(void);
// psi mgr
HERROR vmx_vr_iptvh_psi_mgr_Init(void);
void vmx_vr_iptvh_psi_mgr_UserDataCallback(void *pUserParam, int userdataLength, void *pUserDataValues);
void vmx_vr_iptvh_psi_mgr_OpaqueDataCallback(void*pUserParam, unsigned int nDataSize, void *pIrdOpaqueData);
void vmx_vr_iptvh_psi_mgr_SetOverrideRatingLevelToVm(Handle_t hSTBSession);
void vmx_vr_iptvh_psi_mgr_StartService(Handle_t hSTBSession);
void vmx_vr_iptvh_psi_mgr_StopService(Handle_t hSTBSession);
void vmx_vr_iptvh_psi_mgr_CreateAllChannelCtx(void);
HERROR vmx_vr_iptvh_psi_mgr_CreateChannelCtx(Handle_t hSTBSession);
#endif
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
// ecm betacrypt
HERROR vmx_vr_bc_ecm_ReadEcmBuf(HUINT8 bFilterId, HUINT8* pabBuffer, HUINT16* pwLen);
HERROR xsvc_vmx_mod_bc_psi_ecm_WriteEcmBuf(HUINT8 ucFilterId, HUINT8 * pucEcmBuf, HUINT16 usEcmSize);
void vmx_vr_bc_ecm_FlushEcmBuf(HUINT8 ucFilterId);
HERROR vmx_vr_bc_StartDescrambling(Handle_t hStbHandle, CAS_PmtInfo_t 	*pstPmtInfo);
Handle_t xsvc_vmx_psi_ecm_GetEcmHandleFromEcmPid(HUINT16 usEcmPid);
HERROR vmx_vr_bc_ecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData, HUINT16  usLen, HUINT32 hAction);
HERROR  vmx_vr_bc_StopDescrambling( Handle_t hStbHandle, CAS_PmtInfo_t *pstPmtInfo);
Vmx_EcmFilter_t * xsvc_vmx_psi_ecm_GetEcmFilterInfoFromEcmPid(HUINT16 usEcmPid);
Vmx_EcmFilter_t * xsvc_vmx_psi_ecm_GetEcmFilterInfoFromDescFilterId(HUINT32 usDescFilterId);
HINT16 vmx_vr_bc_ecm_SetEcmFilter(HUINT8 ucFilterId, VmxBc_EcmFilterMode_t eFilterMode, HUINT16 usEcmPid, HUINT8 ucTableId, HUINT8 ucVersion, HUINT8 ucPage);
HERROR vmx_vr_bc_ecm_Init(void);
#endif

// emm
HERROR xsvc_vmx_psi_emm_OnEmmReceived(HUINT32 ulFilterId, HUINT8 *pucRawData);
HERROR xsvc_vmx_psi_emm_InitCore(void);
HERROR xsvc_vmx_psi_emm_UpdateCat(Handle_t hSession, HUINT16 usUniqueTsId, HUINT8 *pucCat);
HERROR  xsvc_vmx_psi_emm_DeleteByTsId(HUINT16 usUniqueTsId);
HUINT8 *xsvc_vmx_core_dvb_emm_QueueGet(void);
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
// emm
HERROR xsvc_vmx_psi_emm_DeleteEmmAdd_ByFilterIndex( HUINT8 ucFilterIndex);
HERROR vmx_vr_bc_emm_ReadEmmBuf(HUINT8* pabBuffer, HUINT16* pwLen, HBOOL *pbError);
HERROR vmx_vr_bc_emm_WriteEmmBuf(HUINT8 * pucEmmBuf, HUINT16 usEmmSize);
void vmx_vr_bc_emm_FlushEmmBuf(void);
void vmx_vr_bc_emm_SaveEmmData(Vmx_Emm_t *pstEmm, HUINT8 *pucData, HUINT16 usSize);
HERROR xsvc_vmx_psi_emm_DeleteAll(void);
HERROR xsvc_vmx_psi_emm_SetEmmPid(HUINT16 usEmmPid);
HERROR  xsvc_vmx_psi_emm_AddByFilterIndex(Vmx_Emm_t *pEmm, HUINT8 ucFilterIndex);
Vmx_Emm_t *xsvc_vmx_psi_emm_GetEmmInfo(void);
HERROR vmx_emm_DeleteEmmAdd_ByFilterIndex( HUINT8 ucFilterIndex);
HERROR vmx_vr_bc_emm_SetEmmFilter(HUINT8 ucFilterIndex, HUINT8 ucAddressLength, HUINT8* pucAddress);
HERROR vmx_vr_bc_emm_SetEmmPid(HUINT16 usEmmPid);
HERROR vmx_vr_bc_emm_Init(void);
// To Betacrypt Lib
HERROR vmx_vr_bc_emm_SetEmmPidToBC(HUINT16 usEmmPid);
#endif

#endif /* !__INT_XSVC_VMXPSI_H__ */
