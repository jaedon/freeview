/***************************************************************
* $Workfile:   ci_hci.c  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <pal_ci.h>
#include <cb_list.h>
#include "ci_plus.h"

/***************************************************************
* local definition
***************************************************************/
#define MAX_CI_HCI_REPLACE	20




/***************************************************************
* typedef
***************************************************************/
typedef struct {
	unsigned char	ucReplacementRef;
	unsigned short	usReplacedPid;
	unsigned short	usReplacementPid;
	unsigned long	ulReplacementCount;
} CI_HCI_REPLACE;

/***************************************************************
* static / extern variables and structures
***************************************************************/
static CI_HCI_REPLACE	stCiHciReplace[MAX_CI_HCI_REPLACE];
static unsigned long	ulReplacementCount = 0;

extern CALLBACK_HANDLE	g_hCbNotifyHostTune;
static CI_NOTIFY_HOST_TUNE_FUNC s_pfnNotifyHostTune;
static SvcCas_CiPlusTuneInfo_t s_stTuneInfo;

extern CALLBACK_HANDLE	g_hCbNotifyPidReplace;
static CI_NOTIFY_PID_REPLACE_FUNC s_pfnNotifyPidReplace;
static CIPLUS_PidInfo_t s_stPidInfo;

static unsigned short s_usCiHciSsNo[PAL_MAX_CISLOT_NUMBER];

/***************************************************************
* static function prototypes
***************************************************************/
static int	CiHci_ProcTune(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int	CiHci_ProcReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int	CiHci_ProcClearReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
static int	CiHci_ProcTuneBroadcastReq(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen); // CI+ v1.3
static int	CiHci_ProcAskReleaseReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen); // CI+ v1.3
static int	CiHci_TuneReplyBySsNo(unsigned short usSsNo, CI_TUNE_STATUS eTuneResult); // CI+ v1.3
#endif

/***************************************************************
* function bodies
***************************************************************/

/***************************************************************
* local Functions for CC resource...
***************************************************************/
HULONG	local_CiHci_GetHciResVersion(unsigned short usSsNo)
{
	CI_SS_HANDLE hCiSession;
	hCiSession = CIRH_GetSs(usSsNo);

	return hCiSession->ulOpendSsResId;
}


/***************************************************************
* CC functions /////////////////////////////////////////////////
***************************************************************/

