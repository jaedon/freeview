/**
	@file     xsvc_sys_freesat.c
	@brief    xsvc_sys_freesat.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_sys.h>

#include "../local_include/_xsvc_sys.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xsvc_sys_GetDeviceId_FreeSat(HUINT8 *pszDeviceId, HUINT32 ulBufSize)
{
	HERROR	hErr = ERR_FAIL;
	HUINT8	szDeviceId[256];

	if (ulBufSize == 0 || ulBufSize >= 256)
		return ERR_FAIL;

#if 1
{
	HxFILE_t	hFile;
	HUINT32		ulReadSize;

	hFile = HLIB_FILE_Open("/tmp/FKP/F-USN.txt", "r");
	if (NULL == hFile)
		return ERR_FAIL;

	HxSTD_MemSet(szDeviceId, 0x00, sizeof(szDeviceId));

	ulReadSize = HLIB_FILE_Read(hFile, szDeviceId, sizeof(szDeviceId), 1);

	(void)HLIB_FILE_Close(hFile);

	hErr = ERR_FAIL;
	if (ulReadSize < 0)
	{
		if (ulReadSize <= ulBufSize)
		{
			HxSTD_MemSet(pszDeviceId, 0x00, ulReadSize);
			HxSTD_MemCopy(pszDeviceId, szDeviceId, ulReadSize-1);
			hErr = ERR_OK;
		}
	}

	return hErr;
}
#else
{
	PAL_FS_UINT32	ulFileDes = 0, ulRead = 0;

	HxSTD_MemSet(szDeviceId, 0x00, sizeof(szDeviceId));

	hErr = PAL_FS_Open(0, "/tmp/FKP/F-USN.txt", PAL_FS_MODE_READ, &ulFileDes);
	if(hErr != ERR_OK)
		return ERR_FAIL;

	hErr = PAL_FS_Read(ulFileDes, szDeviceId, sizeof(szDeviceId), &ulRead);
	if(hErr != ERR_OK)
		return ERR_FAIL;

	PAL_FS_Close(ulFileDes);

	HxSTD_MemSet(pszDeviceId, 0x00, ulBufSize);
	HxSTD_MemCopy(pszDeviceId, szDeviceId, ulBufSize-1);
}
	return hErr;
#endif
}


