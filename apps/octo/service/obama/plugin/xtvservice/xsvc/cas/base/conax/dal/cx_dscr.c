/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_dscr.c
	@brief  	 descrambler adaptation layer

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
#define ________CX_DSCR_Private_Include_______________________________________
/*******************************************************************/
#include <octo_common.h>	// htype.h and hlib.h
#include <conax.h>
#include <_svc_cas_sub_dev_api.h>
#include <_svc_cas_sub_svc_api.h>
#include "../local_include/cx_dscr.h"

/*******************************************************************/
#define ________CX_DSCR_Golbal_Value__________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DSCR_Private_Define________________________________________
/*******************************************************************/
#define CX_DSCR_KEY_LENGTH				8
#define CX_DSCR_SLOT_NUM 				60 	// palDsc_MAX_NUM_OF_ID in pal_dsc.c

typedef struct
{
	HUINT32	ulCxStreamIdx;
	HUINT32	ulDscrHandle;
	HUINT16	usPid;
	HUINT8	aucOddKey[CX_DSCR_KEY_LENGTH];
	HUINT8	aucEvenKey[CX_DSCR_KEY_LENGTH];
	HUINT8	aucProtKey[16];
	void* 	pUserData;
} CX_DSCR_Slot;

/*******************************************************************/
#define ________CX_DSCR_Private_Static_Value__________________________________
/*******************************************************************/
static HULONG				s_ulCxDscSemaId;
static CX_DSCR_Slot* 		s_pstCxDscrSlot;


/*******************************************************************/
#define ________CX_DSCR_Global_Value__________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DSCR_Private_Static_Prototype______________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DSCR_Private_Static_Prototype_Body_________________________
/*******************************************************************/
static void local_cxDalDsc_Init(void)
{
	ENTER_FUNCTION;
	svc_cas_DevDscInit();
	LEAVE_FUNCTION;
}

