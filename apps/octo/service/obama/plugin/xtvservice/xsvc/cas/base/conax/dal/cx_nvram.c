/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_nvram.c
	@brief  	 NVRAM Driver Adaptation Layer for Conax

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
#define ________CX_NVRAM_Private_Include______________________________________
/*******************************************************************/
#include <octo_common.h>	// htype.h and hlib.h
#include <conax.h>
#include "../local_include/cx_nvram.h"


/*******************************************************************/
#define ________CX_NVRAM_Golbal_Value_________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_NVRAM_Private_Define_______________________________________
/*******************************************************************/
#ifdef WIN32
#define CX_CAS_FILE_JFFS2_PATH				""
#else
#define CX_CAS_FILE_JFFS2_PATH				CAS_JFFS2_PATH "/conax/"
#endif

#define CX_CAS_FILE_USERTEXT 				"ConaxUserText.dat"
#define CX_CAS_FILE_FINGERPRINT 			"ConaxFingerprint.dat"
#define CX_CAS_FILE_KEYSTORE 				"ConaxKeyStore.dat"
#define CX_CAS_FILE_PVRKEY					"ConaxPvrKey.dat"
#define CX_CAS_FILE_CAPIN					"ConaxCaPin.dat"

#define CX_NVRAM_TASK_MSGQ_SIZE				64
#define CX_NVRAM_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF + 2)
#define CX_NVRAM_TASK_STACK_SIZE			SIZE_32K

/*******************************************************************/
#define ________CX_NVRAM_Private_Static_Value_________________________________
/*******************************************************************/
static HULONG	s_ulCxNvmSemaId		= 0;
static HULONG	s_ulCxNvmTaskId		= 0;
static HULONG	s_ulCxNvmMsqQId		= 0;
static HBOOL	s_bCxNvmTaskStart	= FALSE;

/*******************************************************************/
#define ________CX_NVRAM_Global_Value_________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_NVRAM_Private_Static_Prototype_____________________________
/*******************************************************************/
static void local_cxDalNvram_Init(void);
static void local_cxDalNvram_Task(void* argv);
static HERROR local_cxDalNvram_CreateTask(void);
static HERROR local_cxDalNvram_CreateSema(void);

static HERROR local_cxDalNvram_GetFileName(CxNvmFileType_t eNvmFileType, HUINT8* pszFileName);
static HERROR local_cxDalNvram_Read(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData);
static HERROR local_cxDalNvram_Write(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData);
static HERROR local_cxDalNvram_NvmDelete(CxNvmFileType_t eNvmFileType);
static HERROR cxDalNvram_Read(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucBuf);
static HERROR cxDalNvram_Write(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData);
static HERROR cxDalNvram_Delete(CxNvmFileType_t eNvmFileType);
static HERROR cxDalNvram_AsyncWrite(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData);

/*******************************************************************/
#define ________CX_NVRAM_Private_Static_Prototype_Body________________________
/*******************************************************************/
static void local_cxDalNvram_Init(void)
{
	HERROR	nResult			= ERR_FAIL;

	ENTER_FUNCTION;

	nResult = local_cxDalNvram_CreateSema();
	if (nResult != ERR_OK)
	{
		LEAVE_FUNCTION;
		return;
	}

	nResult = local_cxDalNvram_CreateTask();
	if (nResult != ERR_OK)
	{
		LEAVE_FUNCTION;
		return;
	}

	LEAVE_FUNCTION;
	return;
}

static void local_cxDalNvram_Task(void* argv)
{
	HUINT32		nResult			= 0;
	CxNvmMsg_t	stCxNvmMsg;
	HUINT8* 	ptr				= NULL;

	ENTER_FUNCTION;
	s_bCxNvmTaskStart = TRUE;

	for (; ;)
	{
		nResult = VK_MSG_Receive(s_ulCxNvmMsqQId, &stCxNvmMsg, sizeof(CxNvmMsg_t));
		if (nResult == 0 && stCxNvmMsg.eNvmCmd < eCxNvmCmd_Max)
		{
			switch (stCxNvmMsg.eNvmCmd)
			{
			case eCxNvmCmd_Write:
				ptr = (HUINT8 *)stCxNvmMsg.ulParam;
				cxDalNvram_Write(stCxNvmMsg.eNvmFileType, stCxNvmMsg.ulOffset, stCxNvmMsg.ulSize, ptr);

				if (ptr != NULL)
				{
					OxCAS_Free(ptr);
					ptr = NULL;
				}
				break;
			default:
				break;
			}
		}
		else
		{
			HxLOG_Error("GetMessage Error !!! : 0x%X\n", nResult);
			continue;
		}
	}

	LEAVE_FUNCTION;
	return;
}

