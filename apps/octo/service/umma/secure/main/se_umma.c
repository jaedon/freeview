/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma.c
	@brief

	Description:  									\n
	Module: Umma API Module			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mount.h>

#include	<unistd.h>
#include	<dirent.h>
#include <fcntl.h>

#include	<vkernel.h>
#include	<hlib.h>
#include <pwd.h>

#include	"se_umma_int.h"
#include	"se_uapi.h"
#include	<ummalib.h>

#include	"di_fs.h"
#include	"di_hdd.h"
#include	"di_usb.h"
#include	"drv_hotplug.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static HINT32	s_nServerHandle;
static HINT32	s_nRPCHandle;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

HERROR	seumma_ConvertFormatListToDi(SEUMMA_FormatList_t *pstIn, DI_FS_FormatList_t *pstOut)
{
	DI_FS_FormatItem_t		*pstPartParam;

	if (pstIn == NULL || pstOut == NULL)
	{
		return ERR_FAIL;
	}

	pstOut->nNumCurrPartition= pstIn->nNumCurrPartition;
	pstOut->nMaxFormatPartition = pstIn->nMaxFormatPartition;

	if (pstIn->nNumFormatPartition > 0)
	{
		pstPartParam = (DI_FS_FormatItem_t *)FS_MEM_Alloc (sizeof(DI_FS_FormatItem_t) * pstIn->nNumFormatPartition);
		if (pstPartParam == NULL)
		{
			FS_MEM_Free (pstPartParam);
			return ERR_FAIL;
		}

		HxSTD_memcpy (pstPartParam, &(pstIn->stPartParam), sizeof(DI_FS_FormatItem_t) * pstIn->nNumFormatPartition);

		pstOut->nNumFormatPartition = pstIn->nNumFormatPartition;
		pstOut->pstPartParam = pstPartParam;
	}

	return ERR_OK;
}

static void	seumma_FormatCallback(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxLOG_Warning("ulEvent : %d, ulParam1 : %d, , ulParam2 : %d, ulParam3 : %d\n", ulEvent, ulParam1, ulParam2, ulParam3);
	HLIB_RPC_Notify(s_nRPCHandle, RPC_SEUMMA_onFormatProgress, "iiii", ulEvent, ulParam1, ulParam2, ulParam3);
}

static void	seumma_DstCallback(HDD_DST_RESULT dstResult, HINT32 nProgress)
{
	HxLOG_Warning("dstResult : %d, nProgress : %d\n", dstResult, nProgress);
	HLIB_RPC_Notify(s_nRPCHandle, RPC_SEUMMA_onDstNotifier, "ii", dstResult, nProgress);
}


static DI_ERR_CODE	seumma_HddEventCallback(DI_HDD_Event_t etEventType, void *pucHddInfo)
{
	HxLOG_Warning("etEventType : %d\n", etEventType);

	HLIB_RPC_Notify(s_nRPCHandle, RPC_SEUMMA_onHddNotifier, "ib", etEventType, pucHddInfo, sizeof(hdd_info_t));

	return DI_ERR_OK;
}

static DI_ERR_CODE seumma_UsbEventCallback (DI_USB_Event_t etUsbEventType, void *pvUsbInfo)
{
	usb_info_t	*pstUsbInfo = (usb_info_t *)pvUsbInfo;
	SEUMMA_USB_Info_t	stSeUmmaUsbInfo;

	HxSTD_memset(&stSeUmmaUsbInfo, 0, sizeof(stSeUmmaUsbInfo));

	HxLOG_Warning("etUsbEventType : %d\n", etUsbEventType);
	stSeUmmaUsbInfo.ulDevCount = pstUsbInfo->devCount;
	stSeUmmaUsbInfo.ulDevNameSize = pstUsbInfo->devNameSize;
	stSeUmmaUsbInfo.ulPortNum = pstUsbInfo->ePortNum;
	stSeUmmaUsbInfo.ulSpeed = pstUsbInfo->eSpeed;
	stSeUmmaUsbInfo.bOverCurrent = pstUsbInfo->isOverCurrent;

	if (pstUsbInfo->devNameSize > 0)
	{
		HxSTD_memcpy(stSeUmmaUsbInfo.szDevNameBuf, pstUsbInfo->devName[0], pstUsbInfo->devNameSize);
	}

	HLIB_RPC_Notify(s_nRPCHandle, RPC_SEUMMA_onUsbNotifier, "ib", etUsbEventType, (void *)&stSeUmmaUsbInfo, sizeof(SEUMMA_USB_Info_t));

	return DI_ERR_OK;
}

