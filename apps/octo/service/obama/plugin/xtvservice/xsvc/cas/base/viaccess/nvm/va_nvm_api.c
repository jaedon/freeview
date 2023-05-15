/*
	Viaccess NVM API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.33
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"

#include "va_nvm.h"
#include "va_setup.h"

#include "va_nvm_api.h"
#include "vkernel.h"

//#include "pal_eeprom.h"

#include <string.h>

#include "svc_cas.h"
#include <_svc_cas_mgr_pal_port.h>
#include <stb_params.h>

//------ACS4.1-------
#include "va_types.h"
#include "va_errors.h"
#include "va_xnvm.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



// operation options
//#define NVRAM_RETURN_FIRST_RW_LATER
#define NVRAM_NO_TASK

typedef enum
{
	VA_NVM_ACTION_INIT = 0,
	VA_NVM_ACTION_READ,
	VA_NVM_ACTION_WRITE,
	VA_NVM_ACTION_NO
} VA_NVM_ACTION_STATUS_ENUM;

typedef enum
{
	VA_NVM_MSG_INIT = 0,
	VA_NVM_MSG_READ,
	VA_NVM_MSG_WRITE,
	VA_NVM_MSG_SYSTEM_EXIT,
	VA_NVM_MSG_ERROR
} VA_NVM_MSG_TYPE_ENUM;

typedef struct {
	DWORD 	dwStbXnvmHandle;
	tVA_XNVM_Segment eStbSegment;
} VA_XNVRAM_t;

static VA_XNVRAM_t  VA_Xnvram[eSEGMENT_LAST];
/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static VA_NVM_ACTION_STATUS_ENUM	sVaNvmActionType;

#ifndef NVRAM_NO_TASK
static UINT32 sVaNvmQid;
static UINT32 sVaNvmTid;
#endif
static UINT32 sVaNvmSemId;

static UINT8 *s_pucVaNvmData;
static UINT8 *s_pucVaLoaderData;
static UINT8 *s_pucVaSWData;

#ifdef VA_BBX_INIT_CHECK
HBOOL bVaBbxInitCheck = 0;
#endif

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_NVM_Destroy();
static void VA_NVM_Task(void * args);

int VA_INT_NVM_ClearAll()
{
	UINT8 *pucData;
	int nErr;

	HxLOG_Print("[Enter]\n");

	pucData = (UINT8 *)VA_MEM_Alloc(VA_NVM_ACS_DATA_SIZE);
	if (pucData == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pucData, 0, VA_NVM_ACS_DATA_SIZE);

	nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET, VA_NVM_ACS_DATA_SIZE, pucData);

	VA_MEM_Free(pucData);

	HxLOG_Print("[Exit]\n");
	return nErr;
}

static int VA_INT_NVM_InitBuffer(tVA_XNVM_Segment eSegment, UINT32 *ulBuffer)
{
	int nErr;

	HxLOG_Print("[Enter]\n");

	if (s_pucVaNvmData == NULL)
		return VA_ERR;

	nErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET, VA_NVM_ACS_DATA_SIZE, ulBuffer);

	HxLOG_Print("[Exit]\n");
	return nErr;
}

#ifndef NVRAM_NO_TASK
static int VA_INT_NVM_ReadCache(UINT32 ulOffset, UINT32 ulSize, BYTE *pData)
{
	HxLOG_Print("[Enter][%x, %x, %x]\n", ulOffset, ulSize, pData);

	if (ulOffset + ulSize > VA_NVM_ACS_DATA_SIZE)
		return VA_ERR;

	if (pData == NULL)
		return VA_ERR;

	VA_memcpy(pData, s_pucVaNvmData + ulOffset, ulSize);

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

static int VA_INT_NVM_WriteCache(UINT32 ulOffset, UINT32 ulSize, BYTE *pData)
{
	int nErr;

	HxLOG_Print("[Enter][%x, %x, %x]\n", ulOffset, ulSize, pData);

	if (ulOffset + ulSize > VA_NVM_ACS_DATA_SIZE)
		return VA_ERR;

	if (pData == NULL)
		return VA_ERR;

	if (s_pucVaNvmData)
	{
		VA_memcpy(s_pucVaNvmData + ulOffset, pData, ulSize);
	}

	nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET + ulOffset, ulSize, pData);

	HxLOG_Print("[Exit]\n");
	return nErr;
}
#endif

/*
	initialize variables and start task
*/
void VA_NVM_Init()
{
	int nErr;
	UINT32 aulMsg[4];
/*	TODO:
    eSEGMENT_SOFTWARE = 0,	// 8M
    eSEGMENT_ACS_DATA,		// 24k
    eSEGMENT_LOADER_DATA,   // 8k
    loader ¿Í software ¿¡ ´ëÇÑ memroy¸¦ Àâ¾Æ ¾ßÇÑ´Ù.
*/

	HxLOG_Print("[Enter]\n");
/* TODO: hjlee3
	VA_XNVM_Open()À¸·Î ´ëÃ¼

	s_pucVaNvmData = (UINT8 *)VA_MEM_Alloc(VA_NVM_ACS_DATA_SIZE);
	nErr = VA_INT_NVM_InitBuffer();
	if (nErr)
	{
		HxLOG_Error("failed to init buffer (%d)\n", nErr);
		return;
	}
*/
	nErr = VA_SEM_Create(VA_NVM_SEM_NAME, &sVaNvmSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		return;
	}

#ifndef NVRAM_NO_TASK
	sVaNvmActionType = VA_NVM_ACTION_INIT;
	VA_CreateMessageQueue(VA_NVM_QUEUE_NAME, &sVaNvmQid);

	nErr = VK_TASK_Create(VA_NVM_Task, VA_NVM_TASK_PRIORITY, VA_NVM_TASK_STACK_SIZE, VA_NVM_TASK_NAME, NULL, &sVaNvmTid, 0);
	if (nErr)
	{
		HxLOG_Error("failed to create task (%d)\n", nErr);
		return;
	}

	nErr = VK_TASK_Start(sVaNvmTid);
	if (nErr)
	{
		HxLOG_Error("failed to start task (%d)\n", nErr);
		return;
	}
#endif

	HxLOG_Print("[Exit]\n");
}

