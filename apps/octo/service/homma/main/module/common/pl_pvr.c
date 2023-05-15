/**************************************************************************************/
/**
 * @file pal_pvr.c
 * defines APIs's body
 *
 * file operation
 *
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <htype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <linux/hdreg.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include <hlib.h>
#include <dlib.h>

#include "pl_fs.h"
#if 0
#include "pl_crypt.h"
#endif

#if 0
#include <stb_params.h>
#else
#include <pl_common.h>
#endif
#include <vkernel.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/* for ugly API */
#ifndef UNUSED
#define UNUSED(_x)		((void)(_x))
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static HERROR _pvr_loadJMetaFile(HCHAR *pszUrl, HCHAR	**ppHjmBuffer, HUINT32 *pulSize)
{
	PL_FS_UINT32 hjmFileHandle = -1;
	HUINT32		ulSize, ulReadedSize;
	HUINT64		ullSize;
	HCHAR		*hjmBuffer = NULL; /* (PL_FS_UINT8 *) */
	HERROR      hErr = ERR_FAIL;
	HUINT32		err = ERR_OK;

	if (NULL == pszUrl|| NULL == ppHjmBuffer)
	{
		HxLOG_Error("[%s/%d] NULL param \n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	err = PL_FS_Open((char*)pszUrl, PL_FS_MODE_READ, (PL_FS_UINT32 *)&hjmFileHandle);
	if (err != ERR_OK)
	{
		HxLOG_Error("[%s/%d] can't open the hjm (%s)\n", __FUNCTION__, __LINE__, pszUrl);
		goto func_error;
	}

	err = PL_FS_Seek (hjmFileHandle, 0, PL_FS_POS_END);
	if (ERR_OK != err)
	{
		HxLOG_Error("[%s/%d] can't seek to the hjm (%s)\n", __FUNCTION__, __LINE__, pszUrl);
		goto func_error;
	}

	err = PL_FS_Tell(hjmFileHandle, &ullSize);
	if (ERR_OK != err)
	{
		HxLOG_Error("[%s/%d] can't get the size of hjm (%s) \n", __FUNCTION__, __LINE__, pszUrl);
		goto func_error;
	}
	ulSize = (HUINT32)ullSize;

	hjmBuffer = HLIB_STD_MemCalloc(ulSize + 1);
	if (NULL == hjmBuffer)
	{
		HxLOG_Error("[%s/%d] can't get the memory for the hjm (%s) with the size (%d)\n"
			, __FUNCTION__, __LINE__, pszUrl, ulSize);
		goto func_error;
	}

	err = PL_FS_Seek (hjmFileHandle, 0, PL_FS_POS_START);
	if (ERR_OK != err)
	{
		HxLOG_Error("[%s/%d] can't seek to the start of hjm (%s)\n", __FUNCTION__, __LINE__, pszUrl);
		goto func_error;
	}

	err = PL_FS_Read(hjmFileHandle, hjmBuffer, ulSize, (PL_FS_UINT32 *)&ulReadedSize);
	if (ERR_OK != err)
	{
		HxLOG_Error("[%s/%d] can't read the hjm (%s) for the size (%d) and readed(%d)\n"
			, __FUNCTION__, __LINE__, pszUrl, ulSize, ulReadedSize);
		goto func_error;
	}

	hjmBuffer[ulSize] = '\0';
	*ppHjmBuffer= hjmBuffer;
	*pulSize	= ulSize;
	hjmBuffer	= NULL;			// to avoid mem free
	hErr		= ERR_OK;

	// 2. load the data - encrypted -> read & decrypt, normal text -> decode directly : TODO : use HxLib
func_error:
	if (-1 != hjmFileHandle)
		PL_FS_Close(hjmFileHandle);

	if (NULL != hjmBuffer)
		HLIB_STD_MemFree(hjmBuffer);

	return hErr;
}

DxRecListData_t *PL_PVR_LoadHJM(const HCHAR *szPath)
{
	HERROR ulRet;
	HxJSON_t hJson;
	HCHAR			*pHjmBuf = NULL;
	HUINT32 		ulHjmSize= 0;
	HUINT32 		ulRecDataSize= 0;
	DxRecListData_t *pstRecOrg = NULL;
	DxRecListData_t *pRec = NULL;

	if ((szPath == NULL) || (HLIB_FILE_Exist(szPath) == FALSE))
	{
		HxLOG_Error("Error> invalid path(%s)\n", szPath);
		return NULL;
	}

	if (HLIB_STD_StrEndWith(szPath, ".hjm") == FALSE)
	{
		//HxLOG_Error("Error> unsupported ext(%s)\n", szPath);
		return NULL;
	}


	ulRet = _pvr_loadJMetaFile((HCHAR *)szPath, &pHjmBuf, &ulHjmSize);
	if (ERR_OK != ulRet && ulHjmSize == 0)
	{
		HxLOG_Error("[%s:%d] (%s) file, failed, maybe meta file broken ? -> Just Keep for the Debug (Not list on the scaned list) \n"
				, __FUNCTION__,__LINE__, szPath);
		return NULL;
	}

	hJson = HLIB_JSON_Decode(pHjmBuf);
	HLIB_STD_MemFree(pHjmBuf);
	if (!hJson)
	{
		HxLOG_Error("[%s:%d] (%s) file, failed, maybe meta file broken ? -> Just Keep for the Debug (Not list on the scaned list) \n"
				, __FUNCTION__,__LINE__, szPath);
		return NULL;
	}

	pstRecOrg = DxRECLISTDATA_Decode(hJson, NULL);
	HLIB_JSON_Delete(hJson);
	if (pstRecOrg == NULL)
	{
		HxLOG_Error("[%s:%d] (%s) file, failed, maybe meta file broken ? -> Just Keep for the Debug (Not list on the scaned list) \n"
				, __FUNCTION__,__LINE__, szPath);
		return NULL;
	}
	ulRecDataSize = sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * pstRecOrg->ulExtEvtCnt;
	pRec = (DxRecListData_t*)HLIB_STD_MemAlloc(ulRecDataSize);
	if (pRec == NULL)
	{
		HxLOG_Error("Error> MemAlloc() fail!\n");
		DLIB_FreeType(DxNAMEOF(DxRecListData_t), pstRecOrg);
		return NULL;
	}
	HxSTD_MemCopy(pRec, pstRecOrg, ulRecDataSize);
	DLIB_FreeType(DxNAMEOF(DxRecListData_t), pstRecOrg);
	return pRec;
}

/* Reference xsvc_dlna_DmsCheckDtcpMgmtState() */
HERROR PL_PVR_CheckDtcpState(DxRecListData_t *pstRecData, HBOOL *isDtcpEnabled)
{
#if defined(CONFIG_OP_UK_DTT)
	switch (pstRecData->extension.ukdtt.ulFtaCmVal)
	{
		case eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc:
			*isDtcpEnabled = TRUE;
			break;
		case eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc_NoEnc:
			*isDtcpEnabled = FALSE;
			break;
		case eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy:
			*isDtcpEnabled = TRUE;
			break;
		case eDxUkdtt_CONTENT_MGMT_STATE_Reserved:		// D-BOOK 6.21 says that not-required
		case eDxUkdtt_CONTENT_MGMT_STATE_None:
			*isDtcpEnabled = FALSE;
			break;
		default:
			*isDtcpEnabled = FALSE;
			break;
	}
	HxLOG_Print("[%s:%d] ulFtaCmVal : %d, *isDtcpEnabled : %d\n", __FUNCTION__, __LINE__, pstRecData->extension.ukdtt.ulFtaCmVal, *isDtcpEnabled);
#else
	*isDtcpEnabled = FALSE;
#endif

	return ERR_OK;
}

