/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_video.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/25/05 3:27p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_video.h $
 * 
 * 2   8/25/05 3:27p erickson
 * PR16819: named Config to Video, and moved BTSC SAP to new Audio panel
 * 
 * 1   2/7/05 8:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 ****************************************************************************/
#ifndef INFO_VIDEO_H__
#define INFO_VIDEO_H__

#include "infopanel.h"

/**
Info plugin used for general configuration and feature demo
It is not page based, therefore is makes navigation on/off the popup easier.
**/
class VideoInfoBarPlugin : public InfoBarPlugin {
public:
	VideoInfoBarPlugin(InfoBar *parent);
	~VideoInfoBarPlugin();
};

#endif //INFO_VIDEO_H__