static void VA_NVM_Destroy()
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter]\n");

#ifndef NVRAM_NO_TASK
	// destory message queue
	VA_MSG_Destroy(sVaNvmQid);
#endif

	if (s_pucVaNvmData)
	{
		VA_MEM_Free(s_pucVaNvmData);
		s_pucVaNvmData = NULL;
	}
	HxLOG_Print("[Exit]\n");
}

#ifndef NVRAM_NO_TASK
static void VA_NVM_Task(void * args)
{
	UINT32 aulMsg[4];
	UINT32 ulMsgType;
	UINT16 usSize;
	UINT32 ulOffset;
	UINT8 *pData;
	int nErr;
	HBOOL bWaitMsg = TRUE;

	HxLOG_Print("[Enter]\n");

	while (bWaitMsg)
	{
		if (VA_GetMessage(sVaNvmQid, (void *)&aulMsg) == VK_OK)
		{
			ulMsgType = aulMsg[0];

HxLOG_Info("[VA_NVM_Task] MSG Received msg_type(%d)\n", ulMsgType);

			switch (ulMsgType)
			{
				case VA_NVM_MSG_READ:
#ifdef NVRAM_RETURN_FIRST_RW_LATER
					ulOffset = aulMsg[1];
					usSize = (UINT16)aulMsg[2];
					pData = (UINT8 *)aulMsg[3];
					nErr = VA_INT_NVM_ReadCache(ulOffset, usSize, pData);
#else
					nErr = (int)aulMsg[1];
#endif
					sVaNvmActionType = VA_NVM_ACTION_NO;
					if (nErr)
					{
						HxLOG_Error("VA_INT_NVM_ReadCache() fail\n");
						nErr = VA_NVM_RequestFailed();
						if (nErr == kVA_UNEXPECTED_CALLBACK)
							HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
					}
					else
					{
						HxLOG_Info("VA_INT_NVM_ReadCache() done\n");
						nErr = VA_NVM_RequestDone();
						if (nErr == kVA_UNEXPECTED_CALLBACK)
							HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
					}

					// to pass NVRAM stress test (driver test)
					// VK_TASK_Sleep(1);
					break;

				case VA_NVM_MSG_WRITE:
#ifdef NVRAM_RETURN_FIRST_RW_LATER
					ulOffset = aulMsg[1];
					usSize = (UINT16)aulMsg[2];
					pData = (UINT8 *)aulMsg[3];
					nErr = VA_INT_NVM_WriteCache(ulOffset, usSize, pData);
#else
					nErr = (int)aulMsg[1];
#endif
					sVaNvmActionType = VA_NVM_ACTION_NO;
					if (nErr)
					{
						HxLOG_Error("VA_INT_NVM_WriteCache() fail\n");
						nErr = VA_NVM_RequestFailed();
						if (nErr == kVA_UNEXPECTED_CALLBACK)
							HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
					}
					else
					{
						HxLOG_Info("VA_INT_NVM_WriteCache() done\n");
						nErr = VA_NVM_RequestDone();
						if (nErr == kVA_UNEXPECTED_CALLBACK)
							HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
					}

					// to pass NVRAM stress test (driver test)
					// VK_TASK_Sleep(1);
					break;

				case VA_NVM_MSG_SYSTEM_EXIT:
					bWaitMsg = FALSE;
					break;

				default:
					HxLOG_Error("Unknown Msg\n");
					break;
			}
		}
	}

	// this task is going to be terminated
	VA_NVM_Destroy();

	HxLOG_Print("[Exit]\n");
}
#endif

