/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_fs_config.c
	@brief

	Description:  									\n
	Module: UMMA File System				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include "umma_fs_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	struct
{
	UMMA_FS_DevType_e	eDevType;
	UMMA_FS_FsType_e	eDefaultFs;	// Format 시
	UMMA_FS_PvrType_e	ePvrType;
}UMMA_FS_CONFIG_t;

typedef	struct
{
	HCHAR	szDevName[16];
	UMMA_FS_DevType_e	eDevType;
}UMMA_FS_DEVMAP_t;
typedef	struct
{
	HCHAR	szPVR[64];
	UMMA_FS_PvrType_e	ePvrType;
}UMMA_FS_PVRMAP_t;
typedef	struct
{
	HCHAR	szFS[8];
	UMMA_FS_FsType_e		eDefaultFs;	// Format 시
}UMMA_FS_FSMAP_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static	HBOOL	bProfileLoaded = FALSE;
static UMMA_FS_CONFIG_t	*s_pstFsConfig;
static	HUINT32		s_ulDevCnt=0;

static UMMA_FS_DEVMAP_t s_astDevMap[6] = {
									{"Internal", eFS_DevType_IntHdd},
									{"Detachable", eFS_DevType_Detachable},
									{"eSATA_HDD", eFS_DevType_eSataHdd},
									{"USB_HDD", eFS_DevType_UsbHdd},
									{"USB_Flash", eFS_DevType_UsbMem},
									{"MMC", eFS_DevType_MMC}
									};
// TODO: should parse seperator '|' and add flags (Not add type every time new flags are needed)
// TODO: Main = record |playback | tsr
// TODO: Sub = subrecord | record | playback | tsr
// TODO: RecStorage = playback
// TODO: Tsr = tsr
static UMMA_FS_PVRMAP_t s_astPvrMap[5] = {
									{"Main", eFS_PVR_Main},
									{"Sub", eFS_PVR_Sub},
									{"Tsr", eFS_PVR_Tsr},
									{"RecStorage", eFS_PVR_RecStorage},
									{"Main|RecStorage", eFS_PVR_Main|eFS_PVR_RecStorage},
									{"None", eFS_PVR_None},
									};
static UMMA_FS_FSMAP_t s_astFsMap[9] = {
									{"EXT2", eFS_FileSystem_EXT2},
									{"EXT3", eFS_FileSystem_EXT3},
									{"EXT4", eFS_FileSystem_EXT4},
									{"XFS", eFS_FileSystem_XFS},
									{"FAT16", eFS_FileSystem_FAT16},
									{"FAT32", eFS_FileSystem_FAT32},
									{"NTFS", eFS_FileSystem_NTFS},
									{"DOS", eFS_FileSystem_DOS},
									{"None", eFS_FileSystem_NONE},
									};

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define	_____LOCAL_STATIC_FUNCTION_______
static HERROR	umma_fs_config_parser(HxJSON_t hJson, HUINT32	ulIdx)
{
	HUINT32	i;
	HCHAR *pszItem;
	HBOOL	bFind = FALSE;

	/////// Device Type ////////
	pszItem =  (HCHAR*)HLIB_JSON_Object_GetString(hJson, "TYPE");
	if (pszItem)
	{
		for (i =0 ; i<6 ; i++)
		{
			if ((HxSTD_StrCmp(pszItem, s_astDevMap[i].szDevName) == 0))
			{
				s_pstFsConfig[ulIdx].eDevType = s_astDevMap[i].eDevType;
				bFind = TRUE;
				break;
			}
		}

		if (bFind != TRUE)
		{
			HxLOG_Error("Can't get device type !!  \n");
			s_pstFsConfig[ulIdx].eDevType = eDxSTORAGE_DEVTYPE_NONE;
		}
	}

	///////// PVR  //////////
	bFind = FALSE;
	pszItem =  (HCHAR*)HLIB_JSON_Object_GetString(hJson, "PVR");
	if (pszItem)
	{
		for (i =0 ; i<5 ; i++)
		{
			if ((HxSTD_StrCmp(pszItem, s_astPvrMap[i].szPVR) == 0))
			{
				s_pstFsConfig[ulIdx].ePvrType = s_astPvrMap[i].ePvrType;
				bFind = TRUE;
				break;
			}
		}
		if (bFind != TRUE)
		{
			HxLOG_Error("Can't get PVR type !!  \n");
			s_pstFsConfig[ulIdx].ePvrType = eFS_PVR_None;
		}
	}
	/////// Default file system  ////////
	bFind = FALSE;
	pszItem =  (HCHAR*)HLIB_JSON_Object_GetString(hJson, "FS");
	if (pszItem)
	{
		for (i =0 ; i<8 ; i++)
		{
			if ((HxSTD_StrCmp(pszItem, s_astFsMap[i].szFS) == 0))
			{
				s_pstFsConfig[ulIdx].eDefaultFs = s_astFsMap[i].eDefaultFs;
				bFind = TRUE;
				break;
			}
		}
		if (bFind != TRUE)
		{
			HxLOG_Error("Can't get File system type !!  \n");
			s_pstFsConfig[ulIdx].eDefaultFs = eFS_FileSystem_NONE;
		}
	}

	return ERR_OK;

}

