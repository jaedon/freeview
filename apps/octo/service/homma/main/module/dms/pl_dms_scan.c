/**************************************************************
*	@file	pl_dms_scan.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMS
**************************************************************/

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

#define	________________Header_Files_______________________________
#include "pl_dms.h"
#include "pl_fs.h"
#include "pl_common.h"
#include "pl_pvr.h"

#include <mxlib_config.h>
#include <mxkernel.h>
#include <mxdlna.h>
#include <mxdlna_dms.h>
#include <mxdlna_diskscan.h>
#include <mxdbc.h>
#include <mxdetect_define.h>
#include <mxmedia.h>
#include <mxmedia_define.h>
#include <mxfile_user.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define ________________Static_Variables___________________________
static 	PL_DMS_MXDLNA_HANDLE_t	pDmsHandles;

#define ________________Internal_Functions_________________________
// nothing

#define ________________Public_Interfaces__________________________
HERROR	PL_DMS_SetDmsHandles(void)
{
	if(PL_DMS_Get_MxDlna_Res(&pDmsHandles) != ERR_OK)
	{
		HxLOG_Debug("[%s:%d] DMS is not started. \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] DmsDB(%p) FSS(%p) Handle(%p) ........  ok\n", __FUNCTION__, __LINE__, \
			pDmsHandles.hDmsDB, pDmsHandles.hFSS, pDmsHandles.hDms);

	return ERR_OK;
}

HERROR	PL_DMS_StartScan( HLONG nInterval, HLONG nInterval_item )
{
	HINT32 nret = 0;

	if (pDmsHandles.hFSS == NULL)
	{
		HxLOG_Error("[PL_DMS] Error> invalid FSS handle for scan!\n");
		return ERR_FAIL;
	}

	/* Scan Interval 설정 */
	nret = mxDlnaFileScanner_setInterval( pDmsHandles.hFSS, nInterval, nInterval_item );
	if (nret != MXDLNA_OK)
	{
		HxLOG_Error("[PL_DMS] Error> failed to mxDlnaFileScanner_setInterval!\n");
		;
	}

	HxLOG_Print("[%s:%d] Start DMS Scan(total:%ld, item:%ld)\n", __FUNCTION__, __LINE__, nInterval, nInterval_item);

	/* Scan 시작,, Scan 중이면 No Action */
	nret = mxDlnaFileScanner_startScan( pDmsHandles.hFSS, 0 );
	if (nret != MXDLNA_OK)
	{
		HxLOG_Error("[PL_DMS] Error> failed to mxDlnaFileScanner_startScan!\n");
		return ERR_FAIL;
	}

	HxLOG_Print( "[ PL_DMS ] startScan ( %d ) \n", nret );
	return ERR_OK;
}

HERROR	PL_DMS_StopScan( void )
{
	/* Scan 정지 */
	if( pDmsHandles.hFSS )
	{
		HxLOG_Print("[%s:%d] Stop DMS Scan()\n", __FUNCTION__, __LINE__);
		mxDlnaFileScanner_stopScan( pDmsHandles.hFSS );
	}

	return ERR_OK;
}

HERROR	PL_DMS_AddScanDirectory( HCHAR *pScanPath, PL_DMS_ScanOpt_t *pstScanOpt )
{
	MXDLNA_FSS_SCANDATA	stScanData = { 0, };
	HLONG				lRet = 0;
	PL_DMS_ScanDepth_e	eDepth = ePL_DMS_ScanDepth_Unknown;

	if ((NULL == pScanPath) || (NULL == pstScanOpt))
	{
		HxLOG_Error( "[ PL_DMS ][%s] Error> NULL arg!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if (mxUtil_getFileDirType(pScanPath) != MXDLNA_FILEDIR_TYPE_DIRECTORY)
	{
		HxLOG_Error( "[ PL_DMS ] Error> unsupported fileType for addScan directory(%s)\n", pScanPath);
		return ERR_FAIL;
	}

	eDepth = pstScanOpt->eDepth;

	if( eDepth == ePL_DMS_ScanDepth_Unknown )
		eDepth = ePL_DMS_ScanDepth_Recursive;

	if( pDmsHandles.hFSS )
	{
		stScanData.path 	= pScanPath;
		stScanData.filter 	= pstScanOpt->pszFilter;
		stScanData.userData = pstScanOpt->pszUserData;
		stScanData.depth 	= eDepth;
		stScanData.option	= pstScanOpt->ePath;

		lRet = mxDlnaFileScanner_addDirectory( pDmsHandles.hFSS, &stScanData, NULL );
		HxLOG_Print( "[ PL_DMS ] AddScanPath( %d ), Path( %s ), Filter( %s ), OPTION( %d )\n", lRet, stScanData.path, stScanData.filter, stScanData.option );
	}

	return ERR_OK;
}

HERROR	PL_DMS_GetScanPathList( HINT32 *pnCount, HxList_t** ppScanPathList )
{
	MXDLNA_FSS_SCANDATA_LIST *pList = NULL;
	MXDLNA_FSS_DETACHABLE_SCANDATA_LIST *pDetachableList = NULL;
	HxList_t *pPathList = NULL;
	HINT32 i = 0;

	HOM_ASSERT(pnCount);
	HOM_ASSERT(ppScanPathList);

	if ( NULL == pDmsHandles.hFSS )
	{
		HxLOG_Error( "[ PL_DMS ] Error> NULL FSS handle!\n");
		return ERR_FAIL;
	}

	pList = mxDlnaFileScanner_getDirectories(pDmsHandles.hFSS);
	pDetachableList = mxDlnaFileScanner_getDetachableDirectories();
	if (pList == NULL && pDetachableList == NULL)
	{
		HxLOG_Error( "[ PL_DMS ] Error> can't get scan dirs!\n");
		*ppScanPathList = NULL;
		*pnCount = 0;
		return ERR_FAIL;
	}

	while(pList)
	{
		pPathList = HLIB_LIST_Append( pPathList, (void*)HLIB_STD_MemDup(&pList->value, sizeof(MXDLNA_FSS_SCANDATA)) );
		HxLOG_Debug("[%s:%d] PATH - %s \n", __func__, __LINE__, pList->value.path);
		pList = pList->pNext;
		i++;
	}

	while(pDetachableList)
	{
		pPathList = HLIB_LIST_Append( pPathList, (void*)HLIB_STD_MemDup(&pDetachableList->value, sizeof(MXDLNA_FSS_SCANDATA)) );
		HxLOG_Debug("[%s:%d] PATH - %s \n", __func__, __LINE__, pDetachableList->value.path);
		pDetachableList = pDetachableList->pNext;
		i++;
	}

	*ppScanPathList = pPathList;
	*pnCount = i;

	return ERR_OK;
}

HERROR PL_DMS_GetScanPath(void *pData, HCHAR **ppszScanPath)
{
	MXDLNA_FSS_SCANDATA *scanData = (MXDLNA_FSS_SCANDATA*)pData;

	if(scanData == NULL)
	{
		*ppszScanPath = NULL;
		return ERR_FAIL;
	}

	*ppszScanPath = scanData->path;
	return ERR_OK;
}

HERROR PL_DMS_GetScanOption(void *pData, HINT32 *pScanOption)
{
	MXDLNA_FSS_SCANDATA *scanData = (MXDLNA_FSS_SCANDATA*)pData;

	if(scanData == NULL)
	{
		*pScanOption = ePL_DMS_ScanPath_Normal;
		return ERR_FAIL;
	}

	*pScanOption = scanData->option;
	return ERR_OK;
}

HERROR PL_DMS_FreeScanPathList(HxList_t *pScanPathList)
{
	if(pScanPathList == NULL)
	{
		return ERR_FAIL;
	}

	HLIB_LIST_RemoveAllFunc( pScanPathList, HLIB_STD_MemFree_CB );
	pScanPathList = NULL;

	return ERR_OK;
}

HERROR	PL_DMS_RemoveScanDirectory( HCHAR *pScanPath )
{
	MXDLNA_FSS_SCANDATA	stScanData = { 0, };
	HLONG				lRet = 0;

	/*if (mxUtil_getFileDirType(pScanPath) != MXDLNA_FILEDIR_TYPE_DIRECTORY)
	{
		HxLOG_Error( "[ PL_DMS ] Error> unsupported fileType for removeScan directory(%s)\n", pScanPath);
		return ERR_FAIL;
	}*/

	if( pDmsHandles.hFSS )
	{
		stScanData.path = pScanPath;
		stScanData.filter = NULL;
		stScanData.userData = NULL;

		lRet = mxDlnaFileScanner_removeDirectory( pDmsHandles.hFSS, &stScanData, NULL );
		HxLOG_Print( "[ PL_DMS ] RemoveScanPath( %d )\n", lRet );
	}

	return ERR_OK;
}

HERROR PL_DMS_RemoveScanFile(HCHAR *pScanFile )
{
	MXDLNA_DMS_REMOVE_PROP *pRemoveProp = NULL;
	HLONG 					lRemovedMediaID = 0;

	if( mxUtil_getFileDirType( pScanFile ) == MXDLNA_FILEDIR_TYPE_DIRECTORY )
	{
		HxLOG_Error( "[ PL_DMS ] Error> unsupported fileType for removeScan file(%s)\n", pScanFile);
		return ERR_FAIL;
	}

	pRemoveProp = mxDlnaDmsDb_newRemoveProp( pScanFile );
	if( pRemoveProp == NULL )
	{
		HxLOG_Error("[ PL_DMS ] Error> Cant create removeProp for (%s)\n", pScanFile);
		return ERR_FAIL;
	}

	lRemovedMediaID = mxDlnaDmsDb_removeCdsItem( pDmsHandles.hDms, pDmsHandles.hDmsDB, pRemoveProp, NULL, SYSTEM_CONTAINER );
	HxLOG_Print("[ PL_DMS ] lRemovedMediaID : %d\n", lRemovedMediaID);

	(void)mxDlnaDmsDb_freeRemoveProp(pRemoveProp);
	return ERR_OK;
}

HERROR	PL_DMS_ScanDirect( HCHAR *pScanPath, PL_DMS_FILEMGR_TYPE_e eMgrType )
{
	// TODO: Sample.c smpDMS_directScan2() 참조 해서 작성해야 함.
	MXDLNA_FSS_SCANDATA	stScanData = { 0, };
	MXDLNA_FSS_SCANDATA *pTmpData = NULL;
	HLONG	lRet = 0;
	HINT32	scanPathLen = 0;
	HCHAR	folderPath[256] = { 0 };
	//HINT32	folderPathLen = 0;
	HINT32	pathLastIdx = 0;
	HINT32	index = -1;
	HUINT32	eObjType = 0;

	if( pDmsHandles.hFSS == NULL || pScanPath == NULL )
	{
		HxLOG_Error( "[ PL_DMS:%d ] Error> invalid scan arg!\n", __LINE__);
		return ERR_FAIL;
	}

	scanPathLen = HxSTD_StrLen(pScanPath);
	if ( sizeof(folderPath) < scanPathLen )
	{
		HxLOG_Error( "[ PL_DMS:%d ] Error> invalid scan path!\n", __LINE__);
		return ERR_FAIL;
	}

	/* pScanPath과 File 인지 Director인지 구별해서 처리해야 한다.*/
	eObjType = mxUtil_getFileDirType( pScanPath );

	if ( MXDLNA_FILEDIR_TYPE_FILE == eObjType )
	{
		HCHAR *temp = pScanPath;
		HINT32 i = 0;
		HUINT32	type = 0;

		/* file 일 경우 상위 Folder의 Path를 구해야 한다 */
		for( i = scanPathLen-1; i >= 0; i-- )
		{
			if ( FOLDER_SEP_CH == temp[i] )
			{
				HxSTD_MemCopy( folderPath, pScanPath, i+1 );
				type = mxUtil_getFileDirType( pScanPath );
				if ( MXDLNA_FILEDIR_TYPE_DIRECTORY == type )
				{
					break;
				}
			}
		}

	}
	else if ( MXDLNA_FILEDIR_TYPE_DIRECTORY == eObjType )
		HxSTD_StrNCpy( folderPath, pScanPath, sizeof(folderPath)-1 );
	else
		return ERR_FAIL;

	pathLastIdx = HxSTD_StrLen( folderPath );
	if ( FOLDER_SEP_CH == folderPath[ pathLastIdx] )
		folderPath[pathLastIdx] = '\0';

	pTmpData = (MXDLNA_FSS_SCANDATA *)mxDlnaFileScanner_getMatchedScanList( pDmsHandles.hFSS, (const char*)folderPath, &index );

	stScanData.path = folderPath;
	stScanData.filter = pTmpData->filter;
	stScanData.userData = NULL;
	stScanData.depth = ePL_DMS_ScanDepth_OneDepth;

	lRet = mxDlnaFileScanner_directScanStart( pDmsHandles.hFSS, &stScanData, index, 1 );
	HxLOG_Print( "[ PL_DMS ] DirectScan( %d ), Path( %s ), Filter( %s )\n", lRet, stScanData.path, stScanData.filter );

	return ERR_OK;
}


HERROR	PL_DMS_RescanFSS( void )
{
	HxLOG_Print( "[ PL_DMS ] RescanFSS++\n");

	PL_DMS_StopScan();
	PL_DMS_StartScan( PL_DMS_DEFAULT_RESCAN_INTERVAL, PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE );

	HxLOG_Print( "[ PL_DMS ] RescanFSS--\n");
	return ERR_OK;
}