/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/

/*
	VA_NVM_Read
		request to read data from NVM
	arguments
		uiOffset
		uiSize
		pReadData: allocated and freed by VA
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
*/
INT VA_NVM_Read(UINT32 uiOffset, UINT32 uiSize, BYTE *pReadData)
{
#if !defined(__VIACCESS50__)
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter][%x, %x, %x]\n", uiOffset, uiSize, pReadData);

	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);

	if (sVaNvmActionType == VA_NVM_ACTION_READ || sVaNvmActionType == VA_NVM_ACTION_WRITE)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Error("other read/write operation is processing\n");
		return kVA_RESOURCE_BUSY;
	}

	if (uiOffset > VA_NVM_ACS_DATA_SIZE || uiOffset + uiSize > VA_NVM_ACS_DATA_SIZE || uiSize == 0 || pReadData == NULL)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pReadData);
		return kVA_INVALID_PARAMETER;
	}

#ifndef NVRAM_NO_TASK

	sVaNvmActionType = VA_NVM_ACTION_READ;

	// request to read data
	// in another task, VA_NVM_RequestDone() or VA_NVM_RequestFailed() will be called

#ifdef NVRAM_RETURN_FIRST_RW_LATER
	aulMsg[0] = VA_NVM_MSG_READ;
	aulMsg[1] = uiOffset;
	aulMsg[2] = uiSize;
	aulMsg[3] = (UINT32)pReadData;
#else
	aulMsg[0] = VA_NVM_MSG_READ;
	aulMsg[1] = VA_INT_NVM_ReadCache(uiOffset, uiSize, pReadData);
#endif

	VA_PostMessage(sVaNvmQid, aulMsg);

#else

	VA_memcpy(pReadData, s_pucVaNvmData + uiOffset, uiSize);
	if (VA_NVM_RequestDone() == kVA_UNEXPECTED_CALLBACK)
		HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");

#endif

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");
#endif
	return kVA_OK;
}