static HERROR seumma_HotplugEventCallback (HUINT32 ulSlotIdx, HUINT32 eDevType, void *pvHotplugInfo)
{
	UMMALIB_HP_Info_t	*pstHpInfo = (UMMALIB_HP_Info_t *)pvHotplugInfo;
	UMMALIB_HP_Info_t 	stSeUmmaHpInfo;

	HxLOG_Warning("eDevType : %d\n", eDevType);

	HxSTD_memcpy(&stSeUmmaHpInfo, pstHpInfo, sizeof(UMMALIB_HP_Info_t));

	HLIB_RPC_Notify(s_nRPCHandle, RPC_SEUMMA_onHotplugNotifier, "iib", ulSlotIdx, eDevType, (void *)&stSeUmmaHpInfo, sizeof(UMMALIB_HP_Info_t));

	return DI_ERR_OK;
}


static HERROR	seumma_rpc_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 0);
	HxLOG_Trace();

	HxLOG_Warning("Welcome!!\n");
	DRV_HOTPLUG_Init();

	return ERR_OK;
}


//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_Mount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT8 *szDstMountDir;
	HUINT32 eFsType;
	HERROR	hErr = ERR_FAIL,  hRes = ERR_FAIL;
	HCHAR	szOpt[128] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 4);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx	= HxOBJECT_INT(apArgv[1]);
	szDstMountDir =HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));
	eFsType = HxOBJECT_INT(apArgv[3]);

	switch (eFsType)
	{
		case DI_FS_FAT16_FILESYSTEM:
		case DI_FS_FAT32_FILESYSTEM:
			HxSTD_snprintf(szOpt, 128,"iocharset=utf8,uid=%d,gid=%d,fmask=0113,dmask=0002", ulUid, ulGid);
			break;

		case DI_FS_NTFS_FILESYSTEM:
			HxSTD_snprintf(szOpt, 128,"nls=utf8,uid=%d,gid=%d,fmask=0113,dmask=0002", ulUid, ulGid);
			break;
	}

	hErr = DI_FS_MountPartitionOpt(eDevIdx, ulPartIdx, szDstMountDir,eFsType, (DI_FS_MNTOPT_NOEXEC|DI_FS_MNTOPT_NOSUID|DI_FS_MNTOPT_NODEV), szOpt);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}
	else
	{
		hRes = HLIB_EXTRA_Chown(szDstMountDir, "settop", "settop");
		if (hRes != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", szDstMountDir);
		}

		hRes = HLIB_EXTRA_Chmod(szDstMountDir, 0770);
		if (hRes != ERR_OK)
		{
			HxLOG_Error(" Fail to chmod path = %s \n", szDstMountDir);
		}
	}
	if (szDstMountDir)		{HLIB_MEM_Free(szDstMountDir);}
	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_Umount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	DI_FileSystemType_t eFsType;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 3);

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx	= HxOBJECT_INT(apArgv[1]);
	eFsType	= HxOBJECT_INT(apArgv[2]);

	HxLOG_Trace();

	hErr = DI_FS_UnmountPartition(eDevIdx, ulPartIdx, eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

static HERROR seumma_rpc_Mount_Ftps(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszStorPath, *pszBindPath;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszStorPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszBindPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));



	hErr = HLIB_DIR_Make( pszBindPath );

	HxSTD_snprintf( szCmd, sizeof(szCmd), "mount --bind %s %s", pszStorPath, pszBindPath);

	VK_SYSTEM_Command( szCmd );

	if (pszStorPath)		{ HLIB_MEM_Free(pszStorPath); }
	if (pszBindPath)		{ HLIB_MEM_Free(pszBindPath); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

static HERROR seumma_rpc_Mount_Smbs(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszStorPath, *pszBindPath;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszStorPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszBindPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));



	hErr = HLIB_DIR_Make( pszBindPath );

	HxSTD_snprintf( szCmd, sizeof(szCmd), "mount --bind %s %s", pszStorPath, pszBindPath);

	VK_SYSTEM_Command( szCmd );

	if (pszStorPath)		{ HLIB_MEM_Free(pszStorPath); }
	if (pszBindPath)		{ HLIB_MEM_Free(pszBindPath); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}


static HERROR seumma_rpc_DoCommand(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszCmd, *pszBindPath;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszCmd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszBindPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));

	HxSTD_snprintf( szCmd, sizeof(szCmd), "%s %s", pszCmd, pszBindPath );
	VK_SYSTEM_Command( szCmd );

	if (pszCmd)		{ HLIB_MEM_Free(pszCmd); }
	if (pszBindPath)		{ HLIB_MEM_Free(pszBindPath); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}


