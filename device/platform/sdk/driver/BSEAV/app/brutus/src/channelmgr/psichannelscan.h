/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psichannelscan.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/19/07 12:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/psichannelscan.h $
 * 
 * 2   9/19/07 12:33p jrubio
 * PR34613: new argument for read, NETACCEL
 * 
 * 1   2/7/05 8:00p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   7/21/04 3:16p erickson
 * PR11682: updated docs, removed destructor
 * 
 * Irvine_BSEAVSW_Devel/8   7/21/04 10:10a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 * 
 * Irvine_BSEAVSW_Devel/7   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/6   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/5   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 *
 * Irvine_BSEAVSW_Devel/4   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 *************************************************************/
#ifndef PSICHANNELSCAN_H__
#define PSICHANNELSCAN_H__

#include "channelscan.h"

class MStringList;

/**
Summary:
Default Brutus channel scan using the "channels.txt" file format.

Description:
Uses tspsimgr to scan channels.
Only supports clear channels.
**/
class PSIChannelScan : public ChannelScan {
public:
	PSIChannelScan(ChannelMap *map, const Settings *settings);

	/**
	Summary:
	Write the channel map to disk in "channels.txt" format.
	**/
	int write(const char *filename) const;

	/**
	Summary:
	Read the channel map from disk in "channels.txt" format.
	**/
	int read(const char *filename, const char * ipv4);

protected:
	/* virtual function called from ChannelScan::scan */
	int readProgramInfo(bband_t band, unsigned stream);

	bool process(MStringList &list, ChannelMap::ChannelMapEntry *str);
	void edit(MStringList &list, int entrynum, ChannelMap::ChannelMapEntry *entry);
	void beginProgramSelect(int entryIndex);
	bool checkProgramSelect(int entryIndex);
};

#endif //PSICHANNELSCAN_H__
