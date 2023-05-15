/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pvrstateicon.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 5/7/08 11:52a $
 *
 * Module Description: Icon displaying pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/pvrstateicon.h $
 * 
 * 3   5/7/08 11:52a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 2   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 1   2/7/05 8:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/3   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 *
 * Irvine_BSEAVSW_Devel/2   2/18/04 2:22p erickson
 * PR9757: unlink PlayEngine's in destructor
 *
 * Irvine_BSEAVSW_Devel/1   6/27/03 10:11a erickson
 * broke pvrtimeline into components
 *
 ***************************************************************************/
#ifndef PVRSTATEICON_H__
#define PVRSTATEICON_H__

#include "mwidget.h"
#include "playengine.h"
#include "mpainter.h"
#include "mtimer.h"
#include "mlabel.h"

class Config;

/**
* Icon displaying pvr state
**/
class PvrStateIcon : public MWidget,
	public PlayEngineEventHandler
{
public:
	PvrStateIcon(const Config *cfg, MWidget *parent, const MRect &rect);
	~PvrStateIcon();
	void setPlayEngine(PlayEngine *engine);

	void onStateChanged(PlayEngine *self);

protected:
	void draw(const MRect &cliprect);

	PlayEngine *_engine;
	MImage *_play, *_pause, *_stop, *_ffwd, *_rew, *_slowfwd, *_slowrew;
};

/**
* Text displaying pvr state
**/
class PvrStateLabel : public MLabel,
	public PlayEngineEventHandler,
	public MTimerEventHandler
{
public:
	PvrStateLabel(MWidget *parent, const MRect &rect);
	~PvrStateLabel();
	void setPlayEngine(PlayEngine *engine);

	void onStateChanged(PlayEngine *self);
	void onSpecialStateMessage(PlayEngine *self);
	void onTimeout(MTimer *self);
protected:
	PlayEngine *_engine;
	MTimer _timer;
};

#endif //PVRSTATEICON_H__