static HERROR local_cxDalNvram_CreateTask(void)
{
	HINT32	nResult;

	ENTER_FUNCTION;
	// Create Task
	nResult = (HINT32)VK_TASK_Create(local_cxDalNvram_Task,				//
									CX_NVRAM_TASK_PRIORITY,			//
									CX_NVRAM_TASK_STACK_SIZE,		//
									"NVM_Task",						//
									NULL,							//
									&s_ulCxNvmTaskId, 0);			//

	if (nResult != ERR_OK)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// Create Message Queue
	nResult = (HINT32)VK_MSG_Create(CX_NVRAM_TASK_MSGQ_SIZE,		//
									sizeof(CxNvmMsg_t),				//
									"NVM_TaskQ",					//
									(unsigned long*)&s_ulCxNvmMsqQId,	//
									VK_SUSPENDTYPE_PRIORITY);		//
	if (nResult != ERR_OK)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// The task start
	VK_TASK_Start(s_ulCxNvmTaskId);

	while (s_bCxNvmTaskStart == FALSE)
	{
		VK_TASK_Sleep(10);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalNvram_CreateSema(void)
{
	HINT32	nResult;

	ENTER_FUNCTION;
	// Create Semaphore
	nResult = VK_SEM_Create(&s_ulCxNvmSemaId, "CxNvmSem", VK_SUSPENDTYPE_PRIORITY);
	if (nResult != ERR_OK)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
	LEAVE_FUNCTION;
	return ERR_OK;
}

static HERROR local_cxDalNvram_GetFileName(CxNvmFileType_t eNvmFileType, HUINT8* pszFileName)
{
	HUINT8	szFileName[256]	=
	{
		0,
	};

	switch (eNvmFileType)
	{
	case eCxNvmFile_UserText:
		snprintf(szFileName, 256, "%s%s", CX_CAS_FILE_JFFS2_PATH, CX_CAS_FILE_USERTEXT);
		break;

	case eCxNvmFile_Fingerprint:
		snprintf(szFileName, 256, "%s%s", CX_CAS_FILE_JFFS2_PATH, CX_CAS_FILE_FINGERPRINT);
		break;

	case eCxNvmFile_KeyStore:
		snprintf(szFileName, 256, "%s%s", CX_CAS_FILE_JFFS2_PATH, CX_CAS_FILE_KEYSTORE);
		break;

	case eCxNvmFile_PvrKey:
		snprintf(szFileName, 256, "%s%s", CX_CAS_FILE_JFFS2_PATH, CX_CAS_FILE_PVRKEY);
		break;

	case eCxNvmFile_CaPin:
		snprintf(szFileName, 256, "%s%s", CX_CAS_FILE_JFFS2_PATH, CX_CAS_FILE_CAPIN);
		break;

	default :
		HxLOG_Error("wrong nvm file type!!! [%d].\n", eNvmFileType);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pszFileName, szFileName, 256);

	return ERR_OK;
}

static HERROR local_cxDalNvram_Read(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulReadSize;
	HINT32			hError;
	HUINT8			szFileName[256];

	HxFILE_t		hFile;
	HERROR			hErr;

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("ReadType[%d], Offset[%d], Size[%d]\n", eNvmFileType, ulOffset, ulSize);

	hError = local_cxDalNvram_GetFileName(eNvmFileType, szFileName);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in _cxDalGetNvmFileName(%d).\n", hError);
		return ERR_FAIL;
	}

	hFile = HLIB_FILE_Open(szFileName, "r");
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", szFileName);
		return ERR_FAIL;
	}
	hErr = HLIB_FILE_Seek(hFile, ulOffset, eHxFILE_SEEK_SET, &nPosOffset);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	ulReadSize = HLIB_FILE_Read(hFile, pucData, 1, ulSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Read.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

static HERROR local_cxDalNvram_Write(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HINT32			hError;
	HUINT8			szFileName[256];

	HxFILE_t		hFile;
	HERROR			hErr;

	if (pucData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("WriteType[%d], Offset[%d], Size[%d]\n", eNvmFileType, ulOffset, ulSize);

	if (HLIB_DIR_IsExist(CX_CAS_FILE_JFFS2_PATH) != TRUE)
	{
		if (HLIB_DIR_Make(CX_CAS_FILE_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in PAL_FS_Mkdir\n");
			return ERR_FAIL;
		}
		else
		{
			HxLOG_Print("Created your Folder!!\r\n");
		}
	}

	hError = local_cxDalNvram_GetFileName(eNvmFileType, szFileName);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in _cxDalGetNvmFileName(%d).\n", hError);
		return ERR_FAIL;
	}

	if (HLIB_FILE_Exist(szFileName) == TRUE)
	{
		hFile = HLIB_FILE_Open(szFileName, "r+");
	}
	else
	{
		hFile = HLIB_FILE_Open(szFileName, "w");
	}

	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", szFileName);
		return ERR_FAIL;
	}
	hErr = HLIB_FILE_Seek(hFile, ulOffset, eHxFILE_SEEK_SET, &nPosOffset);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	ulWrittenSize = HLIB_FILE_Write(hFile, pucData, 1, ulSize);
	if (ulWrittenSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Write.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

static HERROR local_cxDalNvram_NvmDelete(CxNvmFileType_t eNvmFileType)
{
	HINT32		hError;
	HUINT8		szFileName[256];

	HxFILE_t	hFile;

	hError = local_cxDalNvram_GetFileName(eNvmFileType, szFileName);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in _cxDalGetNvmFileName(%d).\n", hError);
		return ERR_FAIL;
	}

	if (HLIB_FILE_Exist(szFileName) == TRUE)
	{
		hFile = HLIB_FILE_Open(szFileName, "r+");
	}
	else
	{
		hFile = HLIB_FILE_Open(szFileName, "w");
	}

	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", szFileName);
		return ERR_FAIL;
	}

	hError = HLIB_FILE_Delete(szFileName);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Delete(%d).\n", hError);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

static HERROR cxDalNvram_Read(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucBuf)
{
	HERROR	nResult			= ERR_FAIL;
	ENTER_FUNCTION;

	VK_SEM_Get(s_ulCxNvmSemaId);

	nResult = local_cxDalNvram_Read(eNvmFileType, ulOffset, ulSize, pucBuf);

	VK_SEM_Release(s_ulCxNvmSemaId);

	LEAVE_FUNCTION;
	return nResult;
}

static HERROR cxDalNvram_Write(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	HERROR	nResult			= ERR_FAIL;
	ENTER_FUNCTION;

	VK_SEM_Get(s_ulCxNvmSemaId);

	nResult = local_cxDalNvram_Write(eNvmFileType, ulOffset, ulSize, pucData);

	VK_SEM_Release(s_ulCxNvmSemaId);

	LEAVE_FUNCTION;
	return nResult;
}

static HERROR cxDalNvram_Delete(CxNvmFileType_t eNvmFileType)
{
	HERROR	nResult			= ERR_FAIL;
	ENTER_FUNCTION;

	VK_SEM_Get(s_ulCxNvmSemaId);

	nResult = local_cxDalNvram_NvmDelete(eNvmFileType);

	VK_SEM_Release(s_ulCxNvmSemaId);

	LEAVE_FUNCTION;
	return nResult;
}

static HERROR cxDalNvram_AsyncWrite(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	HERROR		nResult			= ERR_FAIL;
	CxNvmMsg_t	stNvmMsg;

	ENTER_FUNCTION;

	if (s_bCxNvmTaskStart == TRUE)
	{
		HxSTD_memset(&stNvmMsg, 0x00, sizeof(CxNvmMsg_t));
		stNvmMsg.eNvmCmd = eCxNvmCmd_Write;
		stNvmMsg.eNvmFileType = eNvmFileType;
		stNvmMsg.ulOffset = ulOffset;
		stNvmMsg.ulSize = ulSize;
		stNvmMsg.ulParam = (HUINT32)pucData;

		if (VK_MSG_Send(s_ulCxNvmMsqQId, &stNvmMsg, sizeof(CxNvmMsg_t)) == VK_OK)
		{
			nResult = ERR_OK;
		}
	}

	LEAVE_FUNCTION;
	return nResult;
}


/*******************************************************************/
#define ________CX_NVRAM_Public_Functions_Body________________________________
/*******************************************************************/
void CXNVM_Init(void)
{
	return local_cxDalNvram_Init();
}

HERROR CXNVM_Read(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucBuf)
{
	return cxDalNvram_Read(eNvmFileType, ulOffset, ulSize, pucBuf);
}

HERROR CXNVM_Write(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	return cxDalNvram_Write(eNvmFileType, ulOffset, ulSize, pucData);
}

HERROR CXNVM_Delete(CxNvmFileType_t eNvmFileType)
{
	return cxDalNvram_Delete(eNvmFileType);
}

HERROR CXNVM_AsyncWrite(CxNvmFileType_t eNvmFileType, HUINT32 ulOffset, HUINT32 ulSize, HUINT8* pucData)
{
	return cxDalNvram_AsyncWrite(eNvmFileType, ulOffset, ulSize, pucData);
}

