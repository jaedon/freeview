/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*
	Viaccess Descrambler API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.19
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"
#include "va_dscr.h"
#include "va_dscr_api.h"
#include "va_dscr_adapt.h"
#include "va_pi_api.h"
#include "vkernel.h"

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW
#include "va_schip_api.h"
#include "va_schip.h"
#endif

#if defined(CONFIG_OP_NORDIG_BOXER_VOD)		// VOD Mosaic temporary code
#include "svc_pvr.h"
#include "vamgr_main.h"
#endif

#include <string.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define VA_DSCR_KEY_LENGTH			8
#define VA_DSCR_SLOT_NUM 				25 	// MAX_DSC_NUMBER in di_dsc.c

typedef struct {
	UINT32 dwStreamHandle; // set by VA_DSCR_Open() and reset by VA_DSCR_Close().
	UINT32 ulDscrHandle; // set after VA_DSCR_AdaptOpen() and reset after VA_DSCR_AdaptClose().
	UINT16 usPid; // set after VA_DSCR_AdaptSetPid() and reset after VA_DSCR_AdaptResetPid() or VA_DSCR_AdaptClose().
	UINT8 aucOddKey[VA_DSCR_KEY_LENGTH];
	UINT8 aucEvenKey[VA_DSCR_KEY_LENGTH];
	void *pUserData; // STB manufacturer data. it was given through VA_DMX_NotifyFilteredSection(). currently not used.
} VA_DSCR_Slot;

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static VA_DSCR_Slot *sVaDscrSlot;

static HULONG sVaDscrSemId;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
#include "di_dsc.h"

void local_va_dscr_ChangePid(DWORD dwStbStreamHandle, HBOOL bIsActive, UINT16 usPid);

