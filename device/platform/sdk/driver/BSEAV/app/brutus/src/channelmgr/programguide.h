/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: programguide.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/23/05 12:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/programguide.h $
 * 
 * 2   8/23/05 12:37p erickson
 * PR16660: added subch support to program guide
 * 
 * 1   2/7/05 7:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   3/4/04 1:15p erickson
 * PR9465: updated more comments
 * 
 * Irvine_BSEAVSW_Devel/2   10/10/03 12:29p erickson
 * added std header
 *
 *************************************************************/
#ifndef PROGRAMGUIDE_H__
#define PROGRAMGUIDE_H__

#include <mlist.h>
#include <mstring.h>

/**
Summary:
ProgramGuide is a very simplistic guide which maps to ChannelManager's channel list.

Description:
It's only value is to give channels a name when recording.
This needs to be completely reworked to make it a more realistic guide,
possibly downloading XML data from the Internet.

The file format has two options:
1) Simple program name. In this case, the program name will be
assigned to the next main channel number.
	Example: ESPN
2) Program name with channel prefix. In this case, the program name will
be assigned to the specific main/sub channel number, and will reset
the current channel for the simple program name format.
	Example: 2.1: ESPN
**/
class ProgramGuide {
public:
	ProgramGuide();
	
	/**
	Summary:
	Read a program guide from disk
	**/
	int read(const char *filename);

	/**
	Summary:
	Read the program name for a channel
	Description:
	Channel may contain subchannel. If no subchannel matches, but the main channel matches, the main channel
	program will be returned.
	**/
	const char *programName(int channel);

	/**
	Summary:
	Set the program name of a channel
	**/
	bool setProgramName(int channel, const char *name);

private:
	struct ProgramName {
		int channel;
		MString name;
		ProgramName(int ch, const char *n) {channel=ch;name=n;}
	};
	MAutoList<ProgramName> _guide;
	int _curchannel;
	MString _filename;
	ProgramName *get(int channel);
};

#endif //PROGRAMGUIDE_H__
