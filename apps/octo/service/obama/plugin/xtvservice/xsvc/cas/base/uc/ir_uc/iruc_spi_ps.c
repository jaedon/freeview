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

#include "octo_common.h"
#include "UniversalClient_Common_SPI.h"
#include "UniversalClient_Common_API.h"

#include "iruc_spi.h"
#include "ir_api.h"
#include "ir_evt.h"
#include <_svc_cas_mgr_common.h>


#define IRUC_PS_FILE_NAME_LEN_MAX 	(128)
#define IRUC_PS_FILE_JFFS2_PATH		"/var/lib/humaxtv/cas/irUC/"
#define IRUC_PS_FILE_LIB_PATH		"/var/lib/humaxtv/cas/iruc_backup/"
#define IRUC_PS_FILE 				"IrUC_PSFile"

static HBOOL s_bIrUcPS_Initailized = FALSE;
static HULONG s_ulUcSpiPsSem;

#define PS_SEM_GET				VK_SEM_Get(s_ulUcSpiPsSem)
#define PS_SEM_RELEASE		VK_SEM_Release(s_ulUcSpiPsSem)


/** @defgroup psspi Universal Client Persistent Storage SPIs
 *  Universal Client Persistent Storage SPIs
 *
 *  These SPIs implement the Persistent Storage layer that is required in order to save contents of EMMs
 *  that are delivered to the device. This persistent storage layer must be transaction safe, such that if
 *  the device loses power in the middle of a 'write' operation, the data is not corrupted and the device
 *  can continue to function when it is powered back on.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */
