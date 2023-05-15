/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_audio.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/24/05 5:14p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_audio.h $
 * 
 * 1   8/24/05 5:14p erickson
 * PR16819: initial checkin of audio info panel
 * 
 ****************************************************************************/
#ifndef INFO_AUDIO_H__
#define INFO_AUDIO_H__

#include "infopanel.h"

class AudioInfoBarPlugin : public InfoBarPlugin {
public:
	AudioInfoBarPlugin(InfoBar *parent);
	~AudioInfoBarPlugin();
};

#endif //INFO_AUDIO_H__

