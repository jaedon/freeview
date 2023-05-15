/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recstateicon.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:16p $
 *
 * Module Description: Record state UI
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/recstateicon.cpp $
 * 
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/7   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 * 
 * Irvine_BSEAVSW_Devel/6   4/2/04 5:39p bandrews
 * PR10031: Fixed to use timeline bgcolor and make the recordindicator
 * look like it's part of the timeline while playback is enabled.
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 4:01p erickson
 * PR9107: use default transparency, not chromakey background
 * 
 * Irvine_BSEAVSW_Devel/4   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/3   9/9/03 5:29p erickson
 * made it data driven
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/1   6/30/03 1:34p erickson
 * new ui work
 *
 ***************************************************************************/
#include "recstateicon.h"
#include "control.h"
#include "cfgsettings.h"

RecordIndicator::RecordIndicator(GenericScreen *parent, Control *control) :
	MWidget(parent)
{
	_control = control;
	setBackgroundColor(_control->cfg()->getInt(Config::PVRBAR_OUTLINE_COLOR));
}

void RecordIndicator::draw(const MRect &cliprect) {
	MPainter ptr(this, cliprect);
	int x = 0;
	int total = _control->totalRecords();

	ptr.setFont(MFont(app(), _control->cfg()->get(Config::TINY_FONT)));
	for (int i=0;i<total;i++) {
		ptr.setBrush(_control->cfg()->getInt(Config::PVRBAR_RECORDING_COLOR));
		ptr.fillEllipse(x+height()/2,height()/2,height()/2-4);
		ptr.setPen(textColor());
		ptr.drawEllipse(x+height()/2,height()/2,height()/2-2);

		/* if we have more than one, we need to enumerate them */
		if (total > 1) {
			ptr.setPen(textColor());
			ptr.setBrush(-1);
			ptr.drawText(MRect(x+8,1,width(),height()), MString(i+1),
				-1, MPainter::alLeft, MPainter::valCenter);
		}
		x += height();
	}
	
	ptr.setPen(textColor());
	ptr.setFont(currentFont());
	ptr.drawText(x+4,0,"Rec");

}