static HERROR seumma_rpc_Start_Ftps(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszCmdFtpd, *pszConfigFile;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszCmdFtpd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszConfigFile = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));

	HxSTD_snprintf(szCmd, sizeof(szCmd)-1, "%s%s -c %s -S %s", "/usr/bin/", pszCmdFtpd, pszConfigFile, "0.0.0.0" );
	// use for debug FTP
	//HxSTD_StrNCat(szCmd, " -n -d 20", sizeof(szCmd)-1);

	VK_SYSTEM_Command (szCmd);

	if (pszCmdFtpd)		{ HLIB_MEM_Free(pszCmdFtpd); }
	if (pszConfigFile)		{ HLIB_MEM_Free(pszConfigFile); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

static HERROR seumma_rpc_Start_Smbs(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszCmd, *pszConfigFile;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszCmd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszConfigFile = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));

	HxSTD_snprintf( szCmd, sizeof(szCmd), "%s -s %s", pszCmd, pszConfigFile );
	VK_SYSTEM_Command (szCmd);

	if (pszCmd)		{ HLIB_MEM_Free(pszCmd); }
	if (pszConfigFile)		{ HLIB_MEM_Free(pszConfigFile); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

static HERROR seumma_rpc_PopenCmd_Smbc(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszCmd = NULL, *pszResultFile = NULL;
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulUid = 0, ulGid = 0;

	FILE	*pipeFd = NULL;
	FILE	*pfile = NULL;
	HCHAR	szBuf[128] = { 0 } ;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszCmd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	if(pszCmd == NULL)
	{
		HxLOG_Error("[SMBC] cmd is Null.\n");
		goto END_FUNC;
	}
	pszResultFile = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));
	if(pszResultFile == NULL)
	{
		HxLOG_Error("[SMBC] result File is Null.\n");
		goto END_FUNC;
	}

	pipeFd = popen(pszCmd, "r" );
	if( pipeFd == NULL)
	{
		HxLOG_Error("[SMBC] pOpenCmd failed.\n");
		goto END_FUNC;
	}

	/* 2. parsing output data from pipe  */
	pfile = fopen(pszResultFile, "w");
	if( pfile == NULL)
	{
		HxLOG_Error("[SMBC] Failed to open %s\n", pszResultFile);
		goto END_FUNC;
	}

	while (fgets( szBuf, sizeof(szBuf), pipeFd ) )
	{
		fprintf(pfile, szBuf);
	}

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

END_FUNC:
	if (pfile)			{ fclose(pfile); }
	if (pipeFd)			{ pclose(pipeFd); }
	if (pszCmd)			{ HLIB_MEM_Free(pszCmd); }
	if (pszResultFile)	{ HLIB_MEM_Free(pszResultFile); }

	return hErr;
}

static HERROR seumma_rpc_SetPassword_Smbs(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszCmd, *pszNewPasswd;
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulUid = 0, ulGid = 0;
	FILE	*pipeFd = NULL;

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	pszCmd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszNewPasswd = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));

	pipeFd = popen( pszCmd, "w" );
	if (NULL == pipeFd)
	{
		HLIB_MEM_Free(pszCmd);
		if (pszNewPasswd)		{ HLIB_MEM_Free(pszNewPasswd); }
		return ERR_FAIL;
	}
	//insert new password
	VK_TASK_Sleep(200);
	fprintf( pipeFd, "%s\n", pszNewPasswd );
	VK_TASK_Sleep(200);
	// confirm password
	fprintf( pipeFd, "%s\n", pszNewPasswd );

	pclose(pipeFd);

	HLIB_MEM_Free(pszCmd);
	if (pszNewPasswd)		{ HLIB_MEM_Free(pszNewPasswd); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}



static HERROR	seumma_rpc_Mount_Smbc(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8 *pszIp, *pszSharedName, *pszPath, *pszId, *pszPassword;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szCmd[512] = {0,};
	HUINT32	ulUid = 0, ulGid = 0;

	HAPPY(szCmd);

	if(HLIB_EXTRA_GetGroupIdFromGroupInfo("pvr", &ulGid) == ERR_OK)
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, NULL) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}
	else
	{
		if(HLIB_EXTRA_GetUserAndGroupIdFromUserInfo("umma", &ulUid, &ulGid) != ERR_OK)
		{
			HxLOG_Error("FAIL!! in HLIB_EXTRA_GetUserAndGroupIdFromUserInfo \n");
		}
	}

	HxLOG_Assert(nArgc == 5);
	HxLOG_Trace();

	pszIp = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));
	pszSharedName = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[1]));
	pszPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));
	pszId= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[3]));
	pszPassword = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[4]));

	HLIB_DIR_Make( pszPath );
	HxSTD_PrintToStrN(szCmd, sizeof(szCmd), "mount.cifs \"//%s/%s\" \"%s\" -o user=%s,pass=%s,nounix,noserverino,iocharset=utf8,ro,uid=%d,gid=%d,dir_mode=0775,file_mode=0660 > /tmp/.result_mount",
							pszIp, pszSharedName, pszPath, pszId, pszPassword, ulUid, ulGid);
	HxLOG_Print("\n************ MOUNT (+) *************\n");
	HxLOG_Debug("\n====> Mount Cmd : %s\n", szCmd);
	VK_SYSTEM_Command( szCmd );

	if (pszIp)				{ HLIB_MEM_Free(pszIp); }
	if (pszSharedName)		{ HLIB_MEM_Free(pszSharedName); }
	if (pszPath)				{ HLIB_MEM_Free(pszPath); }
	if (pszId)				{ HLIB_MEM_Free(pszId); }
	if (pszPassword)			{ HLIB_MEM_Free(pszPassword); }

	hErr = ERR_OK;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "i", hErr);

	return ERR_OK;
}

