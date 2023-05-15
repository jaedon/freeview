/**
	@file     	xmgr_swup_usb.c
	@brief    	UI for the cold boot processes of Nothing
	It does nothing, but destroys itself and finish.

	Description:  			\n
	Module: 		app/ui/surfer			 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_param.h>
#include <svc_pipe.h>
#include <svc_si.h>

#include <bus.h>
#include <mgr_action.h>
#include <mgr_common.h>
#include <mgr_swup.h>

#include <mgr_appmgr.h>

#include "local_include/_xmgr_swup_usb.h"

#if defined(CONFIG_PROD_HMS1000S)
	#define		SW_UPDATE_IMAGE_NAME	"hms1000s_upgrade.hdf"
#elif defined(CONFIG_PROD_HMSLITE)
	#define		SW_UPDATE_IMAGE_NAME	"hmslite_upgrade.hdf"
#elif defined(CONFIG_PROD_ICORDPRO)
	#define		SW_UPDATE_IMAGE_NAME	"icordpro_upgrade.hdf"
#elif defined(CONFIG_PROD_HDNEO)
	#define		SW_UPDATE_IMAGE_NAME	"hdneo_upgrade.hdf"
#elif defined(CONFIG_PROD_HGS1000S)
	#define		SW_UPDATE_IMAGE_NAME	"hgs1000s_upgrade.hdf"
#elif defined(CONFIG_PROD_HD9000I)
	#define		SW_UPDATE_IMAGE_NAME	"hd9000i_upgrade.hdf"
#elif defined(CONFIG_PROD_HMS1000T)
	#define		SW_UPDATE_IMAGE_NAME	"hms1000t_upgrade.hdf"
#elif defined(CONFIG_PROD_DIGIMAXT2)
	#define		SW_UPDATE_IMAGE_NAME	"digimaxt2_upgrade.hdf"
#elif defined(CONFIG_PROD_YSR2000)
	#define		SW_UPDATE_IMAGE_NAME	"ysr2000_upgrade.hdf"
#elif defined(CONFIG_PROD_YS1000)
	#define		SW_UPDATE_IMAGE_NAME	"ys1000_upgrade.hdf"
#elif defined(CONFIG_PROD_FVP4000T)
	#define		SW_UPDATE_IMAGE_NAME	"fvp4000t_upgrade.hdf"
	#define		SW_UPDATE_IMAGE_NAME2	"fvp5000t_upgrade.hdf"
#elif defined(CONFIG_PROD_HA7100S)
	#define		SW_UPDATE_IMAGE_NAME	"ha7100s_upgrade.hdf"
#else
	#define		SW_UPDATE_IMAGE_NAME	"hms1000s_upgrade.hdf"
#endif

#if defined (CONFIG_PROD_FVP4000T) // Modified to detect filenames both fvp4000t and fvp5000t.
STATIC	HBOOL xmgr_swup_UsbLoadFileCb (HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata)
{
	HCHAR			 szFileName[MAX_PATH_LENGTH], szSwImageName[MAX_PATH_LENGTH], szSwImageName2[MAX_PATH_LENGTH];
	SvcSwUpdate_UsbDetectInfo_t *pstContents = (SvcSwUpdate_UsbDetectInfo_t *)userdata;

	HxSTD_PrintToStrN (szFileName, MAX_PATH_LENGTH, "%s/%s", pszPath, pszName);
	HxSTD_PrintToStrN (szSwImageName, MAX_PATH_LENGTH, "%s/%s", pszPath, SW_UPDATE_IMAGE_NAME);
	HxSTD_PrintToStrN (szSwImageName2, MAX_PATH_LENGTH, "%s/%s", pszPath, SW_UPDATE_IMAGE_NAME2);

	HxLOG_Debug("Detected *.HDF File. szFileName : %s\n",szFileName);

	if (( HLIB_STD_StrCaseCmp(szFileName, szSwImageName) == 0 ) || ( HLIB_STD_StrCaseCmp(szFileName, szSwImageName2) == 0 ))
	{
		pstContents->bFoundImagePath	= TRUE;
		HLIB_STD_StrUtf8NCpy((HCHAR*)pstContents->szSoftwarePath, szFileName, MAX_PATH_LENGTH);
		return TRUE;
	}
	return FALSE;
}
#else
STATIC	HBOOL xmgr_swup_UsbLoadFileCb (HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata)
{
	HCHAR			 szFileName[MAX_PATH_LENGTH], szSwImageName[MAX_PATH_LENGTH];
	SvcSwUpdate_UsbDetectInfo_t *pstContents = (SvcSwUpdate_UsbDetectInfo_t *)userdata;

	HxSTD_PrintToStrN (szFileName, MAX_PATH_LENGTH, "%s/%s", pszPath, pszName);
	HxSTD_PrintToStrN (szSwImageName, MAX_PATH_LENGTH, "%s/%s", pszPath, SW_UPDATE_IMAGE_NAME);

	HxLOG_Debug("Detected *.HDF File. szFileName : %s\n",szFileName);

	if ( HLIB_STD_StrCaseCmp(szFileName, szSwImageName) == 0 )
	{
		pstContents->bFoundImagePath	= TRUE;
		HLIB_STD_StrUtf8NCpy((HCHAR*)pstContents->szSoftwarePath, szFileName, MAX_PATH_LENGTH);
		return TRUE;
	}
	return FALSE;
}
#endif

STATIC 	HERROR	xmgr_swup_UsbFindSwImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	if (pstContents == NULL)	return ERR_FAIL;

	(void)HLIB_DIR_Find((HUINT8 *)szPath, (HUINT8 *)"*.hdf", (HUINT8 *)"", xmgr_swup_UsbLoadFileCb, pstContents);

	if (pstContents->bFoundImagePath == TRUE)	// if found matched hdf image file.
		return ERR_OK;

	return ERR_FAIL;
}

HERROR	xmgr_swup_UsbFindImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	return xmgr_swup_UsbFindSwImage(hSWUpdate, szPath, pstContents);
}

