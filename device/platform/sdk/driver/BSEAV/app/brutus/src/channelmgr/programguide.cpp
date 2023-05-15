/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: programguide.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 8/24/05 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/programguide.cpp $
 * 
 * 3   8/24/05 6:12p arbisman
 * PR16678: Add support for WinCE
 * 
 * 2   8/23/05 12:37p erickson
 * PR16660: added subch support to program guide
 * 
 * 1   2/7/05 7:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 * 
 * Irvine_BSEAVSW_Devel/3   11/6/03 5:12p erickson
 * PR8570: initial vxworks migration
 * 
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 5:05p erickson
 * initial vxworks migration
 * 
 * Irvine_BSEAVSW_Devel/2   10/10/03 12:29p erickson
 * added std header
 *
 *************************************************************/
#include "programguide.h"
#ifdef _WIN32_WCE
#include <bstd.h>
#endif
#include "brutus_os.h"
#include "channelmap.h" // SUBCH support
#include <mstringlist.h>
BDBG_MODULE(programguide);

ProgramGuide::ProgramGuide()
{
	_curchannel = 0;
}

int ProgramGuide::read(const char *filename) {
	// read the raw guide off disk
	MStringList g;
	_filename = filename;
	if (!readFile(_filename, g))
		return -1;
		
	// process it
	for (const char *s = g.first(); s;) {
		int channel;
		char *find = strskip(s, " \t");
		char *name;
		
		// remove comments and check for blank lines
		if (!find || !*find || *find == '#') {
			g.remove();
			s = g.current();
			continue;
		}

		// we have an entry. see if it has a prefix or is simple
		name = strchr(find, ':');
		if (name && name > find) {
			// we have a prefix
			*name++ = 0; // terminate string in middle
			
			// now search for subchannel
			char *subchan = strchr(find, '.');
			if (subchan) {
				*subchan = 0;
				int subchannum = atoi(subchan+1);
				if (subchannum) subchannum--; /* the ui displays subchan and +1, so allow this in program guide.
					this means that X.0 and X.1 will map to the same thing in the program guide. */
				channel = MAKESUBCH(atoi(find), subchannum);
			}
			else {
				channel = atoi(find);
			}
			_curchannel = channel; // reset for next simple entry
		}
		else {
			channel = _curchannel;
			name = find;
		}
		name += strspn(name, " \t");
		BDBG_WRN(("add program %d.%d: %s", MAINCH(channel), SUBCH(channel), (const char *)name));
		_guide.add(new ProgramName(channel, name));
		
		// inc _curchannel for next simple entry
		_curchannel = MAINCH(_curchannel + 1);
		
		s = g.next();
	}
	
	return _guide.total();
}

ProgramGuide::ProgramName *ProgramGuide::get(int channel)
{
	// search for exact match
	for (ProgramName *p = _guide.first(); p; p = _guide.next()) {
		if (p->channel == channel) {
			BDBG_MSG(("Found exact channel %d.%d (%d.%d)", MAINCH(p->channel), SUBCH(p->channel), MAINCH(channel), SUBCH(channel)));
			return p;
		}
	}
	// search for mainch match
	for (ProgramName *p = _guide.first(); p; p = _guide.next()) {
		if (MAINCH(p->channel) == MAINCH(channel)) {
			BDBG_MSG(("Found main channel %d (%d)", MAINCH(p->channel), MAINCH(channel)));
			return p;
		}
	}
	return NULL;
}

const char *ProgramGuide::programName(int channel) {
	ProgramName *p = get(channel);
	return p?(const char*)p->name:NULL;
}

bool ProgramGuide::setProgramName(int channel, const char *name) {
	ProgramName *p = get(channel);
	if (p) {
		p->name = name;
	}
	else {
		_guide.add(new ProgramName(channel, name));
	}
	
	// save it
	char buf[256];
	FILE *f = fopen(_filename, "w+");
	if (!f) return false;
	fputs("# Brutus Program Guide\n", f);
	for (ProgramName *p = _guide.first(); p; p = _guide.next()) {
		int subch = SUBCH(p->channel);
		if (subch) {
			snprintf(buf, 256, "%d.%d: %s\n", MAINCH(p->channel), subch+1, (const char *)p->name);
		}
		else {
			snprintf(buf, 256, "%d: %s\n", MAINCH(p->channel), (const char *)p->name);
		}
		fputs(buf, f);
	}
	fclose(f);
	b_sync_disk();
	return true;
}