static HERROR	seumma_rpc_Umount_Smbc(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT8	*pszPath;
	int		nErr = 0;

	HxLOG_Assert(nArgc == 1);

	pszPath = HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[0]));

	HxLOG_Trace();

#if defined(CONFIG_PROD_OS_EMULATOR)
	nErr = umount2( pszPath, 0x00000001 | 0x00000002);
#else
	nErr = umount2( pszPath, MNT_FORCE | MNT_DETACH);
#endif

	if (nErr != 0)
	{
		HxLOG_Error("FAIL!! \n");
	}

	if (pszPath)				{ HLIB_MEM_Free(pszPath); }

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, (HERROR)nErr, "i", (HERROR)nErr);

	return ERR_OK;
}

static HERROR	seumma_rpc_MountFlash(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_OK;
#ifdef CONFIG_ENHANCED_SECURITY
	DI_ERR_CODE diErr = DI_ERR_ERROR;

	diErr = DI_FS_FLASH_MountPartitions();

	if (diErr != DI_ERR_OK)
	{
		hErr = ERR_FAIL;
		HxLOG_Error("[%s] FAIL!! \n", __FUNCTION__);
	}

#endif
	HxLOG_Error("RPC MountFLASH Call!!\n");
	return hErr;
}

static HERROR	seumma_rpc_UnmountFlash(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_OK;
#ifdef CONFIG_ENHANCED_SECURITY
	DI_ERR_CODE diErr = DI_ERR_ERROR;

	diErr = DI_FS_FLASH_UmountPartitions();

	if (diErr != DI_ERR_OK)
	{
		hErr = ERR_FAIL;
		HxLOG_Error("[%s] FAIL!! \n", __FUNCTION__);
	}

#endif
	HxLOG_Print("RPC UmountFLASH Call!!\n");
	return hErr;
}