/*
	initialize variables and start task
*/
void VA_DSCR_Init()
{
	int nErr;
	int i;

	HxLOG_Print("[Enter]\n");

	nErr = VA_SEM_Create(VA_DSCR_SEM_NAME, &sVaDscrSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	sVaDscrSlot = (VA_DSCR_Slot *)VA_MEM_Alloc(sizeof(VA_DSCR_Slot) * VA_DSCR_SLOT_NUM);
	VA_ASSERT(sVaDscrSlot != NULL);

	for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
	{
		sVaDscrSlot[i].dwStreamHandle = VA_INVALID_HANDLE;
		sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
		sVaDscrSlot[i].usPid = VA_INVALID_PID;
		sVaDscrSlot[i].pUserData = NULL;
	}

	VA_DSCR_AdaptInit(); // 현재로선 불필요한 코드.

	HxLOG_Print("[Exit]\n");
}


/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/

/*
	VA_DSCR_Open
		allocates a descrambling channel to an elementary stream
 	arguments
		dwStbStreamHandle: identify the elemetary stream (set by STB when VA_PI_AddStream() called)
	return
		kVA_ILLEGAL_HANDLE
		dwStbDscrHandle
*/
DWORD VA_DSCR_Open(DWORD dwStbStreamHandle)
{
	DWORD 	dwStbDscrHandle = VA_INVALID_HANDLE;
	int 		i;
	UINT16 	usPid;
	Handle_t 	hVaAction;

	HxLOG_Print("[Enter][%x]\n", dwStbStreamHandle);
	VA_ASSERT(sVaDscrSemId);
	VA_SEM_Get(sVaDscrSemId);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (VA_DSCR_Adapt_IsPlayback(VA_PI_Adapt_GetVaActionHandleByStreamHandle(dwStbStreamHandle)))
	{
		for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
		{
			if (sVaDscrSlot[i].dwStreamHandle == VA_INVALID_HANDLE)
			{
				HxLOG_Info("[VA_DSCR_Open] slot[%d] is allocated\n", i);
				sVaDscrSlot[i].usPid = VA_INVALID_PID;
				sVaDscrSlot[i].pUserData = NULL;
				sVaDscrSlot[i].dwStreamHandle = dwStbStreamHandle;
				sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
				dwStbDscrHandle = i;
				break;
			}
		}

		HxLOG_Print("[playback mode: handle:%x]\n", dwStbDscrHandle);
                VA_SEM_Release(sVaDscrSemId);
		return dwStbDscrHandle;
	}
#endif

	usPid = VA_PI_Adapt_GetPidFromStreamHandle(dwStbStreamHandle);
	if (usPid == VA_INVALID_PID)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong dwStbStreamHandle (%x)\n", dwStbStreamHandle);
		return kVA_ILLEGAL_HANDLE;
	}

	// check if a slot has been opened for dwStbStreamHandle
	for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
	{
		if (sVaDscrSlot[i].dwStreamHandle == dwStbStreamHandle)
		{
			HxLOG_Print("dwStbStreamHandle is already assigned to a descrambler slot(%d, %x, %x)\n", i, sVaDscrSlot[i].dwStreamHandle, sVaDscrSlot[i].ulDscrHandle);

// for driver test, it must return an error when a slot is already open
// in real environment, it needs to go forward without any error
#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			HxLOG_Info("slot(%d,%x,%x,%x) is opened! close it for %x\n", i, sVaDscrSlot[i].dwStreamHandle, sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid, usPid);
			VA_SEM_Release(sVaDscrSemId);
			return kVA_ILLEGAL_HANDLE;
#else
			HxLOG_Error("slot(%d,%x,%x,%x) is opened! close it for %x\n", i, sVaDscrSlot[i].dwStreamHandle, sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid, usPid);
			// 열려있으면 일단 닫고 아래로 가서 다시 연다..
			if (sVaDscrSlot[i].ulDscrHandle != VA_INVALID_HANDLE)
			{
				if (VA_DSCR_AdaptClose(sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid) != ERR_OK)
				{
					// VA_DSCR_AdaptClose()를 실패하더라도 sVaDscrSlot를 초기화.
					sVaDscrSlot[i].dwStreamHandle = VA_INVALID_HANDLE;
					sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
					sVaDscrSlot[i].usPid = VA_INVALID_PID;
					sVaDscrSlot[i].pUserData = NULL;

					VA_SEM_Release(sVaDscrSemId);
					HxLOG_Error("VA_DSCR_AdaptClose(%d,%x,%x,%x)\n", i, sVaDscrSlot[i].dwStreamHandle, sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid);
					return kVA_ILLEGAL_HANDLE;
				}

				sVaDscrSlot[i].dwStreamHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[i].usPid = VA_INVALID_PID;
				sVaDscrSlot[i].pUserData = NULL;
			}
			else
			{
				// strange situation
				sVaDscrSlot[i].dwStreamHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[i].usPid = VA_INVALID_PID;
				sVaDscrSlot[i].pUserData = NULL;
			}
			break;
#endif
		}
	}

	// find a slot
	for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
	{
		if (sVaDscrSlot[i].dwStreamHandle == VA_INVALID_HANDLE)
		{
			HxLOG_Info("slot[%d] is allocated\n", i);
			sVaDscrSlot[i].dwStreamHandle = dwStbStreamHandle;
			sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
			sVaDscrSlot[i].usPid = VA_INVALID_PID;
			sVaDscrSlot[i].pUserData = NULL;
			dwStbDscrHandle = i;
			break;
		}
	}

	// fail to allocate slot
	if (dwStbDscrHandle == VA_INVALID_HANDLE)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("failed to allocate a descrambler\n");
		return kVA_ILLEGAL_HANDLE;
	}

	// open descrambler
	hVaAction = VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[dwStbDscrHandle].dwStreamHandle);

	if (sVaDscrSlot[dwStbDscrHandle].usPid != usPid)
	{
#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
		// live에서 선택받지 못한 pid는 descrambler사용하지 않도록..
		if (VA_PI_Adapt_IsActive(dwStbStreamHandle))
#endif
		{
			if (VA_DSCR_AdaptOpen(hVaAction, (HUINT32 *)&sVaDscrSlot[dwStbDscrHandle].ulDscrHandle, usPid) != ERR_OK)
			{
				// it failed to open a descrambler slot : it shouln't be happend!!
				// VA_DSCR_AdaptOpen()를 실패하면 sVaDscrSlot를 초기화.
				sVaDscrSlot[dwStbDscrHandle].dwStreamHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[dwStbDscrHandle].ulDscrHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[dwStbDscrHandle].usPid = VA_INVALID_PID;
				sVaDscrSlot[i].pUserData = NULL;
				VA_SEM_Release(sVaDscrSemId);
				HxLOG_Error("VA_DSCR_AdaptSetPid(%d,%x,%x,%x)\n", dwStbDscrHandle, sVaDscrSlot[dwStbDscrHandle].dwStreamHandle, sVaDscrSlot[dwStbDscrHandle].ulDscrHandle, usPid);
				return kVA_ILLEGAL_HANDLE;
			}

			// set pid to descrambler
			if (VA_DSCR_AdaptSetPid(sVaDscrSlot[dwStbDscrHandle].ulDscrHandle, usPid) != ERR_OK)
			{
				// VA_DSCR_AdaptSetPid()를 실패하면 VA_DSCR_AdaptClose() 하고 sVaDscrSlot를 초기화.
				VA_DSCR_AdaptClose(sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid);

				sVaDscrSlot[dwStbDscrHandle].dwStreamHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[dwStbDscrHandle].ulDscrHandle = VA_INVALID_HANDLE;
				sVaDscrSlot[dwStbDscrHandle].usPid = VA_INVALID_PID;
				sVaDscrSlot[i].pUserData = NULL;

				VA_SEM_Release(sVaDscrSemId);
				HxLOG_Error("VA_DSCR_AdaptSetPid(%d,%x,%x,%x)\n", dwStbDscrHandle, sVaDscrSlot[dwStbDscrHandle].dwStreamHandle, sVaDscrSlot[dwStbDscrHandle].ulDscrHandle, usPid);
				return kVA_ILLEGAL_HANDLE;
			}
		}

		sVaDscrSlot[dwStbDscrHandle].usPid = usPid;
	}

	HxLOG_Info("slot(%d,%x,%x,%x)\n", dwStbDscrHandle, sVaDscrSlot[dwStbDscrHandle].dwStreamHandle, sVaDscrSlot[dwStbDscrHandle].ulDscrHandle, sVaDscrSlot[dwStbDscrHandle].usPid);

	VA_SEM_Release(sVaDscrSemId);
	HxLOG_Print("[Exit](%x)\n", sVaDscrSlot[dwStbDscrHandle].ulDscrHandle);
	return dwStbDscrHandle;
}

