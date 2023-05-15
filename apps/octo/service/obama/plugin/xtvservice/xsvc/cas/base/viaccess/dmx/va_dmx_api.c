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
	Viaccess Demux API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.7
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"
#include "va_dmx.h"
#include "va_dmx_api.h"
#include "va_dmx_adapt.h"
#include "vkernel.h"
#include "va_main.h"
#include "va_pi_api.h"

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
#include "va_pvr_adapt.h"
#endif

#include <string.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//#define SHOW_FILTER_COUNT
//#define SHOW_FILTER_PARAM
//#define SHOW_ECM
//#define SHOW_EMM


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/
HULONG g_ulVaDmxSemId;

VA_DMX_Filter_t *g_ulVaDmxFilter;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static HUINT16 local_va_dmx_FindFreeSecFilter();
static void local_va_dmx_BufferAddSection(VA_DMX_Filter_t *pFilter, HUINT8 *pucSection, HUINT16 usLength);
static void local_va_dmx_BufferDeleteAll(VA_DMX_Filter_t *pFilter, HBOOL bFreeMem);

// increase buffer counter and set next position to be written
#define local_va_dmx_IncBuffer(p)	{p->ucBufferCnt++; p->ucBufferWritePos = (p->ucBufferWritePos + 1) % p->usNumBuffer;}
// decrease buffer counter and set next position to be read
#define local_va_dmx_DecBuffer(p)	{p->ucBufferCnt--; p->ucBufferReadPos = (p->ucBufferReadPos + 1) % p->usNumBuffer;}

/*
	initialize variables and start task
*/
void VA_DMX_Init()
{
	int nErr;
	int i, j;

	HxLOG_Print("[Enter]\n");

	nErr = VA_SEM_Create(VA_DMX_SEM_NAME, &g_ulVaDmxSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	g_ulVaDmxFilter = (VA_DMX_Filter_t *)VA_MEM_Alloc(sizeof(VA_DMX_Filter_t) * VA_DMX_MAX_FILTER);
	VA_ASSERT(g_ulVaDmxFilter != NULL);

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		g_ulVaDmxFilter[i].eStatus = VA_DMX_FILTER_FREE;
		g_ulVaDmxFilter[i].ucBufferWritePos = 0;
		g_ulVaDmxFilter[i].ucBufferReadPos = 0;
		g_ulVaDmxFilter[i].ucBufferCnt = 0;
		g_ulVaDmxFilter[i].bBufferLocked 	= FALSE;

		for (j = 0; j < VA_DMX_MAX_BUFFER; j++)
		{
			g_ulVaDmxFilter[i].apSectionBuffer[j] = NULL;
			g_ulVaDmxFilter[i].aulSectionLength[j] = 0;
		}
	}

	HxLOG_Print("[Exit]\n");
}

static HUINT16 local_va_dmx_FindFreeSecFilter()
{
	int i;

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_FREE)
			return i;
	}

	return	VA_DMX_ILLEGAL_FILTER;
}

#ifdef SHOW_FILTER_COUNT
static int local_va_dmx_ShowAllocatedSecFilterCount()
{
	int i, cnt = 0;

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		if (g_ulVaDmxFilter[i].eStatus != VA_DMX_FILTER_FREE)
			cnt++;
	}

	return cnt;
}
#endif

static void local_va_dmx_BufferAddSection(VA_DMX_Filter_t *pFilter, HUINT8 *pucSection, HUINT16 usLength)
{
	int nNextOldestSection;

	// if buffer is full, delete the oldest section and then add new section
	if (pFilter->ucBufferCnt == pFilter->usNumBuffer)
	{
		if (pFilter->bBufferLocked == TRUE)
		{
			// if the oldest section is locked, then delete the next oldest section
			// the next oldest section will be replaced by the oldest section
			nNextOldestSection = (pFilter->ucBufferReadPos + 1) % pFilter->usNumBuffer;
			VA_memcpy(pFilter->apSectionBuffer[nNextOldestSection], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], pFilter->aulSectionLength[pFilter->ucBufferReadPos]);
			pFilter->aulSectionLength[nNextOldestSection] = pFilter->aulSectionLength[pFilter->ucBufferReadPos];
			// = local_va_dmx_DecBuffer()
			pFilter->ucBufferCnt--;
			pFilter->ucBufferReadPos = nNextOldestSection;
		}
		else
		{
			// if the oldest section is not locked, then delete it
			local_va_dmx_DecBuffer(pFilter);
		}
	}

	// driver test 중 DMX_LOAD_SECTION_9에서 1024byte가 넘는 section이 전달되는 경우 있음
	if (usLength > VA_SI_SECTION_SIZE)
	{
		VA_MEM_Free(pFilter->apSectionBuffer[pFilter->ucBufferWritePos]);
		pFilter->apSectionBuffer[pFilter->ucBufferWritePos] = (HUINT8 *)VA_MEM_Alloc(usLength);
	}

	VA_memcpy(pFilter->apSectionBuffer[pFilter->ucBufferWritePos], pucSection, usLength);
	pFilter->aulSectionLength[pFilter->ucBufferWritePos] = usLength;
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	pFilter->ulTimeStamp[pFilter->ucBufferWritePos] = VA_PVR_Rec_GetTimecode(pFilter->ulAcsId);
#endif
	local_va_dmx_IncBuffer(pFilter);
}

