/***************************************************************
* $Workfile:   ci_capvr.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : CA PVR (CI+)
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <cb_list.h>
#include "ci_plus.h"

/***************************************************************
* local definition
***************************************************************/




/***************************************************************
* typedef
***************************************************************/
typedef struct {
	unsigned short     * pusCaSysId;
	CI_CA_PVR_REC_MODE * pucCaRecMode;
	unsigned short       usCaPvrInfoCnt;
} CI_CA_PVR_INST;

/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyCaPvrInfo;
extern CALLBACK_HANDLE	g_hCbNotifyCasState;

/***************************************************************
* static variables and structures
***************************************************************/
static CI_CA_PVR_INST	s_aCiCaPvr[CARM_MAX_CISLOT_NUMBER];
static unsigned short	s_usCiCaPvrSsNo[CARM_MAX_CISLOT_NUMBER];

/***************************************************************
* static function prototypes
***************************************************************/
static int CiCaPvr_AddCaPvrInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCaPvr_DeleteCaPvrInst(unsigned short usSsNo);

static int CiCaPvr_ProcCaPvrPmtReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCaPvr_ProcCaPvrCatReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCaPvr_ProcCaPvrEmmReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCaPvr_ProcCaPvrEcmReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCaPvr_ProcCaPvrPincodeReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