/*
	VA_DSCR_Close
		close a descrambling channel
	arguments
		dwStbDscrHandle: descrambler handle returned by VA_DSCR_Open()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_DSCR_Close(DWORD dwStbDscrHandle)
{
	UINT32 nIdx;

	HxLOG_Print("[Enter][%x]\n", dwStbDscrHandle);

	nIdx = (int)dwStbDscrHandle;

	VA_ASSERT(sVaDscrSemId);
	VA_SEM_Get(sVaDscrSemId);

	// dwStbDscrHandle does not exist
	if (nIdx >= VA_DSCR_SLOT_NUM || sVaDscrSlot[nIdx].dwStreamHandle == VA_INVALID_HANDLE)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong dwStbStreamHandle or already closed (%d)\n", nIdx);
		return kVA_INVALID_PARAMETER;
	}

#if !defined(CONFIG_MW_CAS_VIACCESS_PVR) // live에서 선택받지 못한 pid는 descrambler slot을 할당받지 못하므로 아래와 같이 check해서는 안된다.
	if (sVaDscrSlot[nIdx].ulDscrHandle == VA_INVALID_HANDLE)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong ulDscrHandle\n");
		return kVA_INVALID_PARAMETER;
	}
#endif

	// close descrambler
	HxLOG_Info("slot(%d,%x,%x,%x)\n", nIdx, sVaDscrSlot[nIdx].dwStreamHandle, sVaDscrSlot[nIdx].ulDscrHandle, sVaDscrSlot[nIdx].usPid);

	if (sVaDscrSlot[nIdx].ulDscrHandle != VA_INVALID_HANDLE)
	{
		if (VA_DSCR_AdaptClose(sVaDscrSlot[nIdx].ulDscrHandle, sVaDscrSlot[nIdx].usPid) != ERR_OK)
		{
			VA_SEM_Release(sVaDscrSemId);
			HxLOG_Error("VA_DSCR_AdaptClose(%d,%x,%x,%x)\n", nIdx, sVaDscrSlot[nIdx].dwStreamHandle, sVaDscrSlot[nIdx].ulDscrHandle, sVaDscrSlot[nIdx].usPid);
			return kVA_INVALID_PARAMETER;
		}
	}

	sVaDscrSlot[nIdx].dwStreamHandle = VA_INVALID_HANDLE;
	sVaDscrSlot[nIdx].ulDscrHandle = VA_INVALID_HANDLE;
	sVaDscrSlot[nIdx].usPid = VA_INVALID_PID;

	VA_SEM_Release(sVaDscrSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_DSCR_SetParameters
		set descrambler with control words

	arguments
		dwStbDscrHandle: descrambler handle returned by VA_DSCR_Open()
		pstParameters :
						This parameter contains initialization data for the descrambler and
						it is a pointer to at VA_DSCR_ConfigurationParameters structure.
						The memory is allocated by Viaccess and it is freed
						after the function’s return. This parameter cannot be NULL.

						The field eContentScramblingAlgo is one of the tVA_DSCR_ScramblingAlgo enumerations.
						If the eSCRAMBLING_ALGO_AES_128_CBC algorithm is specified, the pointer pIV is not NULL
						and the pointed buffer is 16 bytes long (as indicated by uiIVLength).
						If eSCRAMBLING_ALGO_DVB_CSA is specified, the pointer pIV is NULL and uiIVLength is set to 0.
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_NOT_IMPLEMENTED
*/
INT VA_DSCR_SetParameters( DWORD dwStbDscrHandle, const tVA_DSCR_ConfigurationParameters *pstParameters )
{
	/* TODO : make this , hjlee3
			how can I ??

			PAL_DSC_Open -> DI_DSC_Open dp
			DI_DSC_EncryptionSetting 부분 확인 해보자.
			next time work
	*/

	return kVA_OK;
}


