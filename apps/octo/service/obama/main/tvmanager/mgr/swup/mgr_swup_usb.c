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

/******************************************************************************/
/********************************** Defines ***********************************/
/******************************************************************************/

/******************************************************************************/
/********************************** Typedef ***********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/
STATIC SvcSwUpdate_UsbDetectInfo_t	s_stUsbOta_Contents;


/******************************************************************************/
/****************************** Static Variables ******************************/
/******************************************************************************/
STATIC	SvcSwUpdate_UsbDetectInfo_t * mgr_swup_usb_contents (void);
STATIC 	HERROR		mgr_swup_usb_find_swimage (Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents);
STATIC	HERROR 		mgr_swup_usb_detect_start (SvcSwUpdate_UsbDetectInfo_t *pstContents, Handle_t hAction, Handle_t hSWUpdate);
STATIC	HERROR		mgr_swup_usb_InitContents (SvcSwUpdate_UsbDetectInfo_t *pstContents, Handle_t hSwupdate);
STATIC	HBOOL 		mgr_swup_usb_cbLoadFile	(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata);




/******************************************************************************/
/************************ Static Functions Declaration ************************/
/******************************************************************************/
STATIC HCHAR *mgr_swup_usb_findNextPointer(HCHAR *buf, HCHAR ch)
{
	HCHAR *tmp = buf;
	while(*tmp)
	{
		if(*tmp++ == ch)
		{
			return tmp;
		}
	}
	return NULL;
}

