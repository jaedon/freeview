/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cxcore_stream.c
	@brief  	 stream table ���� ���

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Include_________________________________
/*******************************************************************/
#include <_svc_cas_sub_svc_api.h>

#include <cxmgr_action.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_sc.h"

#if defined(CONFIG_MW_CAS_CONAX_CPM)
#include "../local_include/cxcore_cpm.h"
#endif

#include "../local_include/cx_dscr.h"
/*******************************************************************/
#define ________CXCORE_STREAM_Golbal_Value____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Define__________________________________
/*******************************************************************/
typedef enum
{
	eCxStream_NotUsed,
	eCxStream_Used,
} CxStreamState_t;

// information about each elementary stream
typedef struct
{
	HUINT16			usPid;
	//CAS_EsType_t 	eType;
	CxStreamState_t	eState;
	Handle_t		hSession;
	Handle_t		hEcm;
	Handle_t		hDescrambler;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT16			usCwpkId;
#endif
} CxStream_t;

/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Value____________________________
/*******************************************************************/
STATIC HULONG	s_ulCxStreamSemId;

// elementary stream table
STATIC CxStream_t		s_astCxStream[CX_MAX_NUM_OF_ES];

/*******************************************************************/
#define ________CXCORE_STREAM_Global_Value____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Prototype________________________
/*******************************************************************/
STATIC HERROR local_cxstream_OpenDescrambler(int nIdx, Handle_t hSession);
STATIC HERROR local_cxstream_CloseDescrambler(int nIdx);

STATIC HUINT16 local_cxstream_GetEcmPidFromPmt(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo);
STATIC HERROR local_cxstream_Add(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo);
STATIC HERROR local_cxstream_Delete(HINT32 nIdx);
STATIC HERROR local_cxstream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid);
STATIC HERROR local_cxstream_UnlinkEcm(HINT32 nIdx);
STATIC HERROR local_cxstream_StartEcmFilter(HINT32 nIdx);
STATIC HERROR local_cxstream_StopEcmFilter(HINT32 nIdx);
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Prototype_Body___________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Public_Functions_Body___________________________
/*******************************************************************/
#define INIT_________________________________________________

HERROR CXSTREAM_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;

	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCStrSem", &s_ulCxStreamSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memset(s_astCxStream, 0, sizeof(CxStream_t) * CX_MAX_NUM_OF_ES);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		s_astCxStream[i].usPid = PID_NULL;
		//		s_astCxStream[i].eType 			= eEsType_None;
		s_astCxStream[i].eState = eCxStream_NotUsed;
		s_astCxStream[i].hSession = HANDLE_NULL;
		s_astCxStream[i].hEcm = HANDLE_NULL;
		s_astCxStream[i].hDescrambler = HANDLE_NULL;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
		s_astCxStream[i].usCwpkId = 0;
#endif
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define DESCRAMBLER__________________________________________________

// ������ descrambler slot�� open �ϴ� ���� �ƴϰ�, ������ ecm�� �ִ��� (scrambled �������)Ȯ�� �ϰ� open ��.
// ��ȿ�� stream ����, �̹� slot�� ������ �ִ� �� � Ȯ����.
// �� �Լ��� ȣ���ϴ� �ʿ����� ���� Ȯ���� �� �ʿ� ����, �ϴ� try �غ��ٴ� ��������.. �Ǹ� �Ǵ°Ű� �ƴϸ� ����..
// nIdx�� stream table ���� index
STATIC HERROR local_cxstream_OpenDescrambler(int nIdx, Handle_t hSession)
{
	HUINT32		ulHandle;
	HUINT8		aucCW[2][CX_CW_LENGTH];
	HUINT8* 	pucCWPK	= NULL;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT8		aucCWPK[CX_CPM_KEY_VALUE_BYTES];
#endif

	ENTER_FUNCTION;

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx\r\n");
		return ERR_FAIL;
	}

	// ��ȿ�� ES���� Ȯ��
	if (s_astCxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}
	if (s_astCxStream[nIdx].eState != eCxStream_Used)
	{
		HxLOG_Error("not used stream\r\n");
		return ERR_FAIL;
	}

	// �̹� descrambler slot �Ҵ� �޾Ҵ��� Ȯ��
	if (s_astCxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		HxLOG_Error("descrambler slot is already assigned\r\n");
		return ERR_FAIL;
	}

	// hEcm �ִ��� Ȯ�� (hEcm == NULL�̸� free �����̰� descrambler �ʿ� ���°� ������)
	if (s_astCxStream[nIdx].hEcm == HANDLE_NULL)
	{
		HxLOG_Error("free service\r\n");
		return ERR_FAIL;
	}

	// descrambler slot �Ҵ�
	// descrambler slot �Ҵ�
	ulHandle = CXDSCR_Open(s_astCxStream[nIdx].hEcm, nIdx, s_astCxStream[nIdx].usPid, CXACT_GetActionHandle(CXSESSION_GetStbHandle(hSession)));
	if (ulHandle == HANDLE_NULL)
	{
		HxLOG_Error("fail to open descrambler slot\r\n");
		return ERR_FAIL;
	}

	s_astCxStream[nIdx].hDescrambler = (Handle_t)ulHandle;

	// ������ ecm�� �޾Ƴ����� �������� ����.. ������ �ȳ��͵� �׸��̴� �ѹ� CW ������ �غ���..
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	if (CXECM_GetCWPK(s_astCxStream[nIdx].hEcm, aucCWPK) == ERR_OK)
	{
		pucCWPK = aucCWPK;
	}
