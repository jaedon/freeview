/**
	@file     xmgr_sw_update_usb_astra.c
	@brief    USB OTA application \n

	Description: sw update USB \n
	Module: Application \n
	Remarks: \n
	 Date			Modification						Name \n
	-----------	-------------------------------------	------------------ \n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/


/******************************************************************************/
/******************************* Header Files *********************************/
/******************************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>


#include <mgr_storage.h>
#include <mgr_swup.h>
#include "_xmgr_swup_usb.h"



/******************************************************************************/
/********************************** Defines ***********************************/
/******************************************************************************/
#define		SW_UPDATE_IMAGE_NAME	"upgrade.dsc"

/******************************************************************************/
/********************************** Typedef ***********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Static Variables ******************************/
/******************************************************************************/
STATIC 	HERROR		xmgr_swup_UsbFindSwImage_Humax (Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents);
STATIC	HBOOL 		xmgr_swup_UsbLoadFileCb(HxDIR_t *pstSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *pvUserData);

/******************************************************************************/
/************************ Static Functions Declaration ************************/
/******************************************************************************/
STATIC HCHAR *xmgr_swup_UsbFindNextPointer_Humax(HCHAR *pszBuf, HCHAR ch)
{
	HCHAR *pszTmp = pszBuf;
	while(*pszTmp)
	{
		if(*pszTmp++ == ch)
		{
			return pszTmp;
			continue;
		}
	}
	return NULL;
}

STATIC HERROR xmgr_swup_UsbParseSdlPath_Humax(HCHAR *pszSrc, HCHAR *pszDes)
{
	HCHAR *pszTmp = xmgr_swup_UsbFindNextPointer_Humax(pszSrc, '"');

	if (pszTmp == NULL)
		return ERR_FAIL;

	while(*pszTmp != '"')
	{
		*pszDes++ = *pszTmp++;
	}
	if (*pszTmp == '"')
	{
		*pszDes = '\0';
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC 	HERROR	xmgr_swup_UsbgetimagepathByFile(HCHAR *pszDstImagePath, const HCHAR *pszFilePath)
{
	HxFILE_t	file;
	HERROR		hErr = ERR_FAIL;

	if (NULL == pszFilePath || pszDstImagePath == NULL)
		return ERR_FAIL;

	file = HLIB_FILE_Open((const HUINT8 *)pszFilePath, (const HUINT8 *)"rb");
	if (file)
	{
		HUINT32 	ulSize;

		if (HLIB_FILE_Size32(file, &ulSize) == ERR_OK)
		{
			HCHAR	*buf;

			buf = HLIB_STD_MemCalloc(ulSize + 1);
			if (buf)
			{
				HLIB_FILE_Read(file, (void *)buf, 1, ulSize);

				HxLOG_Debug("[%s.%d] Read sdl file : %s\n",__FUNCTION__,__LINE__,buf);

				if (HLIB_STD_StrStartWith(buf, "target_upgrade_file") == TRUE)
				{
					HCHAR *pszFindImage = NULL;
					HxLOG_Debug("This File is Correct irdeto s/w update info file!!\n");

					pszFindImage = xmgr_swup_UsbFindNextPointer_Humax(buf, '=');

					if ( pszFindImage != NULL)
					{
						HCHAR tmpBuf[MAX_PATH_LENGTH] ={'\0',};

						if (xmgr_swup_UsbParseSdlPath_Humax (pszFindImage,tmpBuf) == ERR_OK)
						{
							HxLOG_Debug("Find ImagePath(tmpBuf) : %s , len : %d\n", tmpBuf, sizeof(tmpBuf));
							HLIB_STD_StrUtf8NCpy(pszDstImagePath, tmpBuf, MAX_PATH_LENGTH);
							hErr = ERR_OK;
						}
						else
						{
							HxLOG_Debug("NOT Find ImagePath\n");
						}
					}
				}
				HLIB_STD_MemFree(buf);
			}
		}

		HLIB_FILE_Close(file);
	}
	return hErr;
}

STATIC	HBOOL xmgr_swup_UsbLoadFileCb (HxDIR_t *pstSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *pvUserData)
{
	HCHAR			 szFileName[MAX_PATH_LENGTH], szSwImageName[MAX_PATH_LENGTH];
	SvcSwUpdate_UsbDetectInfo_t *pstContents = (SvcSwUpdate_UsbDetectInfo_t *)pvUserData;

	HxSTD_PrintToStrN (szFileName, MAX_PATH_LENGTH, "%s/%s", pszPath, pszName);
	HxSTD_PrintToStrN (szSwImageName, MAX_PATH_LENGTH, "%s/%s", pszPath, SW_UPDATE_IMAGE_NAME);

	HxLOG_Debug("Detected *.dsc File. szFileName : %s, szSwImageName : %s\n",szFileName, szSwImageName);

	if ( HLIB_STD_StrCaseCmp(szFileName, szSwImageName) == 0 )
	{
		HCHAR 	szImagePath[MAX_PATH_LENGTH]	= {'\0',};
		HCHAR	szFullPath[MAX_PATH_LENGTH] 	= {'\0', };

		if (xmgr_swup_UsbgetimagepathByFile(szImagePath, szSwImageName) == ERR_OK)
		{
			pstContents->bFoundImagePath	= TRUE;
			HxSTD_PrintToStrN (szFullPath, MAX_PATH_LENGTH, "%s/%s",pszPath, szImagePath);
			HLIB_STD_StrUtf8NCpy((HCHAR*)pstContents->szSoftwarePath, szFullPath, MAX_PATH_LENGTH);
			return TRUE;
		}
	}
	return FALSE;
}

STATIC 	HERROR	xmgr_swup_UsbFindSwImage_Humax(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	HINT32					nFileCnt	= 0;

	if (pstContents == NULL)	return ERR_FAIL;

	nFileCnt = HLIB_DIR_Find((HUINT8 *)szPath, (HUINT8 *)"*.dsc", (HUINT8 *)"", xmgr_swup_UsbLoadFileCb, pstContents);

	if (pstContents->bFoundImagePath == TRUE)	// if found matched hdf image file.
		return ERR_OK;

	return ERR_FAIL;
}

HERROR	xmgr_swup_UsbFindImage_Humax(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	return xmgr_swup_UsbFindSwImage_Humax(hSWUpdate, szPath, pstContents);
}


