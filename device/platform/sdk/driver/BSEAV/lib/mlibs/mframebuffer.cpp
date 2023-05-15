/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mframebuffer.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mframebuffer.cpp $
 * 
 * 1   2/7/05 11:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   4/30/04 3:13p erickson
 * PR10883: MApplication now keeps a list of MFrameBuffers and
 * MFrameBuffer keeps a list of top level widgets
 *
 *************************************************************************/
#include "mframebuffer.h"
#include "mapplication.h"
#include <string.h>

void MFrameBuffer::getDefaultSettings(Settings *settings)
{
	memset(settings, 0, sizeof(*settings));
}

MFrameBuffer::MFrameBuffer(MApplication *app, const Settings *settings)
{
	_settings = *settings;
	_app = app;
	_app->_fbs.add(this);
}

MFrameBuffer::~MFrameBuffer()
{
	_app->_fbs.remove(this);
}

int MFrameBuffer::screenWidth() const
{
	bwin_framebuffer_settings s;
	bwin_get_framebuffer_settings(_settings.framebuffer, &s);
	return s.width;
}

int MFrameBuffer::screenHeight() const
{
	bwin_framebuffer_settings s;
	bwin_get_framebuffer_settings(_settings.framebuffer, &s);
	return s.height;
}

bool MFrameBuffer::getChildrenZorderRange(int *min, int *max, MWidget *excludeChild) const
{
	bool result = false;
	MListItr<MWidget> itr(&_topWidgets);
	for (MWidget *child = itr.first(); child; child = itr.next()) {
		if (excludeChild == child)
			continue;
		if (!result) {
			*max = *min = child->zorder();
			result = true;
		}
		else {
			if (child->zorder() > *max)
				*max = child->zorder();
			else if (child->zorder() < *min)
				*min = child->zorder();
		}
	}
	return result;
}