int CiHci_Init(void)
{
	HxSTD_memset(stCiHciReplace, 0, sizeof(CI_HCI_REPLACE) * MAX_CI_HCI_REPLACE);

	ulReplacementCount = 0;

	HxSTD_memset(s_usCiHciSsNo, 0, sizeof(unsigned short) * PAL_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

int CiHci_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	HxLOG_Print("[CIHCI]---> CiHci_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiHciSsNo[usSlotId] = usSsNo;

	return ERR_OK;
}

int CiHci_SsClosedProc(unsigned short usSsNo)
{
	CI_SLOT_ID	usSlotId;
	int			i;

	HxLOG_Print("[CIHCI]---> CiHci_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_SsClosedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiHciSsNo[usSlotId] = 0;

	if (ulReplacementCount == 0)
	{
		HxLOG_Print("[CIHCI]---> CiHci_SsClosedProc()---> no replaced\n");
		return ERR_OK;
	}

	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ulReplacementCount == ulReplacementCount)
			break;
	}

	/* 세션이 닫히면 현재 demux에 걸려있는 component 중에 stCiHciReplace[i].usReplacementPid 인 것을 찾아 stCiHciReplace[i].usReplacedPid 로 다시 걸어라 */

	if (i < MAX_CI_HCI_REPLACE)
	{
		HxLOG_Print("[CIHCI]---> CiHci_SsClosedProc()---> PID 0x%x returning to 0x%x ...\n", stCiHciReplace[i].usReplacementPid, stCiHciReplace[i].usReplacedPid);

		s_stPidInfo.eReplaceMode = eCiPlus_Replace_Stop;
		s_stPidInfo.usOriginalPid = stCiHciReplace[i].usReplacedPid;
		s_stPidInfo.usTargetPId = stCiHciReplace[i].usReplacementPid;
		Callback_Reset(g_hCbNotifyPidReplace);
		while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
		{
			if ((*s_pfnNotifyPidReplace)(usSlotId, &s_stPidInfo) != ERR_OK)
			{
				HxLOG_Error("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - PID returning failed !!!\n", usSsNo);
			}
		}
	}

	return ERR_OK;
}

int CiHci_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_TUNE:
		nRtnCode = CiHci_ProcTune(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_REPLACE:
		nRtnCode = CiHci_ProcReplace(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CLEAR_REPLACE:
		nRtnCode = CiHci_ProcClearReplace(usSsNo, pucApduData, ulApduLen);
		break;

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
	case CI_TUNE_BROADCAST_REQ:
		nRtnCode = CiHci_ProcTuneBroadcastReq(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_ASK_RELEASE_REPLY:
		nRtnCode = CiHci_ProcAskReleaseReply(usSsNo, pucApduData, ulApduLen);
		break;
#endif

	default:
		nRtnCode = -1;
		HxLOG_Error("[CIHCI]---> CiHci_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print("[CIHCI]---> CiHci_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

static int	CiHci_ProcTune(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned short	usNetId;
	unsigned short	usOrgNetId;
	unsigned short	usTsId;
	unsigned short	usSvcId;
	CI_SLOT_ID		usSlotId;

	/*
		받은 APDU
		=========
		tune () {
			tune_tag					= CI_TUNE
			length_field()				= 8
			network_id				16 bits
			original_network_id		16 bits
			transport_stream_id		16 bits
			service_id				16 bits
		}

		보낼 APDU (단, CI v1.3 인 경우에 한해서!) -> CiHci_TuneReply() 에서 처리
		=========
		tune_reply () {
			tune_reply_tag			= CI_TUNE_REPLY
			length_field()				= 1
			status_field				8 bits (see CI_TUNE_STATUS)
		}
	*/

	if (pucApduData == NULL || ulApduLen < 8)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTune()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	usNetId = ((unsigned short)pucApduData[0] << 8) & 0xff00;
	usNetId |= ((unsigned short)pucApduData[1] & 0x00ff);
	usOrgNetId = ((unsigned short)pucApduData[2] << 8) & 0xff00;
	usOrgNetId |= ((unsigned short)pucApduData[3] & 0x00ff);
	usTsId = ((unsigned short)pucApduData[4] << 8) & 0xff00;
	usTsId |= ((unsigned short)pucApduData[5] & 0x00ff);
	usSvcId = ((unsigned short)pucApduData[6] << 8) & 0xff00;
	usSvcId |= ((unsigned short)pucApduData[7] & 0x00ff);

	/* find the service to be tuned from DB and change to that service */

	HxLOG_Print("[CIHCI]---> CiHci_ProcTune()---> service tunning to NetId 0x%x, OrgNetId 0x%x, TsId 0x%x, SvcId 0x%x ...\n", usNetId, usOrgNetId, usTsId, usSvcId);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTune()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	HxSTD_memset(&s_stTuneInfo, 0x00, sizeof(SvcCas_CiPlusTuneInfo_t));

#ifdef CONFIG_DEBUG
	HxLOG_Print("[CIHCI]---> CiHci_ProcTune()---> CI+ Version: %d\n", CI_IsCiPlusCam(usSlotId));
#endif

	if (CI_IsCiPlusCam(usSlotId) > 0 // CI+ CAM 이고,
		&& local_CiHci_GetHciResVersion(usSsNo) == CI_HCI_RES_V2_ID)	// 해당 Session이 CI_HCI_RES_V2_ID로 열렸을 경우
	{
		s_stTuneInfo.eTuneMode = eCiPlus_Tune_Start_v2_TuneByTripleId;
	}
	else
	{
		s_stTuneInfo.eTuneMode = eCiPlus_Tune_Start_v1_TuneByTripleId;
	}
	s_stTuneInfo.utInfo.stTripleId.usNetId = usNetId;
	s_stTuneInfo.utInfo.stTripleId.usOnId = usOrgNetId;
	s_stTuneInfo.utInfo.stTripleId.usTsId = usTsId;
	s_stTuneInfo.utInfo.stTripleId.usSvcId = usSvcId;
	Callback_Reset(g_hCbNotifyHostTune);
	while ((s_pfnNotifyHostTune = (CI_NOTIFY_HOST_TUNE_FUNC)Callback_GetNextFunc(g_hCbNotifyHostTune)) != NULL)
	{
		if ((*s_pfnNotifyHostTune)(usSlotId, &s_stTuneInfo) != ERR_OK)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - service tunning failed !!!\n", usSsNo);
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
			if (s_stTuneInfo.eTuneMode == eCiPlus_Tune_Start_v2_TuneByTripleId)
			{
				CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
			}
#endif
		}
	}

	return ERR_OK;
}

static int	CiHci_ProcReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucReplacementRef;
	unsigned short	usReplacedPid;
	unsigned short	usReplacementPid;

	CI_SLOT_ID		usSlotId;
	int				i;

	/*
		받은 APDU
		=========
		tune_broadcast_req() {
			replace_tag				= CI_REPLACE
			length_field()				= 5
			replacement_ref			8 bits
			reserved					3 bits
			replaced_PID				13 bits
			reserved					3 bits
			replacement_PID			13 bits
		}

		보낼 APDU
		=========
		없음
	*/

	if (pucApduData == NULL || ulApduLen < 5)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcReplace()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	ucReplacementRef = pucApduData[0];
	usReplacedPid = ((unsigned short)pucApduData[1] << 8) & 0x1f00;
	usReplacedPid |= ((unsigned short)pucApduData[2] & 0x00ff);
	usReplacementPid = ((unsigned short)pucApduData[3] << 8) & 0x1f00;
	usReplacementPid |= ((unsigned short)pucApduData[4] & 0x00ff);

	// 이전에 받은 replace object 와 같은 것이 있는지 비교
	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ucReplacementRef == ucReplacementRef)
			break;
	}

	// 이전에 받은 것이 없다면
	if (i == MAX_CI_HCI_REPLACE)
	{
		for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
		{
			if (stCiHciReplace[i].ucReplacementRef == 0x00)
				break;
		}
		if (i == MAX_CI_HCI_REPLACE)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcReplace()---> Error : MAX_CI_HCI_REPLACE(%d) is too small !!!\n", MAX_CI_HCI_REPLACE);
			return -1;
		}
	}

	stCiHciReplace[i].ucReplacementRef = ucReplacementRef;
	stCiHciReplace[i].usReplacedPid = usReplacedPid;
	stCiHciReplace[i].usReplacementPid = usReplacementPid;
	stCiHciReplace[i].ulReplacementCount = ++ulReplacementCount;

	/* 현재 demux에 걸려있는 component 중에 usReplacedPid 인 것을 찾아 usReplacementPid 로 다시 걸어라 */

	HxLOG_Print("[CIHCI]---> CiHci_ProcReplace()---> PID 0x%x replacing to 0x%x ...\n", usReplacedPid, usReplacementPid);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcReplace()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_stPidInfo.eReplaceMode = eCiPlus_Replace_Start;
	s_stPidInfo.usOriginalPid = usReplacedPid;
	s_stPidInfo.usTargetPId = usReplacementPid;
	Callback_Reset(g_hCbNotifyPidReplace);
	while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
	{
		if ((*s_pfnNotifyPidReplace)(usSlotId, &s_stPidInfo) != ERR_OK)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - PID replacing failed !!!\n", usSsNo);
		}
	}

	return ERR_OK;
}