/*
	VA_NVM_Write
		request to write data to NVM
	arguments
		uiOffset
		uiSize
		pWriteData
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
*/
INT VA_NVM_Write(UINT32 uiOffset, UINT32 uiSize, BYTE *pWriteData)
{
#if !defined(__VIACCESS50__)
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter][%x, %x, %x]\n", uiOffset, uiSize, pWriteData);

	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);

	if (sVaNvmActionType == VA_NVM_ACTION_READ || sVaNvmActionType == VA_NVM_ACTION_WRITE)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Error("other read/write operation is processing\n");
		return kVA_RESOURCE_BUSY;
	}

	if (uiOffset > VA_NVM_ACS_DATA_SIZE || uiOffset + uiSize > VA_NVM_ACS_DATA_SIZE || uiSize == 0 || pWriteData == NULL)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pWriteData);
		return kVA_INVALID_PARAMETER;
	}

#ifndef NVRAM_NO_TASK

	sVaNvmActionType = VA_NVM_ACTION_WRITE;

	// request to write data
	// in another task, VA_NVM_RequestDone() or VA_NVM_RequestFailed() will be called

#ifdef NVRAM_RETURN_FIRST_RW_LATER
	aulMsg[0] = VA_NVM_MSG_WRITE;
	aulMsg[1] = uiOffset;
	aulMsg[2] = uiSize;
	aulMsg[3] = (UINT32)pWriteData;
#else
	aulMsg[0] = VA_NVM_MSG_WRITE;
	aulMsg[1] = VA_INT_NVM_WriteCache(uiOffset, uiSize, pWriteData);
#endif

	VA_PostMessage(sVaNvmQid, aulMsg);

#else

	VA_memcpy(s_pucVaNvmData + uiOffset, pWriteData, uiSize);

	svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET + uiOffset, uiSize, pWriteData);

	if (VA_NVM_RequestDone() == kVA_UNEXPECTED_CALLBACK)
	{
		HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
	}
	else
	{
		if (uiSize > 0x1f00)
		{
			VA_UTIL_SetBbxUpdated();
			HxLOG_Print("[01;34m BBX data has been written again by ACS (size %d bytes)\n [00m", uiSize);
		}
	}
#endif

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");
#endif
	return kVA_OK;
}

#define __________ACS41_____________
DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment)
{
	int nErr, idx=0;
	DWORD dwStbXnvmHandle;

/*  : Âü°í »çÇ×.
    eSEGMENT_SOFTWARE = 0,	// 8M
    eSEGMENT_ACS_DATA,		// 24k
    eSEGMENT_LOADER_DATA,   // 8k

    segment¿¡ ¸ÂÃçÁø offset °ªÀÌ µé¾î ¿Ã°ÍÀ¸·Î º¸ÀÓ.
    ±×·¡¼­ read/write ÇÒ ½Ã¿¡´Â segment¿¡ ¸ÂÃç¼­ ÇØÁà¾ß ÇÒ °ÍÀ¸·Î º¸ÀÓ.
    ps. ÇöÀç´Â eSEGMENT_ACS_DATA ¿¡¸¸ ¸ÂÃçÁø »óÅÂ¿¡ ´ëÇØ¼­¸¸ °í·ÁµÇ¾î Áü.
*/
	HxLOG_Print("[Enter]\n");
	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);

	/* dwVaXnvmHandle °ªÀº idx ·Î ÃßÃø µÈ´Ù. ±×·¡¼­ .. */
	idx = dwVaXnvmHandle;

	HxLOG_Error("failed to init buffer (%d)\n", nErr);
	if(VA_Xnvram[idx].dwStbXnvmHandle!=NULL )
	{
		// dwVaXnvmHandle ¿Í dwStbXnvmHandle °ªÀÇ Â÷ÀÌ¸¦ µÐ´Ù.
		VA_Xnvram[idx].dwStbXnvmHandle = dwVaXnvmHandle + 10;
		VA_Xnvram[idx].eStbSegment = eSegment;

		if(eSegment == eSEGMENT_ACS_DATA)
		{
			s_pucVaNvmData = (UINT8 *)VA_MEM_Alloc(VA_NVM_ACS_DATA_SIZE);
			nErr = VA_INT_NVM_InitBuffer(eSegment,s_pucVaNvmData);
			if (nErr)
			{
				HxLOG_Error("failed to init buffer (%d)\n", nErr);
				return;
			}
		}
		else if(eSegment == eSEGMENT_LOADER_DATA)
		{
			s_pucVaLoaderData = (UINT8 *)VA_MEM_Alloc(VA_NVM_ACS_LOADER_SIZE);
			nErr = VA_INT_NVM_InitBuffer(eSegment,s_pucVaLoaderData);
			if (nErr)
			{
				HxLOG_Error("failed to init buffer (%d)\n", nErr);
				return;
			}
		}
		else
		{
			s_pucVaSWData = (UINT8 *)VA_MEM_Alloc(VA_NVM_ACS_SOFTWARE_SIZE);
			nErr = VA_INT_NVM_InitBuffer(eSegment,s_pucVaSWData);
			if (nErr)
			{
				HxLOG_Error("failed to init buffer (%d)\n", nErr);
				return;
			}

		}

	}
	else
	{
		VA_SEM_Release(sVaNvmSemId);
		return kVA_ILLEGAL_HANDLE;
	}

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");

	return dwStbXnvmHandle;
}