/***************************************************************
* function bodies
***************************************************************/
int CiCaPvr_Init(void)
{
	HxSTD_memset(s_aCiCaPvr, 0, sizeof(CI_CA_PVR_INST) * CARM_MAX_CISLOT_NUMBER);
	HxSTD_memset(s_usCiCaPvrSsNo, 0, sizeof(unsigned short) * CARM_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

int CiCaPvr_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	unsigned char ucMsg[4];

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiCaPvrSsNo[usSlotId] = usSsNo;

	ucMsg[0] = (unsigned char)((CI_CA_PVR_INFO_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CA_PVR_INFO_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CA_PVR_INFO_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_SsOpenedProc()---> Error : fail to send CI_CA_PVR_INFO_ENQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print(("[CIPVR]---> CiCaPvr_SsOpenedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK; // 캠으로부터 CI_CA_PVR_INFO_REPLY 를 받게 될 것이다.
}

int CiCaPvr_SsClosedProc(unsigned short usSsNo)
{
	if (CiCaPvr_DeleteCaPvrInst(usSsNo) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_SsClosedProc()---> Error : delete CaPvr info. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print(("[CIPVR]---> CiCaPvr_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiCaPvr_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_CA_PVR_INFO_REPLY:
		HxLOG_Print("CI_CA_PVR_INFO_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_AddCaPvrInst(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PVR_PMT_REPLY:
		HxLOG_Print("CI_CA_PVR_PMT_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_ProcCaPvrPmtReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PVR_CAT_REPLY:
		HxLOG_Print("CI_CA_PVR_CAT_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_ProcCaPvrCatReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PVR_EMM_CMD_REPLY:
		HxLOG_Print("CI_CA_PVR_EMM_CMD_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_ProcCaPvrEmmReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PVR_ECM_CMD_REPLY:
		HxLOG_Print("CI_CA_PVR_ECM_CMD_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_ProcCaPvrEcmReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CA_PVR_PINCODE_CMD_REPLY:
		HxLOG_Print("CI_CA_PVR_PINCODE_CMD_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiCaPvr_ProcCaPvrPincodeReply(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CIPVR]---> CiCaPvr_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print(("[CIPVR]---> CiCaPvr_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode));

	return nRtnCode;
}

static int CiCaPvr_ProcCaPvrPmtReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) /* <TODO_CI_PLUS> : 향후를 위해 구현 필요 (Irdeto 에서 지원하기로 했다고 들었음) */
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	/*  CI+ 스펙 1.2 에 명시된 ca_pvr_pmt_reply() 정의에는 braket 오류가 있다. 이는 다음 스펙 개정시 수정될 것으로 생각된다. 일단 아래 내용 참고할 것.
		ca_pvr_pmt_reply() {
			ca_pvr_pmt_reply_tag					= CI_CA_PVR_PMT_REPLY
			length_field()
			program_number							16 bits
			reserved								2 bits
			version_number							5 bits
			current_next_indicator					1 bit
			CA_enable_flag							1 bit
			if (CA_enable_flag == 1) {
				CA_enable							7 bits 	: at programme level
				Number_of_PID_to_record				8 bits
				for (i=0; i<n; i++) {						: list of elementary stream PID to record
					reserved						3 bits
					elementary_PID					13 bits
					reserved						3 bits
					ECM_PID							13 bits	: associated ECM PID of the selected elementary PID
				}
			} else if (CA_enable_flag == 0) {
				Reserved							7 bits
			}
			for (i=0; i<n; i++) {
				reserved							3 bits
				elementary_PID						13 bits	: elementary stream PID to record
				CA_enable_flag						1 bit
				if (CA_enable_flag == 1) {
					CA_enable						7 bits	: at elementary stream level
					reserved						3 bits
					ECM_PID							13 bits	: ECM PID of the selected elementary PID
				} else if (CA_enable_flag == 0) {
					reserved						7 bits
				}
			}
		}
	*/

	// TODO ...

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_ProcCaPvrPmtReply()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PVR_PMT_REPLIED);
	}

	return ERR_OK;
}

static int CiCaPvr_ProcCaPvrCatReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) /* <TODO_CI_PLUS> : 향후를 위해 구현 필요 (Irdeto 에서 지원하기로 했다고 들었음) */
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	/*
		ca_pvr_cat_reply() {
			ca_pvr_cat_reply_tag							= CI_CA_PVR_CAT_REPLY
			length_field()
			CA_enable_flag									1 bit
			if (CA_enable_flag == 1) {
				CA_enable									7 bits	: at programme level
				Number_of_EMM_section_to_be_processed		8 bits
				for (i=0; i<n; i++) {								: list of EMM section to process
					reserved								3 bits
					elementary_PID							13 bits
					Table_Id								8 bits
					Extra_filtering parameters_match[16]	8 x 16 bits
					Extra_filtering parameters_mask[16]		8 x 16 bits
				}
			} else if (CA_enable_flag == 0) {
				reserved									7 bits
			}
		}
	*/

	// TODO ...

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_ProcCaPvrCatReply()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PVR_CAT_REPLIED);
	}

	return ERR_OK;
}

static int CiCaPvr_ProcCaPvrEmmReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) /* <TODO_CI_PLUS> : 향후를 위해 구현 필요 (Irdeto 에서 지원하기로 했다고 들었음) */
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	/*
		ca_pvr_emm_cmd_reply() {
			ca_pvr_emm_cmd_reply_tag	= CI_CA_PVR_EMM_CMD_REPLY
			length_field()
			Status_field				8 bits
		}
	*/

	// TODO ...

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_ProcCaPvrEmmReply()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PVR_EMM_REPLIED);
	}

	return ERR_OK;
}

static int CiCaPvr_ProcCaPvrEcmReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) /* <TODO_CI_PLUS> : 향후를 위해 구현 필요 (Irdeto 에서 지원하기로 했다고 들었음) */
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	/*
		ca_pvr_ecm_cmd_reply() {
			ca_pvr_ecm_cmd_reply_tag	= CI_CA_PVR_ECM_CMD_REPLY
			length_field()
			ECM_counter_index_reply		24 bits
			Status_field				8 bits
			if (Status_field == 0) {
				for (i=0; i<n; i++) {
					ECM_Section_reply	8 bits
				}
			}
		}
	*/

	// TODO ...

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_ProcCaPvrEcmReply()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PVR_ECM_REPLIED);
	}

	return ERR_OK;
}