static int	CiHci_ProcClearReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucReplacementRef;
	CI_SLOT_ID		usSlotId;
	int				i;

	/*
		받은 APDU
		=========
		tune_broadcast_req() {
			clear_replace_tag			= CI_CLEAR_REPLACE
			length_field()				= 1
			replacement_ref			8 bits
		}

		보낼 APDU
		=========
		없음
	*/

	if (pucApduData == NULL || ulApduLen < 1)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcClearReplace()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	ucReplacementRef = pucApduData[0];

	// 이전에 받은 replace object 와 같은 것이 있는지 비교
	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ucReplacementRef == ucReplacementRef)
			break;
	}

	// 이전에 받은 것이 없다면 -> 뭘 어떡 하라구!
	if (i == MAX_CI_HCI_REPLACE)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcClearReplace()---> Error : ucReplacementRef(0x%02X) not received !!!\n", ucReplacementRef);
		return -1;
	}

	/* 현재 demux에 걸려있는 component 중에 stCiHciReplace[i].usReplacementPid 인 것을 찾아 stCiHciReplace[i].usReplacedPid 로 다시 걸어라 */

	HxLOG_Print("[CIHCI]---> CiHci_ProcClearReplace()---> PID 0x%x returning to 0x%x ...\n", stCiHciReplace[i].usReplacementPid, stCiHciReplace[i].usReplacedPid);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcClearReplace()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_stPidInfo.eReplaceMode = eCiPlus_Replace_Cancel;
	s_stPidInfo.usOriginalPid = stCiHciReplace[i].usReplacedPid;
	s_stPidInfo.usTargetPId = stCiHciReplace[i].usReplacementPid;
	Callback_Reset(g_hCbNotifyPidReplace);
	while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
	{
		if ((*s_pfnNotifyPidReplace)(usSlotId, &s_stPidInfo) != ERR_OK)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - PID returning failed !!!\n", usSsNo);
		}
	}

	HxSTD_memset(&stCiHciReplace[i], 0x00, sizeof(CI_HCI_REPLACE));

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
static int	CiHci_ProcTuneBroadcastReq(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) // CI+ v1.3
{
	HBOOL			bPmtFlag;
	unsigned short	usServiceId;
	unsigned short	usDescriptorsLen = 0, usPmtLen = 0;
	unsigned char		*pucDescriptors = NULL, *pucPmt = NULL;
	CI_SLOT_ID		usSlotId;

#ifdef CONFIG_DEBUG
	int i;
	HxLOG_Print("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> received APDU data: [ ");
	for ( i=0; i<ulApduLen; i++)
	{
		HxLOG_Print("%02x ", pucApduData[i]);
	}
	HxLOG_Print("]\n");
#endif

	/*
		받은 APDU
		=========
		tune_broadcast_req() {
			tune_broadcast_req_tag		= CI_TUNE_BROADCAST_REQ
			length_field()
			reserved 					7 bits
			pmt_flag					1 bit
			service_id				16 bits
			reserved					4 bits
			descriptor loop_length		12 bits
			for (i=0; i<N; i++) {
				descriptor()
			}
			if (pmt_flag==1) {
				program_map_section()
			}
		}

		보낼 APDU -> CiHci_TuneReply() 에서 처리 (단, 이 함수 내에서의 에러는 CiHci_TuneReplyBySsNo() 에서 처리함)
		=========
		tune_reply () {
			tune_reply_tag			= CI_TUNE_REPLY
			length_field()				= 1
			status_field				8 bits (see CI_TUNE_STATUS)
		}
	*/

	/*
		ulApduLen = 5 + usDescriptorsLen + usPmtLen
		usDescriptorsLen = descriptor loop_length
		usPmtLen = 3 + pmt section_length
		pmt section_length = ((((unsigned short)pucPmt[1] << 8) & 0x0f00) | ((unsigned short)pucPmt[2] & 0x00ff))
	*/

	if (pucApduData == NULL || ulApduLen < 5)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
		goto ERROR;
	}

	bPmtFlag = (HBOOL)(pucApduData[0] & 0x01);
	usServiceId = ((unsigned short)pucApduData[1] << 8) & 0xff00;
	usServiceId |= ((unsigned short)pucApduData[2] & 0x00ff);
	usDescriptorsLen = ((unsigned short)pucApduData[3] << 8) & 0x0f00;
	usDescriptorsLen |= ((unsigned short)pucApduData[4] & 0x00ff);

	if (bPmtFlag == FALSE && usServiceId != 0) // 이 경우 호스트가 해당 TP 의 PMT 를 직접 처리하여 usServiceId 와 매칭되는 서비스를 찾아야 한다.
	{
		HxLOG_Print("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> bPmtFlag is false but usServiceId isn't 0 (%d)\n", usServiceId);
	}

	if (usServiceId == 0 && bPmtFlag != FALSE) // 스펙대로라면 에러 상황이지만 tuning 을 못하는 건 아니므로 경고만 해주고 넘어가자... (여기서 받은 PMT 는 무시하고 delivery_system_descriptor 참고하여 해당 TP 로 tuning 한다)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : usServiceId is 0 but bPmtFlag is not false !!!\n");
	}

	if (usDescriptorsLen == 0) // 이건 에러 처리하는 것이 맞겠지...
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Wrong usDescriptorsLen[%d] !!!\n", usDescriptorsLen);
		CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
		goto ERROR;
	}

	if (bPmtFlag)
	{
		if (ulApduLen < 5 + usDescriptorsLen + 16) // 16 is the minimum size of PMT except loops
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Wrong ulApduLen[%ld] (with PMT) !!!\n", ulApduLen);
			CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
			goto ERROR;
		}

		usPmtLen = 3 + ((((unsigned short)pucApduData[5+usDescriptorsLen+1] << 8) & 0x0f00) | ((unsigned short)pucApduData[5+usDescriptorsLen+2] & 0x00ff));
	}

	if (ulApduLen != 5 + usDescriptorsLen + usPmtLen)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Wrong ulApduLen[%ld], usDescriptorsLen[%d], usPmtLen[%d] !!!\n", ulApduLen, usDescriptorsLen, usPmtLen);
		CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
		goto ERROR;
	}

	pucDescriptors = (unsigned char *)OxCAS_Malloc(usDescriptorsLen);
	if (pucDescriptors == NULL)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Can't alloc memory for pucDescriptors !!!\n");
		CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Undefined);
		goto ERROR;
	}

	HxSTD_memcpy(pucDescriptors, &pucApduData[5], usDescriptorsLen);

	if (usPmtLen)
	{
		pucPmt = (unsigned char *)OxCAS_Malloc(usPmtLen);
		if (pucPmt == NULL)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : Can't alloc memory for pucPmt !!!\n");
			CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Undefined);
			goto ERROR;
		}

		HxSTD_memcpy(pucPmt, &pucApduData[5+usDescriptorsLen], usPmtLen);
	}

	/* find the service to be tuned from DB and change to that service */

	HxLOG_Print("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> service tunning to SvcId 0x%x %s ...\n", usServiceId, bPmtFlag ? "with PMT" : "w/o PMT");

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Undefined);
		goto ERROR;
	}

	HxSTD_memset(&s_stTuneInfo, 0x00, sizeof(SvcCas_CiPlusTuneInfo_t));
	s_stTuneInfo.eTuneMode = eCiPlus_Tune_Start_v2_TuneByLocation;
	s_stTuneInfo.utInfo.stBroadcastReq.bPmtFlag = bPmtFlag;
	s_stTuneInfo.utInfo.stBroadcastReq.usServiceId = usServiceId;
	s_stTuneInfo.utInfo.stBroadcastReq.usDescriptorsLen = usDescriptorsLen;
	s_stTuneInfo.utInfo.stBroadcastReq.pucDescriptors = pucDescriptors;
	s_stTuneInfo.utInfo.stBroadcastReq.pucPmt = pucPmt;
	Callback_Reset(g_hCbNotifyHostTune);
	while ((s_pfnNotifyHostTune = (CI_NOTIFY_HOST_TUNE_FUNC)Callback_GetNextFunc(g_hCbNotifyHostTune)) != NULL)
	{
		if ((*s_pfnNotifyHostTune)(usSlotId, &s_stTuneInfo) != ERR_OK)
		{
			HxLOG_Error("[CIHCI]---> CiHci_ProcTuneBroadcastReq()---> Error : SsNo[%d] - service tunning failed !!!\n", usSsNo);
			CiHci_TuneReplyBySsNo(usSsNo, CI_TUNE_STATUS_ERR_Parameters);
			goto ERROR;
		}
	}

	return ERR_OK;