#define     _____EXTERNAL_FUNCTION______
void umma_fs_config_Print(void)
{
	int i,j;
	HLIB_CMD_Printf("\n[%s] [%d]\n============== Storage config information==================\n",__FUNCTION__,__LINE__);

	for(i=0 ; i<s_ulDevCnt ; i++)
	{
		for (j =0 ; j<6 ; j++)
		{
			if (s_pstFsConfig[i].eDevType == s_astDevMap[j].eDevType)
			{
				HLIB_CMD_Printf(" DevType : %10s\n", s_astDevMap[j].szDevName);
				break;
			}
		}

		for (j =0 ; j<5 ; j++)
		{
			if (s_pstFsConfig[i].ePvrType == s_astPvrMap[j].ePvrType)
			{
				HLIB_CMD_Printf(" PVR : %s\n", s_astPvrMap[j].szPVR);
				break;
			}
		}

		for (j =0 ; j<8 ; j++)
		{
			if (s_pstFsConfig[i].eDefaultFs == s_astFsMap[j].eDefaultFs)
			{
				HLIB_CMD_Printf(" FS: %10s\n", s_astFsMap[j].szFS);
				break;
			}
		}
		HLIB_CMD_Printf("\n");
	}
	HLIB_CMD_Printf("==============================================\n");
}

HERROR	umma_fs_config_Init(void)
{
	HxJSON_t	hJson = NULL;
	HxJSON_t	hObj, hObj2;
	HINT32	nCount = 0;
	HUINT32	i;
	HCHAR	szIdx[3]={0,};

	hJson = HLIB_JSON_DecodeFile(CONFIG_PROD_DEVICE_STORAGE_PROFILE_PATH);
	if (!hJson)
	{
		HxLOG_Critical("============== Warning!! ================= \n");
		HxLOG_Critical("!!!!!!!! Please add storage profile in \"umma/main/module/fs/profile\" !!!!!!!!!! \n",__FUNCTION__,__LINE__,__FILE__);
		HxLOG_Critical(" \n");
		HxLOG_Assert(0);
		return ERR_FAIL;
	}
	nCount = HLIB_JSON_Object_GetInteger(hJson, "DeviceCount");

	if (nCount <= 0)
	{
		HxLOG_Warning("This STB does not support any kind of storages... \n");
		HLIB_JSON_Delete(hJson);
		return ERR_FAIL;
	}

	s_ulDevCnt = nCount;

	s_pstFsConfig = (UMMA_FS_CONFIG_t *)HLIB_STD_MemAlloc(sizeof(UMMA_FS_CONFIG_t) * nCount);
	hObj = HLIB_JSON_Object_GetObject(hJson, "Profile");

	for(i = 0 ; i < nCount ; i++)
	{
		HxSTD_snprintf(szIdx, 3, "%d", i+1);
		hObj2 = HLIB_JSON_Object_GetObject(hObj, szIdx);
		umma_fs_config_parser(hObj2, i);
	}
	HLIB_JSON_Delete(hJson);

	bProfileLoaded = TRUE;

	return ERR_OK;
}

// 이 Device의 format시 해당 모델이 지원하는 file system을 가져온다.
UMMA_FS_FsType_e umma_fs_config_GetDefaultFS(UMMA_FS_DevType_e eDevType)
{
	HUINT32	ulIdx;
	UMMA_FS_FsType_e	eFs = eFS_FileSystem_NONE;

	for(ulIdx=0 ; ulIdx<s_ulDevCnt ; ulIdx++)
	{
		if (s_pstFsConfig[ulIdx].eDevType == eDevType)
		{
			eFs = s_pstFsConfig[ulIdx].eDefaultFs;
			break;
		}
	}

	return eFs;
}

// 해당 모델이 지원하는 이 Device의 PVR type을 가져온다.
UMMA_FS_PvrType_e umma_fs_config_GetPvrType(UMMA_FS_DevType_e eDevType)
{
	HUINT32 ulIdx;
	UMMA_FS_PvrType_e	ePvrType = eFS_PVR_None;

	for(ulIdx=0 ; ulIdx<s_ulDevCnt ; ulIdx++)
	{
		if (s_pstFsConfig[ulIdx].eDevType == eDevType)
		{
			ePvrType = s_pstFsConfig[ulIdx].ePvrType;
			break;
		}
	}

	return ePvrType;
}

// return : the number of device.
HUINT32 umma_fs_config_GetSupportedDeviceList(UMMA_FS_DevType_e *peList)
{
	HUINT32 ulIdx;

	if (peList == NULL)
	{
		HxLOG_Error("peList == NULL \n");
		return 0;
	}

	// peList must be an already alloced array..
	for(ulIdx=0 ; ulIdx<s_ulDevCnt ; ulIdx++)
	{
		peList[ulIdx] = s_pstFsConfig[ulIdx].eDevType;
	}

	return s_ulDevCnt;
}