INT     VA_XNVM_Close(DWORD dwStbXnvmHandle)
{
	int i;

	HxLOG_Print("[Enter]\n");
	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);

	if(VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_ACS_DATA)
	{
		if(s_pucVaNvmData)
		{
			VA_MEM_Free(s_pucVaNvmData);
			s_pucVaNvmData = NULL;
		}
	}
	else if(VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_LOADER_DATA)
	{
		if(s_pucVaLoaderData)
		{
			VA_MEM_Free(s_pucVaLoaderData);
			s_pucVaLoaderData = NULL;
		}
	}
	else
	{
		if(s_pucVaSWData)
		{
			VA_MEM_Free(s_pucVaSWData);
			s_pucVaSWData = NULL;
		}
	}

	VA_Xnvram[dwStbXnvmHandle-10].dwStbXnvmHandle = NULL;
	VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == NULL;

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

INT     VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData)
{

	UINT32 aulMsg[4];
	UINT32 uisegmentaddr=0;
	UINT16 idx=0;
	HxLOG_Print("[Enter][%x, %x, %x]\n", uiOffset, uiSize, pReadData);

	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);


	if (sVaNvmActionType == VA_NVM_ACTION_READ || sVaNvmActionType == VA_NVM_ACTION_WRITE)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Error("other read/write operation is processing\n");
		return kVA_RESOURCE_BUSY;
	}

/*
	TODO: uioffsetÀÌ segment¿¡ ¸ÂÃç¼­ µé¾î¿À´ÂÁö È®ÀÎ ÇØ¾ß ÇÑ´Ù.
*/
	if (VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_ACS_DATA)
	{
		if (uiOffset > VA_NVM_ACS_DATA_SIZE || uiOffset + uiSize > VA_NVM_ACS_DATA_SIZE || uiSize == 0 || pReadData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pReadData);
			return kVA_INVALID_PARAMETER;
		}
		VA_memcpy(pReadData, s_pucVaNvmData + uisegmentaddr + uiOffset, uiSize);
	}
	else if(VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_LOADER_DATA)
	{
		if (uiOffset > VA_NVM_ACS_LOADER_SIZE || uiOffset + uiSize > VA_NVM_ACS_LOADER_SIZE || uiSize == 0 || pReadData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pReadData);
			return kVA_INVALID_PARAMETER;
		}
		VA_memcpy(pReadData, s_pucVaLoaderData + uisegmentaddr + uiOffset, uiSize);
	}
	else
	{
		if (uiOffset > VA_NVM_ACS_SOFTWARE_SIZE || uiOffset + uiSize > VA_NVM_ACS_SOFTWARE_SIZE || uiSize == 0 || pReadData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pReadData);
			return kVA_INVALID_PARAMETER;
		}
		VA_memcpy(pReadData, s_pucVaSWData + uiOffset, uiSize);

	}