#endif
	CXECM_GetCW(s_astCxStream[nIdx].hEcm, aucCW[0], aucCW[1]);
	CXDSCR_SetKeys(s_astCxStream[nIdx].hDescrambler, CX_CW_LENGTH, aucCW[0], CX_CW_LENGTH, aucCW[1], pucCWPK);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// nIdx�� stream table ���� index
STATIC HERROR local_cxstream_CloseDescrambler(int nIdx)
{
	ENTER_FUNCTION;

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx\r\n");
		return ERR_FAIL;
	}

	// ��ȿ�� ES���� Ȯ��
	if (s_astCxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	// �̹� descrambler slot �Ҵ� �޾Ҵٸ� close
	if (s_astCxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		//## descrambler slot close
		CXDSCR_Close(s_astCxStream[nIdx].hDescrambler, s_astCxStream[nIdx].usPid);
		s_astCxStream[nIdx].hDescrambler = HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSTREAM_SetCW(Handle_t hEcm, HUINT32 ulOddLength, HUINT32 ulEvenLength, HUINT8* pucOddCw, HUINT8* pucEvenCw, HUINT8* pucEncKey, HUINT16 usEncKeyId)
{
	int		i;
	HUINT8	aucCW[2][CX_CW_LENGTH];

	ENTER_FUNCTION;

	if (ulOddLength && pucOddCw == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}
	if (ulEvenLength && pucEvenCw == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	CX_memset(aucCW, 0x00, sizeof(aucCW));

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// ���� ecm�� link�� ��� stream�� CW�� �����Ѵ�
		if (s_astCxStream[i].hEcm == hEcm && s_astCxStream[i].hDescrambler != HANDLE_NULL)
		{
			//## descrambler slot�� CW�� �־��ش�
			if (ulOddLength)
			{
				CX_memcpy(aucCW[0], pucOddCw, CX_CW_LENGTH);
			}

			if (ulEvenLength)
			{
				CX_memcpy(aucCW[1], pucEvenCw, CX_CW_LENGTH);
			}

			// encrypted CW�� �ƴϸ� entropy reduction ���ش� !!
			if (pucEncKey == NULL)
			{
				if (ulOddLength)
				{
					aucCW[0][3] = aucCW[0][0] + aucCW[0][1] + aucCW[0][2];
					aucCW[0][7] = aucCW[0][4] + aucCW[0][5] + aucCW[0][6];
				}

				if (ulEvenLength)
				{
					aucCW[1][3] = aucCW[1][0] + aucCW[1][1] + aucCW[1][2];
					aucCW[1][7] = aucCW[1][4] + aucCW[1][5] + aucCW[1][6];
				}
			}

			CXDSCR_SetKeys(s_astCxStream[i].hDescrambler, ulOddLength, aucCW[0], ulEvenLength, aucCW[1], pucEncKey);

			// CW�� ���� ecm�� �ޱ����� �� ���� ���� �� ������ ������ �صд�.
#if defined(CONFIG_MW_CAS_CONAX_CPM)
			s_astCxStream[i].usCwpkId = usEncKeyId;//remove key ȣ��� �����ϱ� ���� ����...
			if (pucEncKey != NULL)
			{
				CXECM_SetCWPK(hEcm, pucEncKey);
			}
#endif

			CXECM_SetCW(hEcm, aucCW[0], ulOddLength, aucCW[1], ulEvenLength);

#ifdef CX_SUPPORT_SHARED_DESCRAMBLER_SLOT
			// descrambler slot�� �����Ѵٸ�, set key�� �ѹ��� �ϸ� �ȴ�.
			break;
#endif
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define STREAM_TABLE__________________________________________________

// �־��� es�� ecm pid�� ã�´�.
STATIC HUINT16 local_cxstream_GetEcmPidFromPmt(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo)
{
	int		i;
	HUINT16	usPid;
	HUINT16	usCaSysId;
	HINT32	nRet;

	ENTER_FUNCTION;

	if (pstPmtInfo == NULL || pstEsInfo == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return PID_NULL;
	}

	nRet = CXSC_GetCaSysId(CXSESSION_GetScSlot(hSession));
	if (nRet == CX_ERR_INVALID_PARAMETER)
	{
		HxLOG_Error("fail to get ca_sys_id\r\n");
		return ERR_FAIL;
	}
	usCaSysId = (HUINT16)nRet;

	usPid = PID_NULL;

	if (pstEsInfo->bElementCaFlag)
	{
		for (i = 0; i < pstEsInfo->ulNumOfCaDescriptor; i++)
		{
			//## ca system id �� (ca_system_id�� ī�忡�� ���� �� ���� ���̴�.. �׷��� ī�尡 ���ٸ� ��� ����?)
			if (pstEsInfo->stElementCaDescriptor[i].usCaSysId == usCaSysId)
			{
				usPid = pstEsInfo->stElementCaDescriptor[i].usCaPid;
				break;
			}
		}
	}

	// ���� elementary stream level�� ecm pid�� ��ã���� (ca_Descriptor�� ���ٸ�), program level���� ã�´�.
	if (usPid == PID_NULL)
	{
		if (pstPmtInfo->bProgramCaFlag)
		{
			for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
			{
				//## ca system id �� (ca_system_id�� ī�忡�� ���� �� ���� ���̴�.. �׷��� ī�尡 ���ٸ� ��� ����?)
				if (pstPmtInfo->stProgramCaDescriptor[i].usCaSysId == usCaSysId)
				{
					usPid = pstPmtInfo->stProgramCaDescriptor[i].usCaPid;
					break;
				}
			}
		}
	}

	HxLOG_Print("[Exit:ecm pid=%x]\r\n", usPid);
	return usPid;
}

STATIC HERROR local_cxstream_Add(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo)
{
	int		i;
	HUINT16	usEcmPid;

	ENTER_FUNCTION;

	if (pstPmtInfo == NULL || pstEsInfo == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// �ϴ� pid�� �� slot�� Ȯ���غ���
		if (s_astCxStream[i].usPid == PID_NULL)
		{
			s_astCxStream[i].usPid = pstEsInfo->usPid;
			//s_astCxStream[i].eType 	= pstEsInfo->eEsType;
			s_astCxStream[i].hSession = hSession;

			///////////////////////////////////////////////////////////////////////////////////////////////
			//
			// !!! ���� setPid�� �Ⱦ��� pmt ���� ��� stream�� ó���ؾ� �Ѵٸ�, ���⼭ state�� ������ used�� ���ָ� �ȴ�.. �� ����~��~
			// s_astCxStream[i].eState = eCxStream_Used;
			//
			///////////////////////////////////////////////////////////////////////////////////////////////

			usEcmPid = local_cxstream_GetEcmPidFromPmt(hSession, pstPmtInfo, pstEsInfo);
			if (usEcmPid != PID_NULL)
			{
				// ecm pid�� �ִٸ� hEcm ����ϰ�
				local_cxstream_LinkEcm(hSession, i, usEcmPid);

				// [improve speed]: �Ʒ� �Լ� �ȿ� ������ used���� Ȯ�� ��
				if (s_astCxStream[i].eState == eCxStream_Used)
				{
					local_cxstream_StartEcmFilter(i);

					// descrambler�� ����ؾ���
					local_cxstream_OpenDescrambler(i, hSession);
				}
			}
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_Delete(HINT32 nIdx)
{
	ENTER_FUNCTION;

	if (s_astCxStream[nIdx].hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	// descrambler slot�� close�Ѵ�
	local_cxstream_CloseDescrambler(nIdx);

	// ecm�� link �����
	local_cxstream_UnlinkEcm(nIdx);

	s_astCxStream[nIdx].hSession = HANDLE_NULL;
	s_astCxStream[nIdx].eState = eCxStream_NotUsed;
	//s_astCxStream[nIdx].eType 		= eEsType_None;
	s_astCxStream[nIdx].usPid = PID_NULL;

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid)
{
	Handle_t	hEcm;

	ENTER_FUNCTION;

	//���� ecm �ֳ� ã�ƺ��� ������ ����, ������ �߰�
	hEcm = CXECM_Add(hSession, usEcmPid);

	s_astCxStream[nIdx].hEcm = hEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	//## ecm filter start �ؾߵǳ� ?
	//local_cxstream_StartEcmFilter(nIdx);
	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_UnlinkEcm(HINT32 nIdx)
{
	ENTER_FUNCTION;

	// ecm table���� ���� 5IAv4B ecmBJ?!<- >K>F<- GR 0E@S (filter stop55 >K>F<- GR 0M@S)
	CXECM_Delete(s_astCxStream[nIdx].hEcm);

	s_astCxStream[nIdx].hEcm = HANDLE_NULL;

	LEAVE_FUNCTION;
	return ERR_OK;
}

//stream�� not used�̸� filter start �� �ʿ� ����
STATIC HERROR local_cxstream_StartEcmFilter(HINT32 nIdx)
{
	Handle_t	hEcm;

	ENTER_FUNCTION;

	hEcm = s_astCxStream[nIdx].hEcm;
	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (s_astCxStream[nIdx].eState != eCxStream_Used)
	{
		HxLOG_Error("not used stream\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (CXSESSION_IsSelected(s_astCxStream[nIdx].hSession) == FALSE)
	{
		//���� descramble�ϵ��� ���õ��� ���� �����̹Ƿ� ecm filter�� ���� �ʵ��� �Ѵ�.
		HxLOG_Error("not selected\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// filter start: both�� �Ǵ�.. ���ķδ� �ڵ� toggle
	CXECM_StartFilter(hEcm, eCxEcm_Both);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// �ٸ� session���� ���� �ִٸ� filter�� �׳� �ΰ�, �ƴϸ� stop ���ѿ�
HERROR local_cxstream_StopEcmFilter(HINT32 nIdx)
{
	int			i;
	Handle_t	hEcm;
	Handle_t	hSession;

	ENTER_FUNCTION;

	hSession = s_astCxStream[nIdx].hSession;
	hEcm = s_astCxStream[nIdx].hEcm;

	if (hSession == HANDLE_NULL || hEcm == HANDLE_NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	//�ٸ� session���� ���Ǵ��� ã�ƺ���
	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL &&			// ��ȿ�� slot
			s_astCxStream[i].hSession != hSession && 		// �ٸ� session
			s_astCxStream[i].hEcm == hEcm && 				// ���� ecm
			s_astCxStream[i].eState == eCxStream_Used)	// ������� stream
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		// �� ã�Ҵٸ� filter stop
		CXECM_StopFilter(hEcm);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// �� type ���� �ϳ��� stream�� ����Ѵ� ġ��..
//## �� �����غ���... audio�� �ΰ��ϼ��� �־�...
#if 0
HERROR CXSTREAM_SetPid(Handle_t hSession, CxEsPidInfo_t *pstEsInfo)
{
	int i, j;
	HBOOL bExist;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL)
		{
			bExist = FALSE;

			for (j = 0; j < pstEsInfo->ulPidNum; j++)
			{
				if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid == pstEsInfo->usPid[j])
				{
					bExist = TRUE;
					break;
				}
			}

			if (bExist)
			{
VK_Print("SetPID:update[%d,type:%d,pid:%x,st:%d]pid:%x\n", i,s_astCxStream[i].eType, s_astCxStream[i].usPid, s_astCxStream[i].eState, pstEsInfo->usPid[j]);
				// �̹� ��ϵ� pid �̰�, �����ε� ����� pid��
				if (s_astCxStream[i].eState != eCxStream_Used)
				{
					s_astCxStream[i].eState = eCxStream_Used;
					// ���� descrambler slot�� open���� �ʾҴٸ� open �õ�
					local_cxstream_OpenDescrambler(i);
					//## ecm filter start �ؾߵǳ� ?
					local_cxstream_StartEcmFilter(i);
				}
			}
			else
			{
VK_Print("SetPID:delete[%d,type:%d,pid:%x,st:%d]pid:%x\n", i,s_astCxStream[i].eType, s_astCxStream[i].usPid, s_astCxStream[i].eState, pstEsInfo->usPid[j]);
				//�ִ� �ǵ�, ���� ������ pid
				if (s_astCxStream[i].eState != eCxStream_NotUsed)
				{
					// descrambler slot�� close�Ѵ�
					local_cxstream_CloseDescrambler(i);
				}
				s_astCxStream[i].eState  = eCxStream_NotUsed;
			}
		}
	}

	for (j = 0; j < pstEsInfo->ulPidNum; j++)
	{
		if (pstEsInfo->usPid[j] != PID_NULL)
		{
			bExist = FALSE;
			for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
			{
				if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid == pstEsInfo->usPid[j])
				{
					bExist = TRUE;
					break;
				}
			}

			if (bExist)
			{
				// �̹� ��ϵ� pid, �տ��� ó������.
			}
			else
			{
VK_Print("SetPID:add[%d,type:%d,pid:%x]\n", j,pstEsInfo->eType[j],pstEsInfo->usPid[j]);
				//�������� ���� �ǵ�, ���� �߰��� pid
				for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
				{
					// �ϴ� pid�� �� slot�� Ȯ���غ���
					if (s_astCxStream[i].usPid == PID_NULL)
					{
						s_astCxStream[i].usPid 	= pstEsInfo->usPid[j];
						s_astCxStream[i].eType 	= pstEsInfo->eType[j];
						s_astCxStream[i].eState 	= eCxStream_Used;
						s_astCxStream[i].hSession	= hSession;
						break;
					}
				}
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}
#else
//HERROR CXSTREAM_SetPid(Handle_t hSession, CAS_EsType_t eType, HUINT16 usPid, HBOOL bAdd)
HERROR CXSTREAM_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd)
{
	int i;

	HxLOG_Print("[Enter(%x,%x)]\r\n", usPid, bAdd);

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	if (usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
//		if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].eType == eType)
// type ������ ���� ���ƺ���...
		if (s_astCxStream[i].hSession == hSession)
		{
			HxLOG_Print("SetPID[%d,pid:%x,st:%d]pid:%x\n", i, s_astCxStream[i].usPid, s_astCxStream[i].eState, usPid);

			if (s_astCxStream[i].usPid == usPid)
			{
				// �̹� ��ϵ� pid (����� ���� pmt�� �޾Ҵٴ� ����)
				if (bAdd)
				{
					// ���
					if (s_astCxStream[i].eState != eCxStream_Used)
					{
						s_astCxStream[i].eState = eCxStream_Used;
						// ���� descrambler slot�� open���� �ʾҴٸ� open �õ�
						local_cxstream_OpenDescrambler(i, hSession);
						//## ecm filter start �ؾߵǳ� ?
						local_cxstream_StartEcmFilter(i);
					}
				}
				else
				{
					// ����
					if (s_astCxStream[i].eState != eCxStream_NotUsed)
					{
						s_astCxStream[i].eState = eCxStream_NotUsed;
						// descrambler slot�� close�Ѵ�
						local_cxstream_CloseDescrambler(i);
						// ecm filter ó�� - �ٸ����� ���� �׳� �ΰ�, ���� ������ ���ش�.
//						local_cxstream_UnlinkEcm(i);
					}
				}

				CX_SEM_Release(s_ulCxStreamSemId);
				HxLOG_Print("[Exit]1\r\n");
				return ERR_OK;
			}
#if 0
			else
			{
				// �ٸ� pid �� ��ϵǾ� �ִ� ��� (���� ���� ������ ���� ����Ѵ�)
				if (s_astCxStream[i].eState == eCxStream_Used)
				{
					// descrambler slot�� close�Ѵ�
					local_cxstream_CloseDescrambler(i);
					// ecm filter ó�� - �ٸ����� ���� �׳� �ΰ�, ���� ������ ���ش�.
					local_cxstream_UnlinkEcm(i);
				}

				s_astCxStream[i].eState  = eCxStream_NotUsed;
				break;
			}
#endif
		}
	}

	// ������ �߰� (����� �Դٸ� ���� pmt�� ������ ������)
	if (bAdd)
	{
		for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
		{
			// �ϴ� pid�� �� slot�� Ȯ���غ���
			if (s_astCxStream[i].usPid == PID_NULL)
			{
				s_astCxStream[i].usPid 	= usPid;
//				s_astCxStream[i].eType 	= eType;
				s_astCxStream[i].eState 	= eCxStream_Used;
				s_astCxStream[i].hSession	= hSession;
				CX_SEM_Release(s_ulCxStreamSemId);
				HxLOG_Print("[Exit]2\r\n");
				return ERR_OK;
			}
		}
	}
	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}
#endif

HBOOL CXSTREAM_IsConaxStream(Handle_t hSession)
{
	int					i, j;
	SvcCas_PmtInfo_t* 	pstPmtInfo;
	SvcCas_PmtElement_t* 							pstEsInfo;
	HBOOL				bFound			= FALSE;

	ENTER_FUNCTION;

	pstPmtInfo = CXSESSION_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("no pmt\r\n");
		return FALSE;
	}

	// program level���� ã�´�.
	if (pstPmtInfo->bProgramCaFlag)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			//## ca system id ��
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_CX & 0xFF00))
			{
				bFound = TRUE;
				break;
			}
		}
	}

	// es level�� ã�ƺ���
	if (bFound == FALSE)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
		{
			pstEsInfo = &pstPmtInfo->stElementInfo[i];

			if (pstEsInfo->bElementCaFlag)
			{
				for (j = 0; j < pstEsInfo->ulNumOfCaDescriptor; j++)
				{
					//## ca system id ��
					if ((pstEsInfo->stElementCaDescriptor[j].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_CX & 0xFF00))
					{
						bFound = TRUE;
						break;
					}
				}
			}

			// ã������ �׸� ����.
			if (bFound)
			{
				break;
			}
		}
	}

	HxLOG_Print("[Exit(%d)]\r\n", bFound);
	return bFound;
}

// pmt parsing�ϴ� ��
HERROR CXSTREAM_ParsePmt(Handle_t hSession)
{
	int					i, j;
	HBOOL				bFound;
	HUINT16				usEcmPid;
	SvcCas_PmtInfo_t* 	pstPmtInfo;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	pstPmtInfo = CXSESSION_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("null pmt info\r\n");
		return ERR_FAIL;
	}

	// smart card�� ������ ���� ����..
	// ca_system_id �����ϴ� �͵� �׷���.. ecm filter�� ���ʿ䰡 ������.. sc ������ �׶� ����
	// �̰� �ӵ��� ������ �شٸ� ���߿� �ٲټ���. (����� scInserted�� pmt_parse �ٽ� ȣ���ϴ� �κ� �����ϸ� �ɰ���)
	if (CXSESSION_GetScSlot(hSession) == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	//## 1. �̹� ��ϵ� stream ������ updated pmt���� ������Ʈ Ȥ�� ����
	//##		hSession, pid, type ������ update
	//##			hEcm�� ������ skip
	//##			hEcm �ٸ��� ���� hEcm close �õ� (�ٸ� ���� ������̸� ���������� ���� ��), ���ο� hEcm �߰� (state=used�̸� filter start)
	//##			descrambler open �õ� (hEcm �ִµ� descrambler �Ҵ� ������ �����)
	//##		������ �߰ߵ��� ������ delete
	//##			hEcm close �õ�
	//##			descrambler close �õ�
	//## 2. updated pmt�� ���� stream�� �ִٸ� �߰�
	//##		�߰� �ϰ�, hEcm�� �߰��ϰ�.. ���� filter�� descrambler�� open�ϰ�..

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid != PID_NULL)
		{
			// �̹� ����� elementary stream�� ����/���� ���θ� Ȯ���Ѵ�

			bFound = FALSE;
			for (j = 0; j < pstPmtInfo->ulNumOfElement; j++)
			{
				if (s_astCxStream[i].usPid == pstPmtInfo->stElementInfo[j].usPid)
				//&& s_astCxStream[i].eType == pstPmtInfo->stElementInfo[j].eEsType)
				{
					//���� es�� ���ο� pmt���� ������ - ���� ���� Ȯ�� (free <-> scrambled, ecm pid ���� ����)
					HxLOG_Print("ALREADY EXIST(%x)\n", pstPmtInfo->stElementInfo[i].usPid);

					bFound = TRUE;
					usEcmPid = local_cxstream_GetEcmPidFromPmt(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[j]);

					if (usEcmPid == PID_NULL)
					{
						// hEcm = null �̸� ���� ���� ����, �ƴ϶�� update
						if (s_astCxStream[i].hEcm != HANDLE_NULL)
						{
							HxLOG_Print("UPDATE(free)\n");
							// scrambled --> free�� ����
							// descrambler slot ������.
							local_cxstream_CloseDescrambler(i);
							// hEcm ������..
							local_cxstream_UnlinkEcm(i);
						}
					}
					else
					{
						// ecm pid �� ���ٸ� ������ ���ٴ� ��, �ƴ϶�� update
						if (CXECM_GetPid(s_astCxStream[i].hEcm) != usEcmPid)
						{
							HxLOG_Print("UPDATE(scr)\n");
							// hEcm ���� �ʿ���
							//���� hEcm�� Ȯ���ؼ� ���������ϰ�, �� hEcm �߰��Ѵ�.
							if (s_astCxStream[i].hEcm != HANDLE_NULL)
							{
								local_cxstream_UnlinkEcm(i);
							}
							local_cxstream_LinkEcm(hSession, i, usEcmPid);

							// descrambler�� �׳� �ΰ�, ���� free ä�� �̾��ٸ�, descrambler slot �޾ƾ���
							// sc ���� �� ������ service�� stop -> start �� ��� descrambler ��ϰ� ecm filter�� �������� �ʾƿ�...
						}

						local_cxstream_OpenDescrambler(i, hSession);
						local_cxstream_StartEcmFilter(i);
					}
					break;
				}
			}

			if (bFound == FALSE)
			{
				HxLOG_Print("NOT FOUND-DELETE(%x)\n", s_astCxStream[i].usPid);
				// ���� es�� ���ο� pmt�� �������� ���� - ���� ó��
				local_cxstream_Delete(i);
			}
		}
	}

	// ���ο� pmt�� �߰��� stream�� �ִ��� Ȯ��
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		if (pstPmtInfo->stElementInfo[i].usPid != PID_NULL && pstPmtInfo->stElementInfo[i].usPid != 0x0)
		{
			// �̹� �����ϴ� pid�� �տ��� update�Ǿ��� ���̴� skip�ϰ�, ���� �͸� ���� �߰�
			for (j = 0; j < CX_MAX_NUM_OF_ES; j++)
			{
				if (s_astCxStream[j].hSession == hSession && s_astCxStream[j].usPid == pstPmtInfo->stElementInfo[i].usPid)
				{
					break;
				}
			}

			if (j == CX_MAX_NUM_OF_ES)
			{
				HxLOG_Print("NEW!(%x)\n", pstPmtInfo->stElementInfo[i].usPid);
				// ���� pid �߰ߵ��� �ʾ���, �߰�
				local_cxstream_Add(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[i]);
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSTREAM_DeleteAll(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// �ش� session�� ��� stream�� �����Ѵ�
		if (s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_Delete(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// session �� pid ������ �ʱ�ȭ �Ѵ�
HERROR CXSTREAM_ResetStream(Handle_t hSession)
{
	HUINT32	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].hSession == hSession)
		{
			// descrambler slot�� close�Ѵ�
			local_cxstream_CloseDescrambler(i);

			// session ���� �� pid ������ clear
			// descrambler�� sc insert�� parsePMT �ϸ鼭 open�ȴ�
			s_astCxStream[i].hSession = HANDLE_NULL;
			s_astCxStream[i].usPid = PID_NULL;

			// setPid�� �ٽ� �������� �����Ƿ� state�� ����
			//s_astCxStream[i].eState	 	= eCxStream_NotUsed;
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// free or scrambled svc Ȯ��
HERROR CXSTREAM_IsScrambledSvc(Handle_t hSession, HBOOL* pbScrambled)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	*pbScrambled = FALSE;

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			//if (s_astCxStream[i].hEcm != HANDLE_NULL && s_astCxStream[i].eState == eCxStream_Used)
			if (s_astCxStream[i].hEcm != HANDLE_NULL)
			{
				//����� ecm�� �ִٸ� scrambled svc�� ����.
				*pbScrambled = TRUE;
				break;
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// �ش� session (service)�� ���� stream�� ����ϴ� ��� ecm filter�� start ��Ų��.
HERROR CXSTREAM_StartAllEcmFilter(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	// smart card�� ������ filter start �� �ʿ� ����
	if (CXSESSION_GetScSlot(hSession) == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\r\n");
		return ERR_OK;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_StartEcmFilter(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// �ش� session (service)�� ���� stream�� ����ϴ� ��� ecm filter�� stop ��Ų��.
HERROR CXSTREAM_StopAllEcmFilter(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_StopEcmFilter(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// �ش� session (service)�� ���� stream�� ����ϴ� ��� ecm filter�� restart ��Ų��.
HERROR CXSTREAM_RestartAllEcmFilter(Handle_t hSession)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = CXSTREAM_StopAllEcmFilter(hSession);

	if (nErr == ERR_OK)
	{
		nErr = CXSTREAM_StartAllEcmFilter(hSession);
	}

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXSTREAM_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm)
{
	int	i;

	ENTER_FUNCTION;

	//	sem �ɸ� �ȵ� (stream->ecm->stream call)
	//	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL 		//
		&& s_astCxStream[i].hSession == hSession	//
		&& s_astCxStream[i].hEcm == hEcm)			//
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

Handle_t CXSTREAM_GetFirstSessionByEcm(Handle_t hEcm)
{
	int	i;

	ENTER_FUNCTION;

	//	sem �ɸ� �ȵ� (stream->ecm->stream call)
	//	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hEcm == hEcm)
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return s_astCxStream[i].hSession;
}

HUINT32 CXSTREAM_GetAllSessionByEcm(Handle_t hEcm, Handle_t* phList)
{
	int		i, j;
	HUINT32	ulCount;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0, ulCount = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hEcm == hEcm)
		{
			// Ȥ�� ���� session�� �̹� ã�Ҿ����� Ȯ��
			for (j = 0; j < ulCount; j++)
			{
				if (phList[j] == s_astCxStream[i].hSession)
				{
					break;
				}
			}

			if (j == ulCount)
			{
				phList[ulCount++] = s_astCxStream[i].hSession;
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ulCount;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXSTREAM_RemoveCWPK(HUINT16 usKeyId)
{
	int	i;

	HxLOG_Print("[Enter]\n");

	if (usKeyId == 0)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	// �ش� key id�� ����ϴ� descrambler�� CWPK�� clear �Ѵ�.
	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usCwpkId == usKeyId && s_astCxStream[i].hDescrambler != HANDLE_NULL)
		{
			CXDSCR_ClearCWPK(s_astCxStream[i].hDescrambler);
			s_astCxStream[i].usCwpkId = 0;
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}
#endif

#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxstream_DEBUG_GetScStateStr(CxStreamState_t eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCxStream_NotUsed)
		ENUM_TO_STR(eCxStream_Used)
	}

	return "Unknown";
}

void CMD_CXSTREAM_PrintStreamContext(void)
{
	HUINT32	i;

	for (i = 0 ; i < CX_MAX_NUM_OF_ES ; i++)
	{
		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX STREAM (%d)\n"), i);

		HxLOG_Warning("\t hSession (%d, 0x%x)\n", CXSESSION_GetSessionIdx(s_astCxStream[i].hSession));
		HxLOG_Warning("\t eState (%s)\n", local_cxstream_DEBUG_GetScStateStr(s_astCxStream[i].eState));
		HxLOG_Warning("\t usPid (0x%x)\n", s_astCxStream[i].usPid);

		if(s_astCxStream[i].eState != eCxStream_Used)
			continue;

		HxLOG_Warning("\t hEcm (0x%x)\n", s_astCxStream[i].hEcm);
		HxLOG_Warning("\t hDescrambler (%d)\n", s_astCxStream[i].hDescrambler);
		HxLOG_Warning("\t usCwpkId (0x%x)\n", s_astCxStream[i].usCwpkId);
	}
}
#endif