// HUINT32 nonblock, SEUMMA_FormatList_t *pstFormatList
static HERROR	seumma_rpc_Format(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 nonblock;
	SEUMMA_FormatList_t *pstUMMAFormatList;
	DI_FS_FormatList_t	stDIFormatList;
	HERROR	hErr = ERR_FAIL;

	HAPPY(nonblock);
	HxLOG_Assert(nArgc == 2);

	nonblock = HxOBJECT_INT(apArgv[0]);
	pstUMMAFormatList = HxOBJECT_BIN_DATA(apArgv[1]);

	seumma_ConvertFormatListToDi(pstUMMAFormatList, &stDIFormatList);

	HxLOG_Trace();

	hErr = DI_FS_FormatLists(&stDIFormatList, TRUE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

// DI_FS_StorDevIdx_t eDevIdx
static HERROR	seumma_rpc_OpenDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();

	hErr =DI_FS_OpenDevMbrBuf(eDevIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}
	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx
static HERROR	seumma_rpc_DeleteAllPartitionInDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();

	hErr =DI_FS_DeleteAllPartitionInDevMbrBuf(eDevIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

// DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_UpdatePartitionInDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT32 ulStartUnit;
	HUINT32 ulEndUnit;
	DI_FileSystemType_t eFsType;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 5);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	ulStartUnit = HxOBJECT_INT(apArgv[2]);
	ulEndUnit = HxOBJECT_INT(apArgv[3]);
	eFsType  	= HxOBJECT_INT(apArgv[4]);
	HxLOG_Warning("\n");

	hErr =DI_FS_UpdatePartitionInDevMbrBuf(eDevIdx, ulPartIdx, ulStartUnit, ulEndUnit, eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

// DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ullPartSizeKb_left, HUINT32 ullPartSizeKb_right, DI_FileSystemType_t fsType
static HERROR	seumma_rpc_SetPartitionInDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT32 ullPartSizeKb_left;
	HUINT32 ullPartSizeKb_right;
	DI_FileSystemType_t eFsType;
	HERROR	hErr = ERR_FAIL;

	HUINT64 ullPartSizeKb, ullTemp;

	HxLOG_Assert(nArgc == 5);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	ullPartSizeKb_left = HxOBJECT_INT(apArgv[2]);
	ullPartSizeKb_right = HxOBJECT_INT(apArgv[3]);
	eFsType  	= HxOBJECT_INT(apArgv[4]);

	ullTemp = (HUINT64)ullPartSizeKb_left;


	ullPartSizeKb = (ullTemp << 32) | ullPartSizeKb_right;

	HxLOG_Warning("\n");

	hErr = DI_FS_SetPartitionInDevMbrBuf(eDevIdx, ulPartIdx, ullPartSizeKb, eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}


	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx
static HERROR	seumma_rpc_SyncDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);


	HxLOG_Warning("\n");
	hErr = DI_FS_SyncDevMbrBuf(eDevIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx
static HERROR	seumma_rpc_CloseDevMbrBuf(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);

	HxLOG_Warning("\n");

	hErr = DI_FS_CloseDevMbrBuf(eDevIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx
static HERROR	seumma_rpc_GetPartitionFromDeviceMbr(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HERROR	hErr = ERR_FAIL;
	DI_FS_PartitionList_t	stPartList;

	HxLOG_Assert(nArgc == 1);

	HxSTD_MemSet(&stPartList, 0, sizeof(DI_FS_PartitionList_t));
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	HxLOG_Warning("\n");

	hErr = DI_FS_GetPartitionFromDeviceMbr(eDevIdx, &stPartList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "ib", hErr, (void *)&stPartList, sizeof(DI_FS_PartitionList_t));

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_GetMountedPartitionStatus(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	DI_FileSystemType_t eFsType;
	HUINT64 ullTotalKbSize, ullUsedKbSize;
	HBOOL bWritable;
	HERROR	hErr = ERR_FAIL;

	SE_UMMA_RPC_Item_t	stItem;


	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	eFsType  	= HxOBJECT_INT(apArgv[2]);

	HxLOG_Print("[%s] [%d] \n",__FUNCTION__,__LINE__);


	hErr = DI_FS_GetMountedPartitionStatus(eDevIdx, ulPartIdx, eFsType, &ullTotalKbSize, &ullUsedKbSize, &bWritable);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	stItem.ulP1 = hErr;
	stItem.ulP2 = ullTotalKbSize;
	stItem.ulP3 = ullUsedKbSize;
	stItem.ulP4 = bWritable;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "b", (void *)&stItem, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}
//HCHAR *pszDevName
static HERROR	seumma_rpc_GetDevSize(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HUINT64 ullTotalKbSize;
	HCHAR *pszDevName;

	SE_UMMA_RPC_Item_t	stItem;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	pszDevName = HxOBJECT_STR(apArgv[0]);

	HxLOG_Warning("\n");

	hErr = HLIB_STORAGE_GetDevSize(pszDevName, &ullTotalKbSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	stItem.ulP1 = hErr;
	stItem.ulP2 = ullTotalKbSize;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "b", (void *)&stItem, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

//HCHAR *pszDevName
static HERROR	seumma_rpc_UsbIsDeviceRemovable(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_OK;
	DI_FS_StorDevIdx_t eDevIdx;
	HBOOL	bRemovable;
	HINT32			 nRmvFileDsr, nRet;
	HINT32			 nRemovable;
	HCHAR		 szNumString[8], szRmvFileName[32];

	SE_UMMA_RPC_Item_t	stItem;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);

	HxLOG_Warning("\n");

	nRemovable = 0;

	MACRO_GetDevRmvFileName (szRmvFileName, eDevIdx);
	nRmvFileDsr = open (szRmvFileName, O_RDONLY);
	if (nRmvFileDsr >= 0)
	{
		nRet = read (nRmvFileDsr, szNumString, sizeof(char));
		if (nRet > 0)
		{
			szNumString[1] = '\0';
			nRemovable = atoi (szNumString);
		}

		close (nRmvFileDsr);
	}

	bRemovable = (nRemovable != 0) ? TRUE : FALSE;

	stItem.ulP1 = ERR_OK;
	stItem.ulP2 = bRemovable;

	HxLOG_Debug("bRemovable : %d\n",bRemovable);

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "b", (void *)&stItem, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

//HCHAR *pszPathName
static HERROR	seumma_rpc_GetPathSize(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HUINT64 ullKbSize;
	HCHAR *pszPathName;

	SE_UMMA_RPC_Item_t	stItem;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	pszPathName = HxOBJECT_STR(apArgv[0]);

//	HxLOG_Warning("\n");

	hErr = HLIB_STORAGE_GetPathSize(pszPathName, &ullKbSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	stItem.ulP1 = hErr;
	stItem.ulP2 = ullKbSize;

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "b", (void *)&stItem, sizeof(SE_UMMA_RPC_Item_t));

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_ExecuteCheckDisk(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	DI_FileSystemType_t eFsType;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	eFsType  	= HxOBJECT_INT(apArgv[2]);
	HxLOG_Warning("\n");

	hErr = DI_FS_ExecuteCheckDisk(eDevIdx, ulPartIdx, eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszPassword
static HERROR	seumma_rpc_EncrFormat(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HCHAR *pszPassword = NULL;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	pszPassword  	= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));

	hErr = DI_FS_ENCR_Format((DI_FS_StorDevIdx_t)eDevIdx, ulPartIdx, pszPassword);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	if (pszPassword)	{HLIB_MEM_Free(pszPassword);}
#endif

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszMapperDeviceName, char *pszPassword
static HERROR	seumma_rpc_EncrOpen(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HCHAR *pszMapperDeviceName;
	HCHAR *pszPassword = NULL;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 4);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	pszMapperDeviceName  	= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));
	pszPassword  	= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[3]));

	hErr = DI_FS_ENCR_Open(eDevIdx, ulPartIdx, pszMapperDeviceName, pszPassword);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	if (pszMapperDeviceName)	{HLIB_MEM_Free(pszMapperDeviceName);}
	if (pszPassword)	{HLIB_MEM_Free(pszPassword);}
#endif

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx
static HERROR	seumma_rpc_EncrClose(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);

	hErr = DI_FS_ENCR_Close(eDevIdx, ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}
#endif

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszMountDir, DI_FileSystemType_t eFsType
static HERROR	seumma_rpc_EncrMount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HCHAR *pszMountDir;
	DI_FileSystemType_t eFsType;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 4);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	pszMountDir  	= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));
	eFsType = HxOBJECT_INT(apArgv[3]);

	hErr = DI_FS_ENCR_Mount(eDevIdx, ulPartIdx, pszMountDir, eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	if (pszMountDir)	{HLIB_MEM_Free(pszMountDir);}
#endif

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx
static HERROR	seumma_rpc_EncrUmount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);

	hErr = DI_FS_ENCR_Umount(eDevIdx, ulPartIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}
#endif

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulBufsize
static HERROR	seumma_rpc_GetVolumeUuid(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT32 ulBufsize;
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulUuidsize;
	HCHAR	szUUID[64] = {0,};

	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	ulBufsize = HxOBJECT_INT(apArgv[2]);

	hErr = DI_FS_GetVolumeUuid(eDevIdx, ulPartIdx, szUUID, ulBufsize, &ulUuidsize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "iis", hErr, ulUuidsize, szUUID);

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pszName
static HERROR	seumma_rpc_SetVolumeName(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT8 *pszName;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	pszName  	= HLIB_MEM_StrDup(HxOBJECT_STR(apArgv[2]));

	hErr = DI_FS_SetVolumeName(eDevIdx, ulPartIdx, pszName);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	if (pszName)	{HLIB_MEM_Free(pszName);}

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulBufsize
static HERROR	seumma_rpc_GetVolumeName(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HUINT32 ulBufsize;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szName[64] = {0,};

	HxLOG_Assert(nArgc == 3);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);
	ulBufsize = HxOBJECT_INT(apArgv[2]);

	hErr = DI_FS_GetVolumeName(eDevIdx, ulPartIdx, szName, ulBufsize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "is", hErr, szName);

	return ERR_OK;
}

//DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx
static HERROR	seumma_rpc_GetFsType(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	HERROR	hErr = ERR_FAIL;
	DI_FileSystemType_t eFsType;

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	eDevIdx = HxOBJECT_INT(apArgv[0]);
	ulPartIdx = HxOBJECT_INT(apArgv[1]);

	hErr = DI_FS_GetFsType(eDevIdx, ulPartIdx, &eFsType);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "ii", hErr, eFsType);

	return ERR_OK;
}

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)

static HERROR	seumma_rpc_StartDst(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 0);
	HxLOG_Trace();

	DI_HDD_DstStart();

	return ERR_OK;
}