/*
	VA_DSCR_SetKeys
		set descrambler with control words
	arguments
		dwStbDscrHandle: descrambler handle returned by VA_DSCR_Open()
		uiOddKeyLength
		pOddKey: allocated and freed by VA
		uiEvenKeyLength
		pEvenKey: allocated and freed by VA
		pUserData: STB manufacturer data. it was given through VA_DMX_NotifyFilteredSection()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_DSCR_SetKeys(DWORD dwStbDscrHandle, UINT16 uiOddKeyLength, const BYTE *pOddKey, UINT16 uiEvenKeyLength, const BYTE *pEvenKey, void *pUserData)
{
	UINT32 nIdx;
	UINT16 usPid;
#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST) && defined(CONFIG_MW_CAS_VIACCESS_PVR)
	int i;
#endif

	HxLOG_Print("[Enter][%x]\n", dwStbDscrHandle);

	// key lengths are not supported
	if (uiOddKeyLength > VA_DSCR_KEY_LENGTH || uiEvenKeyLength > VA_DSCR_KEY_LENGTH)
	{
		HxLOG_Error("wrong key length (%d,%d)\n", uiOddKeyLength, uiEvenKeyLength);
		return kVA_INVALID_PARAMETER;
	}

	if (pOddKey == NULL && pEvenKey == NULL)
	{
		HxLOG_Error("wrong key (%x,%x)\n", pOddKey, pEvenKey);
		return kVA_INVALID_PARAMETER;
	}

	nIdx = (int)dwStbDscrHandle;

	VA_ASSERT(sVaDscrSemId);
	VA_SEM_Get(sVaDscrSemId);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (dwStbDscrHandle < VA_DSCR_SLOT_NUM &&
		VA_DSCR_Adapt_IsPlayback(VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[dwStbDscrHandle].dwStreamHandle)))
	{
		VaPrint(DBG_PRINT, ("[playback mode: exit (0x%x,%d, 0x%x,%d, 0x%x, 0x%x)]\n", (unsigned int)dwStbDscrHandle, uiOddKeyLength, (unsigned int)pOddKey, uiEvenKeyLength, (unsigned int)pEvenKey, (unsigned int)pUserData));
		VA_SEM_Release(sVaDscrSemId);
		return kVA_OK;
	}
#endif

	// dwStbDscrHandle does not exist
	if (nIdx >= VA_DSCR_SLOT_NUM || sVaDscrSlot[nIdx].dwStreamHandle == VA_INVALID_HANDLE)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong dwStbDscrHandle (%d)\n", nIdx);
		return kVA_INVALID_PARAMETER;
	}

	if (sVaDscrSlot[nIdx].ulDscrHandle == VA_INVALID_HANDLE)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong ulDscrHandle\n");
		return kVA_INVALID_PARAMETER;
	}

	// check if pid is changed
	usPid = VA_PI_Adapt_GetPidFromStreamHandle(sVaDscrSlot[nIdx].dwStreamHandle);
	if (usPid == VA_INVALID_PID)
	{
		VA_SEM_Release(sVaDscrSemId);
		HxLOG_Error("wrong dwStbStreamHandle (%x) for %x\n", sVaDscrSlot[nIdx].dwStreamHandle, nIdx);
		return kVA_INVALID_PARAMETER;
	}

	// set key
	HxLOG_Info("slot(%d,%x,%x,%x,%x) with odd=%x(%d) and even=%x(%d)\n",
		nIdx, sVaDscrSlot[nIdx].dwStreamHandle, sVaDscrSlot[nIdx].ulDscrHandle, sVaDscrSlot[nIdx].usPid, usPid,
		pOddKey, uiOddKeyLength, pEvenKey, uiEvenKeyLength);

	if (sVaDscrSlot[nIdx].usPid != usPid)
	{
		// 여기서는 동일 ES가 pid만 바뀐 경우다.
		HxLOG_Info("local_va_dscr_ChangePid(%x,%x) calling...\n", sVaDscrSlot[nIdx].dwStreamHandle, usPid);
		local_va_dscr_ChangePid(sVaDscrSlot[nIdx].dwStreamHandle, VA_PI_Adapt_IsActive(sVaDscrSlot[nIdx].dwStreamHandle), usPid);
	}

#if defined(CONFIG_PROD_CPU_BCM) && defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
 	VA_SCHIP_SetProtectKey(sVaDscrSlot[nIdx].ulDscrHandle);
#endif

	if (pOddKey)
	{
		VA_memcpy(sVaDscrSlot[nIdx].aucOddKey, pOddKey, VA_DSCR_KEY_LENGTH);

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW
		if (VA_SCHIP_GetCwMode() == eINACTIVE)
#endif
		{
			// calculate entropy - in encrypted CW mode, this calculation must be avoided. it can destroy keys.
			sVaDscrSlot[nIdx].aucOddKey[3] = pOddKey[0] + pOddKey[1] + pOddKey[2];
			sVaDscrSlot[nIdx].aucOddKey[7] = pOddKey[4] + pOddKey[5] + pOddKey[6];
		}

		if (sVaDscrSlot[nIdx].ulDscrHandle != VA_INVALID_HANDLE)
		{
			VA_DSCR_AdaptSetKey(sVaDscrSlot[nIdx].ulDscrHandle, 1, sVaDscrSlot[nIdx].aucOddKey);

#if defined(CONFIG_OP_NORDIG_BOXER_VOD)		// VOD Mosaic temporary code
#if defined(BOXER_VOD_MOSAIC)
			{
				Handle_t hVaVodAction = HANDLE_NULL;
				Handle_t hVodAction = HANDLE_NULL;

				hVaVodAction = VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[nIdx].dwStreamHandle);

				//HxLOG_Print(" >>> idx(%d) dwStreamHandle(0x%X) hVaVodAction(0x%X) \n", nIdx, sVaDscrSlot[nIdx].dwStreamHandle, hVaVodAction);
				if (hVaVodAction != HANDLE_NULL)
				{
					hVodAction = VAMGR_GetActionHandle(hVaVodAction);

					//HxLOG_Print(" >>>>> hVodAction(0x%X)  \n", hVodAction);

					svc_cas_MgrSubUpdateCasEtcEvent(hVodAction, HANDLE_NULL, eDxCAS_GROUPID_VA,
														(EVT_PB_BOXERVOD_SETKEY, 0, 0, 0);
				}
			}
#endif
#endif
		}
	}

	if (pEvenKey)
	{
		VA_memcpy(sVaDscrSlot[nIdx].aucEvenKey, pEvenKey, VA_DSCR_KEY_LENGTH);

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW
		if (VA_SCHIP_GetCwMode() == eINACTIVE)
#endif
		{
			// calculate entropy - in encrypted CW mode, this calculation must be avoided. it can destroy keys.
			sVaDscrSlot[nIdx].aucEvenKey[3] = pEvenKey[0] + pEvenKey[1] + pEvenKey[2];
			sVaDscrSlot[nIdx].aucEvenKey[7] = pEvenKey[4] + pEvenKey[5] + pEvenKey[6];
		}

		if (sVaDscrSlot[nIdx].ulDscrHandle != VA_INVALID_HANDLE)
		{
			VA_DSCR_AdaptSetKey(sVaDscrSlot[nIdx].ulDscrHandle, 0, sVaDscrSlot[nIdx].aucEvenKey);

#if defined(CONFIG_OP_NORDIG_BOXER_VOD)		// VOD Mosaic temporary code
#if defined(BOXER_VOD_MOSAIC)
			{
				Handle_t hVaVodAction = HANDLE_NULL;
				Handle_t hVodAction = HANDLE_NULL;

				hVaVodAction = VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[nIdx].dwStreamHandle);

				//HxLOG_Print(" >>> idx(%d) dwStreamHandle(0x%X) hVaVodAction(0x%X) \n", nIdx, sVaDscrSlot[nIdx].dwStreamHandle, hVaVodAction);
				if (hVaVodAction != HANDLE_NULL)
				{
					hVodAction = VAMGR_GetActionHandle(hVaVodAction);

					//HxLOG_Print(" >>>>> hVodAction(0x%X)  \n", hVodAction);

					svc_cas_MgrSubUpdateCasEtcEvent(hVodAction, HANDLE_NULL, eDxCAS_GROUPID_VA,
														EVT_PB_BOXERVOD_SETKEY, 0, 0, 0);
				}
			}
#endif
#endif
		}
	}

	VA_SEM_Release(sVaDscrSemId);

#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST) && defined(CONFIG_MW_CAS_VIACCESS_PVR)
	// 이게 live action에 대한 처리였다면, 다른 action중에 같은 svc인 것이 있나 확인해보자..
	if (VAMGR_IsLive(VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[nIdx].dwStreamHandle)))
	{
		for (i = 0; i <VA_DSCR_SLOT_NUM; i++)
		{
			if (i != nIdx &&
				sVaDscrSlot[i].dwStreamHandle != HANDLE_NULL &&
				VAMGR_IsSameSvcWithLive(VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[i].dwStreamHandle)) &&
 				sVaDscrSlot[i].usPid == usPid)
 			{
				VA_DSCR_SetKeys(i, uiOddKeyLength, pOddKey, uiEvenKeyLength, pEvenKey, pUserData);
				break;
 			}
		}
	}
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

void local_va_dscr_ChangePid(DWORD dwStbStreamHandle, HBOOL bIsActive, UINT16 usPid)
{
	UINT32 i;
	Handle_t 	hVaAction;

	HxLOG_Print("[Enter](%x,%x)\n", dwStbStreamHandle, usPid);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (VA_DSCR_Adapt_IsPlayback(VA_PI_Adapt_GetVaActionHandleByStreamHandle(dwStbStreamHandle)))
	{
		HxLOG_Error("[playback mode: exit]\n");
		return;
	}
#endif

	if (dwStbStreamHandle == VA_INVALID_HANDLE || usPid == VA_INVALID_PID)
	{
		HxLOG_Error("wrong dwStbStreamHandle (%x,%x)\n", dwStbStreamHandle, usPid);
		return;
	}

	for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
	{
		if (sVaDscrSlot[i].dwStreamHandle == dwStbStreamHandle)
		{
			if (sVaDscrSlot[i].ulDscrHandle != VA_INVALID_HANDLE)
			{
				//  닫고 다시 연다.. (pid만 바꿨다간 demux에서 어떻게 꼬일지 모름)
				if (VA_DSCR_AdaptClose(sVaDscrSlot[i].ulDscrHandle, sVaDscrSlot[i].usPid) != ERR_OK)
			{
					HxLOG_Info("VA_DSCR_AdaptClose error\n");
				return;
			}
				sVaDscrSlot[i].ulDscrHandle = VA_INVALID_HANDLE;
			}
			break;
		}
	}

	for (i = 0; i < VA_DSCR_SLOT_NUM; i++)
			{
		if (sVaDscrSlot[i].dwStreamHandle == dwStbStreamHandle)
		{
			// live에서 선택받지 못한 pid는 descrambler사용하지 않도록..
			if (bIsActive)
			{
				hVaAction = VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[i].dwStreamHandle);
				if (VA_DSCR_AdaptOpen(hVaAction, (HUINT32 *)&sVaDscrSlot[i].ulDscrHandle, usPid) != ERR_OK)
				{
					// it failed to open a descrambler slot : it shouln't be happend!!
					HxLOG_Info("VA_DSCR_AdaptOpen error\n");
					return;
				}

				// set pid to descrambler
				if (VA_DSCR_AdaptSetPid(sVaDscrSlot[i].ulDscrHandle, usPid) != ERR_OK)
				{
					HxLOG_Info("VA_DSCR_AdaptSetPid error\n");
					return;
				}

			sVaDscrSlot[i].usPid = usPid;

#if defined(CONFIG_PROD_CPU_BCM) && defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
 			VA_SCHIP_SetProtectKey(sVaDscrSlot[i].ulDscrHandle);
#endif

			// key를 다시 세팅해버린다. (혹시 같은 key면 빨리 나올테니까..)
				VA_DSCR_AdaptSetKey(sVaDscrSlot[i].ulDscrHandle, 1, sVaDscrSlot[i].aucOddKey);
				VA_DSCR_AdaptSetKey(sVaDscrSlot[i].ulDscrHandle, 0, sVaDscrSlot[i].aucEvenKey);

#if defined(CONFIG_OP_NORDIG_BOXER_VOD)		// VOD Mosaic temporary code
#if defined(BOXER_VOD_MOSAIC)
				{
					Handle_t hVaVodAction = HANDLE_NULL;
					Handle_t hVodAction = HANDLE_NULL;

					hVaVodAction = VA_PI_Adapt_GetVaActionHandleByStreamHandle(sVaDscrSlot[i].dwStreamHandle);

					if (hVaVodAction != HANDLE_NULL)
					{
						hVodAction = VAMGR_GetActionHandle(hVaVodAction);
						svc_cas_MgrSubUpdateCasEtcEvent(hVodAction, HANDLE_NULL, eDxCAS_GROUPID_VA,
															EVT_PB_BOXERVOD_SETKEY, 0, 0, 0);
					}
				}
#endif
#endif

			}

			break;
		}
	}

	HxLOG_Print("[Exit]\n");
}

void VA_DSCR_ChangePid(DWORD dwStbStreamHandle, HBOOL bIsActive, UINT16 usPid)
{
	HxLOG_Print("[Enter](%x,%x)\n", dwStbStreamHandle, usPid);

	VA_ASSERT(sVaDscrSemId);
	VA_SEM_Get(sVaDscrSemId);

	local_va_dscr_ChangePid(dwStbStreamHandle, bIsActive, usPid);

	VA_SEM_Release(sVaDscrSemId);

	HxLOG_Print("[Exit]\n");
}

