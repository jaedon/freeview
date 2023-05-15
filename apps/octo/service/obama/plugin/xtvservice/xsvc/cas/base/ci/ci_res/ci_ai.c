/***************************************************************
* $Workfile:   ci_ai.c  $
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
#include <cb_list.h>

#include "ci.h"
#include "ci_reshandler.h"

#ifdef CONFIG_MW_CI_PLUS
#include "ci_plus.h" // for CI_IsCiPlusCam() and CI_GetDataRate()
#endif

#include <_svc_cas_sub_dev_api.h>

/***************************************************************
* local definition
***************************************************************/




/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyCamState;
extern CALLBACK_HANDLE	g_hCbNotifyAppInfo;

/***************************************************************
* static variables and structures
***************************************************************/
static CI_APP_INFO s_CiAppInfo[CARM_MAX_CISLOT_NUMBER];
static unsigned short s_usCiAiSsNo[CARM_MAX_CISLOT_NUMBER];

/***************************************************************
* static function prototypes
***************************************************************/
static int CiAi_AddAppInfo(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiAi_DeleteAppInfo(unsigned short usSsNo);
#ifdef CONFIG_MW_CI_PLUS
static int CiAi_CicamReset(unsigned short usSsNo);
#endif

/***************************************************************
* function bodies
***************************************************************/
int CiAi_Init(void)
{
	HxSTD_memset(s_CiAppInfo, 0, sizeof(CI_APP_INFO) * CARM_MAX_CISLOT_NUMBER);
	HxSTD_memset(s_usCiAiSsNo, 0, sizeof(unsigned short) * CARM_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

int CiAi_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	unsigned char ucMsg[4];

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAI]---> CiAi_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_usCiAiSsNo[usSlotId] = usSsNo;

	ucMsg[0] = (unsigned char)((CI_APPL_INFO_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_APPL_INFO_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_APPL_INFO_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIAI]---> CiAi_SsOpenedProc()---> Error : fail to send CI_APPL_INFO_ENQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIAI]---> CiAi_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK; // ķ���κ��� CI_APPL_INFO_REPLY �� �ް� �� ���̴�.
}

int CiAi_SsClosedProc(unsigned short usSsNo)
{
	if (CiAi_DeleteAppInfo(usSsNo) < 0)
	{
		HxLOG_Error("[CIAI]---> CiAi_SsClosedProc()---> Error : delete app info. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIAI]---> CiAi_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiAi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	HxLOG_Info("[CIAI]---> CiAi_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	switch (ulApduTag)
	{
	case CI_APPL_INFO_REPLY:
		HxLOG_Info("CI_APPL_INFO_REPLY received\n");
		#ifdef CI_WORKAROUND_FOR_DOUGLAS
		{
			extern void CiAdapt_DouglasWorkAroundTimerCallbackCancel(void);
			CiAdapt_DouglasWorkAroundTimerCallbackCancel();
		}
		#endif
		nRtnCode = CiAi_AddAppInfo(usSsNo, pucApduData, ulApduLen);
		break;

#ifdef CONFIG_MW_CI_PLUS
	case CI_REQUEST_CICAM_RESET:
		HxLOG_Info("CI_REQUEST_CICAM_RESET received\n");
		nRtnCode = CiAi_CicamReset(usSsNo);
		break;
#endif

	default:
		nRtnCode = -1;
		HxLOG_Error("[CIAI]---> CiAi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	return nRtnCode;
}

int CiAi_EnterMenu(unsigned short usSsNo)
{
	unsigned char ucMsg[4];

	ucMsg[0] = (unsigned char)((CI_ENTER_MENU >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_ENTER_MENU >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_ENTER_MENU & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIAI]---> CiAi_EnterMenu()---> Error : fail to send CI_ENTER_MENU for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIAI]---> CiAi_EnterMenu() SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

unsigned short CiAi_GetSsNo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
		return 0;

	return s_usCiAiSsNo[usSlotId];
}

CI_APP_INFO * CiAi_GetAppInfo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
		return NULL;

	return &s_CiAppInfo[usSlotId];
}

static int CiAi_AddAppInfo(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SLOT_ID					usSlotId;
	unsigned char				ucStrLen;
	CI_NOTIFY_APP_INFO_FUNC		pfnNotifyAppInfo;
	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;

	if (pucApduData == NULL || ulApduLen < 6)
	{
		HxLOG_Error("[CIAI]---> CiAi_AddAppInfo()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	/******************************************
	  setup AI instance
	 ******************************************/

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAI]---> CiAi_AddAppInfo()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	s_CiAppInfo[usSlotId].nType = pucApduData[0];
	s_CiAppInfo[usSlotId].usManufacturer = ((unsigned short)pucApduData[1] << 8) & 0xff00;
	s_CiAppInfo[usSlotId].usManufacturer |= ((unsigned short)pucApduData[2] & 0x00ff);
	s_CiAppInfo[usSlotId].usManufacturerCode = ((unsigned short)pucApduData[3] << 8) & 0xff00;
	s_CiAppInfo[usSlotId].usManufacturerCode |= ((unsigned short)pucApduData[4] & 0x00ff);
	s_CiAppInfo[usSlotId].ucIsInitialized = 1;
	ucStrLen = pucApduData[5];
	if (ucStrLen > MAX_CI_STR_LEN)
	{
		HxLOG_Error("[CIAI]---> CiAi_AddAppInfo()---> Error : ucStrLen %d > %d\n", ucStrLen, MAX_CI_STR_LEN);
		ucStrLen = MAX_CI_STR_LEN;
	}
	HxSTD_memset(s_CiAppInfo[usSlotId].szText, 0, MAX_CI_STR_LEN+1);
	HxSTD_memcpy(s_CiAppInfo[usSlotId].szText, &pucApduData[6], ucStrLen);

#ifdef _CI_DEBUG_
{
	int	i;
	HxLOG_Print("\t===CI_APP_INFO[%d]===\n", usSlotId);
	HxLOG_Print("\tType[%d], Manu[0x%x, 0x%x]\n",
		s_CiAppInfo[usSlotId].nType,
		s_CiAppInfo[usSlotId].usManufacturer,
		s_CiAppInfo[usSlotId].usManufacturerCode);
	HxLOG_Print("\t");
	for (i=0; i<MAX_CI_STR_LEN; i++)
	{
		if (s_CiAppInfo[usSlotId].szText[i] == 0x00)
			break;

		HxLOG_Print("%c", s_CiAppInfo[usSlotId].szText[i]);
	}
	HxLOG_Print("\n\t===============\n");
}
#endif

	/******************************************
	  notify CAM state
	 ******************************************/

	Callback_Reset(g_hCbNotifyCamState);
	while ((pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamState)) != NULL)
	{
		(*pfnNotifyCamState)(usSlotId, CI_CAM_INITIALIZED);
	}

	/******************************************
	  notify app info
	 ******************************************/

	Callback_Reset(g_hCbNotifyAppInfo);
	while ((pfnNotifyAppInfo = (CI_NOTIFY_APP_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyAppInfo)) != NULL)
	{
		(*pfnNotifyAppInfo)(usSlotId, &s_CiAppInfo[usSlotId]);
	}

	/* �Ʒ� �ڵ�� CI_AI_RES_V3_ID�� �����ϴ� ķ�� ���ؼ��� ���Ǿ�� �Ѵ�. but...
	   DT ���ҽ��� �����ϴ� ��� ����ķ�� ��� data rate�� ķ�� ������ RM -> AI -> CAS ���ķ� DT ������ ������ �ϴµ� ������ �ʴ´�.
	   ���������� MMI ���ǵ� ������ �ʴ´�.
	   ������ �̳��� ķ�� AI ���ҽ� ������ ȣ��Ʈ�� ������ �״�� �÷��شٴ� ���̴�.
	   ���� AI �� ������ �����ϴ� ķ���� ķ�� �ִ� ������ �Ǵ��� �� ���� ����.
	   ķ���� ���Ǹ� ���ϴ� �κ��̴�. (ķ�� ���� ���� �ڵ带 �����ϵ簡...)
	   �̿� ���� ������ �ϴ� ���Ƶ�.
	   -> SMiT CI+ ķ�� ��� ȣ��Ʈ�� CI_APPL_INFO_REPLY �� ������ ���Ŀ� �� ������ ��ٸ��� ���� �ణ�� ������ ������ ���δ�!
	   -> CI+ ���� 1.2 "CI Plus Compatibility Identification"�� �̿��ϵ��� CI_IsCiPlusCam() �Լ��� �����ϸ� �Ʒ� �ڵ带 �̿��� �� ���� ���̴�!
	   -> ���� �Ϸ��ϰ� SMiT & Neotion CI+ ķ�� ���� ���� Ȯ�� �Ϸ���.
	   -> but... ȣ��Ʈ �ʿ��� 96 Mbps �� ������ ���� ����ϵ��� ����. �������� data rate info �� ������ �ǹ̰� �����Ƿ� �ϴ� ���Ƶ�. */

#if 0
	/******************************************
	 send data rate info
	 ******************************************/

	/* CI+ ���� 1.2 : If data_rate_info APDU is not sent by the host then the maximum data rate supported by the host is 72Mbit/s. */
	if (CI_IsCiPlusCam(usSlotId) > 0
		&& CIRH_GetSsFromRes(usSlotId, CI_AI_RES_V3_ID) != NULL)
	{
		unsigned char ucMsg[5];

		ucMsg[0] = (unsigned char)((CI_DATA_RATE_INFO >> 16) & 0xff);
		ucMsg[1] = (unsigned char)((CI_DATA_RATE_INFO >> 8) & 0xff);
		ucMsg[2] = (unsigned char)(CI_DATA_RATE_INFO & 0xff);
		ucMsg[3] = 0x01;
		ucMsg[4] = CI_GetDataRate();

		if (CIRH_SendApdu(usSsNo, ucMsg, 5) < 0)
		{
			HxLOG_Error("[CIAI]---> CiAi_AddAppInfo()---> Error : send CI_DATA_RATE_INFO. SsNo[%d] !!!\n", usSsNo);
			return -1;
		}

		HxLOG_Print("CI_APPL_INFO_REPLY received -> CI+ with AIv3 -> send CI_DATA_RATE_INFO OK\n");
	}
#endif

	HxLOG_Info("[CIAI]---> CiAi_AddAppInfo()---> success...\n");

	return ERR_OK;
}

static int CiAi_DeleteAppInfo(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	/******************************************
	  release AI instance
	 ******************************************/

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAI]---> CiAi_DeleteAppInfo()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	HxSTD_memset(&s_CiAppInfo[usSlotId], 0, sizeof(CI_APP_INFO));
	s_usCiAiSsNo[usSlotId] = 0;

	/******************************************
	  notify app info -> ����μ� ó�� ����
	 ******************************************/

	HxLOG_Info("[CIAI]---> CiAi_DeleteAppInfo()---> success...\n");

	return ERR_OK;
}

#ifdef CONFIG_MW_CI_PLUS
static int CiAi_CicamReset(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIAI]---> CiAi_CicamReset()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	if (svc_cas_DevCiResetModule(usSlotId) != ERR_OK)
	{
		HxLOG_Error("[CIAI]---> CiAi_CicamReset()---> Error : svc_cas_DevCiResetModule(%d) !!!\n", usSlotId);
		return -1;
	}

	HxLOG_Info("[CIAI]---> CiAi_CicamReset()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}
#endif