static HERROR	seumma_rpc_HddInit(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HxLOG_Assert(nArgc == 0);
	HxLOG_Trace();


	HxLOG_Warning("================= HDD Inti =================  \n");

	hErr = DI_HDD_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	DI_HDD_RegisterCallback(seumma_HddEventCallback);

	return ERR_OK;
}

//DI_HDD_PORT_NUM_E portnum
static HERROR	seumma_rpc_HddGetEncryptionKey(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
	DI_HDD_PORT_NUM_E ePortnum;
	HERROR	hErr = ERR_FAIL;
	HCHAR	szKey[36] ={0,};

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();
	ePortnum = HxOBJECT_INT(apArgv[0]);


	hErr = DI_HDD_GetEncryptionKey(ePortnum, szKey);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	HLIB_RPC_Response(s_nRPCHandle, nClientHandle, nRequestId, hErr, "is", hErr, szKey);
#endif

	return ERR_OK;
}


//DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on
static HERROR	seumma_rpc_HddControlPower(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_HDD_PORT_NUM_E ePortnum;
	DI_HDD_PWR eOnOff;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 2);
	HxLOG_Trace();

	ePortnum = HxOBJECT_INT(apArgv[0]);
	eOnOff = HxOBJECT_INT(apArgv[1]);

	hErr = DI_HDD_ControlPower(ePortnum, eOnOff);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

