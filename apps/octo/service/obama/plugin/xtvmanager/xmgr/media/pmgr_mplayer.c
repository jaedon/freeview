/*******************************************************************************
*
* File Name    : pmgr_mplayer.c
*
* Description  : new media player body for OCTO 2.0
*
*
* Copyright (C) 2013, Humax Co., Ltd. All rights reserved.
*
* Revision History :
*
* Date        Modification                                  Name
* ----        ------------                                 --------
* 2013/03/27	createded									chmin
*
*******************************************************************************/


#include <mgr_media.h>
#include "./local_include/_xmgr_mplayer.h"

/*******************************************************************************
*	Typedef
*******************************************************************************/

/*******************************************************************************
*	Static Variables
*******************************************************************************/

/*******************************************************************************
*	Static functions
*******************************************************************************/

/*******************************************************************************
*	public functions
*******************************************************************************/


MgrMplayer_Proc_t pmgr_mplayer_GetProc(void)
{
	return xproc_mplayer_Base;
}

HERROR pmgr_mplayer_UpdateUserSelectAudioTrackInfo(MGR_MPlayer_t *mplayer, HINT32 nIndex)
{
	return ERR_OK;
}

HERROR pmgr_mplayer_CaptionOnOff(MGR_MPlayer_t *mplayer, HBOOL bOnOff)
{
	return ERR_OK;
}




