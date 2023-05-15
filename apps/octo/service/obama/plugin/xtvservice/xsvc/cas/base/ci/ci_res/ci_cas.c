/* $Header:   //Mars/source/pvcs/VM/BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_cas.c-arc   1.4   Aug 21 2004 08:31:50   wskim  $ */
/***************************************************************
* $Workfile:   ci_cas.c  $
* $Modtime:   Aug 21 2004 07:31:20  $
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
#include <cb_list.h>

#include "ci.h"
#include "ci_reshandler.h"

#include <_svc_cas_sub_dev_api.h>


/***************************************************************
* local definition
***************************************************************/




/***************************************************************
* typedef
***************************************************************/
typedef struct {
	unsigned short * pusCaSysId;
	unsigned short   usCaInfoCnt;
} CI_CAS_INST;

/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyCaInfo;
#ifdef CI_PLUS_PVR_ENABLE // 당장은 CA PVR 지원을 위해 추가한 것이지만 Multiple CA application selection procedure 구현 같은 다른 용도로 확장해서 쓸 여지가 있으므로 일반적으로 정의해 두는 것이 어떨지... 좀더 생각해 보자...
extern CALLBACK_HANDLE	g_hCbNotifyCasState;
#endif

/***************************************************************
* static variables and structures
***************************************************************/
static CI_CAS_INST		s_aCiCas[CARM_MAX_CISLOT_NUMBER];
static unsigned short	s_usCiCasSsNo[CARM_MAX_CISLOT_NUMBER];
//static unsigned char	s_ucCiCasCnt;

/***************************************************************
* static function prototypes
***************************************************************/
static int CiCas_AddCaInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCas_DeleteCaInst(unsigned short usSsNo);

static int CiCas_ProcCaPmtReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

/***************************************************************
* function bodies
***************************************************************/
int CiCas_Init(void)
{
	HxSTD_memset(s_aCiCas, 0, sizeof(CI_CAS_INST) * CARM_MAX_CISLOT_NUMBER);
	HxSTD_memset(s_usCiCasSsNo, 0, sizeof(unsigned short) * CARM_MAX_CISLOT_NUMBER);

//	s_ucCiCasCnt = 0;

	return ERR_OK;
}

int CiCas_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	unsigned char ucMsg[4];

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CICA]---> CiCas_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiCasSsNo[usSlotId] = usSsNo;

	ucMsg[0] = (unsigned char)((CI_CA_INFO_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CA_INFO_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CA_INFO_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CICA]---> CiCas_SsOpenedProc()---> Error : fail to send CI_CA_INFO_ENQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CICA]---> CiCas_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK; // 캠으로부터 CI_CA_INFO_REPLY 를 받게 될 것이다.
}

int CiCas_SsClosedProc(unsigned short usSsNo)
{
	if (CiCas_DeleteCaInst(usSsNo) < 0)
	{
		HxLOG_Error("[CICA]---> CiCas_SsClosedProc()---> Error : delete Ca info. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CICA]---> CiCas_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiCas_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_CA_INFO_REPLY:
		#if 0 /* #ifdef CI_WORKAROUND_FOR_DOUGLAS */ /* CI_APPL_INFO_REPLY 시점으로 당김. CI+ 캠의 경우 이것이 한참 후에 올라오기 때문임. 주의할 것! */
		{
			extern void CiAdapt_DouglasWorkAroundTimerCallbackCancel(void);
			CiAdapt_DouglasWorkAroundTimerCallbackCancel();
		}
		#endif
		nRtnCode = CiCas_AddCaInst(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PMT_REPLY:
		HxLOG_Print("CI_CA_PMT_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCas_ProcCaPmtReply(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CICA]---> CiCas_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print("[CICA]---> CiCas_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

static int CiCas_ProcCaPmtReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) // CI+ 에선 CAM mandatory 사항이다 (CI_CA_PMT with "query" or "ok_mmi" 에 대한 응답으로). 하나 현재 CI mgr 에서 CI_CPCI_QUERY 를 지원하지 않고 있다.
{
#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
	if ((pucApduData != NULL) && (ulApduLen != 0))
	{
		int i;
		HxLOG_Print("\n=== [CICA] : CI_CA_PMT_REPLY (len=%ld) ===\n", ulApduLen);
		for (i = 0; i < ulApduLen; i++)
		{
			HxLOG_Print(" %02X", pucApduData[i]);
		}
		HxLOG_Print("\n=========================\n\n");
	}
#endif

	/*
		ca_pmt_reply() {
			ca_pmt_reply_tag						= CI_CA_PMT_REPLY
			length_field()
			program_number							16 bits
			reserved								2 bits
			version_number							5 bits
			current_next_indicator					1 bit
			CA_enable_flag							1 bit
			if (CA_enable_flag == 1) {
				CA_enable							7 bits 	: at programme level
			} else if (CA_enable_flag == 0) {
				Reserved							7 bits
			}
			for (i=0; i<n; i++) {
				reserved							3 bits
				elementary_PID						13 bits	: elementary stream PID
				CA_enable_flag						1 bit
				if (CA_enable_flag == 1) {
					CA_enable						7 bits	: at elementary stream level
				} else if (CA_enable_flag == 0) {
					reserved						7 bits
				}
			}
		}
	*/

#ifdef CI_PLUS_PVR_ENABLE // 당장은 CA PVR 지원을 위해 추가한 것이지만 Multiple CA application selection procedure 구현 같은 다른 용도로 확장해서 쓸 여지가 있으므로 일반적으로 정의해 두는 것이 어떨지... 좀더 생각해 보자...
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CICA]---> CiCas_ApduProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PMT_REPLIED);
	}
}
#endif

	return ERR_OK;
}