ERROR :

	if (pucDescriptors)
		OxCAS_Free(pucDescriptors);

	if (pucPmt)
		OxCAS_Free(pucPmt);

	return -1;
}

static int	CiHci_ProcAskReleaseReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	/*
		받은 APDU
		=========
		ask_release_reply() {
			ask_release_query_tag		= CI_ASK_RELEASE_REPLY
			length_field()				= 1
			release_rep				8 bits (see CI_TUNE_RELEASE)
		}

		보낼 APDU
		=========
		없음
	*/

	if (pucApduData == NULL || ulApduLen < 1)
	{
		HxLOG_Error("[CIHCI]---> CiHci_ProcAskReleaseReply()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	HxLOG_Print("[CIHCI]---> CiHci_ProcAskReleaseReply()---> TODO\n");

	return ERR_OK;
}

static int	CiHci_TuneReplyBySsNo(unsigned short usSsNo, CI_TUNE_STATUS eTuneResult) // CI+ v1.3
{
	unsigned char ucMsg[5];

	/*
		보낼 APDU
		=========
		tune_reply () {
			tune_reply_tag			= CI_TUNE_REPLY
			length_field()				= 1
			status_field				8 bits (see CI_TUNE_STATUS)
		}
	*/

	ucMsg[0] = (unsigned char)((CI_TUNE_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_TUNE_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_TUNE_REPLY & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = (unsigned char)eTuneResult;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CIHCI]---> CiHci_TuneReplyBySsNo()---> Error : fail to send CI_TUNE_REPLY for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CIHCI]---> CiHci_TuneReplyBySsNo()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiHci_TuneReply(CI_SLOT_ID usSlotId, CI_TUNE_STATUS eTuneResult) // CI+ v1.3
{
	unsigned char ucMsg[5];

	/*
		보낼 APDU (단, CI v1.3 인 경우에 한해서!)
		=========
		tune_reply () {
			tune_reply_tag			= CI_TUNE_REPLY
			length_field()				= 1
			status_field				8 bits (see CI_TUNE_STATUS)
		}
	*/

	HxLOG_Print("[CIHCI]---> CiHci_TuneReply()---> usSlotId[%d], eTuneResult[%d], Session Num[%d]...\n", usSlotId, eTuneResult, s_usCiHciSsNo[usSlotId]);

	if (usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("[CIHCI]---> CiHci_TuneReply()---> Error : Invalid usSlotId(%d) !!!\n", usSlotId);
		return -1;
	}

	if (local_CiHci_GetHciResVersion(s_usCiHciSsNo[usSlotId]) == CI_HCI_RES_V2_ID)	// 해당 Session이 CI_HCI_RES_V2_ID로 열렸을 경우
	{
		ucMsg[0] = (unsigned char)((CI_TUNE_REPLY >> 16) & 0xff);
		ucMsg[1] = (unsigned char)((CI_TUNE_REPLY >> 8) & 0xff);
		ucMsg[2] = (unsigned char)(CI_TUNE_REPLY & 0xff);
		ucMsg[3] = 0x01;
		ucMsg[4] = (unsigned char)eTuneResult;

		if (CIRH_SendApdu(s_usCiHciSsNo[usSlotId], ucMsg, 5) != ERR_OK)
		{
			HxLOG_Error("[CIHCI]---> CiHci_TuneReply()---> Error : fail to send CI_TUNE_REPLY for usSlotId[%d], SsNo[%d] !!!\n", usSlotId, s_usCiHciSsNo[usSlotId]);
			return -1;
		}
	}

	return ERR_OK;
}
#endif