static void local_va_dmx_BufferDeleteAll(VA_DMX_Filter_t *pFilter, HBOOL bFreeMem)
{
	int i;

	if (bFreeMem)
	{
		for (i = 0; i < VA_DMX_MAX_BUFFER; i++) // pFilter->usNumBuffer 만큼만 돌려도 되겠음.
		{
			if (pFilter->apSectionBuffer[i])
			{
				VA_MEM_Free(pFilter->apSectionBuffer[i]);
				pFilter->apSectionBuffer[i] = NULL;
				pFilter->aulSectionLength[i] = 0;
			}
		}
	}

	pFilter->ucBufferWritePos = 0;
	pFilter->ucBufferReadPos = 0;
	pFilter->ucBufferCnt = 0;
}

/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/

/*
	VA_DMX_AllocateSectionFilter
		allocates a filter to a PID
	arguments
		dwAcsId: ACS instance
		dwVaFilterHandle: VA uses this value to identify a filter instance
		wPid: VA may allocate several filters with the same PID
		uiNbBuffers: number of buffers to store sections (1 ~ kVA_SETUP_NBMAX_SECTIONBUFFERS
	return
		kVA_ILLEGAL_HANDLE
		dwStbFilterHandle
*/
#ifdef __VIACCESS50__
DWORD VA_DMX_AllocateSectionFilter(DWORD dwAcsId, DWORD dwVaFilterHandle, WORD  wPid, tVA_DMX_NotifyFilteredSection pfVA_DMX_NotifyFilteredSection)
#else
DWORD VA_DMX_AllocateSectionFilter(DWORD dwAcsId, DWORD dwVaFilterHandle, WORD wPid, UINT16 uiNbBuffers)
#endif
{
	VA_DMX_Filter_t *pFilter;
	HUINT16 usFilterId;
#ifdef __VIACCESS50__
	UINT16 uiNbBuffers=4;
#endif
	int i;
	HxLOG_Info("[Enter(acs:%d,handle:%x,pid:%x,nb:%d)]\n", dwAcsId, dwVaFilterHandle, wPid, uiNbBuffers);

	if (dwAcsId >= VA_MAIN_ACS_NUM)
	{
		HxLOG_Error("wrong dwAcsId (%d)\n", dwAcsId);
		return kVA_ILLEGAL_HANDLE;
	}

	if (dwVaFilterHandle == kVA_ILLEGAL_HANDLE)
	{
		HxLOG_Error("wrong dwVaFilterHandle (%x)\n", dwVaFilterHandle);
		return kVA_ILLEGAL_HANDLE;
	}

	if (wPid > 0x1FFF)
	{
		HxLOG_Error("wrong wPid (%x)\n", wPid);
		return kVA_ILLEGAL_HANDLE;
	}

	if (uiNbBuffers == 0 || uiNbBuffers > kVA_SETUP_NBMAX_SECTIONBUFFERS)
	{
		HxLOG_Error("wrong uiNbBuffers (%d) \n", uiNbBuffers);
		return kVA_ILLEGAL_HANDLE;
	}

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	// allocate filter
	usFilterId = local_va_dmx_FindFreeSecFilter();
	if (usFilterId == VA_DMX_ILLEGAL_FILTER)
	{
		HxLOG_Error("fail to allocated filter\n");
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_ILLEGAL_HANDLE;
	}

	pFilter = &g_ulVaDmxFilter[usFilterId];

	pFilter->ulAcsId			= dwAcsId;
	pFilter->ulVaFilterHandle	= dwVaFilterHandle;
/* hjlee3, just temp
   How can I process uiNbBuffers ??
   thinking
*/
	pFilter->usNumBuffer		= uiNbBuffers;

	pFilter->usPid				= wPid;
	pFilter->eStatus			= VA_DMX_FILTER_ALLOCATED;
	pFilter->eMode				= VA_DMX_FILTER_MODE_INVALID;
	pFilter->usLoadId			= VA_DMX_FILTER_LOADID_INVALID;
	pFilter->ucBufferWritePos	= 0;
	pFilter->ucBufferReadPos	= 0;
	pFilter->ucBufferCnt		= 0;
	pFilter->bBufferLocked 		= FALSE;
#ifdef __VIACCESS50__
	pFilter->pfVa_DMX_NotifyFilteredSection = &pfVA_DMX_NotifyFilteredSection;
#endif
	// ECM/EMM 섹션이 처리될 때마다 매번 memory alloc/free 를 하지 않도록 여기서 최대 섹션 사이즈만큼 미리 잡아둔다.
	// Viaccess 가 요구하는 버퍼 수가 통상 4 이므로 4 x VA_SI_SECTION_SIZE 정도의 메모리가 미리 할당된다.
	for (i = 0; i < uiNbBuffers; i++)
	{
		pFilter->apSectionBuffer[i] = (HUINT8 *)VA_MEM_Alloc(VA_SI_SECTION_SIZE);
		VA_ASSERT(pFilter->apSectionBuffer[i]);
	}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (VA_PI_Adapt_IsPlayback(dwAcsId))
	{
		pFilter->eMode = VA_DMX_FILTER_MODE_PVR;
	}
#endif

#ifdef SHOW_FILTER_COUNT
	HxLOG_Print("[VA_DMX_AllocateSectionFilter] total %d allocated now\n", local_va_dmx_ShowAllocatedSecFilterCount());
#endif

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Info("[Exit(%x)]\n", usFilterId);
	return (DWORD)usFilterId;
}