int CiCas_SendCaPmt(unsigned short usSsNo, unsigned char *pucCaPmt, unsigned short usCaPmtLen)
{
	/*
		ca_pmt() {
			ca_pmt_tag							= CI_CA_PMT
			length_field()
			ca_pmt_list_management				8 bits
			program_number						16 bits
			reserved							2 bits
			version_number						5 bits
			current_next_indicator				1 bit
			reserved							4 bits
			program_info_length					12 bits
			if (program_info_length != 0) {
				ca_pmt_cmd_id					8 bits	: at program level
				for (i=0; i<n; i++) {
					CA_descriptor()						: CA descriptor at programme level
				}
			}
			for (i=0; i<n; i++) {
				stream_type						8 bits
				reserved						3 bits
				elementary_PID					13 bits	: elementary stream PID
				reserved						4 bits
				ES_info_length					12 bits
				if (ES_info_length != 0) {
					ca_pmt_cmd_id				8 bits	: at ES level
					for (i=0; i<n; i++) {
						CA_descriptor()					: CA descriptor at elementary stream level
					}
				}
			}
		}
	*/

	if (pucCaPmt == NULL || usCaPmtLen == 0)
	{
		HxLOG_Error("[CICA]---> CiCas_SendCaPmt()---> Error : No PMT data. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	if (CIRH_SendApdu(usSsNo, pucCaPmt, usCaPmtLen) != ERR_OK)
	{
		HxLOG_Error("[CICA]---> CiCas_SendCaPmt()---> Error : fail to send CI_CA_PMT for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

unsigned short CiCas_GetSsNo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
		return 0;

	return s_usCiCasSsNo[usSlotId];
}

static int CiCas_AddCaInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SLOT_ID				usSlotId;
	unsigned short			*pusCaSysId = NULL;
	unsigned short			usCaInfoCnt = 0;
	CI_NOTIFY_CA_INFO_FUNC	pfnNotifyCaInfo;
	int i;

#if 0
	if (pucApduData == NULL || ulApduLen == 0 || ulApduLen % 2 != 0)
	{
		HxLOG_Error("[CICA]---> CiCas_AddCaInst()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}
#endif

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CICA]---> CiCas_AddCaInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	/******************************************
	  setup CA instance
	 ******************************************/

	if(s_aCiCas[usSlotId].pusCaSysId != NULL) // 이런 경우는 없을 것이지만 확실을 기하기 위해서...
	{
		OxCAS_Free(s_aCiCas[usSlotId].pusCaSysId);
	}

	pusCaSysId = (unsigned short *)OxCAS_Malloc(ulApduLen);
	if(pusCaSysId == NULL)
	{
		HxLOG_Error("[CICA]---> CiCas_AddCaInst()---> Error : Can't alloc memory for %d !!!\n", usSsNo);
		return -1;
	}

	for (i = 0; i < ulApduLen; i += 2)
	{
		pusCaSysId[i/2] = ((unsigned short)pucApduData[i] << 8) & 0xff00;
		pusCaSysId[i/2] |= ((unsigned short)pucApduData[i+1] & 0x00ff);
	}

	usCaInfoCnt = (unsigned short)(ulApduLen / sizeof(unsigned short));
	s_aCiCas[usSlotId].pusCaSysId = pusCaSysId;
	s_aCiCas[usSlotId].usCaInfoCnt = usCaInfoCnt;

//	s_ucCiCasCnt++;

	/******************************************
	  for debugging
	 ******************************************/

#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
{
	HxLOG_Print("\n");
	HxLOG_Print(" --- # of CA_system_id = %d ---\n\n", usCaInfoCnt);
	for (i = 0; i < usCaInfoCnt; i++)
	{
		HxLOG_Print(" %04X", pusCaSysId[i]);
	}
	HxLOG_Print("\n\n");
}
#endif

	/******************************************
	  notify CA info
	 ******************************************/

	Callback_Reset(g_hCbNotifyCaInfo);
	while ((pfnNotifyCaInfo = (CI_NOTIFY_CA_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyCaInfo)) != NULL)
	{
		(*pfnNotifyCaInfo)(usSlotId, usCaInfoCnt, pusCaSysId);
	}

	return ERR_OK;
}

static int CiCas_DeleteCaInst(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CA_INFO_FUNC pfnNotifyCaInfo;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CICA]---> CiCas_DeleteCaInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	/******************************************
	  release CA instance
	 ******************************************/

	if(s_aCiCas[usSlotId].pusCaSysId != NULL)
	{
		OxCAS_Free(s_aCiCas[usSlotId].pusCaSysId);
//		s_aCiCas[usSlotId].pusCaSysId = NULL;
	}

	HxSTD_memset(&s_aCiCas[usSlotId], 0, sizeof(CI_CAS_INST));
	s_usCiCasSsNo[usSlotId] = 0;

//	s_ucCiCasCnt--;

	/******************************************
	  notify CA info
	 ******************************************/

	Callback_Reset(g_hCbNotifyCaInfo);
	while ((pfnNotifyCaInfo = (CI_NOTIFY_CA_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyCaInfo)) != NULL)
	{
		(*pfnNotifyCaInfo)(usSlotId, 0, (unsigned short *)NULL);
	}

	return ERR_OK;
}