static int CiCaPvr_ProcCaPvrPincodeReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen) /* <TODO_CI_PLUS> : 향후를 위해 구현 필요 (Irdeto 에서 지원하기로 했다고 들었음) */
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CAS_STATE_FUNC pfnNotifyCasState;

	/*
		ca_pvr_PINcode_cmd_reply() {
			ca_pvr_PINcode_cmd_reply_tag	= CI_CA_PVR_PINCODE_CMD_REPLY
			length_field()
			PINcode_status_field			8 bits
		}
	*/

	// TODO ...

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_ProcCaPvrPincodeReply()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	Callback_Reset(g_hCbNotifyCasState);
	while ((pfnNotifyCasState = (CI_NOTIFY_CAS_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCasState)) != NULL)
	{
		(*pfnNotifyCasState)(usSlotId, CI_CAS_STATE_CA_PVR_PINCODE_REPLIED);
	}

	return ERR_OK;
}

int CiCaPvr_SendCaPvrCmd(unsigned short usSsNo, unsigned char *pucCaPvrCmd, unsigned short usCaPvrCmdLen)
{
	/*
		ca_pvr_pmt() {
			ca_pvr_pmt_tag						= CI_CA_PVR_PMT
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

		ca_pvr_cat() {
			ca_pvr_cat_tag						= CI_CA_PVR_CAT
			length_field()
			for (i=0; i<n; i++) {
				ca_pvr_cat_data 				8 bits
			}
		}

		ca_pvr_emm_cmd() {
			ca_pvr_emm_cmd_tag					= CI_CA_PVR_EMM_CMD
			length_field()
			for (i=0; i<n; i++) {
				EMM_Section_filtering 			8 bits
			}
		}

		ca_pvr_ecm_cmd() {
			ca_pvr_ecm_cmd_tag					= CI_CA_PVR_ECM_CMD
			length_field()
			ECM_counter_index					24 bits
			for (i=0; i<n; i++) {
				ECM_Section_filtering 			8 bits
			}
		}

		ca_pvr_PINcode_cmd() {
			ca_pvr_PINcode_cmd_tag				= CI_CA_PVR_PINCODE_CMD
			length_field()
			for (i=0; i<n; i++) {
				PINcode_data_byte 				8 bits
			}
		}
	*/

	if (pucCaPvrCmd == NULL || usCaPvrCmdLen == 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_SendCaPvrCmd()---> Error : No data. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	if (CIRH_SendApdu(usSsNo, pucCaPvrCmd, usCaPvrCmdLen) != ERR_OK)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_SendCaPvrCmd()---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

unsigned short CiCaPvr_GetSsNo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
		return 0;

	return s_usCiCaPvrSsNo[usSlotId];
}

static int CiCaPvr_AddCaPvrInst(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SLOT_ID					usSlotId;
	unsigned short				*pusCaSysId = NULL;
	unsigned char 				*pucCaRecMode = NULL; // CI_CA_PVR_REC_MODE *
	unsigned short				usCaPvrInfoCnt = 0;
	CI_NOTIFY_CA_PVR_INFO_FUNC	pfnNotifyCaPvrInfo;
	int i, j;

#if 0
	if (pucApduData == NULL || ulApduLen == 0 || ulApduLen % 3 != 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_AddCaPvrInst()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}
#endif

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_AddCaPvrInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	/******************************************
	  setup CA PVR instance
	 ******************************************/

	if(s_aCiCaPvr[usSlotId].pusCaSysId != NULL) // 이런 경우는 없을 것이지만 확실을 기하기 위해서...
	{
		OxCAS_Free(s_aCiCaPvr[usSlotId].pusCaSysId);
	}

	if(s_aCiCaPvr[usSlotId].pucCaRecMode != NULL) // 이런 경우는 없을 것이지만 확실을 기하기 위해서...
	{
		OxCAS_Free(s_aCiCaPvr[usSlotId].pucCaRecMode);
	}

	pusCaSysId = (unsigned short *)OxCAS_Malloc((ulApduLen / 3) * 2); // 따로 체크는 하지 않지만 ulApduLen % 3 == 0 이다
	if(pusCaSysId == NULL)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_AddCaPvrInst()---> Error : Can't alloc memory for %d !!!\n", usSsNo);
		return -1;
	}
	pucCaRecMode = (unsigned char *)OxCAS_Malloc(ulApduLen / 3); // 따로 체크는 하지 않지만 ulApduLen % 3 == 0 이다
	if(pucCaRecMode == NULL)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_AddCaPvrInst()---> Error : Can't alloc memory for %d !!!\n", usSsNo);

		if (pusCaSysId != NULL)
			OxCAS_Free(pusCaSysId);

		return -1;
	}

	for (i = 0, j = 0; i < ulApduLen; i += 3, j++)
	{
		pusCaSysId[j] = ((unsigned short)pucApduData[i] << 8) & 0xff00;
		pusCaSysId[j] |= ((unsigned short)pucApduData[i+1] & 0x00ff);
		pucCaRecMode[j] = pucApduData[i+2];
	}

	usCaPvrInfoCnt = j;	// = (unsigned short)(ulApduLen / 3)	// = (unsigned short)(ulApduLen / (sizeof(unsigned short) + sizeof(unsigned char)));
	s_aCiCaPvr[usSlotId].pusCaSysId = pusCaSysId;
	s_aCiCaPvr[usSlotId].pucCaRecMode = (CI_CA_PVR_REC_MODE *)pucCaRecMode;
	s_aCiCaPvr[usSlotId].usCaPvrInfoCnt = usCaPvrInfoCnt;

	/******************************************
	  for debugging
	 ******************************************/