// hjlee3 , just for build
//	if (VA_XNVM_RequestDone(dwStbXnvmHandle-10) == kVA_UNEXPECTED_CALLBACK)
//		HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

INT     VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter][%x, %x, %x]\n", uiOffset, uiSize, pWriteData);

	VA_ASSERT(sVaNvmSemId);
	VA_SEM_Get(sVaNvmSemId);

	if (sVaNvmActionType == VA_NVM_ACTION_READ || sVaNvmActionType == VA_NVM_ACTION_WRITE)
	{
		VA_SEM_Release(sVaNvmSemId);
		HxLOG_Error("other read/write operation is processing\n");
		return kVA_RESOURCE_BUSY;
	}

	if (VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_ACS_DATA)
	{
		if (uiOffset > VA_NVM_ACS_DATA_SIZE || uiOffset + uiSize > VA_NVM_ACS_DATA_SIZE || uiSize == 0 || pWriteData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pWriteData);
			return kVA_INVALID_PARAMETER;
		}
		sVaNvmActionType = VA_NVM_ACTION_WRITE;
		// request to write data
		// in another task, VA_NVM_RequestDone() or VA_NVM_RequestFailed() will be called

		VA_memcpy(s_pucVaNvmData + uiOffset, pWriteData, uiSize);
		svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET + uiOffset, uiSize, pWriteData);
	}
	else if(VA_Xnvram[dwStbXnvmHandle-10].eStbSegment == eSEGMENT_LOADER_DATA)
	{
		if (uiOffset > VA_NVM_ACS_LOADER_SIZE || uiOffset + uiSize > VA_NVM_ACS_LOADER_SIZE || uiSize == 0 || pWriteData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pWriteData);
			return kVA_INVALID_PARAMETER;
		}
		sVaNvmActionType = VA_NVM_ACTION_WRITE;
		// request to write data
		// in another task, VA_NVM_RequestDone() or VA_NVM_RequestFailed() will be called
		VA_memcpy(s_pucVaLoaderData + uiOffset, pWriteData, uiSize);
		/*  TODO: hjlee3 offset ½ÃÀÛ À§Ä¡¸¦ ¾Ë¾Æ¾ß ÇÑ´Ù.  */
//		svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET + uiOffset, uiSize, pWriteData);
	}
	else
	{
		if (uiOffset > VA_NVM_ACS_SOFTWARE_SIZE || uiOffset + uiSize > VA_NVM_ACS_SOFTWARE_SIZE || uiSize == 0 || pWriteData == NULL)
		{
			VA_SEM_Release(sVaNvmSemId);
			HxLOG_Info("wrong param [%d, %d, %x]\n", uiOffset, uiSize, pWriteData);
			return kVA_INVALID_PARAMETER;
		}
		sVaNvmActionType = VA_NVM_ACTION_WRITE;
		// request to write data
		// in another task, VA_NVM_RequestDone() or VA_NVM_RequestFailed() will be called

		VA_memcpy(s_pucVaSWData + uiOffset, pWriteData, uiSize);
		/*  TODO: hjlee3 offset ½ÃÀÛ À§Ä¡¸¦ ¾Ë¾Æ¾ß ÇÑ´Ù.  */
//		svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM__DATA_OFFSET + uiOffset, uiSize, pWriteData);
	}

/* hjlee3, just for build
	if (VA_XNVM_RequestDone(dwStbXnvmHandle-10) == kVA_UNEXPECTED_CALLBACK)
	{
		HxLOG_Error("kVA_UNEXPECTED_CALLBACK\n");
	}
	else
*/
	{
		if (uiSize > 0x1f00)
		{
			VA_UTIL_SetBbxUpdated();
			HxLOG_Print("[01;34m BBX data has been written again by ACS (size %d bytes)\n [00m", uiSize);
		}
	}

	VA_SEM_Release(sVaNvmSemId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