STATIC HERROR mgr_swup_usb_ParseSdlPath(HCHAR *src, HCHAR *des)
{
	HCHAR *tmp =mgr_swup_usb_findNextPointer(src, '"');

	if (tmp == NULL)
		return ERR_FAIL;

	while(*tmp != '"')
	{
		*des++ = *tmp++;
	}
	if (*tmp == '"')
	{
		*des = '\0';
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC	SvcSwUpdate_UsbDetectInfo_t * mgr_swup_usb_contents(void)
{
	return &s_stUsbOta_Contents;
}

STATIC	HERROR	mgr_swup_usb_InitContents(SvcSwUpdate_UsbDetectInfo_t *pstContents, Handle_t hSwupdate)
{
	if (pstContents == NULL)
	{
		MGR_SWUP_PostMessage(hSwupdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0, 0, 0);
		return ERR_FAIL;
	}

	pstContents->hSwUpdate			= hSwupdate;
	pstContents->bFoundImagePath	= FALSE;
	HxSTD_MemSet(pstContents->szSoftwarePath, '\0', MAX_PATH_LENGTH);
	return ERR_OK;
}

STATIC 	HERROR	mgr_swup_usb_getimagepathByFile(HCHAR *dstImagePath, const HCHAR *pszFilePath)
{
	HxFILE_t	file;
	HERROR		hErr = ERR_FAIL;

	if (NULL == pszFilePath || dstImagePath == NULL)
		return ERR_FAIL;

	file = HLIB_FILE_Open(pszFilePath, "rb");
	if (file)
	{
		HUINT32 	ulSize;

		if (HLIB_FILE_Size32(file, &ulSize) == ERR_OK)
		{
			HCHAR	*buf;
			HUINT32	readResult;

			buf = HLIB_STD_MemCalloc(ulSize + 1);
			if (buf)
			{
				readResult = HLIB_FILE_Read(file, buf, 1, ulSize);
				if (readResult != ulSize)
				{
					HxLOG_Error("reading error\n");
					readResult = ulSize;
				}
				buf[readResult] = '\0';

				HxLOG_Debug("[%s.%d] Read sdl file : %s\n",__FUNCTION__,__LINE__,buf);

				if (HLIB_STD_StrStartWith(buf, "target_upgrade_file") == TRUE)
				{
					HCHAR *pszFindImage = NULL;
					HxLOG_Debug("This File is Correct irdeto s/w update info file!!\n");

					pszFindImage = mgr_swup_usb_findNextPointer(buf, '=');

					if ( pszFindImage != NULL)
					{
						HCHAR tmpBuf[MAX_PATH_LENGTH] ={'\0',};

						if (mgr_swup_usb_ParseSdlPath (pszFindImage,tmpBuf) == ERR_OK)
						{
							HxLOG_Debug("Find ImagePath(tmpBuf) : %s , len : %d\n", tmpBuf, sizeof(tmpBuf));
							HLIB_STD_StrUtf8NCpy(dstImagePath, tmpBuf, MAX_PATH_LENGTH);
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

STATIC	HERROR 	mgr_swup_usb_detect_start(SvcSwUpdate_UsbDetectInfo_t *pstContents, Handle_t hAction, Handle_t hSWUpdate)
{
	SvcFs_DevInfo_t	stStorageInfo;
	HINT32					nCount		=	0;
	HERROR			hErr;
	HBOOL			bUsbConnection	= FALSE;

	if (mgr_swup_usb_InitContents(pstContents, hSWUpdate) != ERR_OK)
		return ERR_FAIL;

	bUsbConnection = SVC_FS_IsUsbConnected();
	if (bUsbConnection == FALSE)
	{
		MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0, 0, 0);
		return ERR_BUS_NO_ERROR;
	}

	for (nCount = 0 ; nCount < eSVC_FS_DEV_MAX ; nCount++)
	{
		hErr = SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)nCount, &stStorageInfo);
		if (ERR_OK != hErr)
		{
			continue;
		}

		if ((stStorageInfo.bMounted == TRUE) && (stStorageInfo.eDevType == eSVC_FS_DevType_UsbMem))
		{
			if ( MGR_SWUP_FindUsbSwImage( hSWUpdate, stStorageInfo.szMountPath, pstContents ) == ERR_OK)
				break;	// if found matched hdf image file then break for loop
		}
	}

	if (pstContents->bFoundImagePath == TRUE)
	{
		SvcSwUpdate_DetectResult_t			*pstDetectResult = NULL;

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			return ERR_FAIL;
		}

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound     = TRUE;
		pstDetectResult->ulDataVersion = -1;		// usb ota 시 version 이 -1 이면, loader 에서 usb 로 인식.
		pstDetectResult->eSource		 = eDxSWUPDATE_SOURCE_Usb;

		HxSTD_MemSet(pstDetectResult->data.stUsbInfo.szSoftwarePath, '\0', MAX_PATH_LENGTH);
		HLIB_STD_StrUtf8NCpy(pstDetectResult->data.stUsbInfo.szSoftwarePath, pstContents->szSoftwarePath, MAX_PATH_LENGTH);
		HxLOG_Print("stUsbInfo.szSoftwarePath = %s\n", pstDetectResult->data.stUsbInfo.szSoftwarePath);

		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0, 0, 0);
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
		}
		else
		{
			MGR_SWUP_SetDetectInfo(hSWUpdate, pstDetectResult);
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, 0,0,0);
		}

		if(pstDetectResult != NULL)
		{
			HLIB_STD_MemFree(pstDetectResult);
		}
	}
	else
	{
		MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0, 0, 0);
	}

	return ERR_BUS_NO_ERROR;
}

/******************************************************************************/
/****************************** Global Functions ******************************/
/******************************************************************************/
BUS_Result_t proc_swup_usb_Main(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HBOOL			 s_bFirstStart = TRUE;
	SvcSwUpdate_UsbDetectInfo_t	*pstContents;
	HERROR					 hErr	= ERR_FAIL;

	pstContents = mgr_swup_usb_contents();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_MemSet(pstContents, 0, sizeof(SvcSwUpdate_UsbDetectInfo_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[eMEVT_BUS_CREATE]\n\n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "proc_swup_usb_Main");
		// p1 is software update handle
		hErr = mgr_swup_usb_detect_start (pstContents, hAction, p1);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[eMEVT_BUS_DESTROY]\n\n");
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[eMEVT_BUS_GO_OPERATION]\n\n");
		break;

	default:
		break;
	}

	if (hErr == MESSAGE_BREAK)
	{
		return MESSAGE_BREAK;
	}

	HxLOG_Print("proc_swup_usb_Main() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR			MGR_SWUP_CreateUsb(HUINT32 ulTag)
{
	BUS_Mgr_t			mgr;

	mgr = BUS_MGR_Get(proc_swup_usb_Main);
	if (mgr)
	{
		BUS_MGR_Destroy(proc_swup_usb_Main);
	}
	BUS_MGR_Create("SWUP_Usb_Proc", APP_DEFAULT_PRIORITY, proc_swup_usb_Main, 0, (HINT32)ulTag /*hSwHandle*/, 0, 0);

	return ERR_OK;
}

HERROR			MGR_SWUP_DestroyUsb(void)
{
	BUS_Mgr_t			mgr;

	mgr = BUS_MGR_Get(proc_swup_usb_Main);
	if (mgr)
	{
		BUS_MGR_Destroy(proc_swup_usb_Main);
	}

	return ERR_OK;
}

