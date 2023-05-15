/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_constellation.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:08p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_constellation.h $
 * 
 * 1   2/7/05 8:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/3   8/21/03 9:43a erickson
 * venom2 settop api work
 * 
 * Irvine_BSEAVSW_Devel/2   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 * 
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 ****************************************************************************/
#ifndef INFO_CONSTELLATION_H__
#define INFO_CONSTELLATION_H__

#include <mlabel.h>
#include "bsettop.h"

/**
* Display a QAM or QPSK constellation by reading soft_decision
* data from btuner_get_soft_decisions.
**/
class Constellation : public MLabel {
public:
	Constellation(MWidget *parent, const MRect &rect = MRect(0,0,300,300));
	~Constellation();

	void set(btuner_t tuner);
	void update();

	// overrides
	void setGeometry(const MRect &rect);

protected:
	void draw(const MRect &cliprect);
	void resizeBuffer();

	int _xborder, _yborder, _borderColor;
	int _lastrefresh;
	char *_buffer;
	unsigned _buffersize;
	btuner_t _tuner;
};

#endif //INFO_CONSTELLATION_H__