/*
	VA_DMX_FreeSectionFilter
		frees a filter
	arguments
		dwStbFilterHandle: filter handle returned by VA_DMX_AllocateSectionFilter()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_STILL_LOCKED
		kVA_STOP_FILTER_FIRST
*/
INT VA_DMX_FreeSectionFilter(DWORD dwStbFilterHandle)
{
	HUINT32 			ulIdx;
	VA_DMX_Filter_t 	*pFilter;

	HxLOG_Info("[Enter(%x)]\n", dwStbFilterHandle);

	ulIdx = dwStbFilterHandle;

	// dwStbFilterHandle does not exist
	if (ulIdx >= VA_DMX_MAX_FILTER)
	{
		HxLOG_Error("wrong dwStbFilterHandle (%x)\n", ulIdx);
		return kVA_INVALID_PARAMETER;
	}

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	pFilter = &g_ulVaDmxFilter[ulIdx];

	// filter is not allocated or alreay freed
	if (pFilter->eStatus == VA_DMX_FILTER_FREE)
	{
		HxLOG_Error("filter is not allocated or alreay freed\n");
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_INVALID_PARAMETER;
	}

	// filter is not stopped
	if (pFilter->eStatus == VA_DMX_FILTER_RUNNING)
	{
		HxLOG_Error("filter is not stopped\n");
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_STOP_FILTER_FIRST;
	}

	 //section이 burst하게 들어오면서, ACS lib와 MW간 sync문제가 있다.
	 //즉, unlock -> stop -> free 순으로 call되어야 하나 stop -> unlock -> free 순으로 call되면서 section free를 못해버림.
	 //따라서 일단 filter stop이 되고 난 상태에서는 free를 해줄 수 있도록 WA 함.
	// a buffer is locked
#if !defined(__VIACCESS50__)
	if (pFilter->bBufferLocked == TRUE && pFilter->eStatus != VA_DMX_FILTER_STOPPED)
	{
		HxLOG_Error("buffer %x is locked\n", ulIdx);
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_RESOURCE_STILL_LOCKED;
	}
#endif
	// free filter
	local_va_dmx_BufferDeleteAll(pFilter, TRUE);
	pFilter->eStatus = VA_DMX_FILTER_FREE;

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_DMX_SetSectionFilterParameters
		set value and mask of a given filter
	arguments
		dwStbFilterHandle: filter handle returned by VA_DMX_AllocateSectionFilter()
		uiLength: size of filter value and mask
		pValue: filter value (allocated and freed by VA)
		pMask: filter mask (allocated and freed by VA)
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
#if defined(__VIACCESS50__)
INT VA_DMX_SetSectionFilterParameters(DWORD dwStbFilterHandle, UINT32 uiLength, BYTE *pValue, BYTE *pMask, tVA_DMX_NotificationMode eNotificationMode)
#else
INT VA_DMX_SetSectionFilterParameters(DWORD dwStbFilterHandle, UINT32 uiLength, BYTE *pValue, BYTE *pMask)
#endif
{
	HUINT32 			ulIdx;
//	HUINT8 			*pLockedSection;
//	HUINT16 			usLockedSectionLen;
	VA_DMX_Filter_t 	*pFilter;
	Handle_t			hVaAction;

	HxLOG_Info("[Enter(%x)]\n", dwStbFilterHandle);

	ulIdx = dwStbFilterHandle;

	// dwStbFilterHandle does not exist
	if (ulIdx >= VA_DMX_MAX_FILTER || g_ulVaDmxFilter[ulIdx].eStatus == VA_DMX_FILTER_FREE)
	{
		HxLOG_Error("wrong dwStbFilterHandle (%x)\n", ulIdx);
		return kVA_INVALID_PARAMETER;
	}

	if (uiLength > kVA_DMX_MAX_FILTER_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", uiLength);
		return kVA_INVALID_PARAMETER;
	}

	if (uiLength > 0 && (pValue == NULL || pMask == NULL))
	{
		HxLOG_Error("no filter value or mask (%x, %x)\n", pValue, pMask);
		return kVA_INVALID_PARAMETER;
	}

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	// set filter
	// even if the filter is already running, filter parameter should be changed without interruption
	pFilter = &g_ulVaDmxFilter[ulIdx];

#ifdef SHOW_FILTER_PARAM

	{
		int i;

		HxLOG_Print("<%d> PID[%x]", dwStbFilterHandle, pFilter->usPid);
		HxLOG_Print(" Filter[%d]:", uiLength);
		for (i = 0; i < uiLength; i++)
			HxLOG_Print(" %02X", pValue[i]);
		HxLOG_Print(" Mask[%d]:", uiLength);
		for (i = 0; i < uiLength; i++)
			HxLOG_Print(" %02X", pMask[i]);
		HxLOG_Print("\n");
	}
#endif

	// if filter is running, stop it
	if (pFilter->eStatus == VA_DMX_FILTER_RUNNING)
	{
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		if (pFilter->eMode == VA_DMX_FILTER_MODE_PVR)
		{
			// 아무것도 안한다..
		}
		else
#endif
		if (pFilter->eMode == VA_DMX_FILTER_MODE_ECM_0 || pFilter->eMode == VA_DMX_FILTER_MODE_ECM_1)
		{
			VA_DMX_StopEcmSection(pFilter->usLoadId);
			HxLOG_Print("[%x][%x] VA_DMX_StopEcmSection(%x)\n", dwStbFilterHandle, pFilter->usPid, pFilter->usLoadId);
		}
		else
		{
			VA_DMX_StopEmmSection(pFilter->usLoadId);
			HxLOG_Print("[%x][%x] VA_DMX_StopEmmSection(%x)\n", dwStbFilterHandle, pFilter->usPid, pFilter->usLoadId);
		}

		// if filter criteria is changed, STB must remove all of the unlocked sections (do not remove locked section)
		if (pFilter->aucValue[0] != pValue[0] || VA_memcmp(pFilter->aucValue, pValue, uiLength))
		{
			if (pFilter->bBufferLocked)
			{
				// delete all except the locked section
				pFilter->ucBufferWritePos = (pFilter->ucBufferReadPos + 1) % pFilter->usNumBuffer;
				pFilter->ucBufferCnt = 1;
			}
			else
			{
				// delete all
				local_va_dmx_BufferDeleteAll(pFilter, FALSE);
			}
		}
	}

	pFilter->ucFilterLength = (UINT8)uiLength;

	VA_memset(pFilter->aucValue, 0x00, VA_DMX_FILTER_LENGTH);
	VA_memset(pFilter->aucMask, 0x00, VA_DMX_FILTER_LENGTH);	// Don't Care

	if (uiLength)
	{
		VA_memcpy(pFilter->aucValue, pValue, uiLength);
		VA_memcpy(pFilter->aucMask, pMask, uiLength);
	}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (pFilter->eMode == VA_DMX_FILTER_MODE_PVR)
	{
		VA_SEM_Release(g_ulVaDmxSemId);

//		VaPrint(DBG_PRINT, ("[Exit]\n"));
		return kVA_OK;
	}
#endif

	switch (pFilter->aucValue[0])
	{
		case	VA_ECM_0 :		pFilter->eMode = VA_DMX_FILTER_MODE_ECM_0; break;
		case	VA_ECM_1 :		pFilter->eMode = VA_DMX_FILTER_MODE_ECM_1; break;
		case	VA_EMMGA_0 : 	pFilter->eMode = VA_DMX_FILTER_MODE_EMMGA_0; break;
		case	VA_EMMGA_1 : 	pFilter->eMode = VA_DMX_FILTER_MODE_EMMGA_1; break;
		case	VA_EMMGH_0 : 	pFilter->eMode = VA_DMX_FILTER_MODE_EMMGH_0; break;
		case	VA_EMMGH_1 : 	pFilter->eMode = VA_DMX_FILTER_MODE_EMMGH_1; break;
		case	VA_EMMU :		pFilter->eMode = VA_DMX_FILTER_MODE_EMMU; break;
		case	VA_EMMS :		pFilter->eMode = VA_DMX_FILTER_MODE_EMMS; break;
		default :
			pFilter->eMode = VA_DMX_FILTER_MODE_UNKNOWN;
			HxLOG_Info("Unknown mode [%x]\n", pFilter->aucValue[0]);
			break;
	}

	if (pFilter->eStatus == VA_DMX_FILTER_RUNNING)
	{
		hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(pFilter->ulAcsId);

		if (pFilter->eMode == VA_DMX_FILTER_MODE_ECM_0 || pFilter->eMode == VA_DMX_FILTER_MODE_ECM_1)
		{
			HxLOG_Print("[%d][%x] SI_ECM_Retrieve : tid[0x%x] mask[0x%x]\n", dwStbFilterHandle, pFilter->usPid, pFilter->aucValue[0], pFilter->aucMask[0]);

			VA_DMX_RetrieveEcmSection(hVaAction, pFilter);
		}
		else
		{
			HxLOG_Print("[%d][%x] SI_EMM_Retrieve : tid[0x%x] mask[0x%x]\n", dwStbFilterHandle, pFilter->usPid, pFilter->aucValue[0], pFilter->aucMask[0]);

			VA_DMX_RetrieveEmmSection(hVaAction, pFilter);
		}
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_DMX_StartSectionFilter
		start filtering
	arguments
		dwStbFilterHandle: filter handle returned by VA_DMX_AllocateSectionFilter()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_SET_FILTER_PARAMETERS_FIRST
*/
INT VA_DMX_StartSectionFilter(DWORD dwStbFilterHandle)
{
	HUINT32 			ulIdx;
	VA_DMX_Filter_t 	*pFilter;
	Handle_t			hVaAction;

	HxLOG_Info("[Enter(%x)]\n", dwStbFilterHandle);

	ulIdx = dwStbFilterHandle;

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	// dwStbFilterHandle does not exist
	if (ulIdx >= VA_DMX_MAX_FILTER || g_ulVaDmxFilter[ulIdx].eStatus == VA_DMX_FILTER_FREE)
	{
		HxLOG_Error("wrong dwStbFilterHandle (%x)\n", ulIdx);
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_INVALID_PARAMETER;
	}

	// if filter have not been set
	pFilter = &g_ulVaDmxFilter[ulIdx];
	if (pFilter->eMode == VA_DMX_FILTER_MODE_INVALID)
	{
		HxLOG_Error("filter have not been set\n");
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_SET_FILTER_PARAMETERS_FIRST;
	}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (pFilter->eMode == VA_DMX_FILTER_MODE_PVR)
	{
		pFilter->eStatus = VA_DMX_FILTER_RUNNING;

		VA_PVR_SendEcmByPid(pFilter->ulAcsId, pFilter->usPid);

		VA_SEM_Release(g_ulVaDmxSemId);

HxLOG_Print("[Exit-PVR mode-do nothing]\n");
		return kVA_OK;
	}

#endif

	// start filter
	// if the filter is already started, then do nothing and return kVA_OK
	if (pFilter->eStatus != VA_DMX_FILTER_RUNNING)
	{
		hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(pFilter->ulAcsId);

		if (pFilter->eMode == VA_DMX_FILTER_MODE_ECM_0 || pFilter->eMode == VA_DMX_FILTER_MODE_ECM_1)
		{
			HxLOG_Print("[%d][%x] SI_ECM_Retrieve : tid[0x%x] mask[0x%x]\n", dwStbFilterHandle, pFilter->usPid, pFilter->aucValue[0], pFilter->aucMask[0]);

			VA_DMX_RetrieveEcmSection(hVaAction, pFilter);
		}
		else
		{
			HxLOG_Print("[%d][%x] SI_EMM_Retrieve : tid[0x%x] mask[0x%x]\n", dwStbFilterHandle, pFilter->usPid, pFilter->aucValue[0], pFilter->aucMask[0]);

			VA_DMX_RetrieveEmmSection(hVaAction, pFilter);
		}

		pFilter->eStatus = VA_DMX_FILTER_RUNNING;
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}
/*
	VA_DMX_StopSectionFilter
		stop filtering
	arguments
		dwStbFilterHandle: filter handle returned by VA_DMX_AllocateSectionFilter()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_DMX_StopSectionFilter(DWORD dwStbFilterHandle)
{
	HUINT32 			ulIdx;
	VA_DMX_Filter_t 	*pFilter;

	HxLOG_Info("[Enter(%x)]\n", dwStbFilterHandle);

	ulIdx = dwStbFilterHandle;

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	// dwStbFilterHandle does not exist
	if (ulIdx >= VA_DMX_MAX_FILTER || g_ulVaDmxFilter[ulIdx].eStatus == VA_DMX_FILTER_FREE)
	{
		HxLOG_Error("wrong dwStbFilterHandle (%x)\n", ulIdx);
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_INVALID_PARAMETER;
	}

	pFilter = &g_ulVaDmxFilter[ulIdx];

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (pFilter->eMode == VA_DMX_FILTER_MODE_PVR)
	{
		pFilter->eStatus = VA_DMX_FILTER_STOPPED;
HxLOG_Print("[Exit-PVR mode-do nothing]\n");
	}
	else
#endif

	// stop filter
	// if the filter is already stopped, then do nothing and return kVA_OK
	if (pFilter->eStatus == VA_DMX_FILTER_RUNNING)
	{
		if (pFilter->eMode == VA_DMX_FILTER_MODE_ECM_0 || pFilter->eMode == VA_DMX_FILTER_MODE_ECM_1)
		{
			VA_DMX_StopEcmSection(pFilter->usLoadId);
			HxLOG_Print("[%x][%x] VA_DMX_StopEcmSection(%x)\n", ulIdx, pFilter->usPid, pFilter->usLoadId);
		}
		else
		{
			VA_DMX_StopEmmSection(pFilter->usLoadId);
			HxLOG_Print("[%x][%x] VA_DMX_StopEmmSection(%x)\n", ulIdx, pFilter->usPid, pFilter->usLoadId);
		}

		pFilter->eStatus = VA_DMX_FILTER_STOPPED;
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_DMX_UnlockSectionFilter
		unlock a filter that has been previously notified
	arguments
		dwStbFilterHandle: filter handle returned by VA_DMX_AllocateSectionFilter()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_NOT_LOCKED
*/
INT VA_DMX_UnlockSectionFilter(DWORD dwStbFilterHandle)
{
	HUINT32 			ulIdx;
	INT 				nErr;
	VA_DMX_Filter_t 	*pFilter;

	HxLOG_Info("[Enter(%x)]\n", dwStbFilterHandle);

	ulIdx = dwStbFilterHandle;

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	// dwStbFilterHandle does not exist
	if (ulIdx >= VA_DMX_MAX_FILTER || g_ulVaDmxFilter[ulIdx].eStatus == VA_DMX_FILTER_FREE)
	{
		HxLOG_Error("wrong dwStbFilterHandle (%x)\n", ulIdx);
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_INVALID_PARAMETER;
	}

	pFilter = &g_ulVaDmxFilter[ulIdx];

	// no locked buffer
	if (pFilter->bBufferLocked == FALSE)
	{
		HxLOG_Error("no locked buffer\n");
		VA_SEM_Release(g_ulVaDmxSemId);
		return kVA_RESOURCE_NOT_LOCKED;
	}

	// unlock filter
	if (pFilter->ucBufferCnt > 0)
	{
		local_va_dmx_DecBuffer(pFilter);
	}

	pFilter->bBufferLocked = FALSE;

	// now next section in the buffer can be handled
	if (pFilter->ucBufferCnt > 0)
	{
		// buffer is not empty. so notify Viaccess that a section is available
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
#if __VIACCESS50__
		nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#else
		nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#endif
#else
#if __VIACCESS50__
		nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#else
		nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#endif
#endif

		HxLOG_Info("VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

		if (nErr == kVA_OK)
		{
			pFilter->bBufferLocked = TRUE;
		}
		else // kVA_INVALID_PARAMETER or kVA_SECTION_REJECTED
		{
			HxLOG_Error("VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

			// in case of error, free the section
			local_va_dmx_DecBuffer(pFilter);
		}
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

////////////////////////
// callback
////////////////////////
HERROR local_va_dmx_PutEcmToCa(HUINT8 *pucSection, HUINT16 usLength, HUINT16 usLoadId)
{
	VA_DMX_Filter_t	*pFilter;
	int				i, nTemp, nErr;

	HxLOG_Print("[Enter]\n");

	nTemp = -1;

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_RUNNING && g_ulVaDmxFilter[i].usLoadId == usLoadId
			&& (pucSection[0] == g_ulVaDmxFilter[i].aucValue[0] || (pucSection[0] & 0xFE) == (g_ulVaDmxFilter[i].aucValue[0] & 0xFE) || (g_ulVaDmxFilter[i].aucMask[0] & 0x01) == 0))
			break;

		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_RUNNING && g_ulVaDmxFilter[i].usLoadId == usLoadId)
		{
			nTemp = i;
		}
	}

	if (i == VA_DMX_MAX_FILTER)
	{
		if (nTemp >= 0)
		{
			pFilter = &g_ulVaDmxFilter[nTemp];

			HxLOG_Info("no tid matching PID[%x] L[%x:%x] T[%x:%x] M[%x]\n", pFilter->usPid, pFilter->usLoadId, usLoadId, pFilter->aucValue[0], pucSection[0], pFilter->aucMask[0]);
		}
		else
		{
			HxLOG_Info("unknown usLoadId L[%x] T[%x]\n", usLoadId, pucSection[0]);
		}
	}
	else
	{
		pFilter = &g_ulVaDmxFilter[i];

		HxLOG_Info("F[%d] PID[%x] L[%x] T[%x]\n", i, pFilter->usPid, usLoadId, pucSection[0]);

		local_va_dmx_BufferAddSection(pFilter, pucSection, usLength);

		if (pFilter->bBufferLocked != TRUE)
		{
#ifdef SHOW_ECM
 			{
				int x;

				HxLOG_Print("[%x] ECM (%d) :", pFilter->usPid, usLength);
				for (x = 0; x < usLength; x++)
				{
					HxLOG_Print(" %02X", pucSection[x]);
				}
				HxLOG_Print("\n");
			}
#endif
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
#if __VIACCESS50__
			nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#else

			nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#endif
#else

#if __VIACCESS50__
			nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#else
			nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#endif
#endif
			HxLOG_Info("%d: VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", i, pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

			if (nErr == kVA_OK)
			{
				pFilter->bBufferLocked = TRUE;
			}
			else // kVA_INVALID_PARAMETER or kVA_SECTION_REJECTED
			{
				HxLOG_Error("%d: VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", i, pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

				// in case of error, free the section
				local_va_dmx_DecBuffer(pFilter);
			}
		}
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR local_va_dmx_PutEmmToCa(HUINT8 *pucSection, HUINT16 usLength, HUINT16 usLoadId)
{
	VA_DMX_Filter_t	*pFilter;
	int				i, nTemp, nErr;

	HxLOG_Print("[Enter]\n");

	nTemp = -1;

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_RUNNING && g_ulVaDmxFilter[i].usLoadId == usLoadId
			&& (pucSection[0] == g_ulVaDmxFilter[i].aucValue[0] || (pucSection[0] & 0xFE) == (g_ulVaDmxFilter[i].aucValue[0] & 0xFE) || (g_ulVaDmxFilter[i].aucMask[0] & 0x01) == 0))
			break;

		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_RUNNING && g_ulVaDmxFilter[i].usLoadId == usLoadId)
		{
			nTemp = i;
		}
	}

	if (i == VA_DMX_MAX_FILTER)
	{
		if (nTemp >= 0)
		{
			pFilter = &g_ulVaDmxFilter[nTemp];

			HxLOG_Info("no tid matching PID[%x] L[%x:%x] T[%x:%x] M[%x]\n", pFilter->usPid, pFilter->usLoadId, usLoadId, pFilter->aucValue[0], pucSection[0], pFilter->aucMask[0]);
		}
		else
		{
			HxLOG_Info("unknown usLoadId L[%x] T[%x]\n", usLoadId, pucSection[0]);
		}
	}
	else
	{
		pFilter = &g_ulVaDmxFilter[i];

		HxLOG_Info("F[%d] PID[%x] L[%x] T[%x]\n", i, pFilter->usPid, usLoadId, pucSection[0]);

		local_va_dmx_BufferAddSection(pFilter, pucSection, usLength);

		if (pFilter->bBufferLocked != TRUE)
		{
#ifdef SHOW_EMM
 			{
				int x;

				HxLOG_Print("[%x] EMM (%d) :", pFilter->usPid, usLength);
				for (x = 0; x < usLength; x++)
				{
					HxLOG_Print(" %02X", pucSection[x]);
				}
				HxLOG_Print("\n");
			}
#endif

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
#if __VIACCESS50__
			nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#else
			nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)pFilter->ulTimeStamp[pFilter->ucBufferReadPos]);
#endif
#else
#if __VIACCESS50__
			nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#else
			nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#endif
#endif
			HxLOG_Info("%d: VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", i, pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

			if (nErr == kVA_OK)
			{
				pFilter->bBufferLocked = TRUE;
			}
			else // kVA_INVALID_PARAMETER or kVA_SECTION_REJECTED
			{
				HxLOG_Error("%d: VA_DMX_NotifyFilteredSection(W=%d,R=%d) err:%d\n", i, pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

				// in case of error, free the section
				local_va_dmx_DecBuffer(pFilter);
			}
		}
	}

	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_DMX_PutEcmToCa(HUINT8 *pusRaw, HUINT16 usLoadId)
{
	HUINT16	usLength;
	HERROR	nErr = ERR_OK;

HxLOG_Print("[Enter]\n");

	usLength = Get12bit(pusRaw + 1) + 3;  // section length

#if 0 	// driver test 중 DMX_LOAD_SECTION_9에서 1024byte가 넘는 section이 전달되는 경우 있음
	if (usLength > VA_SI_SECTION_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", usLength);
		return 0;
	}
#endif

	nErr = local_va_dmx_PutEcmToCa(pusRaw, usLength, usLoadId);

HxLOG_Print("[Exit]\n");
	return nErr;
}

HERROR VA_DMX_PutEmmToCa(HUINT8 *pusRaw, HUINT16 usLoadId)
{
	HUINT16	usLength;
	HERROR	nErr = ERR_OK;

HxLOG_Print("[Enter]\n");

	usLength = Get12bit(pusRaw + 1) + 3;  // section length

#if 0 	// driver test 중 DMX_LOAD_SECTION_9에서 1024byte가 넘는 section이 전달되는 경우 있음
	if (usLength > VA_SI_SECTION_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", usLength);
		return 0;
	}
#endif

	nErr = local_va_dmx_PutEmmToCa(pusRaw, usLength, usLoadId);

HxLOG_Print("[Exit]\n");
	return nErr;
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HERROR VA_DMX_PutEcmFromPVR(HUINT32 ulAcsId, HUINT8 *pusRaw, HUINT16 usPid)
{
	HUINT16			usLength;
	VA_DMX_Filter_t	*pFilter;
	int				i, nErr;
	HERROR			nRet = ERR_OK;

	HxLOG_Print("[Enter(acs:%d,pid:%x, %02X)]\n", ulAcsId, usPid, pusRaw[0]);

	usLength = Get12bit(pusRaw + 1) + 3;  // section length
#if 0 	// driver test 중 DMX_LOAD_SECTION_9에서 1024byte가 넘는 section이 전달되는 경우 있음
	if (usLength > VA_SI_SECTION_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", usLength);
		return 0;
		usLength = VA_SI_SECTION_SIZE;
	}
#endif

	VA_ASSERT(g_ulVaDmxSemId);
	VA_SEM_Get(g_ulVaDmxSemId);

	for (i = 0; i < VA_DMX_MAX_FILTER; i++)
	{
		if (g_ulVaDmxFilter[i].eStatus == VA_DMX_FILTER_RUNNING &&
			g_ulVaDmxFilter[i].eMode == VA_DMX_FILTER_MODE_PVR &&
			g_ulVaDmxFilter[i].ulAcsId == ulAcsId &&
			g_ulVaDmxFilter[i].usPid == usPid &&
			(pusRaw[0] == g_ulVaDmxFilter[i].aucValue[0] || (pusRaw[0] & 0xFE) == (g_ulVaDmxFilter[i].aucValue[0] & 0xFE) || (g_ulVaDmxFilter[i].aucMask[0] & 0x01) == 0))
			break;
	}

	if (i == VA_DMX_MAX_FILTER)
	{
		HxLOG_Info("unknown Pid [%x] T[%x]\n", usPid, pusRaw[0]);

		nRet = ERR_FAIL;
	}
	else
	{
		HxLOG_Info("F[%d], pid[%x], T[%x]\n", i, usPid, pusRaw[0]);

		pFilter = &g_ulVaDmxFilter[i];

		local_va_dmx_BufferAddSection(pFilter, pusRaw, usLength);

		if (pFilter->bBufferLocked != TRUE)
		{
#ifdef SHOW_ECM
			if (g_VaDmxApi_Level & DBG_DATA)
 			{
				int x;

				HxLOG_Print("[%x] ECM (%d) :", pFilter->usPid, usLength);
				for (x = 0; x < usLength; x++)
				{
					HxLOG_Print(" %02X", pusRaw[x]);
				}
				HxLOG_Print("\n");
			}
#endif

#if __VIACCESS50__
			nErr = pFilter->pfVa_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#else
			nErr = VA_DMX_NotifyFilteredSection(pFilter->ulVaFilterHandle, pFilter->aulSectionLength[pFilter->ucBufferReadPos], pFilter->apSectionBuffer[pFilter->ucBufferReadPos], (void *)NULL);
#endif
			HxLOG_Info("[VA_DMX_PutEcmFromPVR(%d)] VA_DMX_NotifyFilteredSection(%d,%d) err:%d\n", i, pFilter->ucBufferWritePos, pFilter->ucBufferReadPos, nErr);

			if (nErr == kVA_OK)
			{
				pFilter->bBufferLocked = TRUE;
			}
			else
			{
				// in case of error, free the section
				local_va_dmx_DecBuffer(pFilter);
//				nRet = ERR_FAIL;
			}

		}
	}
	VA_SEM_Release(g_ulVaDmxSemId);

	HxLOG_Print("[Exit]\n");
	return nRet;
}
#endif

