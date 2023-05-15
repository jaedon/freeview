/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recstateicon.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:16p $
 *
 * Module Description: Record state UI
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/recstateicon.h $
 * 
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   6/30/03 1:34p erickson
 * new ui work
 *
 ***************************************************************************/
#ifndef RECSTATEICON_H__
#define RECSTATEICON_H__

#include "mwidget.h"
#include "genericscreen.h"

class Control;

class RecordIndicator : public MWidget {
public:
	RecordIndicator(GenericScreen *parent, Control *control);

protected:
	void draw(const MRect &cliprect);
	Control *_control;
};

#endif //RECSTATEICON_H__
