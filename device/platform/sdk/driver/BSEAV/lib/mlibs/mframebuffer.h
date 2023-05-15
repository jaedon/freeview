/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mframebuffer.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/7/07 11:35a $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mframebuffer.h $
 * 
 * 2   11/7/07 11:35a haisongw
 * PR36757: for gcc 4.2 compilation
 * 
 * 1   2/7/05 11:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   4/30/04 3:13p erickson
 * PR10883: MApplication now keeps a list of MFrameBuffers and
 * MFrameBuffer keeps a list of top level widgets
 *
 * Irvine_BSEAVSW_Devel/1   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 *************************************************************************/
#ifndef MFRAMEBUFFER_H__
#define MFRAMEBUFFER_H__

#include "mlist.h"
class MWidget;
class MApplication;
#ifdef BWIN_SUPPORT
/* bwin must be exposed outside of mlibs. This is because
we want to create the framebuffers outside of mlibs.
*/
#include "bwin.h"
#endif

class MFrameBuffer {
	friend class MWidget; // _topWidgets.add/remove
	friend class MApplication; // _topWidgets search
public:
	struct Settings {
#ifdef BWIN_SUPPORT
		bwin_framebuffer_t framebuffer;
#endif
	};
	static void getDefaultSettings(Settings *settings);
	MFrameBuffer(MApplication *app, const Settings *settings);
	virtual ~MFrameBuffer();

	MApplication *app() const {return _app;}
	int id() const {return (int)_settings.framebuffer;}
	void getSettings(Settings *settings) const {*settings = _settings;}

	int screenWidth() const;
	int screenHeight() const;

	bool getChildrenZorderRange(int *min, int *max, MWidget *excludeChild = NULL) const;

private:
	Settings _settings;
	MApplication *_app;
	MList<MWidget> _topWidgets;
};

#endif //MFRAMEBUFFER_H__