static HERROR local_cxDalDsc_ChangePid(HUINT32 ulDscrHandle, HUINT16 usNewPid)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = svc_cas_DevDscSetPid(ulDscrHandle, usNewPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscSetPid() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalDsc_LinkWithPid(HUINT32* pulDscrHandle, HUINT16 usPid, Handle_t hAction)
{
	HERROR	nErr;
	HUINT32	ulDeviceId;

	ENTER_FUNCTION;
	HxLOG_Print("link pid : 0x%04x\r\n", usPid);

	if (pulDscrHandle == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	nErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, &ulDeviceId);
	if (nErr != ERR_OK)
	{
		HxLOG_Print("svc_cas_SubSvcRmGetResourceId() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// eCAS_DSC_DATA_TYPE_PSI_PES --> 이거 별 의미 없어 보임..
	nErr = svc_cas_DevDscOpen((HUINT32)ulDeviceId, eCAS_DSC_TYPE_DVB, eCAS_DSC_DATA_TYPE_PSI_PES, pulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscOpen() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	nErr = svc_cas_DevDscSetPid(*pulDscrHandle, usPid);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscSetPid() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	HxLOG_Print("svc_cas_DevDscSetPid() returns desc handle (0x%08x)\r\n", *pulDscrHandle);
	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalDsc_Unlink(HUINT32 ulDscrHandle, HUINT16 usPid)
{
	HERROR	nErr;
	HUINT8	aucNullCW[8];

	ENTER_FUNCTION;
	HxLOG_Print("[%x]link pid:%x\n", ulDscrHandle, usPid);

	HxSTD_memset(aucNullCW, 0, 8);

	// Maturity rating을 설정한 channel에서 pincode입력을 해서
	//				CW를 얻어 설정한 상태에서 다른 channel에 갔다가 돌아왔을 때 아래 code가
	//				없으면 이전에 설정된 CW로 인해 descramble이됨.
	// =-> decrambler link를 끊는 경우 Key 값도 초기화 해주어야 함.
	nErr = svc_cas_DevDscSetKey(ulDscrHandle, eCAS_DSC_PARITY_EVEN, aucNullCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscSetKey() returns fail ERR(0x%08x)\r\n", nErr);
	}
	nErr = svc_cas_DevDscSetKey(ulDscrHandle, eCAS_DSC_PARITY_ODD, aucNullCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscSetKey() returns fail ERR(0x%08x)\r\n", nErr);
	}

	nErr = svc_cas_DevDscClose(ulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscClose() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalDsc_SetCW(HUINT32 ulDscrHandle, HUINT8 ucType, HUINT8* pucCW)
{
	HERROR						nErr;
	SvcCas_DevDscKeyParity_e	eParity;

	ENTER_FUNCTION;

	if (ucType == 0)
	{
		eParity = eCAS_DSC_PARITY_EVEN;
	}
	else
	{
		eParity = eCAS_DSC_PARITY_ODD;
	}

	nErr = svc_cas_DevDscSetKey(ulDscrHandle, eParity, pucCW);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscClose() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
static HERROR local_cxDalDsc_SetProtectionKey(HUINT32 ulDscrHandle, HUINT8* pucProtectionKey)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = svc_cas_DevDscSetProtectionKey(ulDscrHandle, pucProtectionKey);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscSetProtectionKey() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalDsc_ClearProtectionKey(HUINT32 ulDscrHandle)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = svc_cas_DevDscClearProtectionKey(ulDscrHandle);
	if (nErr != ERR_OK)
	{
		HxLOG_Info("svc_cas_DevDscClearProtectionKey() returns fail ERR(0x%08x)\r\n", nErr);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif

void cxDalDsc_Init(void)
{
	int	nErr;
	int	i;

	ENTER_FUNCTION;

	nErr = VK_SEM_Create(&s_ulCxDscSemaId, "CxDscr", VK_SUSPENDTYPE_PRIORITY);
	if (nErr)
	{
		HxLOG_Info("failed to create semaphore (%d)\n", nErr);
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_pstCxDscrSlot = (CX_DSCR_Slot *)OxCAS_Malloc(sizeof(CX_DSCR_Slot) * CX_DSCR_SLOT_NUM);
	HxLOG_Assert(s_pstCxDscrSlot != NULL);

	for (i = 0; i < CX_DSCR_SLOT_NUM; i++)
	{
		s_pstCxDscrSlot[i].ulCxStreamIdx = HANDLE_NULL;
	}

	local_cxDalDsc_Init();

	LEAVE_FUNCTION;
}

/*
	stop task
*/
void cxDalDsc_Exit(void)
{
	int	i;

	ENTER_FUNCTION;

	if (s_pstCxDscrSlot == NULL)
	{
		return;
	}

	for (i = 0; i < CX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstCxDscrSlot[i].ulCxStreamIdx != HANDLE_NULL && s_pstCxDscrSlot[i].pUserData)
		{
			OxCAS_Free(s_pstCxDscrSlot[i].pUserData);
		}
	}

	VK_SEM_Destroy(s_ulCxDscSemaId);

	if (s_pstCxDscrSlot == NULL)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	OxCAS_Free(s_pstCxDscrSlot);
	s_pstCxDscrSlot = NULL;

	LEAVE_FUNCTION;
}

HUINT32 cxDalDsc_Open(Handle_t hEcm, HUINT32 ulCxStreamIdx, HUINT16 usPid, Handle_t hAction)
{
	HUINT32	ulStbDscrHandle = HANDLE_NULL;
	int		i;

	ENTER_FUNCTION;
	HxLOG_Print("Open pid : 0x%04x\r\n", usPid);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Action is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulCxDscSemaId);

	// allocate descramler
	// actually, a decrambler slot is opened later (see CX_DSCR_SetKeys() and local_cxDalDsc_LinkWithPid() in CX_DSCR_adapt.c)
	// check if a slot has been opened for dwStbStreamHandle
	for (i = 0; i < CX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstCxDscrSlot[i].ulCxStreamIdx == ulCxStreamIdx)
		{
			HxLOG_Print("ulCxStreamIdx is already assigned to a descrambler slot(%d, %x, %x)\n", i, s_pstCxDscrSlot[i].ulCxStreamIdx, s_pstCxDscrSlot[i].ulDscrHandle);

			if (s_pstCxDscrSlot[i].ulDscrHandle != HANDLE_NULL)
			{
				if (s_pstCxDscrSlot[i].usPid != usPid)
				{
					local_cxDalDsc_ChangePid(s_pstCxDscrSlot[i].ulDscrHandle, usPid);
					s_pstCxDscrSlot[i].usPid = usPid;
				}

				VK_SEM_Release(s_ulCxDscSemaId);
				return i;
			}
			else
			{
				// strange situation
				s_pstCxDscrSlot[i].ulCxStreamIdx = HANDLE_NULL;
			}

			break;
		}
	}

	// find a slot
	for (i = 0; i < CX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstCxDscrSlot[i].ulCxStreamIdx == HANDLE_NULL)
		{
			HxLOG_Print("slot[%d] is allocated\n", i);
			s_pstCxDscrSlot[i].usPid = PID_NULL;
			s_pstCxDscrSlot[i].pUserData = NULL;
			s_pstCxDscrSlot[i].ulCxStreamIdx = ulCxStreamIdx;
			s_pstCxDscrSlot[i].ulDscrHandle = HANDLE_NULL;
			ulStbDscrHandle = i;
			break;
		}
	}

	// fail to allocate descrambler
	if (ulStbDscrHandle == HANDLE_NULL)
	{
		HxLOG_Info("failed to allocate a descrambler\n");
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	// set pid
	if (INVALID_PID(usPid))
	{
		HxLOG_Info("wrong dwStbStreamHandle\n");
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	if (s_pstCxDscrSlot[ulStbDscrHandle].usPid != usPid)
	{
		if (local_cxDalDsc_LinkWithPid(&s_pstCxDscrSlot[ulStbDscrHandle].ulDscrHandle, usPid, hAction) != ERR_OK)
		{
			HxLOG_Info("local_cxDalDsc_Unlink error\n");
			VK_SEM_Release(s_ulCxDscSemaId);
			// it failed to open a descrambler slot : it shouln't be happend!!
			LEAVE_FUNCTION;
			return HANDLE_NULL;
		}

		s_pstCxDscrSlot[ulStbDscrHandle].usPid = usPid;
	}

	VK_SEM_Release(s_ulCxDscSemaId);
	HxLOG_Print("[Exit][%x]\n", s_pstCxDscrSlot[ulStbDscrHandle].ulDscrHandle);
	LEAVE_FUNCTION;
	return ulStbDscrHandle;
}


HERROR cxDalDsc_Close(HUINT32 ulStbDscrHandle, HUINT16 usPid)
{
	HUINT32	nIdx;

	ENTER_FUNCTION;
	HxLOG_Print("[Enter][%x]\n", ulStbDscrHandle);

	nIdx = (int)ulStbDscrHandle;

	VK_SEM_Get(s_ulCxDscSemaId);

	// ulStbDscrHandle does not exist
	if (nIdx >= CX_DSCR_SLOT_NUM || s_pstCxDscrSlot[nIdx].ulCxStreamIdx == HANDLE_NULL)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		HxLOG_Info("wrong dwStbStreamHandle or already closed\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// close descrambler
	HxLOG_Info("[%x %x %x]\n", s_pstCxDscrSlot[ulStbDscrHandle].ulCxStreamIdx, s_pstCxDscrSlot[ulStbDscrHandle].ulDscrHandle, s_pstCxDscrSlot[ulStbDscrHandle].usPid);

	if (local_cxDalDsc_Unlink(s_pstCxDscrSlot[nIdx].ulDscrHandle, s_pstCxDscrSlot[nIdx].usPid) != ERR_OK)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		HxLOG_Info("local_cxDalDsc_Unlink error\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	s_pstCxDscrSlot[nIdx].ulCxStreamIdx = HANDLE_NULL;
	s_pstCxDscrSlot[nIdx].ulDscrHandle = HANDLE_NULL;

	if (s_pstCxDscrSlot[nIdx].pUserData)
	{
		OxCAS_Free(s_pstCxDscrSlot[nIdx].pUserData);
	}

	VK_SEM_Release(s_ulCxDscSemaId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR cxDalDsc_SetKeys(HUINT32 ulStbDscrHandle, HUINT16 usOddKeyLength, const HUINT8* pucOddKey, HUINT16 usEvenKeyLength, const HUINT8* pucEvenKey, void* pUserData)
{
	HUINT32	nIdx;

	ENTER_FUNCTION;

	nIdx = (int)ulStbDscrHandle;

	VK_SEM_Get(s_ulCxDscSemaId);

	// ulStbDscrHandle does not exist
	if (nIdx >= CX_DSCR_SLOT_NUM || s_pstCxDscrSlot[nIdx].ulCxStreamIdx == HANDLE_NULL)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// key lengths are not supported
	if (usOddKeyLength > CX_DSCR_KEY_LENGTH || usEvenKeyLength > CX_DSCR_KEY_LENGTH)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pucOddKey == NULL && pucEvenKey == NULL)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// set key
#if 0
	// check if pid is changed
	usPid = VA_PI_Adapt_GetPidFromStreamHandle(s_pstCxDscrSlot[nIdx].ulCxStreamIdx);
	if (usPid == PID_NULL)
	{
		HxLOG_Info("wrong dwStbStreamHandle\n");
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (s_pstCxDscrSlot[nIdx].usPid != usPid)
	{
		local_cxDalDsc_ChangePid(s_pstCxDscrSlot[nIdx].ulDscrHandle, usPid);
		s_pstCxDscrSlot[nIdx].usPid = usPid;
	}
#endif

#if 0	// invalid pUserData is coming during LABCOM_SUB_2011_IHM-CAS test. temporarily, block this code.
	if (pUserData)
	{
		if (s_pstCxDscrSlot[nIdx].pUserData)
		{
			OxCAS_Free(s_pstCxDscrSlot[nIdx].pUserData);
		}
		s_pstCxDscrSlot[nIdx].pUserData = 	pUserData;
	}
#endif
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	if (pUserData)	// Protection Key로 그냥 쓰자
	{
		HxSTD_memcpy(s_pstCxDscrSlot[nIdx].aucProtKey, pUserData, (CX_DSCR_VALUE_BYTES - 16));

		if (local_cxDalDsc_SetProtectionKey(s_pstCxDscrSlot[nIdx].ulDscrHandle, pUserData) != ERR_OK)
		{
			HxLOG_Error("SetProtectionKey() Fail\n");
			VK_SEM_Release(s_ulCxDscSemaId);
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
	else
	{
		if (local_cxDalDsc_ClearProtectionKey(s_pstCxDscrSlot[nIdx].ulDscrHandle) != ERR_OK)
		{
			HxLOG_Error("local_cxDalDsc_ClearProtectionKey() Fail\n");
			VK_SEM_Release(s_ulCxDscSemaId);
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
#endif

	if (usOddKeyLength && pucOddKey)
	{
		HxSTD_memcpy(s_pstCxDscrSlot[nIdx].aucOddKey, pucOddKey, CX_DSCR_KEY_LENGTH);

		local_cxDalDsc_SetCW(s_pstCxDscrSlot[nIdx].ulDscrHandle, 1, s_pstCxDscrSlot[nIdx].aucOddKey);
	}

	if (usEvenKeyLength && pucEvenKey)
	{
		HxSTD_memcpy(s_pstCxDscrSlot[nIdx].aucEvenKey, pucEvenKey, CX_DSCR_KEY_LENGTH);

		local_cxDalDsc_SetCW(s_pstCxDscrSlot[nIdx].ulDscrHandle, 0, s_pstCxDscrSlot[nIdx].aucEvenKey);
	}

	VK_SEM_Release(s_ulCxDscSemaId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

void cxDalDsc_ChangePid(HUINT32 ulCxStreamIdx, HUINT16 usPid)
{
	HUINT32	i;

	ENTER_FUNCTION;

	VK_SEM_Get(s_ulCxDscSemaId);

	if (ulCxStreamIdx == HANDLE_NULL)
	{
		VK_SEM_Release(s_ulCxDscSemaId);
		LEAVE_FUNCTION;
		return;
	}

	// find a slot
	for (i = 0; i < CX_DSCR_SLOT_NUM; i++)
	{
		if (s_pstCxDscrSlot[i].ulCxStreamIdx == ulCxStreamIdx)
		{
			if (s_pstCxDscrSlot[i].usPid != usPid)
			{
				local_cxDalDsc_ChangePid(s_pstCxDscrSlot[i].ulDscrHandle, usPid);
				s_pstCxDscrSlot[i].usPid = usPid;
			}
			break;
		}
	}

	VK_SEM_Release(s_ulCxDscSemaId);
	LEAVE_FUNCTION;
}

#if defined (CONFIG_MW_CAS_CONAX_CPM)
HERROR cxDalDsc_ClearCWPK(HUINT32 ulDscrHandle)
{
	return local_cxDalDsc_ClearProtectionKey(ulDscrHandle);
}
#endif
/*******************************************************************/
#define ________CX_DSCR_Public_Functions_Body_________________________________
/*******************************************************************/
void CXDSCR_Init(void)
{
	return cxDalDsc_Init();
}
void CXDSCR_Exit(void)
{
	return cxDalDsc_Init();
}

HUINT32 CXDSCR_Open(Handle_t hEcm, HUINT32 ulCxStreamIdx, HUINT16 usPid, Handle_t hAction)
{
	return cxDalDsc_Open(hEcm, ulCxStreamIdx, usPid, hAction);
}


HERROR CXDSCR_Close(HUINT32 ulStbDscrHandle, HUINT16 usPid)
{
	return cxDalDsc_Close(ulStbDscrHandle, usPid);
}

HERROR CXDSCR_SetKeys(HUINT32 ulStbDscrHandle, HUINT16 usOddKeyLength, const HUINT8* pucOddKey, HUINT16 usEvenKeyLength, const HUINT8* pucEvenKey, void* pUserData)
{
	return cxDalDsc_SetKeys(ulStbDscrHandle, usOddKeyLength, pucOddKey, usEvenKeyLength, pucEvenKey, pUserData);
}

void CXDSCR_ChangePid(HUINT32 ulCxStreamIdx, HUINT16 usPid)
{
	return cxDalDsc_ChangePid(ulCxStreamIdx, usPid);
}

#if defined (CONFIG_MW_CAS_CONAX_CPM)
HERROR CXDSCR_ClearCWPK(HUINT32 ulDscrHandle)
{
	return cxDalDsc_ClearCWPK(ulDscrHandle);
}
#endif