//DI_HDD_PWR OnOff
static HERROR	seumma_rpc_HddPwCtrl(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_HDD_PWR OnOff;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	OnOff = HxOBJECT_INT(apArgv[0]);

	hErr = DI_HDD_PWR_Ctrl(OnOff);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
static HERROR	seumma_rpc_UsbInit(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HxLOG_Assert(nArgc == 0);
	HxLOG_Trace();

	HxLOG_Warning("================= USB Inti =================  \n");

	hErr = DI_USB_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	DI_USB_RegisterCallback(seumma_UsbEventCallback);

	return ERR_OK;
}

//DI_USB_PWR_t OnOff
static HERROR	seumma_rpc_UsbPwCtrl(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DI_HDD_PWR eOnOff;
	HERROR	hErr = ERR_FAIL;

	HxLOG_Assert(nArgc == 1);
	HxLOG_Trace();

	eOnOff = HxOBJECT_INT(apArgv[0]);

	hErr = DI_USB_PWR_Ctrl(eOnOff);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	return ERR_OK;
}

#endif

static HERROR	seumma_rpc_HotplugInit(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HxLOG_Assert(nArgc == 0);
	HxLOG_Trace();

	HxLOG_Warning("================= Hotplug Inti =================  \n");

	hErr = UMMALIB_Hotplug_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL!! \n");
	}

	UMMALIB_Hotplug_RegisterCallback(seumma_HotplugEventCallback);

	return ERR_OK;
}



static void		seumma_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	HLIB_RPC_ProcessRPC(s_nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		seumma_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	if (eStatus == HxSOCKET_REMOVE_CLIENT)
	{
		HLIB_RPC_RemoveClientHandle(s_nRPCHandle, nClientHandle);
	}
}