/**
 * Deletes an existing resource.
 *
 * This function is called to delete an existing resource in the Persistent Store, identified by 'index'.
 * If the resource
 * cannot be found, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * The index may have previously been created by a call to ::UniversalClientSPI_PS_Write.
 * A very simple implementation of this may just delete the file corresponding with the index of the resource.
 *
 * @param[in] index Index of the resource to delete.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_PS_Delete(uc_uint32 index)
{
	HCHAR	szFileName[IRUC_PS_FILE_NAME_LEN_MAX+1] = {0,};
	HINT32	nErrCode;
	PAL_FS_UINT32 hFileHandle = 0;

	PS_SEM_GET;

	HxLOG_Info("idx = %d\n", index);

	if(!s_bIrUcPS_Initailized)
	{
		PS_SEM_RELEASE;
		return UC_ERROR_ALREADY_STOPPED;
	}

	if (svc_cas_DevFsIsDirExist(IRUC_PS_FILE_JFFS2_PATH) != TRUE)
	{
		HxLOG_Error("Directory(%s) is not found.\n", IRUC_PS_FILE_JFFS2_PATH);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	HxSTD_snprintf(szFileName, IRUC_PS_FILE_NAME_LEN_MAX, "%s%s_%d", IRUC_PS_FILE_JFFS2_PATH, IRUC_PS_FILE, index);
	nErrCode = svc_cas_DevFsOpen(0, szFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("not found file (%s)\n", szFileName);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	nErrCode = svc_cas_DevFsDelete(0, szFileName);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsDelete(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	// TODO: open 으로 연 아이를 fflush 하면 안됨. open을 fopen으로 바꾸던가,... 나중에 확인 해주세요.
	fsync(hFileHandle);
	//HLIB_FILE_Flush((HxFILE_t)hFileHandle);

	svc_cas_DevFsClose(hFileHandle);

	PS_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}

/**
 * Writes data to an existing resource.
 *
 * This function is called to write a block of data to a resource identified by an index.  The data length and offset
 * within the resource to begin writing is also specified.
 *
 * If the resource did not exist before the call, it should be created.
 *
 * A very simple implementation of this may just write the data to a file corresponding to the index of the resource.
*
 * @param[in] index Index of the resource to write into. Different indexes correspond to completely different resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.
 * @param[in] offset Offset within the resource to begin writing. Data before this offset must not be affected.
 * @param[in] pData Buffer containing data to write to the resource. The uc_bytes::bytes member
 *     points to the start of the buffer, and the uc_bytes::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Write(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData)
{
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HINT32	nErrCode;
	HUINT8	szFileName[IRUC_PS_FILE_NAME_LEN_MAX+1] = {0,};
	HMSG	hmsg;

	PS_SEM_GET;

	HxLOG_Info("idx=%d\n", index);

	if(!s_bIrUcPS_Initailized)
	{
		PS_SEM_RELEASE;
		return UC_ERROR_ALREADY_STOPPED;
	}

	if (pData->bytes == NULL)
	{
		HxLOG_Error("pData->bytes NULL\n");
		PS_SEM_RELEASE;
		return UC_ERROR_NULL_PARAM;
	}

/* CA file handling을 알아보기 위해 switch를 수행한 것으로 release시에는 확인될 필요 없으니 DEBUG로 묶음. */
#if defined(CONFIG_DEBUG)
	switch(index)
	{
		case 0:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("SecureCore...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		case 1:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("FlexiFlash...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		case 9:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("CCA CFG...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		default:
			break;
	}
#endif

	if (svc_cas_DevFsIsDirExist(IRUC_PS_FILE_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(IRUC_PS_FILE_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in svc_cas_DevFsMkdir\n");
			PS_SEM_RELEASE;
			return UC_ERROR_UNABLE_TO_CREATE;
		}
	}

	HxSTD_snprintf(szFileName, IRUC_PS_FILE_NAME_LEN_MAX, "%s%s_%d", IRUC_PS_FILE_JFFS2_PATH, IRUC_PS_FILE, index);

	HxLOG_Print("filename = %s, Offset[%d], Size[%d]\n",szFileName, offset, pData->length);

	nErrCode = svc_cas_DevFsOpen(0, szFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, offset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pData->bytes, pData->length, &ulWrittenSize);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsWrite(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	svc_cas_DevFsClose(hFileHandle);

/* index_1 copy 완료 되어서 app에 update msg 보낸다. */
	if(index == 1)
	{
		hmsg.usMsgClass = IREVT_CLASS_FLEXIFLASH_STATUS_UPDATE;
		hmsg.ulParam = (HUINT32)NULL;
		hmsg.usParam = (HUINT32)1 ;
		CAS_IR_EVT_SendEvent(CAS_IRUC_GetActionHandleFlexiFlash() , &hmsg);
	}

	PS_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}

/**
 * Reads data from an existing resource.
 *
 * This function is called to read a block of data from a resource identified by an index. The data length and offset
 * within the resource to begin reading is also specified.
 *
 * If the resource did not exist before the call, the function should return UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * A very simple implementation of this may just read the data from a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to write into. Different indexes correspond to completely different resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.
 * @param[in] offset Offset within the resource to begin reading. Data before this offset must not be affected.
 * @param[in,out] pData Buffer to read resource data into.
 *     On input, the uc_bytes::bytes member points to a buffer in memory, and the uc_bytes::length member
 *     to the length of the buffer, in bytes.  The function will read uc_bytes::length bytes from the resource
 *     and store them in uc_bytes::bytes.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Read(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData)
{
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	nReadedLength;
	HINT32	nErrCode;
	HUINT8	szFileName[IRUC_PS_FILE_NAME_LEN_MAX];

	PS_SEM_GET;

	HxLOG_Info("idx = %d, offset= %d\n", index, offset);

	if(!s_bIrUcPS_Initailized)
	{
		PS_SEM_RELEASE;
		return UC_ERROR_ALREADY_STOPPED;
	}

	if (pData->bytes == NULL)
	{
		HxLOG_Error("pData->bytes NULL\n");
		PS_SEM_RELEASE;
		return UC_ERROR_NULL_PARAM;
	}

/* CA file handling을 알아보기 위해 switch를 수행한 것으로 release시에는 확인될 필요 없으니 DEBUG로 묶음. */
#if defined(CONFIG_DEBUG)
	switch(index)
	{
		case 0:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("SecureCore...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		case 1:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("FlexiFlash...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		case 9:
			HxLOG_Warning(HxANSI_COLOR_YELLOW("CCA CFG...(%s_%d)")"\n", IRUC_PS_FILE, index);
			break;

		default:
			break;
	}
#endif

	HxSTD_snprintf(szFileName, IRUC_PS_FILE_NAME_LEN_MAX, "%s%s_%d", IRUC_PS_FILE_JFFS2_PATH, IRUC_PS_FILE, index);

	HxLOG_Print("file = %s, Offset[%d], Size[%d]\n", szFileName,  offset, pData->length);

	nErrCode = svc_cas_DevFsOpen(0, szFileName, PAL_FS_MODE_READ, &hFileHandle);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
		HxSTD_snprintf(szFileName, IRUC_PS_FILE_NAME_LEN_MAX, "%s%s_%d", IRUC_PS_FILE_LIB_PATH, IRUC_PS_FILE, index);
		nErrCode = svc_cas_DevFsOpen(0, szFileName, PAL_FS_MODE_READ, &hFileHandle);
		if (nErrCode)
		{
			HxLOG_Error("second error occurred in svc_cas_DevFsOpen(%d).\n", nErrCode);
			PS_SEM_RELEASE;
			return UC_ERROR_RESOURCE_NOT_FOUND;
		}
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, offset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsLSeek(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	nErrCode = svc_cas_DevFsRead(hFileHandle, pData->bytes, pData->length, &nReadedLength );
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsRead(%d).\n", nErrCode);
		svc_cas_DevFsClose(hFileHandle);
		PS_SEM_RELEASE;
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	svc_cas_DevFsClose(hFileHandle);

	PS_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}

/**
 * Performs any initialization of the persistent storage layer
 *
 * This function is called during to allow any initialization of the persistent storage layer to occur
 * before any calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete are made.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */

uc_result UniversalClientSPI_PS_Initialize(void)
{
	PS_SEM_GET;
	HxLOG_Info("\n" );
	s_bIrUcPS_Initailized = TRUE;
	PS_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Performs any termination of the persistent storage layer
 *
 * This function is called during to allow any clean up of the persistent storage layer done by ::UniversalClientSPI_PS_Initialize to occur.
 * No calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete will be made after this function
 * returns.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Terminate(void)
{
	PS_SEM_GET;
	HxLOG_Info("\n" );
	s_bIrUcPS_Initailized = FALSE;
//	PAL_FS_Flush();
	sync();
	PS_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}


void CAS_IRUC_SPI_PS_Init()
{
	if(VK_SEM_Create(&s_ulUcSpiPsSem, "s_ulUcSpiPsSem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Error("Semaphore Create Error\n");
	}

	/* Index 10, 11, 12는 Pre-Enablement Product 용도이므로 read error 발생하더라도 무시하면 됩니다. */
	return;
}

/** @} */ /* End of ps */
