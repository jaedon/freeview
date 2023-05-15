/**
	@file     	mgr_media_util.c
	@brief    	media utility application

	Description: 	media util \n
	Module: 		app/app_ray/ui/media			\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <linkedlist.h>

#include <svc_fs.h>
#if defined(CONFIG_MW_MM_PVR)
#include <svc_rec.h>
#include <svc_meta.h>
#endif
#if defined(CONFIG_MW_EPG_TVTV)
#include <svc_epg.h>
#endif
#include <mgr_storage.h>

#include <mgr_common.h>
#include <mgr_media.h>
#include <mgr_action.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Variables     ********************/
/*******************************************************************/

STATIC HUINT8					 s_szMedia_VideoLastPath[FS_MAX_FILE_NAME_LEN + 1]  = {0,0,};

/*******************************************************************/
/********************      Proto Type define     ********************/
/*******************************************************************/
#define _________PROTO_TYPE__________


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define __GLOBAL_FUNCTIONS__MEDIA__

void MGR_MEDIA_SetVideoLoadedPath(HUINT8 *pPath)
{
	if(pPath != NULL)
	{
		HLIB_STD_StrUtf8NCpy (s_szMedia_VideoLastPath, pPath, FS_MAX_FILE_NAME_LEN);
	}
}

HUINT8 *MGR_MEDIA_GetVideoLoadedPath(void)
{
	return (HUINT8*)s_szMedia_VideoLastPath;
}


/* Media Item */

void MGR_MEDIA_FreeMediaItem (void *pvContents)
{
	if ((MgrMedia_Item_t *)pvContents != NULL)
	{
#if defined(CONFIG_MW_MM_PVR)
		MgrMedia_Item_t *pItem = (MgrMedia_Item_t *)pvContents;
		if(pItem->ucValidThumbImage == 1)
		{
			SVC_META_ReleaseThumbnail(pItem->pulThumbImage);
			pItem->ucValidThumbImage = 0;
			pItem->pulThumbImage = NULL;
		}
#endif
		OxAP_Free((MgrMedia_Item_t *)pvContents);
	}

	return;
}
