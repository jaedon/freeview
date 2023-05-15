/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_playbackbuffer.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 12/19/06 10:53a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_playbackbuffer.h $
 * 
 * 5   12/19/06 10:53a erickson
 * PR26644: added check for bdecode_pts_type
 * 
 * 4   11/15/06 7:34p bandrews
 * PR25456: Added audio passthru channel buffer display
 * 
 * 3   1/26/06 9:47a erickson
 * PR19003: convert TSM monitor to units of msec for MPEG2/DSS support
 * 
 * 2   12/20/05 6:23a erickson
 * PR18256: moved progress bar code into reuseable file
 * 
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/4   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/3   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 * 
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 * 
 * Irvine_BSEAVSW_Devel/4   4/17/03 2:14p erickson
 * added #include for NO_PVR mode
 * 
 * Irvine_BSEAVSW_Devel/3   4/16/03 4:00p erickson
 * added pts/pcr bars
 * 
 * Irvine_BSEAVSW_Devel/2   4/15/03 12:40p erickson
 * 7115 work, added more build conditionals
 *
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 ****************************************************************************/
#ifndef INFO_PLAYBACKBUFFER_H__
#define INFO_PLAYBACKBUFFER_H__

#include <mlabel.h>
#include <mtimer.h>
#include "infopanel.h"

class MProgressBar;

class PlaybackBufferPanel : public InfoPanelPage, MTimerEventHandler {
public:
	PlaybackBufferPanel(InfoPanel *infoPanel, const MRect &rect);
	void start();
	void stop();
	void update();

	// overrides
	void onTimeout(MTimer *timer);
protected:
	MProgressBar *_bar[8];
	MLabel *_sizes[4];
	MLabel *_depths[4];
	MLabel *_bitrate, *_primAudioPTS, *_secAudioPTS, *_videoPTS;
	MTimer _timer;
	unsigned long startTick, bytesAtStartTick;
	void setStcPtsDiff(bool is_audio, bool is_primary, int diff, bool is_dss, bool valid);
};

#endif //INFO_PLAYBACKBUFFER_H__