HINT32	main(HINT32 argc, char ** argv)
{
	VK_Init();

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif
	HxLOG_Trace();

	s_nServerHandle = HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)SEUMMA_SIGNAL_UDS);
	s_nRPCHandle    = HLIB_RPC_Open(s_nServerHandle, HLIB_STD_GetPID());

	// Init rpc func.
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_Init, NULL, seumma_rpc_Init, NULL, "Init");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_Mount, "iisi", seumma_rpc_Mount, NULL, "Mount (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_Umount, "iii", seumma_rpc_Umount, NULL, "Umount (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_MountFtps, "ss", seumma_rpc_Mount_Ftps, NULL, "Mount_Ftps (HUINT8 *pszStorPath, HUINT8 *pszBindPath)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_MountSmbs, "ss", seumma_rpc_Mount_Smbs, NULL, "Mount_Smbs (HUINT8 *pszStorPath, HUINT8 *pszBindPath)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_DoCommand, "ss", seumma_rpc_DoCommand, NULL, "Mount_Ftps (HUINT8 *pszCmd, HUINT8 *pszBindPath)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_StartFtps, "ss", seumma_rpc_Start_Ftps, NULL, "Start_Ftps (HUINT8 *pszCmdFtpd, HUINT8 *pszConfigFile)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_StartSmbs, "ss", seumma_rpc_Start_Smbs, NULL, "Start_Smbs (HUINT8 *pszCmd, HUINT8 *pszConfigFile)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_SetPasswordSmbs, "ss", seumma_rpc_SetPassword_Smbs, NULL, "SetPassword_Smbs (HUINT8 *pszCmd, HUINT8 *pszNewPasswd)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_PopenCmdSmbc, "ss", seumma_rpc_PopenCmd_Smbc, NULL, "PopenCmd_Smbc (HUINT8 *pszCmd, HUINT8 *pszResultFile)");



	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_MountSmbc, "sssss", seumma_rpc_Mount_Smbc, NULL, "Mount_Smbc (HUINT8 *szIp, HUINT8 *szSharedName, HUINT8 *szPath, HUINT8 *szId, HUINT8 *szPassword)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_UmountSmbc, "s", seumma_rpc_Umount_Smbc, NULL, "Umount_Smbc (HUINT8 *pszPath)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_MountFlash, NULL, seumma_rpc_MountFlash, NULL, "MountNand");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_UnmountFlash, NULL, seumma_rpc_UnmountFlash, NULL, "UnmountNand");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_Format, "ib", seumma_rpc_Format, NULL, "Format(HUINT32 nonblock, DI_FS_FormatList_t *pstFormatList)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_OpenDevMbrBuf, "i", seumma_rpc_OpenDevMbrBuf, NULL, "OpenDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_DeleteAllPartitionInDevMbrBuf, "i", seumma_rpc_DeleteAllPartitionInDevMbrBuf, NULL, "DeleteAllPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_UpdatePartitionInDevMbrBuf, "iiiii", seumma_rpc_UpdatePartitionInDevMbrBuf, NULL, "UpdatePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_SetPartitionInDevMbrBuf,  "iiiii", seumma_rpc_SetPartitionInDevMbrBuf, NULL, "SetPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ullPartSizeKb_left, HUINT32 ullPartSizeKb_right, DI_FileSystemType_t fsType)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_SyncDevMbrBuf, "i", seumma_rpc_SyncDevMbrBuf, NULL, "SyncDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_CloseDevMbrBuf, "i", seumma_rpc_CloseDevMbrBuf, NULL, "CloseDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetPartitionFromDeviceMbr, "i", seumma_rpc_GetPartitionFromDeviceMbr, NULL, "GetPartitionFromDeviceMbr (DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetMountedPartitionStatus, "iii", seumma_rpc_GetMountedPartitionStatus, NULL, "GetMountedPartitionStatus(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetDevSize, "s", seumma_rpc_GetDevSize, NULL, "GetDevSize(HCHAR *pszDevName)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetPathSize, "s", seumma_rpc_GetPathSize, NULL, "GetDevSize(HCHAR *pszPathName)");

	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_ExecuteCheckDisk, "iii", seumma_rpc_ExecuteCheckDisk, NULL, "ExecuteCheckDisk(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetVolumeUuid, "iii", seumma_rpc_GetVolumeUuid, NULL, "GetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulBufsize)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_SetVolumeName, "iis", seumma_rpc_SetVolumeName, NULL, "SetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetVolumeName, "iii", seumma_rpc_GetVolumeName, NULL, "GetVolumeName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulBufsize)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_GetFsType, "ii", seumma_rpc_GetFsType, NULL, "GetFsType(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)");

	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_EncrFormat, "iis", seumma_rpc_EncrFormat, NULL, "EncrFormat(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszPassword)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_EncrOpen, "iiss", seumma_rpc_EncrOpen, NULL, "EncrOpen(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszMapperDeviceName, char *pszPassword)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_EncrClose, "ii", seumma_rpc_EncrClose, NULL, "EncrClose(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_EncrMount, "iisi", seumma_rpc_EncrMount, NULL, "EncrMount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, char *pszMountDir, DI_FileSystemType_t eFsType)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_EncrUmount, "ii", seumma_rpc_EncrUmount, NULL, "EncrUmount(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)");

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_StartDst, NULL, seumma_rpc_StartDst, NULL, "StartDst");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_HddInit, NULL, seumma_rpc_HddInit, NULL, "HddInit");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_HddPwCtrl, "i", seumma_rpc_HddPwCtrl, NULL, "HddPwCtrl(DI_HDD_PWR_t OnOff)");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_HddControlPower, "ii", seumma_rpc_HddControlPower, NULL, "UsbPwCtrl(DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_HddGetEncryptionKey, "i", seumma_rpc_HddGetEncryptionKey, NULL, "HddGetEncryptionKey(DI_HDD_PORT_NUM_E portnum)");
	HLIB_RPC_CreateNotification(s_nRPCHandle, RPC_SEUMMA_onHddNotifier, "ib", "HDD noti");
	HLIB_RPC_CreateNotification(s_nRPCHandle, RPC_SEUMMA_onDstNotifier, "ii", "DST noti");
#endif

#if defined(CONFIG_PROD_DEVICE_STORAGE_USB)
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_UsbInit, NULL, seumma_rpc_UsbInit, NULL, "UsbInit");
	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_UsbPwCtrl, "i", seumma_rpc_UsbPwCtrl, NULL, "UsbPwCtrl(DI_USB_PWR_t OnOff)");
	HLIB_RPC_RegisterRequest(s_nRPCHandle, RPC_SEUAPI_UsbIsDeviceRemovable, "i", seumma_rpc_UsbIsDeviceRemovable, NULL, "UsbIsDeviceRemovable(DI_FS_StorDevIdx_t eDevIdx)");
	HLIB_RPC_CreateNotification(s_nRPCHandle, RPC_SEUMMA_onUsbNotifier, "ib", "USB noti");
#endif

	HLIB_RPC_CreateNotification(s_nRPCHandle, RPC_SEUMMA_onFormatProgress, "iiii", "format progress noti");

	HLIB_RPC_RegisterCall(s_nRPCHandle, RPC_SEUAPI_HotplugInit, NULL, seumma_rpc_HotplugInit, NULL, "Hotplug Init");
	HLIB_RPC_CreateNotification(s_nRPCHandle, RPC_SEUMMA_onHotplugNotifier, "iib", "Hotplug noti");

	// Init notigier
	DI_FS_RegisterFormatProgressCallback(seumma_FormatCallback);
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	DI_HDD_RegisterDstCallback(seumma_DstCallback);
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,

	while (1)
		HLIB_SOCKET_Listen(s_nServerHandle, TRUE, seumma_onPacketReceived, seumma_onStatusChanged, NULL);

	return 0;
}


