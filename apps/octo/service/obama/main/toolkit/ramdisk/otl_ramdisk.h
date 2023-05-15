/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  otl_ramdisk.h
	@brief	  MW의 common utility 파일 헤더

	Description: MW와 AP가 모두 사용하는 DRM관련  API의 헤더 파일이다.
	Module: MWC/UTIL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__OTL_RAMDISK_H__
#define	__OTL_RAMDISK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
typedef enum
{
	eFileOrigin_Set,
	eFileOrigin_Cursor,
	eFileOrigin_End
} OtlRamDisk_RamfileAccessOrigin_e;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR OTL_RAMDISK_Init(void);
HERROR OTL_RAMDISK_OpenFile(HUINT8 *szFileName, HUINT8 *szMode, Handle_t *phRFileHandle);
HERROR OTL_RAMDISK_CloseFile(Handle_t hRFileHandle);
HERROR OTL_RAMDISK_GetFileSize (Handle_t hRFileHandle, HUINT32 *pulFileSize);
HERROR OTL_RAMDISK_SeekFile (Handle_t hRFileHandle, HINT32 nOffset, OtlRamDisk_RamfileAccessOrigin_e eOrigin);
HERROR OTL_RAMDISK_TellFile (Handle_t hRFileHandle, HINT32 *pnOffset);
HERROR OTL_RAMDISK_ReadFile (Handle_t hRFileHandle, HUINT8 *pucBuffer, HUINT32 ulSize, HUINT32 *pulRealReadSize);
HERROR OTL_RAMDISK_WriteFile(Handle_t hRFileHandle, HUINT8 *pucBuffer, HUINT32 ulSize);
HERROR OTL_RAMDISK_DeleteFile (HUINT8 *szFileName);
HERROR OTL_RAMDISK_DeleteAllFiles(void);
HERROR OTL_RAMDISK_CountAllFiles(HUINT32 *pulFileNum);
HERROR OTL_RAMDISK_GetFileName (HUINT32 ulIndex, HUINT8 *szFileName);
HERROR OTL_RAMDISK_GetFileNameList (HUINT32 *pulFileNum, HUINT8 ***ppszFileNameArray);
HERROR OTL_RAMDISK_FreeFileNameList (HUINT32 ulFileNum, HUINT8 **pszFileNameArray);

#endif /* !__OTL_RAMDISK_H__ */