#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
{
	HxLOG_Print("\n --- # of CA_system_id = %d ---\n\n", usCaPvrInfoCnt);
	for (i = 0; i < usCaPvrInfoCnt; i++)
	{
		HxLOG_Print(" %04X(%02X)", pusCaSysId[i], pucCaRecMode[i]);
	}
	HxLOG_Print("\n\n");
}
#endif

	/******************************************
	  notify CA PVR info
	 ******************************************/

	Callback_Reset(g_hCbNotifyCaPvrInfo);
	while ((pfnNotifyCaPvrInfo = (CI_NOTIFY_CA_PVR_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyCaPvrInfo)) != NULL)
	{
		(*pfnNotifyCaPvrInfo)(usSlotId, usCaPvrInfoCnt, pusCaSysId, (CI_CA_PVR_REC_MODE *)pucCaRecMode);
	}

	return ERR_OK;
}

static int CiCaPvr_DeleteCaPvrInst(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	CI_NOTIFY_CA_PVR_INFO_FUNC pfnNotifyCaPvrInfo;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIPVR]---> CiCaPvr_DeleteCaPvrInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	/******************************************
	  release CA PVR instance
	 ******************************************/

	if(s_aCiCaPvr[usSlotId].pusCaSysId != NULL)
	{
		OxCAS_Free(s_aCiCaPvr[usSlotId].pusCaSysId);
//		s_aCiCaPvr[usSlotId].pusCaSysId = NULL;
	}

	if(s_aCiCaPvr[usSlotId].pucCaRecMode != NULL)
	{
		OxCAS_Free(s_aCiCaPvr[usSlotId].pucCaRecMode);
//		s_aCiCaPvr[usSlotId].pucCaRecMode = NULL;
	}

	HxSTD_memset(&s_aCiCaPvr[usSlotId], 0, sizeof(CI_CA_PVR_INST));
	s_usCiCaPvrSsNo[usSlotId] = 0;

	/******************************************
	  notify CA PVR info
	 ******************************************/

	Callback_Reset(g_hCbNotifyCaPvrInfo);
	while ((pfnNotifyCaPvrInfo = (CI_NOTIFY_CA_PVR_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyCaPvrInfo)) != NULL)
	{
		(*pfnNotifyCaPvrInfo)(usSlotId, 0, (unsigned short *)NULL, (CI_CA_PVR_REC_MODE *)NULL);
	}

	return ERR_OK;
}

