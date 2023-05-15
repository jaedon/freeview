/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pvrstateicon.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/7/08 11:52a $
 *
 * Module Description: Icon displaying pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/pvrstateicon.cpp $
 * 
 * 4   5/7/08 11:52a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 3   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 2   2/14/05 5:00p erickson
 * PR11077: need background color for rgb png's
 *
 * 1   2/7/05 8:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/9   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 *
 * Irvine_BSEAVSW_Devel/8   2/18/04 2:22p erickson
 * PR9757: unlink PlayEngine's in destructor
 *
 * Irvine_BSEAVSW_Devel/7   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/6   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/5   10/7/03 1:19p erickson
 * PR8241: don't update pvr timeline or state icon state unless pvr is
 * active
 *
 * Irvine_BSEAVSW_Devel/4   10/6/03 11:43a erickson
 * PR8105: PvrStateIcon, PvrStateLabel and PvrTimeline need to update
 * state when the PlayEngine is
 * reset. This happens during swap.
 *
 * Irvine_BSEAVSW_Devel/3   9/5/03 3:27p erickson
 * adapted to new settop api pvr api
 *
 * Irvine_BSEAVSW_Devel/2   6/30/03 1:34p erickson
 * new ui work
 *
 * Irvine_BSEAVSW_Devel/1   6/27/03 10:11a erickson
 * broke pvrtimeline into components
 *
 ***************************************************************************/
#include "pvrstateicon.h"
#include "cfgsettings.h"

#define PIXMAPPATH (MString(cfg->get(Config::IMAGES_PATH)) + "/vcr/")

PvrStateIcon::PvrStateIcon(const Config *cfg, MWidget *parent, const MRect &rect) :
	MWidget(parent, rect, "pvrstateicon")
{
	_play = new MImage(app(), PIXMAPPATH + "play.png");
	_pause = new MImage(app(), PIXMAPPATH + "pause.png");
	_stop = new MImage(app(), PIXMAPPATH + "stop.png");
	_ffwd = new MImage(app(), PIXMAPPATH + "ffwd.png");
	_rew = new MImage(app(), PIXMAPPATH + "rew.png");
	_slowfwd = new MImage(app(), PIXMAPPATH + "slowfwd.png");
	_slowrew = new MImage(app(), PIXMAPPATH + "slowrew.png");

	_engine = NULL;

	/* We might be blitting onto a transparent background, so we need to set
	the background color. TODO: something's not right here. I think this is a
	change needed in bwin. */
	setBackgroundColor(0xFF000000);
}

PvrStateIcon::~PvrStateIcon()
{
	// unlink
	setPlayEngine(NULL);
	delete _play;
	delete _pause;
	delete _stop;
	delete _ffwd;
	delete _rew;
	delete _slowfwd;
	delete _slowrew;
}

void PvrStateIcon::draw(const MRect &cliprect) {
	MPainter ptr(this, cliprect);
	MImage *state;

	if (!_engine)
		return;

	switch (_engine->state()) {
	case bplayback_state_playing:
		state = _play; break;
	case bplayback_state_frameadvance:
	case bplayback_state_paused:
		state = _pause; break;
	case bplayback_state_trickmode:
		if (_engine->isFastTrickMode())
			state = _engine->direction()>0?_ffwd:_rew;
		else
			state = _engine->direction()>0?_slowfwd:_slowrew;
		break;
	default:
		state = _stop; break;
	}
	ptr.drawImage(*state,0,0);
}

void PvrStateIcon::onStateChanged(PlayEngine *self) {
	repaint();
}

void PvrStateIcon::setPlayEngine(PlayEngine *engine) {
	if (_engine != engine) {
		if (_engine)
			_engine->removeHandler(this);
		_engine = engine;
		if (_engine) {
			_engine->addHandler(this);
			if (_engine->video()) {
				onStateChanged(engine);
			}
		}
	}
}

/////////////////////////////////////////

#define SPECIAL_STATE_MESSAGE_TIMEOUT 300 //msec

PvrStateLabel::PvrStateLabel(MWidget *parent, const MRect &rect) :
	MLabel(parent, rect, "pvrstatelabel"), _timer(app())
{
	_timer.addHandler(this);
	_engine = NULL;
}

PvrStateLabel::~PvrStateLabel()
{
	// unlink
	setPlayEngine(NULL);
}

void PvrStateLabel::onSpecialStateMessage(PlayEngine *self) {
	setText(_engine->specialStateMessage());
	_timer.start(SPECIAL_STATE_MESSAGE_TIMEOUT);
}

void PvrStateLabel::onStateChanged(PlayEngine *engine) {
	setText(_engine?_engine->stateStr():NULL);
	repaint();
}

void PvrStateLabel::onTimeout(MTimer *self) {
	onStateChanged(_engine);
}

void PvrStateLabel::setPlayEngine(PlayEngine *engine) {
	if (_engine != engine) {
		if (_engine)
			_engine->removeHandler(this);
		_engine = engine;
		if (_engine) {
			_engine->addHandler(this);
			if (_engine->video()) {
				onStateChanged(engine);
			}
		}
	}
}
